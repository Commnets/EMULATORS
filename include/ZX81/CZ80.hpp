/** \ingroup ZX81 */
/*@{*/

/**
 *	@file
 *	File: CZ80.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/09/2026 \n
 *	Description: Z80 interrupt wiring for ZX80 and ZX81 machines.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_CZ80__
#define __ZX81_CZ80__

#include <FZ80/CZ80.hpp>
#include <CORE/OBool.hpp>

namespace ZX81
{
	class ULA;

	/** Z80 wiring shared by the ZX80 and ZX81 models. */
	class CZ80 final : public FZ80::CZ80
	{
		public:
		CZ80 (int id, const FZ80::Z80PortsMap& pts = { })
			: FZ80::CZ80 (id, pts),
			  _ula (nullptr),
			  _instructionCompleted (false),
			  _interruptCompleted (false),
			  _nmiWaitCycles (0)
							{ }

		/** Non-owning connection established by the computer before execution. */
		void linkToULA (ULA* u)
							{ _ula = u; }

		/** Returns and clears the latest instruction-completion indication. */
		bool takeInstructionCompleted () const
							{ return (_instructionCompleted); }
		/** Returns and clears the latest interrupt-response completion indication. */
		bool takeInterruptCompleted () const
							{ return (_interruptCompleted); }

		virtual bool initialize () override;

		protected:
		/** Preserves Z80 acknowledge handling, records INT and times HALT/NMI synchronization. */
		virtual void aknowledgeInterrupt
			(const MCHEmul::CPUInterruptRequest& iR) override;

		/** Completion is recorded only after the transaction has finished successfully. */
		virtual bool executeNextInterruptRequest_PerCycle (unsigned int& e) override;
		virtual bool executeNextInterruptRequest_Full (unsigned int& e) override;
		virtual bool executeNextInstruction_PerCycle (unsigned int& e) override;
		virtual bool executeNextInstruction_Full (unsigned int& e) override;

		private:
		//-----
		/** Records the decision before interrupt execution changes HALT or PC. */
		void debugNMISynchronization (const MCHEmul::CPUInterruptRequest& iR,
			bool halted, unsigned int nominalCycles) const;
		//-----

		/** The computer owns the ULA. */
		ULA* _ula;
		/** These indicators describe the latest completed transaction, not an event queue. */
		MCHEmul::OBool _instructionCompleted;
		MCHEmul::OBool _interruptCompleted;
		/** Calculated at acceptance and consumed once by the selected execution mode. */
		unsigned int _nmiWaitCycles;
	};
}

#endif

// End of the file
/*@}*/
