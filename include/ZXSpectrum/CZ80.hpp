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

namespace ZXSPECTRUM
{
	/** Z80 with memory-access recording for Spectrum contention. */
	class CZ80 final : public FZ80::CZ80
	{
		public:
		/** Identifies the CPU operation owning the current access record. */
		enum class MemoryAccessOperation
		{
			_NONE,
			_INSTRUCTION,
			_INTERRUPT
		};

		/** Lifecycle of the current access record. \n
			Capture can be suspended while an operation remains in progress. */
		enum class MemoryAccessState
		{
			_IDLE,
			_IN_PROGRESS,
			_PENDING_PORT_COMPLETION,
			_COMPLETED,
			_FAILED
		};

		/** A requested access to potentially contended memory. \n
			Its position in the collection preserves notification order. \n
			It does not yet identify a machine cycle or an access clock. */
		struct MemoryAccess
		{
			enum class Type
			{
				_READ,
				_WRITE
			};

			unsigned short _address;
			Type _type;
		};

		using MemoryAccesses = std::vector <MemoryAccess>;

		CZ80 (int id, const FZ80::Z80PortsMap& pts = { });

		/** Whether notifications are currently being collected. */
		bool recordingMemoryAccesses () const
							{ return (_recordingMemoryAccesses); }
		MemoryAccessOperation memoryAccessOperation () const
							{ return (_memoryAccessOperation); }
		MemoryAccessState memoryAccessState () const
							{ return (_memoryAccessState); }
		/** PC before the instruction or interrupt response starts. */
		unsigned short memoryAccessPC () const
							{ return (_memoryAccessPC); }
		/** CPU clock at the beginning of the owning operation. */
		unsigned int memoryAccessStartClockCycle () const
							{ return (_memoryAccessStartClockCycle); }
		/** Records individual bytes in notification order. \n
			Only addresses from 0x4000 to 0x7fff are retained. \n
			Calls outside an enabled capture interval are ignored. */
		void recordMemoryAccess
			(const MCHEmul::Address& a, size_t nB, MemoryAccess::Type t);
		/** Exposes the current operation's records, which can still be incomplete. \n
			Starting another operation clears the collection. */
		const MemoryAccesses& memoryAccesses () const
							{ return (_memoryAccesses); }

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
		/** Starts a new operation and clears the previous access record. */
		void beginMemoryAccessOperation (MemoryAccessOperation o);
		/** Stops capture and determines whether completion is deferred or final. */
		void finishMemoryAccessOperation (bool failed);
		/** Clears previous records and enables capture. */
		void beginMemoryAccessRecording ();
		/** Suspends capture without changing the operation or its records. */
		void endMemoryAccessRecording ()
							{ _recordingMemoryAccesses = false; }

		private:
		MemoryAccessOperation _memoryAccessOperation;
		MemoryAccessState _memoryAccessState;
		unsigned short _memoryAccessPC;
		unsigned int _memoryAccessStartClockCycle;
		bool _recordingMemoryAccesses;
		MemoryAccesses _memoryAccesses;
	};
}

#endif

// End of the file
/*@}*/
