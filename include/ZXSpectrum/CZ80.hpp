/** \ingroup ZXSPECTRUM */
/*@{*/

/**
 *	@file
 *	File: CZ80.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 18/10/2026 \n
 *	Description: Z80 access recording, timing association and Spectrum contention.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_CZ80__
#define __ZXSPECTRUM_CZ80__

#include <FZ80/CZ80.hpp>
#include <FZ80/Instruction.hpp>

namespace ZXSPECTRUM
{
	class ULA;

	/** Z80 coordinating access recording, nominal timing and Spectrum contention. \n
		CPU-visible RAM forwards access requests; the CPU filters their context and range. \n
		Retained bytes are associated with precalculated instruction or interrupt-response M-cycles. \n
		Each calculated wait shifts the phase of subsequent accesses. \n
		Memory-only instructions consume their accumulated delay as terminal CPU wait. \n
		Mixed instructions include memory-before, port and memory-after waits in their duration. \n
		Port effects remain deferred; this does not provide exact intra-instruction chip interleaving. \n
		Partially retained, ambiguous data-transfer groups are not assigned contention timing yet. \n
		Crossing a range boundary does not itself prevent capture of the contained bytes. \n
		Memory contention is supported in Full mode and PerCycle mode with one tick per call. \n
		Interrupt timing covers IM1, IM2, NMI and IM0 RST responses. \n
		Multi-tick batches, other IM0 instructions and recognition/refresh contention remain unsupported. \n
		Unsupported memory cases have no approximate reconstruction fallback; I/O retains its port-only calculation. */
	class CZ80 final : public FZ80::CZ80
	{
		public:
		/** Identifies the CPU operation owning the access transaction. */
		enum class MemoryAccessOperation
		{
			/** No operation has started since initialization. */
			_NONE,
			/** Execution of an instruction, including its deferred port completion. */
			_INSTRUCTION,
			/** Execution of an accepted interrupt response. */
			_INTERRUPT
		};

		/** Execution state of the operation owning the access transaction. \n
			Recording can be temporarily suspended while the operation remains active. \n
			A completed operation can still have contention wait pending. */
		enum class MemoryAccessState
		{
			/** No operation has started since initialization. */
			_IDLE,
			/** Instruction or interrupt execution is still in progress. */
			_IN_PROGRESS,
			/** Execution has ended, but a deferred port effect remains pending. */
			_PENDING_PORT_COMPLETION,
			/** Execution and deferred port completion have finished successfully. */
			_COMPLETED,
			/** Execution or deferred port completion has failed. */
			_FAILED
		};

		/** One requested byte access to potentially contended memory. \n
			Collection order preserves the order of access notifications. \n
			External consumers may use timing fields only after the transaction publishes timing availability. \n
			Internal mixed-I/O calculation may use the partial association it has just validated. */
		struct MemoryAccess
		{
			enum class Type
			{
				_READ,
				_WRITE
			};

			enum class Origin
			{
				/** Byte obtained through the classified instruction-byte loading path. */
				_INSTRUCTION_BYTE,
				/** Data transfer requested by instruction or interrupt execution. */
				_DATA
			};

			/** CPU-visible address of the requested byte. \n
				Only addresses from 0x4000 to 0x7fff are currently retained. */
			unsigned short _address;
			/** Requested transfer direction, independent of write buffering. */
			Type _type;
			/** Distinguishes instruction-byte loading from execution data transfers. */
			Origin _origin;
			/** Zero-based M-cycle index within the associated instruction or response structure. \n
				Initialized to FZ80::MCycle::_NOCYCLE until timing association. */
			size_t _mCycle;
			/** Nominal contention point, expressed in CPU T-states from operation start. \n
				Excludes contention waits and is not an absolute CPU clock value. \n
				Initialized to FZ80::MCycle::_NOCYCLE until timing association. */
			size_t _contentionCycle;
			/** Nominal data-transfer point, expressed in CPU T-states from operation start. \n
				Excludes contention waits and is not an absolute CPU clock value. \n
				Initialized to FZ80::MCycle::_NOCYCLE until timing association. */
			size_t _dataCycle;
		};

		using MemoryAccesses = std::vector <MemoryAccess>;

		/** Access recording and contention state belonging to one CPU operation. \n
			Owns its access collection and preserves it across partial execution calls. \n
			Does not own CPU or ULA connections and never advances the CPU clock. \n
			The record remains available until initialization or the next operation. */
		struct MemoryAccessTransaction
		{
			MemoryAccessTransaction ();

			/** Restores the initial state, discarding records and pending wait. \n
				Reserved collection capacity is retained for subsequent operations. \n
				Used during CPU initialization, when previous execution is abandoned. */
			void reset ();
			/** Starts another operation and enables recording. \n
				The caller must have consumed the preceding transaction's pending wait. \n
				Previously allocated collection capacity is reused. */
			void begin
				(MemoryAccessOperation o, unsigned short pc, unsigned int cC);
			/** Retains immutable response timing selected at interrupt acceptance. \n
				Null identifies an unsupported response; ownership remains with the interrupt. */
			void setInterruptTiming (const FZ80::MCyclesData* timing)
							{ _interruptTiming = timing; }
			/** Enables capture without discarding the current operation's records. */
			void resumeRecording () { _recording = true; }
			/** Disables capture without changing records or execution state. */
			void suspendRecording () { _recording = false; }
			/** Suspends capture and updates operation completion state. \n
				Failure takes precedence over deferred port completion. \n
				Recorded accesses remain available in every resulting state. */
			void finish (bool failed, bool pendingPortAccess);
			/** Publishes the selected alternative after every retained access was associated. */
			void setTimingAssociation (size_t n);
			/** Publishes the calculated memory-contention delay, including a valid zero delay. \n
				total excludes the port's own contention delay, which is accounted for separately. \n
				included identifies memory-wait T-states already added to instruction duration. \n
				Only total minus included becomes terminal CPU wait. */
			void setContentionDelay
				(unsigned int total, unsigned int included = 0);
			/** Consumes all pending wait in Full mode, or one T-state in PerCycle mode. \n
				Returns the number of consumed CPU T-states without advancing its clock. */
			unsigned int consumeContentionDelay (bool full);

			// Implementation
			/** Kind of CPU operation owning the record. \n
				_NONE is used only before an operation has started. */
			MemoryAccessOperation _operation;
			/** Execution and deferred-port-completion state of the owning operation. \n
				_COMPLETED does not imply that pending contention wait is zero. */
			MemoryAccessState _state;
			/** CPU program counter immediately before the operation starts. \n
				Used to identify instruction-byte offsets, including address wrapping. */
			unsigned short _pc;
			/** Absolute CPU clock at the beginning of the owning operation. \n
				Expressed in CPU T-states using the CPU clock counter's representation. */
			unsigned int _startClockCycle;
			/** Non-owning complete response descriptor selected at interrupt acceptance. \n
				The interrupt outlives this transaction and never rebuilds the descriptor. \n
				Null for instructions and unsupported interrupt responses; preserved by finish. */
			const FZ80::MCyclesData* _interruptTiming;
			/** Associated instruction alternative, or zero for the captured interrupt descriptor. \n
				For interrupts, zero identifies _interruptTiming rather than lastInstruction. \n
				Invalid until complete timing association succeeds. */
			size_t _mCyclesStructure;
			/** Whether every retained access has an unambiguous nominal timing. \n
				Published after complete association; false for unsupported or ambiguous associations. \n
				External consumers must not use individual timing fields while this is false. \n
				Internal mixed-I/O calculation may use its validated partial association before publication. */
			bool _timingAvailable;
			/** Whether memory contention was evaluated for this operation. \n
				True also when the calculated delay is zero. \n
				Prevents a prepared mixed operation from being calculated again on completion. */
			bool _contentionHandled;
			/** Total memory-contention T-states calculated for this operation. \n
				Excludes the port's own contention delay in mixed instructions. \n
				Remains unchanged while pending wait is consumed. \n
				Only represents a calculated result when _contentionHandled is true. */
			unsigned int _contentionClockCycles;
			/** Memory-contention T-states already included in instruction duration. \n
				They must not be consumed again as terminal CPU wait. */
			unsigned int _includedContentionClockCycles;
			/** Additional CPU T-states still to be consumed before another operation. \n
				Buffered effects remain retained while this value is nonzero. \n
				Decreases during wait consumption without changing the total delay. */
			unsigned int _pendingContentionClockCycles;
			/** Whether memory notifications are currently accepted into this record. \n
				This is a capture flag, independent of the operation's execution state. */
			bool _recording;
			/** Retained byte accesses in notification order. \n
				Writes represent requests, even when their effects remain buffered. \n
				An empty collection is valid and does not itself indicate failure. */
			MemoryAccesses _accesses;
		};

		CZ80 (int id, const FZ80::Z80PortsMap& pts = { });

		/** Non-owning connection established by the computer before execution. */
		void linkToULA (ULA* u) { _ula = u; }

		// Information about the contention...
		/** Whether memory contention was evaluated for the recorded operation, including zero delay. */
		bool memoryContentionHandled () const
							{ return (_memoryAccessTransaction._contentionHandled); }
		/** Total memory-contention wait calculated for the recorded operation. */
		unsigned int memoryContentionClockCycles () const
							{ return (_memoryAccessTransaction._contentionClockCycles); }
		/** Memory-contention T-states already included in instruction duration. */
		unsigned int includedMemoryContentionClockCycles () const
							{ return (_memoryAccessTransaction._includedContentionClockCycles); }
		/** Additional wait still to be consumed before buffered effects are released. */
		unsigned int pendingMemoryContentionClockCycles () const
							{ return (_memoryAccessTransaction._pendingContentionClockCycles); }

		// Information about the access to the contented memory if any...
		/** Whether notifications are currently being collected. */
		bool recordingMemoryAccesses () const
							{ return (_memoryAccessTransaction._recording); }
		/** Kind of operation owning the current record. */
		MemoryAccessOperation memoryAccessOperation () const
							{ return (_memoryAccessTransaction._operation); }
		/** Execution state, independent of any remaining contention wait. */
		MemoryAccessState memoryAccessState () const
							{ return (_memoryAccessTransaction._state); }
		/** PC before the instruction or interrupt response starts. */
		unsigned short memoryAccessPC () const
							{ return (_memoryAccessTransaction._pc); }
		/** CPU clock at the beginning of the owning operation. */
		unsigned int memoryAccessStartClockCycle () const
							{ return (_memoryAccessTransaction._startClockCycle); }
		/** Whether every retained access has an unambiguous nominal timing. \n
			This does not mean that contention delays have been calculated. */
		bool memoryAccessTimingAvailable () const
							{ return (_memoryAccessTransaction._timingAvailable); }

		/** Associated instruction alternative, or zero for the captured interrupt response. \n
			Usable only when timing is available; interrupt zero is not an index into lastInstruction. */
		size_t memoryAccessMCyclesStructure () const
							{ return (_memoryAccessTransaction._mCyclesStructure); }

		/** Records individual bytes in notification order. \n
			Only addresses from 0x4000 to 0x7fff are retained. \n
			Inspection and instruction-selection reads are excluded. \n
			Instruction bytes and execution data retain different origins. \n
			Instruction-byte loading during interrupt responses is excluded. \n
			Calls outside an enabled capture interval are ignored. */
		void recordMemoryAccess
			(const MCHEmul::Address& a, size_t nB, MemoryAccess::Type t);
		/** Exposes the current operation's records, which can still be incomplete. \n
			Starting another operation clears the collection. */
		const MemoryAccesses& memoryAccesses () const
							{ return (_memoryAccessTransaction._accesses); }

		/** Calculates surrounding memory contention and publishes the included delay. */
		virtual IOAccessTiming calculateIOAccessTiming
			(const FZ80::Instruction& instruction,
			 const FZ80::Z80Port::Access& access, unsigned int cC) override;

		/** Releases buffered effects only after terminal memory wait has been consumed. \n
			Pending terminal wait prevents release even when force is true. \n
			A deferred block-input write is captured within its original transaction. \n
			Delegated release preserves the order of preceding writes, port effect and following writes. */
		virtual void executeBufferedCommands (bool force = false) override;
		virtual bool initialize () override;

		protected:
		/** Starts interrupt recording only after the request has been accepted. */
		virtual void aknowledgeInterrupt
			(const MCHEmul::CPUInterruptRequest& iR) override;

		virtual bool executeNextInterruptRequest_PerCycle
			(unsigned int& e) override;
		virtual bool executeNextInterruptRequest_Full
			(unsigned int& e) override;
		virtual bool executeNextInstruction_PerCycle
			(unsigned int& e) override;
		virtual bool executeNextInstruction_Full
			(unsigned int& e) override;

		private:
		/** Calculates memory and port waits for one prepared I/O transaction. \n
			Unsupported memory contexts retain the generic port-only calculation. */
		IOAccessTiming calculateMemoryAndIOAccessTiming
			(const FZ80::Instruction& instruction,
			 const FZ80::Z80Port::Access& access, unsigned int cC);
		/** Resolves the executed instruction without reading or decoding memory. */
		const FZ80::Instruction* executedInstruction () const;
		/** Associates a completed instruction or interrupt transaction. \n
			Returns its stable descriptor after publishing complete timing association. \n
			Returns nullptr for incomplete, unsupported or ambiguous operations. */
		const FZ80::MCyclesData* associateMemoryAccessTiming ();
		/** Associates retained accesses with nominal transfer positions. \n
			instructionBytes is zero for interrupt responses. \n
			Does not publish timing availability, read memory or advance CPU execution. \n
			Mixed-I/O preparation can use its validated prefix before a deferred write. */
		bool associateMemoryAccessTiming
			(const FZ80::MCyclesData& timing, size_t instructionBytes);
		/** Prepares terminal memory wait for an associated operation without I/O. \n
			Mixed instructions already account for their waits during I/O preparation. */
		void prepareMemoryContentionDelay (const FZ80::MCyclesData& timing);
		/** Consumes pending wait instead of accepting another CPU operation. */
		bool consumeMemoryContentionDelay (bool full);
		/** Starts a transaction using the current CPU program counter and clock. */
		void beginMemoryAccessOperation (MemoryAccessOperation o);
		/** Finishes capture and coordinates timing association and contention. */
		void finishMemoryAccessOperation (bool failed);

		private:
		/** Non-owning ULA connection established by the computer. \n
			The connection survives CPU initialization. */
		ULA* _ula;

		// Implementation
		/** Access capture, timing association and contention wait of one CPU operation. */
		MemoryAccessTransaction _memoryAccessTransaction;
	};
}

#endif

// End of the file
/*@}*/
