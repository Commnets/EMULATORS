/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA Chip.
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIA__
#define __COMMODORE_VIA__

#include <CORE/incs.hpp>
#include <COMMODORE/VIA/VIARegisters.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>
#include <COMMODORE/VIA/VIAPort.hpp>

namespace COMMODORE
{
	/** Reusable VIA with machine-supplied register bank and interrupt routing. \n
		CPU-timed operation predicts the read point from instruction bus metadata,
		advances internal activity before ordinary CPU reads, then completes that cycle
		after those reads and replays buffered writes at their positions in the remaining cycles. \n
		This complete-instruction model does not provide independent timing for every
		read in one instruction, timestamped external inputs or global cross-chip interleaving. */
	class VIA : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 107;

		/** Receives the id of the chip in the computer, the id of the registers linked
			and the id of the interruption to launch when needed. \n
			In a computer there could be many of these with little differences. */
		VIA (int id, int rId, unsigned int intId);

		/** To allow the external conexions. */
		const MCHEmul::Wire& CA1 () const
							{ return (_CA1.wire ()); }
		MCHEmul::Wire& CA1 ()
							{ return (_CA1.wire ()); }
		const MCHEmul::Wire& CA2 () const
							{ return (_CA2.wire ()); }
		MCHEmul::Wire& CA2 ()
							{ return (_CA2.wire ()); }
		const MCHEmul::Wire& CB1 () const
							{ return (_CB1.wire ()); }
		MCHEmul::Wire& CB1 ()
							{ return (_CB1.wire ()); }
		const MCHEmul::Wire& CB2 () const
							{ return (_CB2.wire ()); }
		MCHEmul::Wire& CB2 ()
							{ return (_CB2.wire ()); }

		/** To allow external conexions. */
		const MCHEmul::Bus& portA () const
							{ return (_PA.bus ()); }
		MCHEmul::Bus& portA ()
							{ return (_PA.bus ()); }
		const MCHEmul::Bus& portB () const
							{ return (_PB.bus ()); }
		MCHEmul::Bus& portB ()
							{ return (_PB.bus ()); }
		void setPortAExternalPins (const MCHEmul::UByte& v)
							{ _PA.setExternalPins (v); }
		void setPortBExternalPins (const MCHEmul::UByte& v)
							{ _PB.setExternalPins (v); }

		/** Enables buffered CPU writes and advance to the instruction read point. \n
			The CPU must execute complete instructions. \n
			Call after initializing the CPU and VIA. */
		bool enableCPUTimedWrites (MCHEmul::CPU* cpu);
		virtual void CPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt) override;
		virtual void CPUAboutToExecute
			(const MCHEmul::InterruptContextEventData* dt) override;

		virtual bool initialize () override;

		/** Completes any advanced access cycle, applies buffered writes and catches up to the CPU clock. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones coming from the parent class and: \n
		  * Registers		= InfoStructure: Value of the registers. \n
		  *	VIATimer1		= InfoStructure: Timer 1 information. \n
		  *	VIATimer2		= InfoStructure: Timer 2 information. \n
		  *	VIAShiftReg		= InfoStructure: Shift register information. \n
		  *	PortA			= InfoStructure: Port A registers and pin values. \n
		  *	PortB			= InfoStructure: Port B registers and pin values. \n
		  * VIACtrlA1		= InfoStructure: Control line CA1 information. \n
		  * VIACtrlA2		= InfoStructure: Control line CA2 information. \n
		  * VIACtrlB1		= InfoStructure: Control line CB1 information. \n
		  * VIACtrlB2		= InfoStructure: Control line CB2 information.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void prepareCPUTransaction
			(MCHEmul::CPU* cpu, const MCHEmul::BusCycleData* busData);
		bool extractPendingRegisterWrites ();
		bool executePendingRegisterWriteAt (unsigned int cycle);
		/** Advances explicit cycles; the last awaits CPU-access completion. */
		bool simulate (MCHEmul::CPU* cpu, unsigned int cycles);
		bool simulateVIACycle
			(MCHEmul::CPU* cpu, unsigned int cycle, bool completeCycle);
		bool finishVIACycle
			(MCHEmul::CPU* cpu, unsigned int cycle, bool afterCPURead);

		protected:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugVIACycle (unsigned int cycle);
		// -----

		protected:
		const int _registersId;
		VIARegisters* _VIARegisters;
		unsigned int _interruptId;
		// The different elements of the VIA Chip...
		VIAControlLineType1 _CA1, _CB1;
		VIAControlLineType2 _CA2, _CB2;
		VIATimer1 _T1;
		VIATimer2 _T2;
		VIAShiftRegister _SR;
		VIAPortA _PA;
		VIAPortB _PB;

		// Implementation
		/** First cycle whose internal activity has not been processed. */
		unsigned int _lastClockCycles;
		/** Whether the VIA interrupt output was asserted in the previous cycle. */
		bool _interruptOutputActive;

		private:
		/** Non-owning reference; nullptr preserves direct-access operation. */
		MCHEmul::CPU* _timedWriteCPU;
		/** Non-owning reference to immutable bus data of the current transaction. */
		const MCHEmul::BusCycleData* _writeBusCycleData;
		unsigned int _writeStartCycle;
		MCHEmul::SetMemoryCommands _pendingRegisterWrites;
		/** Internal cycles already advanced for the current instruction. */
		unsigned int _cyclesSimulatedInAdvance;
		/** The last advanced cycle still needs post-access completion. */
		bool _advancedCyclePending;
	};
}

#endif
  
// End of the file
/*@}*/
