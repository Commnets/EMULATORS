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
	class Instruction : public MCHEmul::Instruction
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t);

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

	// LD
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
		// None of these instructions affects the status register!...
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
	_INST_FROM (0x3E,	2, 7, 7,	"LD A,[#1]",			LD_A, LD_General);
	_INST_FROM (0x7F,	1, 4, 4,	"LD A,A",				LD_AFromA, LD_General);
	_INST_FROM (0x78,	1, 4, 4,	"LD A,B",				LD_AFromB, LD_General);
	_INST_FROM (0x79,	1, 4, 4,	"LD A,C",				LD_AFromC, LD_General);
	_INST_FROM (0x7A,	1, 4, 4,	"LD A,D",				LD_AFromD, LD_General);
	_INST_FROM (0x7B,	1, 4, 4,	"LD A,E",				LD_AFromE, LD_General);
	_INST_FROM (0x7C,	1, 4, 4,	"LD A,F",				LD_AFromF, LD_General);
	_INST_FROM (0x7D,	1, 4, 4,	"LD A,L",				LD_AFromL, LD_General);
	_INST_FROM (0xED57, 2, 4, 4,	"LD A,I",				LD_AFromI, LD_General);
	_INST_FROM (0xED5F, 2, 4, 4,	"LD A,R",				LD_AFromR, LD_General);
	_INST_FROM (0x7E,	1, 7, 7,	"LD A,(HL)",			LD_AFromAddressHL, LD_General);
	_INST_FROM (0x0A,	1, 7, 7,	"LD A,(BC)",			LD_AFromAddressBC, LD_General);
	_INST_FROM (0x1A,	1, 7, 7,	"LD A,(DE)",			LD_AFromAddressDE, LD_General);
	_INST_FROM (0xDD7E,	3, 19, 19,	"LD A,(IX+[#1])",		LD_AFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD7E,	3, 19, 19,	"LD A,(IY+[#1])",		LD_AFromAddressIndexIY, LD_General);
	_INST_FROM (0x3A,	3, 13, 13,	"LD A,([$2])",			LD_AFromAddress, LD_General);
	// Target B
	_INST_FROM (0x06,	2, 7, 7,	"LD B,[#1]",			LD_B, LD_General);
	_INST_FROM (0x47,	1, 4, 4,	"LD B,A",				LD_BFromA, LD_General);
	_INST_FROM (0x40,	1, 4, 4,	"LD B,B",				LD_BFromB, LD_General);
	_INST_FROM (0x41,	1, 4, 4,	"LD B,C",				LD_BFromC, LD_General);
	_INST_FROM (0x42,	1, 4, 4,	"LD B,D",				LD_BFromD, LD_General);
	_INST_FROM (0x43,	1, 4, 4,	"LD B,E",				LD_BFromE, LD_General);
	_INST_FROM (0x44,	1, 4, 4,	"LD B,F",				LD_BFromF, LD_General);
	_INST_FROM (0x45,	1, 4, 4,	"LD B,L",				LD_BFromL, LD_General);
	_INST_FROM (0xDD46,	3, 19, 19,	"LD B,(IX+[#1])",		LD_BFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD46,	3, 19, 19,	"LD B,(IY+[#1])",		LD_BFromAddressIndexIY, LD_General);
	// Target C
	_INST_FROM (0x0E,	2, 7, 7,	"LD C,[#1]",			LD_C, LD_General);
	_INST_FROM (0x4F,	1, 4, 4,	"LD C,A",				LD_CFromA, LD_General);
	_INST_FROM (0x48,	1, 4, 4,	"LD C,B",				LD_CFromB, LD_General);
	_INST_FROM (0x49,	1, 4, 4,	"LD C,C",				LD_CFromC, LD_General);
	_INST_FROM (0x4A,	1, 4, 4,	"LD C,D",				LD_CFromD, LD_General);
	_INST_FROM (0x4B,	1, 4, 4,	"LD C,E",				LD_CFromE, LD_General);
	_INST_FROM (0x4C,	1, 4, 4,	"LD C,F",				LD_CFromF, LD_General);
	_INST_FROM (0x4D,	1, 4, 4,	"LD C,L",				LD_CFromL, LD_General);
	_INST_FROM (0xDD4E,	3, 19, 19,	"LD C,(IX+[#1])",		LD_CFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD4E,	3, 19, 19,	"LD C,(IY+[#1])",		LD_CFromAddressIndexIY, LD_General);
	// Target D
	_INST_FROM (0x16,	2, 7, 7,	"LD D,[#1]",			LD_D, LD_General);
	_INST_FROM (0x57,	1, 4, 4,	"LD D,A",				LD_DFromA, LD_General);
	_INST_FROM (0x50,	1, 4, 4,	"LD D,B",				LD_DFromB, LD_General);
	_INST_FROM (0x51,	1, 4, 4,	"LD D,C",				LD_DFromC, LD_General);
	_INST_FROM (0x52,	1, 4, 4,	"LD D,D",				LD_DFromD, LD_General);
	_INST_FROM (0x53,	1, 4, 4,	"LD D,E",				LD_DFromE, LD_General);
	_INST_FROM (0x54,	1, 4, 4,	"LD D,F",				LD_DFromF, LD_General);
	_INST_FROM (0x55,	1, 4, 4,	"LD D,L",				LD_DFromL, LD_General);
	_INST_FROM (0xDD56,	3, 19, 19,	"LD D,(IX+[#1])",		LD_DFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD56,	3, 19, 19,	"LD D,(IY+[#1])",		LD_DFromAddressIndexIY, LD_General);
	// Target E
	_INST_FROM (0x1E,	2, 7, 7,	"LD E,[#1]",			LD_E, LD_General);
	_INST_FROM (0x5F,	1, 4, 4,	"LD E,A",				LD_EFromA, LD_General);
	_INST_FROM (0x58,	1, 4, 4,	"LD E,B",				LD_EFromB, LD_General);
	_INST_FROM (0x59,	1, 4, 4,	"LD E,C",				LD_EFromC, LD_General);
	_INST_FROM (0x5A,	1, 4, 4,	"LD E,D",				LD_EFromD, LD_General);
	_INST_FROM (0x5B,	1, 4, 4,	"LD E,E",				LD_EFromE, LD_General);
	_INST_FROM (0x5C,	1, 4, 4,	"LD E,F",				LD_EFromF, LD_General);
	_INST_FROM (0x5D,	1, 4, 4,	"LD E,L",				LD_EFromL, LD_General);
	_INST_FROM (0xDD5E,	3, 19, 19,	"LD E,(IX+[#1])",		LD_EFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD5E,	3, 19, 19,	"LD E,(IY+[#1])",		LD_EFromAddressIndexIY, LD_General);
	// Target H
	_INST_FROM (0x26,	2, 7, 7,	"LD E,[#1]",			LD_H, LD_General);
	_INST_FROM (0x67,	1, 4, 4,	"LD H,A",				LD_HFromA, LD_General);
	_INST_FROM (0x60,	1, 4, 4,	"LD H,B",				LD_HFromB, LD_General);
	_INST_FROM (0x61,	1, 4, 4,	"LD H,C",				LD_HFromC, LD_General);
	_INST_FROM (0x62,	1, 4, 4,	"LD H,D",				LD_HFromD, LD_General);
	_INST_FROM (0x63,	1, 4, 4,	"LD H,E",				LD_HFromE, LD_General);
	_INST_FROM (0x64,	1, 4, 4,	"LD H,F",				LD_HFromF, LD_General);
	_INST_FROM (0x65,	1, 4, 4,	"LD H,L",				LD_HFromL, LD_General);
	_INST_FROM (0xDD66,	3, 19, 19,	"LD H,(IX+[#1])",		LD_HFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD66,	3, 19, 19,	"LD H,(IY+[#1])",		LD_HFromAddressIndexIY, LD_General);
	// Target L
	_INST_FROM (0x2E,	2, 7, 7,	"LD L,[#1]",			LD_L, LD_General);
	_INST_FROM (0x6F,	1, 4, 4,	"LD L,A",				LD_LFromA, LD_General);
	_INST_FROM (0x68,	1, 4, 4,	"LD L,B",				LD_LFromB, LD_General);
	_INST_FROM (0x69,	1, 4, 4,	"LD L,C",				LD_LFromC, LD_General);
	_INST_FROM (0x6A,	1, 4, 4,	"LD L,D",				LD_LFromD, LD_General);
	_INST_FROM (0x6B,	1, 4, 4,	"LD L,E",				LD_LFromE, LD_General);
	_INST_FROM (0x6C,	1, 4, 4,	"LD L,F",				LD_LFromF, LD_General);
	_INST_FROM (0x6D,	1, 4, 4,	"LD L,L",				LD_LFromL, LD_General);
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
	_INST_FROM (0x32,	3, 13, 13,	"LD ([$2]),A",			LD_AddressFromA, LD_General);
	// Target I
	_INST_FROM (0xED47,	2, 4, 4,	"LD I,A",				LD_IFromA, LD_General);
	// Target R
	_INST_FROM (0xED4F,	2, 4, 4,	"LD R,A",				LD_RFromA, LD_General);
	// Target BC
	_INST_FROM (0x01,	3, 10, 10,	"LD BC,[#2]",			LD_BC, LD_General);
	_INST_FROM (0xED48,	4, 20, 20,	"LD BC,([$2])",			LD_BCFromAddress, LD_General);
	// Target DE
	_INST_FROM (0x11,	3, 10, 10,	"LD DE,[#2]",			LD_DE, LD_General);
	_INST_FROM (0xED5B,	4, 20, 20,	"LD DE,([$2])",			LD_DEFromAddress, LD_General);
	// Target HL
	_INST_FROM (0x21,	3, 10, 10,	"LD HL,[#2]",			LD_HL, LD_General);
	_INST_FROM (0x2A,	3, 16, 16,	"LD HL,([$2])",			LD_HLFromAddress, LD_General);
	// Target IX
	_INST_FROM (0xDD21,	4, 10, 10,	"LD IX,[#2]",			LD_IX, LD_General);
	_INST_FROM (0xDD2A,	4, 20, 20,	"LD IX,([$2])",			LD_IXFromAddress, LD_General);
	// Target IY
	_INST_FROM (0xFD21,	4, 10, 10,	"LD IY,[#2]",			LD_IY, LD_General);
	_INST_FROM (0xFD2A,	4, 20, 20,	"LD IY,([$2])",			LD_IYFromAddress, LD_General);
	// Target SP
	_INST_FROM (0x31,	3, 10, 10,	"LD SP,[#2]",			LD_SP, LD_General);
	_INST_FROM (0xF9,	1, 6, 6,	"LD SP,HL",				LD_SPFromHL, LD_General);
	_INST_FROM (0xDDF9,	2, 10, 10,	"LD SP,IX",				LD_SPFromIX, LD_General);
	_INST_FROM (0xFDF9,	2, 10, 10,	"LD SP,IY",				LD_SPFromIY, LD_General);
	_INST_FROM (0xED7B,	4, 10, 10,	"LD SP,([$2])",			LD_SPFromAddress, LD_General);
}

#endif
  
// End of the file
/*@}*/
