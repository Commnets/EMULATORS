/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Instructions.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included. \n
 *				 Only the stable ones. \n
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INSTRUCTIONS__
#define __FZX80_INSTRUCTIONS__

#include <CORE/incs.hpp>
#include <FZ80/CZ80.hpp>

namespace FZ80
{
	class Instruction : public MCHEmul::InstructionDefined
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: MCHEmul::InstructionDefined (c, mp, cc, t, false)
							{ }

		// To get the reference to the registers...
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

	/** Instruction code in the second byte in memory next to program counter location. */
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

	/** Instruction code in the fourth byte in memory next to program counter location. */
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

	// ----------
	/** LD_General: To load any register / memory location with a value. */
	class LD_General : public Instruction
	{
		public:
		LD_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// LD can be used both to load register or memory positions (in other micros this is a storage command)
		// None of these instructions affect the status register!...
		/** To load a single register with a single byte. */
		bool executeWith (MCHEmul::Register& r, const MCHEmul::UByte& u)
							{ r.set ({ u }); return (true); }
		/** To load a extended register with two bytes. 
			It is supossed that the bytes come as they are in memory, so that's it in little - endian order. */
		bool executeWith (MCHEmul::RefRegisters& r, const MCHEmul::UBytes& u)
							{ r [0] -> set ({ u [1] }); r [1] -> set ({ u [0] }); return (true); }
		/** To load a memory position with the value of a byte. */
		bool executeWith (const MCHEmul::Address& a, const MCHEmul::UByte& u)
							{ memory () -> set (a, { u }); return (true); }
		/** To load a memory position with two bytes. \n
			It is supossed that the bytes come as they are in memory, so that's it in litlee endian order. */
		bool executeWith (const MCHEmul::Address& a, const MCHEmul::UBytes& u)
							{ memory () -> set (a, { u [1], u [0] }); return (true); }
	};

	// Target Main Registers
	// Target A
	_INST_FROM (0x3E,	2, 7, 7,	"LD A,[#1]",			LD_A, LD_General);					// Also undocumented with codes: DD3E & FD3E
	_INST_FROM (0x7F,	1, 4, 4,	"LD A,A",				LD_AFromA, LD_General);				// Also undocumented with codes: DD7F & FD7F
	_INST_FROM (0x78,	1, 4, 4,	"LD A,B",				LD_AFromB, LD_General);				// Also undocumented with codes: DD78 & FD78
	_INST_FROM (0x79,	1, 4, 4,	"LD A,C",				LD_AFromC, LD_General);				// Also undocumented with codes: DD79 & FD79
	_INST_FROM (0x7A,	1, 4, 4,	"LD A,D",				LD_AFromD, LD_General);				// Also undocumented with codes: DD7A & FD7A
	_INST_FROM (0x7B,	1, 4, 4,	"LD A,E",				LD_AFromE, LD_General);				// Also undocumented with codes: DD7B & FD7B
	_INST_FROM (0x7C,	1, 4, 4,	"LD A,H",				LD_AFromH, LD_General);
	_INST_FROM (0x7D,	1, 4, 4,	"LD A,L",				LD_AFromL, LD_General);
	_INST_FROM (0xED57, 2, 9, 9,	"LD A,I",				LD_AFromI, LD_General);
	_INST_FROM (0xED5F, 2, 9, 9,	"LD A,R",				LD_AFromR, LD_General);
	_INST_FROM (0x7E,	1, 7, 7,	"LD A,(HL)",			LD_AFromAddressHL, LD_General);
	_INST_FROM (0x0A,	1, 7, 7,	"LD A,(BC)",			LD_AFromAddressBC, LD_General);
	_INST_FROM (0x1A,	1, 7, 7,	"LD A,(DE)",			LD_AFromAddressDE, LD_General);
	_INST_FROM (0xDD7E,	3, 19, 19,	"LD A,(IX+[#1])",		LD_AFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD7E,	3, 19, 19,	"LD A,(IY+[#1])",		LD_AFromAddressIndexIY, LD_General);
	_INST_FROM (0x3A,	3, 13, 13,	"LD A,([$2])",			LD_AFromAddress, LD_General);
	_INST_FROM (0xDD7C,	2, 8, 8,	"LD A,IXH",				LD_AFromIXH, LD_General);			// Undocumented
	_INST_FROM (0xDD7D,	2, 8, 8,	"LD A,IXL",				LD_AFromIXL, LD_General);			// Undocumented
	_INST_FROM (0xFD7C,	2, 8, 8,	"LD A,IYH",				LD_AFromIYH, LD_General);			// Undocumented
	_INST_FROM (0xFD7D,	2, 8, 8,	"LD A,IYL",				LD_AFromIYL, LD_General);			// Undocumented

	// Target B
	_INST_FROM (0x06,	2, 7, 7,	"LD B,[#1]",			LD_B, LD_General);					// Also undocumented with codes: DD06 & FD06
	_INST_FROM (0x47,	1, 4, 4,	"LD B,A",				LD_BFromA, LD_General);				// Also undocumented with codes: DD47 & FD47
	_INST_FROM (0x40,	1, 4, 4,	"LD B,B",				LD_BFromB, LD_General);				// Also undocumented with codes: DD40 & FD40
	_INST_FROM (0x41,	1, 4, 4,	"LD B,C",				LD_BFromC, LD_General);				// Also undocumented with codes: DD41 & FD41
	_INST_FROM (0x42,	1, 4, 4,	"LD B,D",				LD_BFromD, LD_General);				// Also undocumented with codes: DD42 & FD42
	_INST_FROM (0x43,	1, 4, 4,	"LD B,E",				LD_BFromE, LD_General);				// Also undocumented with codes: DD43 & FD43
	_INST_FROM (0x44,	1, 4, 4,	"LD B,H",				LD_BFromH, LD_General);
	_INST_FROM (0x45,	1, 4, 4,	"LD B,L",				LD_BFromL, LD_General);
	_INST_FROM (0x46,	1, 7, 7,	"LD B,(HL)",			LD_BFromAddressHL, LD_General);
	_INST_FROM (0xDD46,	3, 19, 19,	"LD B,(IX+[#1])",		LD_BFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD46,	3, 19, 19,	"LD B,(IY+[#1])",		LD_BFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD44,	2, 8, 8,	"LD B,IXH",				LD_BFromIXH, LD_General);			// Undocumented
	_INST_FROM (0xDD45,	2, 8, 8,	"LD B,IXL",				LD_BFromIXL, LD_General);			// Undocumented
	_INST_FROM (0xFD44,	2, 8, 8,	"LD B,IYH",				LD_BFromIYH, LD_General);			// Undocumented
	_INST_FROM (0xFD45,	2, 8, 8,	"LD B,IYL",				LD_BFromIYL, LD_General);			// Undocumented

	// Target C
	_INST_FROM (0x0E,	2, 7, 7,	"LD C,[#1]",			LD_C, LD_General);					// Also undocumented with codes: DD0E & FD0E
	_INST_FROM (0x4F,	1, 4, 4,	"LD C,A",				LD_CFromA, LD_General);				// Also undocumented with codes: DD4F & FD4F
	_INST_FROM (0x48,	1, 4, 4,	"LD C,B",				LD_CFromB, LD_General);				// Also undocumented with codes: DD48 & FD48
	_INST_FROM (0x49,	1, 4, 4,	"LD C,C",				LD_CFromC, LD_General);				// Also undocumented with codes: DD49 & FD49
	_INST_FROM (0x4A,	1, 4, 4,	"LD C,D",				LD_CFromD, LD_General);				// Also undocumented with codes: DD4A & FD4A
	_INST_FROM (0x4B,	1, 4, 4,	"LD C,E",				LD_CFromE, LD_General);				// Also undocumented with codes: DD4B & FD4B
	_INST_FROM (0x4C,	1, 4, 4,	"LD C,H",				LD_CFromH, LD_General);
	_INST_FROM (0x4D,	1, 4, 4,	"LD C,L",				LD_CFromL, LD_General);
	_INST_FROM (0x4E,	1, 7, 7,	"LD C,(HL)",			LD_CFromAddressHL, LD_General);
	_INST_FROM (0xDD4E,	3, 19, 19,	"LD C,(IX+[#1])",		LD_CFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD4E,	3, 19, 19,	"LD C,(IY+[#1])",		LD_CFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD4C,	2, 8, 8,	"LD C,IXH",				LD_CFromIXH, LD_General);			// Undocumented
	_INST_FROM (0xDD4D,	2, 8, 8,	"LD C,IXL",				LD_CFromIXL, LD_General);			// Undocumented
	_INST_FROM (0xFD4C,	2, 8, 8,	"LD C,IYH",				LD_CFromIYH, LD_General);			// Undocumented
	_INST_FROM (0xFD4D,	2, 8, 8,	"LD C,IYL",				LD_CFromIYL, LD_General);			// Undocumented

	// Target D
	_INST_FROM (0x16,	2, 7, 7,	"LD D,[#1]",			LD_D, LD_General);					// Also undocumented with codes: DD16 & FD16
	_INST_FROM (0x57,	1, 4, 4,	"LD D,A",				LD_DFromA, LD_General);				// Also undocumented with codes: DD57 & FD57
	_INST_FROM (0x50,	1, 4, 4,	"LD D,B",				LD_DFromB, LD_General);				// Also undocumented with codes: DD50 & FD50
	_INST_FROM (0x51,	1, 4, 4,	"LD D,C",				LD_DFromC, LD_General);				// Also undocumented with codes: DD51 & FD51
	_INST_FROM (0x52,	1, 4, 4,	"LD D,D",				LD_DFromD, LD_General);				// Also undocumented with codes: DD52 & FD52
	_INST_FROM (0x53,	1, 4, 4,	"LD D,E",				LD_DFromE, LD_General);				// Also undocumented with codes: DD53 & FD53
	_INST_FROM (0x54,	1, 4, 4,	"LD D,H",				LD_DFromH, LD_General);
	_INST_FROM (0x55,	1, 4, 4,	"LD D,L",				LD_DFromL, LD_General);
	_INST_FROM (0x56,	1, 7, 7,	"LD D,(HL)",			LD_DFromAddressHL, LD_General);
	_INST_FROM (0xDD56,	3, 19, 19,	"LD D,(IX+[#1])",		LD_DFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD56,	3, 19, 19,	"LD D,(IY+[#1])",		LD_DFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD54,	2, 8, 8,	"LD D,IXH",				LD_DFromIXH, LD_General);			// Undocumented
	_INST_FROM (0xDD55,	2, 8, 8,	"LD D,IXL",				LD_DFromIXL, LD_General);			// Undocumented
	_INST_FROM (0xFD54,	2, 8, 8,	"LD D,IYH",				LD_DFromIYH, LD_General);			// Undocumented
	_INST_FROM (0xFD55,	2, 8, 8,	"LD D,IYL",				LD_DFromIYL, LD_General);			// Undocumented

	// Target E
	_INST_FROM (0x1E,	2, 7, 7,	"LD E,[#1]",			LD_E, LD_General);					// Also undocumented with codes: DD1E & FD1E
	_INST_FROM (0x5F,	1, 4, 4,	"LD E,A",				LD_EFromA, LD_General);				// Also undocumented with codes: DD5F & FD5F
	_INST_FROM (0x58,	1, 4, 4,	"LD E,B",				LD_EFromB, LD_General);				// Also undocumented with codes: DD58 & FD58
	_INST_FROM (0x59,	1, 4, 4,	"LD E,C",				LD_EFromC, LD_General);				// Also undocumented with codes: DD59 & FD59
	_INST_FROM (0x5A,	1, 4, 4,	"LD E,D",				LD_EFromD, LD_General);				// Also undocumented with codes: DD5A & FD5A
	_INST_FROM (0x5B,	1, 4, 4,	"LD E,E",				LD_EFromE, LD_General);				// Also undocumented with codes: DD5B & FD5B
	_INST_FROM (0x5C,	1, 4, 4,	"LD E,H",				LD_EFromH, LD_General);
	_INST_FROM (0x5D,	1, 4, 4,	"LD E,L",				LD_EFromL, LD_General);
	_INST_FROM (0x5E,	1, 7, 7,	"LD E,(HL)",			LD_EFromAddressHL, LD_General);
	_INST_FROM (0xDD5E,	3, 19, 19,	"LD E,(IX+[#1])",		LD_EFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD5E,	3, 19, 19,	"LD E,(IY+[#1])",		LD_EFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD5C,	2, 8, 8,	"LD E,IXH",				LD_EFromIXH, LD_General);			// Undocumented
	_INST_FROM (0xDD5D,	2, 8, 8,	"LD E,IXL",				LD_EFromIXL, LD_General);			// Undocumented
	_INST_FROM (0xFD5C,	2, 8, 8,	"LD E,IYH",				LD_EFromIYH, LD_General);			// Undocumented
	_INST_FROM (0xFD5D,	2, 8, 8,	"LD E,IYL",				LD_EFromIYL, LD_General);			// Undocumented

	// Target H
	_INST_FROM (0x26,	2, 7, 7,	"LD H,[#1]",			LD_H, LD_General);
	_INST_FROM (0x67,	1, 4, 4,	"LD H,A",				LD_HFromA, LD_General);
	_INST_FROM (0x60,	1, 4, 4,	"LD H,B",				LD_HFromB, LD_General);
	_INST_FROM (0x61,	1, 4, 4,	"LD H,C",				LD_HFromC, LD_General);
	_INST_FROM (0x62,	1, 4, 4,	"LD H,D",				LD_HFromD, LD_General);
	_INST_FROM (0x63,	1, 4, 4,	"LD H,E",				LD_HFromE, LD_General);
	_INST_FROM (0x64,	1, 4, 4,	"LD H,H",				LD_HFromH, LD_General);
	_INST_FROM (0x65,	1, 4, 4,	"LD H,L",				LD_HFromL, LD_General);
	_INST_FROM (0x66,	1, 7, 7,	"LD H,(HL)",			LD_HFromAddressHL, LD_General);
	_INST_FROM (0xDD66,	3, 19, 19,	"LD H,(IX+[#1])",		LD_HFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD66,	3, 19, 19,	"LD H,(IY+[#1])",		LD_HFromAddressIndexIY, LD_General);

	// Target L
	_INST_FROM (0x2E,	2, 7, 7,	"LD L,[#1]",			LD_L, LD_General);
	_INST_FROM (0x6F,	1, 4, 4,	"LD L,A",				LD_LFromA, LD_General);
	_INST_FROM (0x68,	1, 4, 4,	"LD L,B",				LD_LFromB, LD_General);
	_INST_FROM (0x69,	1, 4, 4,	"LD L,C",				LD_LFromC, LD_General);
	_INST_FROM (0x6A,	1, 4, 4,	"LD L,D",				LD_LFromD, LD_General);
	_INST_FROM (0x6B,	1, 4, 4,	"LD L,E",				LD_LFromE, LD_General);
	_INST_FROM (0x6C,	1, 4, 4,	"LD L,H",				LD_LFromH, LD_General);
	_INST_FROM (0x6D,	1, 4, 4,	"LD L,L",				LD_LFromL, LD_General);
	_INST_FROM (0x6E,	1, 7, 7,	"LD L,(HL)",			LD_LFromAddressHL, LD_General);
	_INST_FROM (0xDD6E,	3, 19, 19,	"LD L,(IX+[#1])",		LD_LFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD6E,	3, 19, 19,	"LD L,(IY+[#1])",		LD_LFromAddressIndexIY, LD_General);

	// Target (HL)
	_INST_FROM (0x36,	2, 10, 10,	"LD (HL),[#1]",			LD_IndirectHL, LD_General);
	_INST_FROM (0x77,	1, 7, 7,	"LD (HL),A",			LD_IndirectHLFromA, LD_General);
	_INST_FROM (0x70,	1, 7, 7,	"LD (HL),B",			LD_IndirectHLFromB, LD_General);
	_INST_FROM (0x71,	1, 7, 7,	"LD (HL),C",			LD_IndirectHLFromC, LD_General);
	_INST_FROM (0x72,	1, 7, 7,	"LD (HL),D",			LD_IndirectHLFromD, LD_General);
	_INST_FROM (0x73,	1, 7, 7,	"LD (HL),E",			LD_IndirectHLFromE, LD_General);
	_INST_FROM (0x74,	1, 7, 7,	"LD (HL),F",			LD_IndirectHLFromF, LD_General);
	_INST_FROM (0x75,	1, 7, 7,	"LD (HL),L",			LD_IndirectHLFromL, LD_General);

	// Target (BC)
	_INST_FROM (0x02,	1, 7, 7,	"LD (BC),A",			LD_IndirectBCFromA, LD_General);

	// Target (DE)
	_INST_FROM (0x12,	1, 7, 7,	"LD (DE),A",			LD_IndirectDEFromA, LD_General);

	// Target (IX + d)
	_INST_FROM (0xDD36,	4, 19, 19,	"LD (IX+[#1]),[#1]",	LD_IndirectIndexIX, LD_General);
	_INST_FROM (0xDD77,	3, 19, 19,	"LD (IX+[#1]),A",		LD_IndirectIndexIXFromA, LD_General);
	_INST_FROM (0xDD70,	3, 19, 19,	"LD (IX+[#1]),B",		LD_IndirectIndexIXFromB, LD_General);
	_INST_FROM (0xDD71,	3, 19, 19,	"LD (IX+[#1]),C",		LD_IndirectIndexIXFromC, LD_General);
	_INST_FROM (0xDD72,	3, 19, 19,	"LD (IX+[#1]),D",		LD_IndirectIndexIXFromD, LD_General);
	_INST_FROM (0xDD73,	3, 19, 19,	"LD (IX+[#1]),E",		LD_IndirectIndexIXFromE, LD_General);
	_INST_FROM (0xDD74,	3, 19, 19,	"LD (IX+[#1]),F",		LD_IndirectIndexIXFromF, LD_General);
	_INST_FROM (0xDD75,	3, 19, 19,	"LD (IX+[#1]),L",		LD_IndirectIndexIXFromL, LD_General);

	// Target (IX + d)
	_INST_FROM (0xFD36,	4, 19, 19,	"LD (IY+[#1]),[#1]",	LD_IndirectIndexIY, LD_General);
	_INST_FROM (0xFD77,	3, 19, 19,	"LD (IY+[#1]),A",		LD_IndirectIndexIYFromA, LD_General);
	_INST_FROM (0xFD70,	3, 19, 19,	"LD (IY+[#1]),B",		LD_IndirectIndexIYFromB, LD_General);
	_INST_FROM (0xFD71,	3, 19, 19,	"LD (IY+[#1]),C",		LD_IndirectIndexIYFromC, LD_General);
	_INST_FROM (0xFD72,	3, 19, 19,	"LD (IY+[#1]),D",		LD_IndirectIndexIYFromD, LD_General);
	_INST_FROM (0xFD73,	3, 19, 19,	"LD (IY+[#1]),E",		LD_IndirectIndexIYFromE, LD_General);
	_INST_FROM (0xFD74,	3, 19, 19,	"LD (IY+[#1]),F",		LD_IndirectIndexIYFromF, LD_General);
	_INST_FROM (0xFD75,	3, 19, 19,	"LD (IY+[#1]),L",		LD_IndirectIndexIYFromL, LD_General);

	// Target (Address)
	_INST_FROM (0x32,	3, 13, 13,	"LD ([#2]),A",			LD_AddressFromA, LD_General);
	_INST_FROM (0x22,	3, 16, 16,	"LD ([#2]),HL",			LD_AddressFromHL, LD_General);			// Also undocumented with code: ED63
	_INST_FROM (0xDD22,	4, 20, 20,	"LD ([#2]),IX",			LD_AddressFromIX, LD_General);
	_INST_FROM (0xFD22,	4, 20, 20,	"LD ([#2]),IY",			LD_AddressFromIY, LD_General);
	_INST_FROM (0xED43,	4, 20, 20,	"LD ([#2]),BC",			LD_AddressFromBC, LD_General);
	_INST_FROM (0xED53,	4, 20, 20,	"LD ([#2]),DE",			LD_AddressFromDE, LD_General);
	_INST_FROM (0xED73,	4, 20, 20,	"LD ([#2]),SP",			LD_AddressFromSP, LD_General);

	// Target I
	_INST_FROM (0xED47,	2, 9, 9,	"LD I,A",				LD_IFromA, LD_General);

	// Target R
	_INST_FROM (0xED4F,	2, 9, 9,	"LD R,A",				LD_RFromA, LD_General);

	// Target BC
	_INST_FROM (0x01,	3, 10, 10,	"LD BC,[#2]",			LD_BC, LD_General);
	_INST_FROM (0xED4B,	4, 20, 20,	"LD BC,([$2])",			LD_BCFromAddress, LD_General);

	// Target DE
	_INST_FROM (0x11,	3, 10, 10,	"LD DE,[#2]",			LD_DE, LD_General);
	_INST_FROM (0xED5B,	4, 20, 20,	"LD DE,([$2])",			LD_DEFromAddress, LD_General);

	// Target HL
	_INST_FROM (0x21,	3, 10, 10,	"LD HL,[#2]",			LD_HL, LD_General);
	_INST_FROM (0x2A,	3, 16, 16,	"LD HL,([$2])",			LD_HLFromAddress, LD_General);			// Also undocumented with code: ED6B

	// Target IX
	_INST_FROM (0xDD21,	4, 14, 14,	"LD IX,[#2]",			LD_IX, LD_General);
	_INST_FROM (0xDD2A,	4, 20, 20,	"LD IX,([$2])",			LD_IXFromAddress, LD_General);

	// Target IY
	_INST_FROM (0xFD21,	4, 14, 14,	"LD IY,[#2]",			LD_IY, LD_General);
	_INST_FROM (0xFD2A,	4, 20, 20,	"LD IY,([$2])",			LD_IYFromAddress, LD_General);

	// Target SP
	_INST_FROM (0x31,	3, 10, 10,	"LD SP,[#2]",			LD_SP, LD_General);
	_INST_FROM (0xF9,	1, 6, 6,	"LD SP,HL",				LD_SPFromHL, LD_General);
	_INST_FROM (0xDDF9,	2, 10, 10,	"LD SP,IX",				LD_SPFromIX, LD_General);
	_INST_FROM (0xFDF9,	2, 10, 10,	"LD SP,IY",				LD_SPFromIY, LD_General);
	_INST_FROM (0xED7B,	4, 10, 10,	"LD SP,([$2])",			LD_SPFromAddress, LD_General);
	// ----------

	// ----------
	/** PUSH_General: To move the content of a register into the stack. */
	class PUSH_General : public Instruction
	{
		public:
		PUSH_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** These intructions don't affect either to any flag. */
		bool executeWith (MCHEmul::RefRegisters& r);
	};

	// PUSH
	// From AF
	_INST_FROM (0xF5,	1, 11, 11,	"PUSH AF",				PUSH_AF, PUSH_General);
	// From BC
	_INST_FROM (0xC5,	1, 11, 11,	"PUSH BC",				PUSH_BC, PUSH_General);
	// From DE
	_INST_FROM (0xD5,	1, 11, 11,	"PUSH DE",				PUSH_DE, PUSH_General);
	// From HL
	_INST_FROM (0xE5,	1, 11, 11,	"PUSH HL",				PUSH_HL, PUSH_General);
	// From IX
	_INST_FROM (0xDDE5,	2, 15, 15,	"PUSH IX",				PUSH_IX, PUSH_General);
	// From IY
	_INST_FROM (0xFDE5,	2, 15, 15,	"PUSH IY",				PUSH_IY, PUSH_General);
	// ----------

	// ----------
	/** POP_General: To move the content of the stack to a register. */
	class POP_General : public Instruction
	{
		public:
		POP_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** These intructions don't affect either to any flag. */
		bool executeWith (MCHEmul::RefRegisters& r);
	};

	// PULL
	// To AF
	_INST_FROM (0xF1,	1, 10, 10,	"POP AF",				POP_AF, POP_General);
	// To BC
	_INST_FROM (0xC1,	1, 10, 10,	"POP BC",				POP_BC, POP_General);
	// To DE
	_INST_FROM (0xD1,	1, 10, 10,	"POP DE",				POP_DE, POP_General);
	// To HL
	_INST_FROM (0xE1,	1, 10, 10,	"POP HL",				POP_HL, POP_General);
	// To IX
	_INST_FROM (0xDDE1,	2, 14, 14,	"POP IX",				POP_IX, POP_General);
	// To IY
	_INST_FROM (0xFDE1,	2, 14, 14,	"POP IY",				POP_IY, POP_General);
	// ----------

	// ----------
	/** These instructions are used to interchange content between registers. 
		The registers affected are usually double registers. */
	class EX_General : public Instruction
	{
		public:
		EX_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** These instructions don't affect either to the registers. */
		bool executeWith (MCHEmul::RefRegisters& r, MCHEmul::RefRegisters& rp);
	};

	// AF and AF'
	_INST_FROM (0x08,	1, 4, 4,	"EX AF,AF'",			EX_AF, EX_General);
	// DE and HL
	_INST_FROM (0xEB,	1, 4, 4,	"EX DE,HL",				EX_DEWithHL, EX_General);
	// EXX (all)
	_INST_FROM (0xD9,	1, 4, 4,	"EXX",					EX_X, EX_General);
	// With SP
	_INST_FROM (0xE3,	1, 19, 19,	"EX (SP),HL",			EX_SPWithHL, EX_General);
	_INST_FROM (0xDDE3,	2, 23, 23,	"EX (SP),IX",			EX_SPWithIX, EX_General);
	_INST_FROM (0xFDE3,	2, 23, 23,	"EX (SP),IY",			EX_SPWithIY, EX_General);
	// ----------

	// ----------
	/** To move a block of data. */
	class InstBlock_General : public Instruction
	{
		public:
		InstBlock_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t),
			  _inExecution (false),
			  _bc0 (false)
							{ }

		protected:
		/** The parameter a idicates the quantity to move up or down. \n
			It has to be -1 or 1. */
		bool executeMoveWith (int a);
		bool executeCompareWith (int a);

		// Implementation
		/** True when it is in execution. */
		bool _inExecution;
		/** True when _bc0. */
		bool _bc0;
	};

	// (DE) <- (HL) & Next memory position
	_INST_FROM (0xEDA0,	2, 16, 16,	"LDI",					LDI, InstBlock_General);
	// (DE) <- (HL) & Next memory position until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INST_FROM (0xEDB0,	2, 16, 16,	"LDIR",					LDIR, InstBlock_General);
	// (DE) <- (HL) & Previous memory position
	_INST_FROM (0xEDA8,	2, 16, 16,	"LDD",					LDD, InstBlock_General);
	// (DE) <- (HL) & Previous memory position until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INST_FROM (0xEDB8,	2, 16, 16,	"LDDR",					LDDR, InstBlock_General);
	// A compared with (HL) & Next memory position
	_INST_FROM (0xEDA1,	2, 16, 16,	"CPI",					CPI, InstBlock_General);
	// A compared with (HL) & Next memory position until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INST_FROM (0xEDB1,	2, 16, 16,	"CPIR",					CPIR, InstBlock_General);
	// A compared with (HL) & Previous memory position
	_INST_FROM (0xEDA9,	2, 16, 16,	"CPD",					CPD, InstBlock_General);
	// A compared with (HL) & Previous memory position until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INST_FROM (0xEDB9,	2, 16, 16,	"CPDR",					CPDR, InstBlock_General);
	// ----------

	// ----------
	/** AND is always over the value of A. */
	class AND_General : public Instruction
	{
		public:
		AND_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xA7,	1, 4, 4,	"AND A",					AND_A, AND_General);				// Also undocumented with codes: DDA7 & FDA7 
	// With B
	_INST_FROM (0xA0,	1, 4, 4,	"AND B",					AND_B, AND_General);				// Also undocumented with codes: DDA0 & FDA0
	// With C
	_INST_FROM (0xA1,	1, 4, 4,	"AND C",					AND_C, AND_General);				// Also undocumented with codes: DDA1 & FDA1
	// With D
	_INST_FROM (0xA2,	1, 4, 4,	"AND D",					AND_D, AND_General);				// Also undocumented with codes: DDA2 & FDA2
	// With E
	_INST_FROM (0xA3,	1, 4, 4,	"AND E",					AND_E, AND_General);				// Also undocumented with codes: DDA3 & FDA3
	// With H
	_INST_FROM (0xA4,	1, 4, 4,	"AND H",					AND_H, AND_General);
	// With L
	_INST_FROM (0xA5,	1, 4, 4,	"AND L",					AND_L, AND_General);
	// With (HL)
	_INST_FROM (0xA6,	1, 7, 7,	"AND (HL)",					AND_IndirectHL, AND_General);
	// With (IX + d)
	_INST_FROM (0xDDA6, 3, 19, 19,	"AND (IX+[#1])",			AND_IndirectIndexIX, AND_General);
	// With (IX + d)
	_INST_FROM (0xFDA6,	3, 19, 19,	"AND (IY+[#1])",			AND_IndirectIndexIY, AND_General);
	// With Value
	_INST_FROM (0xE6,	2, 7, 7,	"AND [#1]",					AND, AND_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDA4,	2, 8, 8,	"AND IXH",					AND_IXH, AND_General);				// Undocumented
	_INST_FROM (0xDDA5,	2, 8, 8,	"AND IXL",					AND_IXL, AND_General);				// Undocumented
	_INST_FROM (0xFDA4,	2, 8, 8,	"AND IYH",					AND_IYH, AND_General);				// Undocumented
	_INST_FROM (0xFDA5,	2, 8, 8,	"AND IYL",					AND_IYL, AND_General);				// Undocumented
	// ----------

	// ----------
	/** OR is always over the value of A. */
	class OR_General : public Instruction
	{
		public:
		OR_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xB7,	1, 4, 4,	"OR A",					OR_A, OR_General);						// Also undocumented with codes: DDB7 & FDB7 
	// With B
	_INST_FROM (0xB0,	1, 4, 4,	"OR B",					OR_B, OR_General);						// Also undocumented with codes: DDB0 & FDB0
	// With C
	_INST_FROM (0xB1,	1, 4, 4,	"OR C",					OR_C, OR_General);						// Also undocumented with codes: DDB1 & FDB1
	// With D
	_INST_FROM (0xB2,	1, 4, 4,	"OR D",					OR_D, OR_General);						// Also undocumented with codes: DDB2 & FDB2
	// With E
	_INST_FROM (0xB3,	1, 4, 4,	"OR E",					OR_E, OR_General);						// Also undocumented with codes: DDB3 & FDB3
	// With H
	_INST_FROM (0xB4,	1, 4, 4,	"OR H",					OR_H, OR_General);
	// With L
	_INST_FROM (0xB5,	1, 4, 4,	"OR L",					OR_L, OR_General);
	// With (HL)
	_INST_FROM (0xB6,	1, 7, 7,	"OR (HL)",				OR_IndirectHL, OR_General);
	// With (IX + d)
	_INST_FROM (0xDDB6, 3, 19, 19,	"OR (IX+[#1])",			OR_IndirectIndexIX, OR_General);
	// With (IX + d)
	_INST_FROM (0xFDB6,	3, 19, 19,	"OR (IY+[#1])",			OR_IndirectIndexIY, OR_General);
	// With Value
	_INST_FROM (0xF6,	2, 7, 7,	"OR [#1]",				OR, OR_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDB4,	2, 8, 8,	"OR IXH",				OR_IXH, OR_General);					// Undocumented
	_INST_FROM (0xDDB5,	2, 8, 8,	"OR IXL",				OR_IXL, OR_General);					// Undocumented
	_INST_FROM (0xFDB4,	2, 8, 8,	"OR IYH",				OR_IYH, OR_General);					// Undocumented
	_INST_FROM (0xFDB5,	2, 8, 8,	"OR IYL",				OR_IYL, OR_General);					// Undocumented
	// ----------

	// ----------
	/** XOR is always over the value of A. */
	class XOR_General : public Instruction
	{
		public:
		XOR_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xAF,	1, 4, 4,	"XOR A",				XOR_A, XOR_General);					// Also undocumented with codes: DDAF & FDAF
	// WithB
	_INST_FROM (0xA8,	1, 4, 4,	"XOR B",				XOR_B, XOR_General);					// Also undocumented with codes: DDA8 & FDA8
	// Wit C
	_INST_FROM (0xA9,	1, 4, 4,	"XOR C",				XOR_C, XOR_General);					// Also undocumented with codes: DDA9 & FDA9
	// Wih D
	_INST_FROM (0xAA,	1, 4, 4,	"XOR D",				XOR_D, XOR_General);					// Also undocumented with codes: DDAA & FDAA
	// Wth E
	_INST_FROM (0xAB,	1, 4, 4,	"XOR E",				XOR_E, XOR_General);					// Also undocumented with codes: DDAB & FDAB
	// With H
	_INST_FROM (0xAC,	1, 4, 4,	"XOR H",				XOR_H, XOR_General);
	// With L
	_INST_FROM (0xAD,	1, 4, 4,	"XOR L",				XOR_L, XOR_General);
	// With (HL)
	_INST_FROM (0xAE,	1, 7, 7,	"XOR (HL)",				XOR_IndirectHL, XOR_General);
	// With (IX + d)
	_INST_FROM (0xDDAE, 3, 19, 19,	"XOR (IX+[#1])",		XOR_IndirectIndexIX, XOR_General);
	// With (IX + d)
	_INST_FROM (0xFDAE,	3, 19, 19,	"XOR (IY+[#1])",		XOR_IndirectIndexIY, XOR_General);
	// With Value
	_INST_FROM (0xEE,	2, 7, 7,	"XOR [#1]",				XOR, XOR_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDAC,	2, 8, 8,	"XOR IXH",				XOR_IXH, XOR_General);					// Undocumented
	_INST_FROM (0xDDAD,	2, 8, 8,	"XOR IXL",				XOR_IXL, XOR_General);					// Undocumented
	_INST_FROM (0xFDAC,	2, 8, 8,	"XOR IYH",				XOR_IYH, XOR_General);					// Undocumented
	_INST_FROM (0xFDAD,	2, 8, 8,	"XOR IYL",				XOR_IYL, XOR_General);					// Undocumented
	// ----------

	// ----------
	/** ADD is always over the value of A. */
	class ADD_General : public Instruction
	{
		public:
		ADD_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a value. \n
			The value of the carry is or not taken into account according with the parameter c. \n
			No carry is taken into account by default. */
		bool executeWith (MCHEmul::Register& r, const MCHEmul::UByte& v, bool c = false);
		/** With a double register and a value. \n
			Same importance to the carry. */
		bool executeWith (MCHEmul::RefRegisters& r, const MCHEmul::UBytes& v, bool c = false);
	};

	// Without carry
	// With A
	_INST_FROM (0x87,	1, 4, 4,	"ADD A,A",				ADD_AWithA, ADD_General);				// Also undocumented with codes: DD87 & FD87
	// With B
	_INST_FROM (0x80,	1, 4, 4,	"ADD A,B",				ADD_AWithB, ADD_General);				// Also undocumented with codes: DD80 & FD80
	// With C
	_INST_FROM (0x81,	1, 4, 4,	"ADD A,C",				ADD_AWithC, ADD_General);				// Also undocumented with codes: DD81 & FD81
	// With D
	_INST_FROM (0x82,	1, 4, 4,	"ADD A,D",				ADD_AWithD, ADD_General);				// Also undocumented with codes: DD82 & FD82
	// With E
	_INST_FROM (0x83,	1, 4, 4,	"ADD A,E",				ADD_AWithE, ADD_General);				// Also undocumented with codes: DD83 & FD83
	// With H
	_INST_FROM (0x84,	1, 4, 4,	"ADD A,H",				ADD_AWithH, ADD_General);
	// With L
	_INST_FROM (0x85,	1, 4, 4,	"ADD A,L",				ADD_AWithL, ADD_General);
	// With (HL)
	_INST_FROM (0x86,	1, 7, 7,	"ADD A,(HL)",			ADD_AWithIndirectHL, ADD_General);
	// With (IX + d)
	_INST_FROM (0xDD86, 3, 19, 19,	"ADD A,(IX+[#1])",		ADD_AWithIndirectIndexIX, ADD_General);
	// With (IX + d)
	_INST_FROM (0xFD86,	3, 19, 19,	"ADD A,(IY+[#1])",		ADD_AWithIndirectIndexIY, ADD_General);
	// With Value
	_INST_FROM (0xC6,	2, 7, 7,	"ADD A,[#1]",			ADD_A, ADD_General);
	// With HL and BC
	_INST_FROM (0x09,	1, 11, 11,	"ADD HL,BC",			ADD_HLWithBC, ADD_General);
	// With HL and DE
	_INST_FROM (0x19,	1, 11, 11,	"ADD HL,DE",			ADD_HLWithDE, ADD_General);
	// With HL and HL
	_INST_FROM (0x29,	1, 11, 11,	"ADD HL,HL",			ADD_HLWithHL, ADD_General);
	// With HL and SP
	_INST_FROM (0x39,	1, 11, 11,	"ADD HL,SP",			ADD_HLWithSP, ADD_General);
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD84,	2, 8, 8,	"ADD A,IXH",			ADD_AWithIXH, ADD_General);				// Undocumented
	_INST_FROM (0xDD85,	2, 8, 8,	"ADD A,IXL",			ADD_AWithIXL, ADD_General);				// Undocumented
	_INST_FROM (0xFD84,	2, 8, 8,	"ADD A,IYH",			ADD_AWithIYH, ADD_General);				// Undocumented
	_INST_FROM (0xFD85,	2, 8, 8,	"ADD A,IYL",			ADD_AWithIYL, ADD_General);				// Undocumented
	// With IX from BC
	_INST_FROM (0xDD09,	2, 15, 15,	"ADD IX,BC",			ADD_IXWithBC, ADD_General);
	// With IX from DE
	_INST_FROM (0xDD19,	2, 15, 15,	"ADD IX,DE",			ADD_IXWithDE, ADD_General);
	// With IX from IX
	_INST_FROM (0xDD29,	2, 15, 15,	"ADD IX,IX",			ADD_IXWithIX, ADD_General);
	// With IX from SP
	_INST_FROM (0xDD39,	2, 15, 15,	"ADD IX,SP",			ADD_IXWithSP, ADD_General);
	// With IY from BC
	_INST_FROM (0xFD09,	2, 15, 15,	"ADD IY,BC",			ADD_IYWithBC, ADD_General);
	// With IY from DE
	_INST_FROM (0xFD19,	2, 15, 15,	"ADD IY,DE",			ADD_IYWithDE, ADD_General);
	// With IY from IY
	_INST_FROM (0xFD29,	2, 15, 15,	"ADD IY,IY",			ADD_IYWithIY, ADD_General);
	// With IY from SP
	_INST_FROM (0xFD39,	2, 15, 15,	"ADD IY,SP",			ADD_IYWithSP, ADD_General);
	// ----------

	// With carry
	// With A
	_INST_FROM (0x8F,	1, 4, 4,	"ADC A,A",				ADC_AWithA, ADD_General);				// Also undocumented with codes: DD8F & FD8F
	// With B
	_INST_FROM (0x88,	1, 4, 4,	"ADC A,B",				ADC_AWithB, ADD_General);				// Also undocumented with codes: DD88 & FD88
	// With C
	_INST_FROM (0x89,	1, 4, 4,	"ADC A,C",				ADC_AWithC, ADD_General);				// Also undocumented with codes: DD89 & FD89
	// With D
	_INST_FROM (0x8A,	1, 4, 4,	"ADC A,D",				ADC_AWithD, ADD_General);				// Also undocumented with codes: DD8A & FD8A
	// Wth E
	_INST_FROM (0x8B,	1, 4, 4,	"ADC A,E",				ADC_AWithE, ADD_General);				// Also undocumented with codes: DD8B & FD8B
	// With H
	_INST_FROM (0x8C,	1, 4, 4,	"ADC A,H",				ADC_AWithH, ADD_General);
	// With L
	_INST_FROM (0x8D,	1, 4, 4,	"ADC A,L",				ADC_AWithL, ADD_General);
	// With (HL)
	_INST_FROM (0x8E,	1, 7, 7,	"ADC A,(HL)",			ADC_AWithIndirectHL, ADD_General);
	// With(IX + d)
	_INST_FROM (0xDD8E, 3, 19, 19,	"ADC A,(IX+[#1])",		ADC_AWithIndirectIndexIX, ADD_General);
	// With (IX + d)
	_INST_FROM (0xFD8E,	3, 19, 19,	"ADC A,(IY+[#1])",		ADC_AWithIndirectIndexIY, ADD_General);
	// With Value
	_INST_FROM (0xCE,	2, 7, 7,	"ADC A,[#1]",			ADC_A, ADD_General);
	// With HL and BC
	_INST_FROM (0xED4A,	2, 15, 15,	"ADC HL,BC",			ADC_HLWithBC, ADD_General);
	// With HL and DE
	_INST_FROM (0xED5A,	2, 15, 15,	"ADC HL,DE",			ADC_HLWithDE, ADD_General);
	// With HL and HL
	_INST_FROM (0xED6A,	2, 15, 15,	"ADC HL,HL",			ADC_HLWithHL, ADD_General);
	// With HL and SP
	_INST_FROM (0xED7A,	2, 15, 15,	"ADC HL,SP",			ADC_HLWithSP, ADD_General);
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD8C,	2, 8, 8,	"ADC A,IXH",			ADC_AWithIXH, ADD_General);				// Undocumented
	_INST_FROM (0xDD8D,	2, 8, 8,	"ADC A,IXL",			ADC_AWithIXL, ADD_General);				// Undocumented
	_INST_FROM (0xFD8C,	2, 8, 8,	"ADC A,IYH",			ADC_AWithIYH, ADD_General);				// Undocumented
	_INST_FROM (0xFD8D,	2, 8, 8,	"ADC A,IYL",			ADC_AWithIYL, ADD_General);				// Undocumented

	/** SUB is always over the value of A. */
	class SUB_General : public Instruction
	{
		public:
		SUB_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a value. \n
			The value of the carry is or not taken into account according with the parameter c. \n
			No carry is taken into account by default. */
		bool executeWith (MCHEmul::Register& r, const MCHEmul::UByte& v, bool c = false);
		/** With a complex register and a value. */
		bool executeWith (MCHEmul::RefRegisters& r, const MCHEmul::UBytes& v, bool c = false);
	};

	// Without carry
	// With A
	_INST_FROM (0x97,	1, 4, 4,	"SUB A,A",				SUB_AWithA, SUB_General);				// Also undocumented with codes: DD97 & FD97
	// With B
	_INST_FROM (0x90,	1, 4, 4,	"SUB A,B",				SUB_AWithB, SUB_General);				// Also undocumented with codes: DD90 & FD90
	// Wit C
	_INST_FROM (0x91,	1, 4, 4,	"SUB A,C",				SUB_AWithC, SUB_General);				// Also undocumented with codes: DD91 & FD91
	// Wih D
	_INST_FROM (0x92,	1, 4, 4,	"SUB A,D",				SUB_AWithD, SUB_General);				// Also undocumented with codes: DD92 & FD92
	// Wth E
	_INST_FROM (0x93,	1, 4, 4,	"SUB A,E",				SUB_AWithE, SUB_General);				// Also undocumented with codes: DD93 & FD93
	// Wih H
	_INST_FROM (0x94,	1, 4, 4,	"SUB A,H",				SUB_AWithH, SUB_General);
	// Wih L
	_INST_FROM (0x95,	1, 4, 4,	"SUB A,L",				SUB_AWithL, SUB_General);
	// With (HL)
	_INST_FROM (0x96,	1, 7, 7,	"SUB A,(HL)",			SUB_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INST_FROM (0xDD96, 3, 19, 19,	"SUB A,(IX+[#1])",		SUB_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INST_FROM (0xFD96,	3, 19, 19,	"SUB A,(IY+[#1])",		SUB_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INST_FROM (0xD6,	2, 7, 7,	"SUB A,[#1]",			SUB_A, SUB_General);
	// With HL and BC
	_INST_FROM (0xED42,	2, 15, 15,	"SUB HL,BC",			SUB_HLWithBC, SUB_General);
	// With HL and DE
	_INST_FROM (0xED52,	2, 15, 15,	"SUB HL,DE",			SUB_HLWithDE, SUB_General);
	// With HL and HL
	_INST_FROM (0xED62,	2, 15, 15,	"SUB HL,HL",			SUB_HLWithHL, SUB_General);
	// With HL and SP
	_INST_FROM (0xED72,	1, 15, 15,	"SUB HL,SP",			SUB_HLWithSP, SUB_General);
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD94,	2, 8, 8,	"SUB A,IXH",			SUB_AWithIXH, SUB_General);				// Undocumented
	_INST_FROM (0xDD95,	2, 8, 8,	"SUB A,IXL",			SUB_AWithIXL, SUB_General);				// Undocumented
	_INST_FROM (0xFD94,	2, 8, 8,	"SUB A,IYH",			SUB_AWithIYH, SUB_General);				// Undocumented
	_INST_FROM (0xFD95,	2, 8, 8,	"SUB A,IYL",			SUB_AWithIYL, SUB_General);				// Undocumented

	// With carry
	// With A
	_INST_FROM (0x9F,	1, 4, 4,	"SBC A,A",				SBC_AWithA, SUB_General);				// Also undocumented with codes: DD9F & FD9F
	// With B
	_INST_FROM (0x98,	1, 4, 4,	"SBC A,B",				SBC_AWithB, SUB_General);				// Also undocumented with codes: DD98 & FD98
	// Wit C
	_INST_FROM (0x99,	1, 4, 4,	"SBC A,C",				SBC_AWithC, SUB_General);				// Also undocumented with codes: DD99 & FD99
	// Wih D
	_INST_FROM (0x9A,	1, 4, 4,	"SBC A,D",				SBC_AWithD, SUB_General);				// Also undocumented with codes: DD9A & FD9A
	// Wth E
	_INST_FROM (0x9B,	1, 4, 4,	"SBC A,E",				SBC_AWithE, SUB_General);				// Also undocumented with codes: DD9B & FD9B
	// Wih H
	_INST_FROM (0x9C,	1, 4, 4,	"SBC A,H",				SBC_AWithH, SUB_General);
	// Wih L
	_INST_FROM (0x9D,	1, 4, 4,	"SBC A,L",				SBC_AWithL, SUB_General);
	// With (HL)
	_INST_FROM (0x9E,	1, 7, 7,	"SBC A,(HL)",			SBC_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INST_FROM (0xDD9E, 3, 19, 19,	"SBC A,(IX+[#1])",		SBC_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INST_FROM (0xFD9E,	3, 19, 19,	"SBC A,(IY+[#1])",		SBC_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INST_FROM (0xDE,	2, 7, 7,	"SBC A,[#1]",			SBC_A, SUB_General);
	// With HL and BC
	_INST_FROM (0xED42,	2, 15, 15,	"SBC HL,BC",			SBC_HLWithBC, SUB_General);
	// With HL and DE
	_INST_FROM (0xED52,	2, 15, 15,	"SBC HL,DE",			SBC_HLWithDE, SUB_General);
	// With HL and HL
	_INST_FROM (0xED62,	2, 15, 15,	"SBC HL,HL",			SBC_HLWithHL, SUB_General);
	// With HL and SP
	_INST_FROM (0xED72,	2, 15, 15,	"SBC HL,SP",			SBC_HLWithSP, SUB_General);
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD9C,	2, 8, 8,	"SBC A,IXH",			SBC_AWithIXH, SUB_General);				// Undocumented
	_INST_FROM (0xDD9D,	2, 8, 8,	"SBC A,IXL",			SBC_AWithIXL, SUB_General);				// Undocumented
	_INST_FROM (0xFD9C,	2, 8, 8,	"SBC A,IYH",			SBC_AWithIYH, SUB_General);				// Undocumented
	_INST_FROM (0xFD9D,	2, 8, 8,	"SBC A,IYL",			SBC_AWithIYL, SUB_General);				// Undocumented

	/** Increment the value of several things. */
	class INC_General : public Instruction
	{
		public:
		INC_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::Register& r);
		bool executeWith (MCHEmul::RefRegisters& r);
		bool executeWith (const MCHEmul::Address& a);
	};

	// With A
	_INST_FROM (0x3C,	1, 4, 4,	"INC A",				INC_A, INC_General);					// Also undocumented with codes: DD3C & FD3C
	// With B
	_INST_FROM (0x04,	1, 4, 4,	"INC B",				INC_B, INC_General);					// Also undocumented with codes: DD04 & FD04
	// Wit C
	_INST_FROM (0x0C,	1, 4, 4,	"INC C",				INC_C, INC_General);					// Also undocumented with codes: DD0C & FD0C
	// Wih D
	_INST_FROM (0x14,	1, 4, 4,	"INC D",				INC_D, INC_General);					// Also undocumented with codes: DD14 & FD14
	// Wth E
	_INST_FROM (0x1C,	1, 4, 4,	"INC E",				INC_E, INC_General);					// Also undocumented with codes: DD1C & FD1C
	// Wih F
	_INST_FROM (0x24,	1, 4, 4,	"INC H",				INC_H, INC_General);
	// Wih L
	_INST_FROM (0x2C,	1, 4, 4,	"INC L",				INC_L, INC_General);
	// With (HL)
	_INST_FROM (0x34,	1, 11, 11,	"INC (HL)",				INC_IndirectHL, INC_General);
	// With(IX + d)
	_INST_FROM (0xDD34, 3, 23, 23,	"INC (IX+[#1])",		INC_IndirectIndexIX, INC_General);
	// With (IX + d)
	_INST_FROM (0xFD34,	3, 23, 23,	"INC (IY+[#1])",		INC_IndirectIndexIY, INC_General);
	// With BC
	_INST_FROM (0x03,	1, 6, 6,	"INC BC",				INC_BC, INC_General);
	// With DE
	_INST_FROM (0x13,	1, 6, 6,	"INC DE",				INC_DE, INC_General);
	// With HL
	_INST_FROM (0x23,	1, 6, 6,	"INC HL",				INC_HL, INC_General);
	// With SP
	_INST_FROM (0x33,	1, 6, 6,	"INC SP",				INC_SP, INC_General);
	// With IX
	_INST_FROM (0xDD23,	2, 10, 10,	"INC IX",				INC_IX, INC_General);
	// With IY
	_INST_FROM (0xFD23,	2, 10, 10,	"INC IY",				INC_IY, INC_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDD24,	2, 8, 8,	"INC IXH",				INC_IXH, INC_General);						// Undocumented
	_INST_FROM (0xDD2C,	2, 8, 8,	"INC IXL",				INC_IXL, INC_General);						// Undocumented
	_INST_FROM (0xFD24,	2, 8, 8,	"INC IYH",				INC_IYH, INC_General);						// Undocumented
	_INST_FROM (0xFD2C,	2, 8, 8,	"INC IYL",				INC_IYL, INC_General);						// Undocumented

	/** Decrement the value of several things. */
	class DEC_General : public Instruction
	{
		public:
		DEC_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::Register& r);
		bool executeWith (MCHEmul::RefRegisters& r);
		bool executeWith (const MCHEmul::Address& a);
	};

	// With A
	_INST_FROM (0x3D,	1, 4, 4,	"DEC A",				DEC_A, DEC_General);					// Also undocumented with codes: DD3D & FD3D
	// With B
	_INST_FROM (0x05,	1, 4, 4,	"DEC B",				DEC_B, DEC_General);					// Also undocumented with codes: DD05 & FD05
	// Wit C
	_INST_FROM (0x0D,	1, 4, 4,	"DEC C",				DEC_C, DEC_General);					// Also undocumented with codes: DD0D & FD0D
	// Wih D
	_INST_FROM (0x15,	1, 4, 4,	"DEC D",				DEC_D, DEC_General);					// Also undocumented with codes: DD15 & FD15
	// Wth E
	_INST_FROM (0x1D,	1, 4, 4,	"DEC E",				DEC_E, DEC_General);					// Also undocumented with codes: DD1D & FD1D
	// Wih F
	_INST_FROM (0x25,	1, 4, 4,	"DEC H",				DEC_H, DEC_General);
	// Wih L
	_INST_FROM (0x2D,	1, 4, 4,	"DEC L",				DEC_L, DEC_General);
	// With (HL)
	_INST_FROM (0x35,	1, 11, 11,	"DEC (HL)",				DEC_IndirectHL, DEC_General);
	// With(IX + d)
	_INST_FROM (0xDD35, 3, 23, 23,	"DEC (IX+[#1])",		DEC_IndirectIndexIX, DEC_General);
	// With (IX + d)
	_INST_FROM (0xFD35,	3, 23, 23,	"DEC (IY+[#1])",		DEC_IndirectIndexIY, DEC_General);
	// With BC
	_INST_FROM (0x0B,	1, 6, 6,	"DEC BC",				DEC_BC, DEC_General);
	// With DE
	_INST_FROM (0x1B,	1, 6, 6,	"DEC DE",				DEC_DE, DEC_General);
	// With HL
	_INST_FROM (0x2B,	1, 6, 6,	"DEC HL",				DEC_HL, DEC_General);
	// With SP
	_INST_FROM (0x3B,	1, 6, 6,	"DEC SP",				DEC_SP, DEC_General);
	// With IX
	_INST_FROM (0xDD2B,	2, 10, 10,	"DEC IX",				DEC_IX, DEC_General);
	// With IY
	_INST_FROM (0xFD2B,	2, 10, 10,	"DEC IY",				DEC_IY, DEC_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDD25,	2, 8, 8,	"DEC IXH",				DEC_IXH, DEC_General);						// Undocumented
	_INST_FROM (0xDD2D,	2, 8, 8,	"DEC IXL",				DEC_IXL, DEC_General);						// Undocumented
	_INST_FROM (0xFD25,	2, 8, 8,	"DEC IYH",				DEC_IYH, DEC_General);						// Undocumented
	_INST_FROM (0xFD2D,	2, 8, 8,	"DEC IYL",				DEC_IYL, DEC_General);						// Undocumented

	/** CP, to compare always A register with a value. */
	class CP_General : public Instruction
	{
		public:
		CP_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a a value. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xBF,	1, 4, 4,	"CP A",					CP_WithA, CP_General);					// Also undocumented with codes: DDBF & FDBF
	// With B
	_INST_FROM (0xB8,	1, 4, 4,	"CP B",					CP_WithB, CP_General);					// Also undocumented with codes: DDB8 & FDB8
	// With C
	_INST_FROM (0xB9,	1, 4, 4,	"CP C",					CP_WithC, CP_General);					// Also undocumented with codes: DDB9 & FDB9
	// With D
	_INST_FROM (0xBA,	1, 4, 4,	"CP D",					CP_WithD, CP_General);					// Also undocumented with codes: DDBA & FDBA
	// With E
	_INST_FROM (0xBB,	1, 4, 4,	"CP E",					CP_WithE, CP_General);					// Also undocumented with codes: DDBB & FDBB
	// With F
	_INST_FROM (0xBC,	1, 4, 4,	"CP H",					CP_WithH, CP_General);
	// With L
	_INST_FROM (0xBD,	1, 4, 4,	"CP L",					CP_WithL, CP_General);
	// With (HL)
	_INST_FROM (0xBE,	1, 7, 7,	"CP (HL)",				CP_WithIndirectHL, CP_General);
	// With(IX + d)
	_INST_FROM (0xDDBE, 3, 19, 19,	"CP (IX+[#1])",			CP_WithIndirectIndexIX, CP_General);
	// With (IX + d)
	_INST_FROM (0xFDBE,	3, 19, 19,	"CP (IY+[#1])",			CP_WithIndirectIndexIY, CP_General);
	// With Value
	_INST_FROM (0xFE,	2, 7, 7,	"CP [#1]",				CP, CP_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDDBC,	2, 8, 8,	"CP IXH",				CP_WithIXH, CP_General);				// Undocumented
	_INST_FROM (0xDDBD,	2, 8, 8,	"CP IXL",				CP_WithIXL, CP_General);				// Undocumented
	_INST_FROM (0xFDBC,	2, 8, 8,	"CP IYH",				CP_WithIYH, CP_General);				// Undocumented
	_INST_FROM (0xFDBD,	2, 8, 8,	"CP IYL",				CP_WithIYL, CP_General);				// Undocumented

	// ALU Generic Instructions
	// Decimal Adjust Accumulator
	_INST_FROM (0x27,	1, 4, 4,	"DAA",					DAA, Instruction);
	// Complement Accumulator
	_INST_FROM (0x2F,	1, 4, 4,	"CPL",					CPL, Instruction);
	// Negated Accumulator (two's complement)
	_INST_FROM (0xED44,	2, 8, 8,	"NEG",					NEG, Instruction);
	// Complement carry flag
	_INST_FROM (0x3F,	1, 4, 4,	"CCF",					CCF, Instruction);
	// Set carry flag
	_INST_FROM (0x37,	1, 4, 4,	"SCF",					SCF, Instruction);

	/** ROTATELEFT in many different ways. */
	class BITSHIFTLeft_General : public Instruction
	{
		public:
		BITSHIFTLeft_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// This is really rotate to the left, but moving the bit 7 into the carry flag
		/** A register. */
		bool executeWith (MCHEmul::Register& r);
		/** A memory address. */
		bool executeWith (const MCHEmul::Address& a);
		/** A memory address and the result is left in a regiter. */
		bool executeWith (const MCHEmul::Address& a, MCHEmul::Register& r);
	};

	// Within A
	_INST_FROM (0x07,	1, 4, 4,	"RLCA",					RLC_A, BITSHIFTLeft_General);
	// Within A, but slower...
	_INST_FROM (0xCB07,	2, 8, 8,	"RLC A",				RLC_ASlower, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB00,	2, 8, 8,	"RLC B",				RLC_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB01,	2, 8, 8,	"RLC C",				RLC_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB02,	2, 8, 8,	"RLC D",				RLC_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB03,	2, 8, 8,	"RLC E",				RLC_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB04,	2, 8, 8,	"RLC H",				RLC_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB06,	2, 8, 8,	"RLC L",				RLC_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB0E,	2, 15, 16,	"RLC (HL)",				RLC_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB06,	4, 23, 23,	"RLC (IX+[#1])",	RLC_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB07,	4, 23, 23,	"RLC (IX+[#1]), A",	RLC_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB00,	4, 23, 23,	"RLC (IX+[#1]), B",	RLC_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB01,	4, 23, 23,	"RLC (IX+[#1]), C",	RLC_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB02,	4, 23, 23,	"RLC (IX+[#1]), D",	RLC_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB03,	4, 23, 23,	"RLC (IX+[#1]), E",	RLC_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB04,	4, 23, 23,	"RLC (IX+[#1]), H",	RLC_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB05,	4, 23, 23,	"RLC (IX+[#1]), L",	RLC_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB06,	4, 23, 23,	"RLC (IY+[#1])",	RLC_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB07,	4, 23, 23,	"RLC (IY+[#1]), A",	RLC_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB00,	4, 23, 23,	"RLC (IY+[#1]), B",	RLC_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB01,	4, 23, 23,	"RLC (IY+[#1]), C",	RLC_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB02,	4, 23, 23,	"RLC (IY+[#1]), D",	RLC_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB03,	4, 23, 23,	"RLC (IY+[#1]), E",	RLC_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB04,	4, 23, 23,	"RLC (IY+[#1]), H",	RLC_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB05,	4, 23, 23,	"RLC (IY+[#1]), L",	RLC_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented

	/** ROTATERIGHT in many different ways. */
	class BITSHIFTRight_General : public Instruction
	{
		public:
		BITSHIFTRight_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// This is really rotate to the right, but moving the bit 7 into the carry flag
		/** A register. */
		bool executeWith (MCHEmul::Register& r);
		/** A memory address. */
		bool executeWith (const MCHEmul::Address& a);
		/** A memory address and the result is left in a regiter. */
		bool executeWith (const MCHEmul::Address& a, MCHEmul::Register& r);
	};

	// Within A
	_INST_FROM (0x0F,	1, 4, 4,	"RRCA",					RRC_A, BITSHIFTRight_General);
	// Within A, but slower...
	_INST_FROM (0xCB0F,	2, 8, 8,	"RRC A",				RRC_ASlower, BITSHIFTRight_General);
	// Within B
	_INST_FROM (0xCB08,	2, 8, 8,	"RRC B",				RRC_B, BITSHIFTRight_General);
	// Within C
	_INST_FROM (0xCB09,	2, 8, 8,	"RRC C",				RRC_C, BITSHIFTRight_General);
	// Within D
	_INST_FROM (0xCB0A,	2, 8, 8,	"RRC D",				RRC_D, BITSHIFTRight_General);
	// Within E
	_INST_FROM (0xCB0B,	2, 8, 8,	"RRC E",				RRC_E, BITSHIFTRight_General);
	// Within F
	_INST_FROM (0xCB0C,	2, 8, 8,	"RRC F",				RRC_F, BITSHIFTRight_General);
	// Within L
	_INST_FROM (0xCB0D,	2, 8, 8,	"RRC L",				RRC_L, BITSHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB0E,	2, 15, 16,	"RRC (HL)",				RRC_IndirectHL, BITSHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	_INST_FROM (0xDDCB0E,	4, 23, 23,	"RRC (IX+[#1])",		RRC_IndirectIndexIX, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RRC (IX+[#1]), A",	RRC_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RRC (IX+[#1]), B",	RRC_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RRC (IX+[#1]), C",	RRC_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RRC (IX+[#1]), D",	RRC_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RRC (IX+[#1]), E",	RRC_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RRC (IX+[#1]), H",	RRC_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RRC (IX+[#1]), L",	RRC_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RRC (IY+[#1])",	RRC_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RRC (IX+[#1]), A",	RRC_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RRC (IX+[#1]), B",	RRC_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RRC (IX+[#1]), C",	RRC_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RRC (IX+[#1]), D",	RRC_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RRC (IX+[#1]), E",	RRC_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RRC (IX+[#1]), H",	RRC_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RRC (IX+[#1]), L",	RRC_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented
}

#endif
  
// End of the file
/*@}*/
