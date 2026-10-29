/** \ingroup ZXSPECTRUM */
/*@{*/

/**
 *	@file
 *	File: CZ80.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 18/10/2026 \n
 *	Description: Z80 memory-access recording for Spectrum contention.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_CZ80__
#define __ZXSPECTRUM_CZ80__

#include <FZ80/CZ80.hpp>
#include <FZ80/Instruction.hpp>

namespace ZXSPECTRUM
{
	class ULA;

	/** Z80 with memory-access recording for Spectrum contention. */
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
			Timing fields are valid only when the owning transaction publishes timing availability. */
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
			/** Zero-based M-cycle index within the associated instruction structure. \n
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
			/** Publishes the calculated contention delay, including a valid zero delay. \n
				Called once for a supported operation, before consuming its wait. */
			void setContentionDelay (unsigned int cC);
			/** Consumes all pending wait in Full mode, or one T-state in PerCycle mode. \n
				Returns the number of consumed CPU T-states without advancing its clock. */
			unsigned int consumeContentionDelay (bool full);

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
			/** M-cycle alternative used to associate this transaction's retained accesses. \n
				Indexes the executed instruction's precalculated metadata; no structures are copied. \n
				Invalid until timing association succeeds. */
			size_t _mCyclesStructure;
			/** Whether every retained access has an unambiguous nominal timing. \n
				False during recording and for unsupported or ambiguous associations. \n
				Individual timing fields must not be consumed while this is false. */
			bool _timingAvailable;
			/** Whether the new memory-contention path evaluated this operation. \n
				True also when the calculated delay is zero. \n
				Prevents the legacy path from evaluating the same operation again. */
			bool _contentionHandled;
			/** Total additional CPU T-states calculated for this operation. \n
				Remains unchanged while pending wait is consumed. \n
				Only represents a calculated result when _contentionHandled is true. */
			unsigned int _contentionClockCycles;
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
		/** Whether memory contention for the recorded operation used the new path. */
		bool memoryContentionHandled () const
							{ return (_memoryAccessTransaction._contentionHandled); }
		/** Total additional wait calculated for the recorded operation. */
		unsigned int memoryContentionClockCycles () const
							{ return (_memoryAccessTransaction._contentionClockCycles); }
		/** Additional wait still to be consumed before buffered effects are released. */
		unsigned int pendingMemoryContentionClockCycles () const
							{ return (_memoryAccessTransaction._pendingContentionClockCycles); }

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

		/** Alternative associated with the retained accesses; usable only when timing is available. */
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

		/** Completes pending port effects within their original access record. */
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
		/** Resolves the executed instruction without reading or decoding memory. */
		const FZ80::Instruction* executedInstruction () const;
		/** Associates retained accesses with a single M-cycle structure. \n
			Returns false when the operation or its mapping is not supported yet. \n
			No memory is read and no execution state or clock is changed. */
		bool associateMemoryAccessTiming ();
		/** Prepares a terminal wait for supported memory-only instructions. */
		void prepareMemoryContentionDelay ();
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

		/** Access capture, timing association and contention wait of one CPU operation. */
		MemoryAccessTransaction _memoryAccessTransaction;
	};
}

#endif

// End of the file
/*@}*/
