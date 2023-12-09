/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Instruction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description:	The type of instruction used by FZ80 family.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INSTRUCTION__
#define __FZX80_INSTRUCTION__

#include <CORE/incs.hpp>
#include <FZ80/CZ80.hpp>

namespace FZ80
{
	/** The global definition of a Z80 instruction. */
	class Instruction : public MCHEmul::InstructionDefined
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: MCHEmul::InstructionDefined (c, mp, cc, t, false)
							{ }

		// To get the reference to registers...
		MCHEmul::Register& registerA ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> aRegister ()); }
		const MCHEmul::Register& registerA () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> aRegister ()); }
		MCHEmul::Register& registerF ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> fRegister ()); }
		const MCHEmul::Register& registerF () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> fRegister ()); }
		MCHEmul::RefRegisters& registerAF ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> afRegister ()); }
		const MCHEmul::RefRegisters& registerAF () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> afRegister ()); }
		MCHEmul::RefRegisters& registerAFP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> afpRegister ()); }
		const MCHEmul::RefRegisters& registerAFP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> afpRegister ()); }

		MCHEmul::Register& registerB ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bRegister ()); }
		const MCHEmul::Register& registerB () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bRegister ()); }
		MCHEmul::Register& registerC ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> cRegister ()); }
		const MCHEmul::Register& registerC () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> cRegister ()); }
		MCHEmul::RefRegisters& registerBC ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bcRegister ()); }
		const MCHEmul::RefRegisters& registerBC () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bcRegister ()); }
		MCHEmul::RefRegisters& registerBCP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bcpRegister ()); }
		const MCHEmul::RefRegisters& registerBCP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bcpRegister ()); }

		MCHEmul::Register& registerD ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> dRegister ()); }
		const MCHEmul::Register& registerD () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> dRegister ()); }
		MCHEmul::Register& registerE ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> eRegister ()); }
		const MCHEmul::Register& registerE () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> eRegister ()); }
		MCHEmul::RefRegisters& registerDE ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> deRegister ()); }
		const MCHEmul::RefRegisters& registerDE () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> deRegister ()); }
		MCHEmul::RefRegisters& registerDEP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> depRegister ()); }
		const MCHEmul::RefRegisters& registerDEP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> depRegister ()); }

		MCHEmul::Register& registerH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hRegister ()); }
		const MCHEmul::Register& registerH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hRegister ()); }
		MCHEmul::Register& registerL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> lRegister ()); }
		const MCHEmul::Register& registerL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> lRegister ()); }
		MCHEmul::RefRegisters& registerHL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hlRegister ()); }
		const MCHEmul::RefRegisters& registerHL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hlRegister ()); }
		MCHEmul::RefRegisters& registerHLP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hlpRegister ()); }
		const MCHEmul::RefRegisters& registerHLP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hlpRegister ()); }

		MCHEmul::Register& registerI ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iRegister ()); }
		const MCHEmul::Register& registerI () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iRegister ()); }
		MCHEmul::Register& registerR ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> rRegister ()); }
		const MCHEmul::Register& registerR () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> rRegister ()); }

		MCHEmul::Register& registerIXH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixhRegister ()); }
		const MCHEmul::Register& registerIXH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixhRegister ()); }
		MCHEmul::Register& registerIXL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixlRegister ()); }
		const MCHEmul::Register& registerIXL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixlRegister ()); }
		MCHEmul::RefRegisters& registerIX ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixRegister ()); }
		const MCHEmul::RefRegisters& registerIX () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixRegister ()); }

		MCHEmul::Register& registerIYH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iyhRegister ()); }
		const MCHEmul::Register& registerIYH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iyhRegister ()); }
		MCHEmul::Register& registerIYL () 
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iylRegister ()); }
		const MCHEmul::Register& registerIYL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iylRegister ()); }
		MCHEmul::RefRegisters& registerIY ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iyRegister ()); }
		const MCHEmul::RefRegisters& registerIY () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iyRegister ()); }

		// To get the value of the registers...
		const MCHEmul::UByte& valueRegisterA () const
							{ return (registerA ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterF () const
							{ return (registerF ().values ()[0]); }
		MCHEmul::UBytes valueRegisterAF () const
							{ return (MCHEmul::UBytes ({ valueRegisterA (), valueRegisterF () })); }

		const MCHEmul::UByte& valueRegisterB () const
							{ return (registerB ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterC () const
							{ return (registerC ().values ()[0]); }
		MCHEmul::UBytes valueRegisterBC () const
						{ return (MCHEmul::UBytes ({ valueRegisterB (), valueRegisterC () })); }

		const MCHEmul::UByte& valueRegisterD () const
							{ return (registerD ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterE () const
							{ return (registerE ().values ()[0]); }
		MCHEmul::UBytes valueRegisterDE () const
							{ return (MCHEmul::UBytes ({ valueRegisterD (), valueRegisterE () })); }

		const MCHEmul::UByte& valueRegisterH () const
							{ return (registerH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterL () const
							{ return (registerL ().values () [0]); }
		MCHEmul::UBytes valueRegisterHL () const
							{ return (MCHEmul::UBytes ({ valueRegisterH (), valueRegisterL () })); }
		inline MCHEmul::UBytes valueRegisterSP () const;

		const MCHEmul::UByte& valueRegisterI () const
							{ return (registerI ().values () [0]); }
		const MCHEmul::UByte& valueRegisterR () const
							{ return (registerR ().values () [0]); }

		const MCHEmul::UByte& valueRegisterIXH () const
							{ return (registerIXH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterIXL () const
							{ return (registerIXL ().values () [0]); }
		MCHEmul::UBytes valueRegisterIX () const
							{ return (MCHEmul::UBytes ({ valueRegisterIXH (), valueRegisterIXL () })); }
		const MCHEmul::UByte& valueRegisterIYH () const
							{ return (registerIYH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterIYL () const
							{ return (registerIYL ().values () [0]); }
		MCHEmul::UBytes valueRegisterIY () const
							{ return (MCHEmul::UBytes ({ valueRegisterIYH (), valueRegisterIYL () })); }

		// To get the address pointed by the register...
		MCHEmul::Address addressBC (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterBC (), true /** Addresses in Registers are big - endian. */) + n); }
		MCHEmul::Address addressDE (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterDE (), true) + n); }
		MCHEmul::Address addressHL (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterHL (), true) + n); }
		MCHEmul::Address addressIX (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterIX (), true) + n); }
		MCHEmul::Address addressIY (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterIY (), true) + n); }
		MCHEmul::Address addressSP (size_t n = 0) const
							{ return (MCHEmul::Address (valueRegisterSP (), true) + n); }

		// To get the value pointed by the registers...
		const MCHEmul::UByte& valueAddressBC (size_t n = 0) const
							{ return (memory () -> value (addressBC (n))); }
		const MCHEmul::UByte& valueAddressDE (size_t n = 0) const
							{ return (memory () -> value (addressDE (n))); }
		const MCHEmul::UByte& valueAddressHL (size_t n = 0) const
							{ return (memory () -> value (addressHL (n))); }
		const MCHEmul::UByte& valueAddressIX (size_t n = 0) const
							{ return (memory () -> value (addressIX (n))); }
		const MCHEmul::UByte& valueAddressIY (size_t n = 0) const
							{ return (memory () -> value (addressIY (n))); }
		const MCHEmul::UByte& valueAddressSP (size_t n = 0) const
							{ return (memory () -> value (addressSP (n))); }
	};

	// ---
	inline MCHEmul::UBytes Instruction::valueRegisterSP () const
	{ 
		MCHEmul::UInt spP = MCHEmul::UInt::fromInt (memory () -> stack () -> position ());
		spP.setMinLength (2); 
							 
		return (spP.bytes ()); 
	}

	/** In Z80 there are instructions that are undefined reading the first byte.
		It is needed to read more bytes to know what the final instruction is. */
	class InstructionUndefined : public MCHEmul::InstructionUndefined
	{
		public:
		/** c = the code that all instructions shared. */
		InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst);

		protected:
		// Implementation
		/** To speed up the access to the list of instructions later. */
		MCHEmul::ListOfInstructions _rawInstructions;
	};

	/** Instruction code in the second byte in memory next to program counter location. 
		This is only useful for execution purposes. */
	class Byte2InstructionCode final : public InstructionUndefined
	{
		public:
		Byte2InstructionCode (unsigned int c, const MCHEmul::Instructions& inst)
			: InstructionUndefined (c, inst)
							{ }

		private:
		virtual MCHEmul::Instruction* selectInstruction (MCHEmul::CPU* c, MCHEmul::Memory* m, 
			MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc) override
							{ // The code is given by the second byte next to program counter in the memory...
							  return (_rawInstructions [size_t (m -> values (pc -> asAddress (), 2)[1].value ())]); }
	};

	/** Instruction code in the fourth byte in memory next to program counter location. 
		This is only useful for execution purposes. */
	class Byte4InstructionCode final : public InstructionUndefined
	{
		public:
		Byte4InstructionCode (unsigned int c, const MCHEmul::Instructions& inst)
			: InstructionUndefined (c, inst)
							{ }

		private:
		virtual MCHEmul::Instruction* selectInstruction (MCHEmul::CPU* c, MCHEmul::Memory* m, 
			MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc) override
							{ // The code is given by the second byte next to program counter in the memory...
							  return (_rawInstructions [size_t (m -> values (pc -> asAddress (), 4)[3].value ())]); }
	};
}

#endif
  
// End of the file
/*@}*/
