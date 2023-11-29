/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ADD.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of LD instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_LDINSTRUCTIONS__
#define __FZX80_LDINSTRUCTIONS__

namespace FZ80
{
	/** LD_General: To load any register / memory location with a value. 
		None LD instruction, except LD A,I and LD A,R, affects the flag status. */
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
	_INST_FROM (0x3E,	2, 7, 7,	"LD A,[#1]",			LD_A, LD_General);						// Also undocumented with codes: DD3E & FD3E
	_INST_FROM (0x7F,	1, 4, 4,	"LD A,A",				LD_AFromA, LD_General);					// Also undocumented with codes: DD7F & FD7F
	_INST_FROM (0x78,	1, 4, 4,	"LD A,B",				LD_AFromB, LD_General);					// Also undocumented with codes: DD78 & FD78
	_INST_FROM (0x79,	1, 4, 4,	"LD A,C",				LD_AFromC, LD_General);					// Also undocumented with codes: DD79 & FD79
	_INST_FROM (0x7A,	1, 4, 4,	"LD A,D",				LD_AFromD, LD_General);					// Also undocumented with codes: DD7A & FD7A
	_INST_FROM (0x7B,	1, 4, 4,	"LD A,E",				LD_AFromE, LD_General);					// Also undocumented with codes: DD7B & FD7B
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
	_INST_FROM (0xDD7C,	2, 8, 8,	"LD A,IXH",				LD_AFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD7D,	2, 8, 8,	"LD A,IXL",				LD_AFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xFD7C,	2, 8, 8,	"LD A,IYH",				LD_AFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD7D,	2, 8, 8,	"LD A,IYL",				LD_AFromIYL, LD_General);				// Undocumented

	// Target B
	_INST_FROM (0x06,	2, 7, 7,	"LD B,[#1]",			LD_B, LD_General);						// Also undocumented with codes: DD06 & FD06
	_INST_FROM (0x47,	1, 4, 4,	"LD B,A",				LD_BFromA, LD_General);					// Also undocumented with codes: DD47 & FD47
	_INST_FROM (0x40,	1, 4, 4,	"LD B,B",				LD_BFromB, LD_General);					// Also undocumented with codes: DD40 & FD40
	_INST_FROM (0x41,	1, 4, 4,	"LD B,C",				LD_BFromC, LD_General);					// Also undocumented with codes: DD41 & FD41
	_INST_FROM (0x42,	1, 4, 4,	"LD B,D",				LD_BFromD, LD_General);					// Also undocumented with codes: DD42 & FD42
	_INST_FROM (0x43,	1, 4, 4,	"LD B,E",				LD_BFromE, LD_General);					// Also undocumented with codes: DD43 & FD43
	_INST_FROM (0x44,	1, 4, 4,	"LD B,H",				LD_BFromH, LD_General);
	_INST_FROM (0x45,	1, 4, 4,	"LD B,L",				LD_BFromL, LD_General);
	_INST_FROM (0x46,	1, 7, 7,	"LD B,(HL)",			LD_BFromAddressHL, LD_General);
	_INST_FROM (0xDD46,	3, 19, 19,	"LD B,(IX+[#1])",		LD_BFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD46,	3, 19, 19,	"LD B,(IY+[#1])",		LD_BFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD44,	2, 8, 8,	"LD B,IXH",				LD_BFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD45,	2, 8, 8,	"LD B,IXL",				LD_BFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xFD44,	2, 8, 8,	"LD B,IYH",				LD_BFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD45,	2, 8, 8,	"LD B,IYL",				LD_BFromIYL, LD_General);				// Undocumented

	// Target C
	_INST_FROM (0x0E,	2, 7, 7,	"LD C,[#1]",			LD_C, LD_General);						// Also undocumented with codes: DD0E & FD0E
	_INST_FROM (0x4F,	1, 4, 4,	"LD C,A",				LD_CFromA, LD_General);					// Also undocumented with codes: DD4F & FD4F
	_INST_FROM (0x48,	1, 4, 4,	"LD C,B",				LD_CFromB, LD_General);					// Also undocumented with codes: DD48 & FD48
	_INST_FROM (0x49,	1, 4, 4,	"LD C,C",				LD_CFromC, LD_General);					// Also undocumented with codes: DD49 & FD49
	_INST_FROM (0x4A,	1, 4, 4,	"LD C,D",				LD_CFromD, LD_General);					// Also undocumented with codes: DD4A & FD4A
	_INST_FROM (0x4B,	1, 4, 4,	"LD C,E",				LD_CFromE, LD_General);					// Also undocumented with codes: DD4B & FD4B
	_INST_FROM (0x4C,	1, 4, 4,	"LD C,H",				LD_CFromH, LD_General);
	_INST_FROM (0x4D,	1, 4, 4,	"LD C,L",				LD_CFromL, LD_General);
	_INST_FROM (0x4E,	1, 7, 7,	"LD C,(HL)",			LD_CFromAddressHL, LD_General);
	_INST_FROM (0xDD4E,	3, 19, 19,	"LD C,(IX+[#1])",		LD_CFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD4E,	3, 19, 19,	"LD C,(IY+[#1])",		LD_CFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD4C,	2, 8, 8,	"LD C,IXH",				LD_CFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD4D,	2, 8, 8,	"LD C,IXL",				LD_CFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xFD4C,	2, 8, 8,	"LD C,IYH",				LD_CFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD4D,	2, 8, 8,	"LD C,IYL",				LD_CFromIYL, LD_General);				// Undocumented

	// Target D
	_INST_FROM (0x16,	2, 7, 7,	"LD D,[#1]",			LD_D, LD_General);						// Also undocumented with codes: DD16 & FD16
	_INST_FROM (0x57,	1, 4, 4,	"LD D,A",				LD_DFromA, LD_General);					// Also undocumented with codes: DD57 & FD57
	_INST_FROM (0x50,	1, 4, 4,	"LD D,B",				LD_DFromB, LD_General);					// Also undocumented with codes: DD50 & FD50
	_INST_FROM (0x51,	1, 4, 4,	"LD D,C",				LD_DFromC, LD_General);					// Also undocumented with codes: DD51 & FD51
	_INST_FROM (0x52,	1, 4, 4,	"LD D,D",				LD_DFromD, LD_General);					// Also undocumented with codes: DD52 & FD52
	_INST_FROM (0x53,	1, 4, 4,	"LD D,E",				LD_DFromE, LD_General);					// Also undocumented with codes: DD53 & FD53
	_INST_FROM (0x54,	1, 4, 4,	"LD D,H",				LD_DFromH, LD_General);
	_INST_FROM (0x55,	1, 4, 4,	"LD D,L",				LD_DFromL, LD_General);
	_INST_FROM (0x56,	1, 7, 7,	"LD D,(HL)",			LD_DFromAddressHL, LD_General);
	_INST_FROM (0xDD56,	3, 19, 19,	"LD D,(IX+[#1])",		LD_DFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD56,	3, 19, 19,	"LD D,(IY+[#1])",		LD_DFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD54,	2, 8, 8,	"LD D,IXH",				LD_DFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD55,	2, 8, 8,	"LD D,IXL",				LD_DFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xFD54,	2, 8, 8,	"LD D,IYH",				LD_DFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD55,	2, 8, 8,	"LD D,IYL",				LD_DFromIYL, LD_General);				// Undocumented

	// Target E
	_INST_FROM (0x1E,	2, 7, 7,	"LD E,[#1]",			LD_E, LD_General);						// Also undocumented with codes: DD1E & FD1E
	_INST_FROM (0x5F,	1, 4, 4,	"LD E,A",				LD_EFromA, LD_General);					// Also undocumented with codes: DD5F & FD5F
	_INST_FROM (0x58,	1, 4, 4,	"LD E,B",				LD_EFromB, LD_General);					// Also undocumented with codes: DD58 & FD58
	_INST_FROM (0x59,	1, 4, 4,	"LD E,C",				LD_EFromC, LD_General);					// Also undocumented with codes: DD59 & FD59
	_INST_FROM (0x5A,	1, 4, 4,	"LD E,D",				LD_EFromD, LD_General);					// Also undocumented with codes: DD5A & FD5A
	_INST_FROM (0x5B,	1, 4, 4,	"LD E,E",				LD_EFromE, LD_General);					// Also undocumented with codes: DD5B & FD5B
	_INST_FROM (0x5C,	1, 4, 4,	"LD E,H",				LD_EFromH, LD_General);
	_INST_FROM (0x5D,	1, 4, 4,	"LD E,L",				LD_EFromL, LD_General);
	_INST_FROM (0x5E,	1, 7, 7,	"LD E,(HL)",			LD_EFromAddressHL, LD_General);
	_INST_FROM (0xDD5E,	3, 19, 19,	"LD E,(IX+[#1])",		LD_EFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD5E,	3, 19, 19,	"LD E,(IY+[#1])",		LD_EFromAddressIndexIY, LD_General);
	_INST_FROM (0xDD5C,	2, 8, 8,	"LD E,IXH",				LD_EFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD5D,	2, 8, 8,	"LD E,IXL",				LD_EFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xFD5C,	2, 8, 8,	"LD E,IYH",				LD_EFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD5D,	2, 8, 8,	"LD E,IYL",				LD_EFromIYL, LD_General);				// Undocumented

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
}

#endif
  
// End of the file
/*@}*/
