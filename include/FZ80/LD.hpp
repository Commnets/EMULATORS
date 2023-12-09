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

#include <FZ80/Instruction.hpp>

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
	_INST_FROM (0x3E,	2, 7, 7,	"LD A,[#1]",			LD_A, LD_General);
	_INST_FROM (0xDD3E,	3, 11, 11,	"U1LD A,[#1]",			U1LD_A, LD_General);					// Undocumented
	_INST_FROM (0xFD3E,	3, 11, 11,	"U2LD A,[#1]",			U2LD_A, LD_General);					// Undocumented
	_INST_FROM (0x7F,	1, 4, 4,	"LD A,A",				LD_AFromA, LD_General);
	_INST_FROM (0xDD7F,	2, 8, 8,	"U1LD A,A",				U1LD_AFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD7F,	2, 8, 8,	"U2LD A,A",				U2LD_AFromA, LD_General);				// Undocumented
	_INST_FROM (0x78,	1, 4, 4,	"LD A,B",				LD_AFromB, LD_General);
	_INST_FROM (0xDD78,	2, 8, 8,	"U1LD A,B",				U1LD_AFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD78,	2, 8, 8,	"U2LD A,B",				U2LD_AFromB, LD_General);				// Undocumented
	_INST_FROM (0x79,	1, 4, 4,	"LD A,C",				LD_AFromC, LD_General);
	_INST_FROM (0xDD79,	2, 8, 8,	"U1LD A,C",				U1LD_AFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD79,	2, 8, 8,	"U2LD A,C",				U2LD_AFromC, LD_General);				// Undocumented
	_INST_FROM (0x7A,	1, 4, 4,	"LD A,D",				LD_AFromD, LD_General);
	_INST_FROM (0xDD7A,	2, 8, 8,	"U1LD A,D",				U1LD_AFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD7A,	2, 8, 8,	"U2LD A,D",				U2LD_AFromD, LD_General);				// Undocumented
	_INST_FROM (0x7B,	1, 4, 4,	"LD A,E",				LD_AFromE, LD_General);
	_INST_FROM (0xDD7B,	2, 8, 8,	"U1LD A,E",				U1LD_AFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD7B,	2, 8, 8,	"U2LD A,E",				U2LD_AFromE, LD_General);				// Undocumented
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
	_INST_FROM (0x06,	2, 7, 7,	"LD B,[#1]",			LD_B, LD_General);
	_INST_FROM (0xDD06,	3, 11, 11,	"U1LD B,[#1]",			U1LD_B, LD_General);					// Undocumented
	_INST_FROM (0xFD06,	3, 11, 11,	"U2LD B,[#1]",			U2LD_B, LD_General);					// Undocumented
	_INST_FROM (0x47,	1, 4, 4,	"LD B,A",				LD_BFromA, LD_General);
	_INST_FROM (0xDD47,	2, 8, 8,	"U1LD B,A",				U1LD_BFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD47,	2, 8, 8,	"U2LD B,A",				U2LD_BFromA, LD_General);				// Undocumented
	_INST_FROM (0x40,	1, 4, 4,	"LD B,B",				LD_BFromB, LD_General);
	_INST_FROM (0xDD40,	2, 8, 8,	"U1LD B,B",				U1LD_BFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD40,	2, 8, 8,	"U2LD B,B",				U2LD_BFromB, LD_General);				// Undocumented
	_INST_FROM (0x41,	1, 4, 4,	"LD B,C",				LD_BFromC, LD_General);
	_INST_FROM (0xDD41,	2, 8, 8,	"U1LD B,C",				U1LD_BFromC, LD_General);				// Undcoumented
	_INST_FROM (0xFD41,	2, 8, 8,	"U2LD B,C",				U2LD_BFromC, LD_General);				// Undcoumented
	_INST_FROM (0x42,	1, 4, 4,	"LD B,D",				LD_BFromD, LD_General);
	_INST_FROM (0xDD42,	2, 8, 8,	"U1LD B,D",				U1LD_BFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD42,	2, 8, 8,	"U2LD B,D",				U2LD_BFromD, LD_General);				// Undocumented
	_INST_FROM (0x43,	1, 4, 4,	"LD B,E",				LD_BFromE, LD_General);
	_INST_FROM (0xDD43,	2, 8, 8,	"U1LD B,E",				U1LD_BFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD43,	2, 8, 8,	"U2LD B,E",				U2LD_BFromE, LD_General);				// Undocumented
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
	_INST_FROM (0x0E,	2, 7, 7,	"LD C,[#1]",			LD_C, LD_General);
	_INST_FROM (0xDD0E,	3, 11, 11,	"U1LD C,[#1]",			U1LD_C, LD_General);					// Undocumented
	_INST_FROM (0xFD0E,	3, 11, 11,	"U2LD C,[#1]",			U2LD_C, LD_General);					// Undocumented
	_INST_FROM (0x4F,	1, 4, 4,	"LD C,A",				LD_CFromA, LD_General);
	_INST_FROM (0xDD4F,	2, 8, 8,	"U1LD C,A",				U1LD_CFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD4F,	2, 8, 8,	"U2LD C,A",				U2LD_CFromA, LD_General);				// Undocumented
	_INST_FROM (0x48,	1, 4, 4,	"LD C,B",				LD_CFromB, LD_General);
	_INST_FROM (0xDD48,	2, 8, 8,	"U1LD C,B",				U1LD_CFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD48,	2, 8, 8,	"U2LD C,B",				U2LD_CFromB, LD_General);				// Undocumented
	_INST_FROM (0x49,	1, 4, 4,	"LD C,C",				LD_CFromC, LD_General);
	_INST_FROM (0xDD49,	2, 8, 8,	"U1LD C,C",				U1LD_CFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD49,	2, 8, 8,	"U2LD C,C",				U2LD_CFromC, LD_General);				// Undocumented
	_INST_FROM (0x4A,	1, 4, 4,	"LD C,D",				LD_CFromD, LD_General);
	_INST_FROM (0xDD4A,	2, 8, 8,	"U1LD C,D",				U1LD_CFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD4A,	2, 8, 8,	"U2LD C,D",				U2LD_CFromD, LD_General);				// Undocumented
	_INST_FROM (0x4B,	1, 4, 4,	"LD C,E",				LD_CFromE, LD_General);
	_INST_FROM (0xDD4B,	2, 8, 8,	"U1LD C,E",				U1LD_CFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD4B,	2, 8, 8,	"U2LD C,E",				U2LD_CFromE, LD_General);				// Undocumented
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
	_INST_FROM (0x16,	2, 7, 7,	"LD D,[#1]",			LD_D, LD_General);
	_INST_FROM (0xDD16,	3, 11, 11,	"U1LD D,[#1]",			U1LD_D, LD_General);					// Undocumented
	_INST_FROM (0xFD16,	3, 11, 11,	"U2LD D,[#1]",			U2LD_D, LD_General);					// Undocumented
	_INST_FROM (0x57,	1, 4, 4,	"LD D,A",				LD_DFromA, LD_General);
	_INST_FROM (0xDD57,	2, 8, 8,	"U1LD D,A",				U1LD_DFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD57,	2, 8, 8,	"U2LD D,A",				U2LD_DFromA, LD_General);				// Undocumented
	_INST_FROM (0x50,	1, 4, 4,	"LD D,B",				LD_DFromB, LD_General);
	_INST_FROM (0xDD50,	2, 8, 8,	"U1LD D,B",				U1LD_DFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD50,	2, 8, 8,	"U2LD D,B",				U2LD_DFromB, LD_General);				// Undocumented
	_INST_FROM (0x51,	1, 4, 4,	"LD D,C",				LD_DFromC, LD_General);
	_INST_FROM (0xDD51,	2, 8, 8,	"U1LD D,C",				U1LD_DFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD51,	2, 8, 8,	"U2LD D,C",				U2LD_DFromC, LD_General);				// Undocumented
	_INST_FROM (0x52,	1, 4, 4,	"LD D,D",				LD_DFromD, LD_General);
	_INST_FROM (0xDD52,	2, 8, 8,	"U1LD D,D",				U1LD_DFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD52,	2, 8, 8,	"U2LD D,D",				U2LD_DFromD, LD_General);				// Undocumented
	_INST_FROM (0x53,	1, 4, 4,	"LD D,E",				LD_DFromE, LD_General);
	_INST_FROM (0xDD53,	2, 8, 8,	"U1LD D,E",				U1LD_DFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD53,	2, 8, 8,	"U2LD D,E",				U2LD_DFromE, LD_General);				// Undocumented
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
	_INST_FROM (0x1E,	2, 7, 7,	"LD E,[#1]",			LD_E, LD_General);
	_INST_FROM (0xDD1E,	3, 11, 11,	"U1LD E,[#1]",			U1LD_E, LD_General);					// Undocumented
	_INST_FROM (0xFD1E,	3, 11, 11,	"U2LD E,[#1]",			U2LD_E, LD_General);					// Undocumented
	_INST_FROM (0x5F,	1, 4, 4,	"LD E,A",				LD_EFromA, LD_General);
	_INST_FROM (0xDD5F,	2, 8, 8,	"U1LD E,A",				U1LD_EFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD5F,	2, 8, 8,	"U2LD E,A",				U2LD_EFromA, LD_General);				// Undocumented
	_INST_FROM (0x58,	1, 4, 4,	"LD E,B",				LD_EFromB, LD_General);
	_INST_FROM (0xDD58,	2, 8, 8,	"U1LD E,B",				U1LD_EFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD58,	2, 8, 8,	"U2LD E,B",				U2LD_EFromB, LD_General);				// Undocumented
	_INST_FROM (0x59,	1, 4, 4,	"LD E,C",				LD_EFromC, LD_General);
	_INST_FROM (0xDD59,	2, 8, 8,	"U1LD E,C",				U1LD_EFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD59,	2, 8, 8,	"U2LD E,C",				U2LD_EFromC, LD_General);				// Undocumented
	_INST_FROM (0x5A,	1, 4, 4,	"LD E,D",				LD_EFromD, LD_General);
	_INST_FROM (0xDD5A,	2, 8, 8,	"U1LD E,D",				U1LD_EFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD5A,	2, 8, 8,	"U2LD E,D",				U2LD_EFromD, LD_General);				// Undocumented
	_INST_FROM (0x5B,	1, 4, 4,	"LD E,E",				LD_EFromE, LD_General);
	_INST_FROM (0xDD5B,	2, 8, 8,	"U1LD E,E",				U1LD_EFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD5B,	2, 8, 8,	"U2LD E,E",				U2LD_EFromE, LD_General);				// Undocumented
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
	_INST_FROM (0xDD74,	3, 19, 19,	"LD (IX+[#1]),H",		LD_IndirectIndexIXFromH, LD_General);
	_INST_FROM (0xDD75,	3, 19, 19,	"LD (IX+[#1]),L",		LD_IndirectIndexIXFromL, LD_General);

	// Target (IX + d)
	_INST_FROM (0xFD36,	4, 19, 19,	"LD (IY+[#1]),[#1]",	LD_IndirectIndexIY, LD_General);
	_INST_FROM (0xFD77,	3, 19, 19,	"LD (IY+[#1]),A",		LD_IndirectIndexIYFromA, LD_General);
	_INST_FROM (0xFD70,	3, 19, 19,	"LD (IY+[#1]),B",		LD_IndirectIndexIYFromB, LD_General);
	_INST_FROM (0xFD71,	3, 19, 19,	"LD (IY+[#1]),C",		LD_IndirectIndexIYFromC, LD_General);
	_INST_FROM (0xFD72,	3, 19, 19,	"LD (IY+[#1]),D",		LD_IndirectIndexIYFromD, LD_General);
	_INST_FROM (0xFD73,	3, 19, 19,	"LD (IY+[#1]),E",		LD_IndirectIndexIYFromE, LD_General);
	_INST_FROM (0xFD74,	3, 19, 19,	"LD (IY+[#1]),H",		LD_IndirectIndexIYFromH, LD_General);
	_INST_FROM (0xFD75,	3, 19, 19,	"LD (IY+[#1]),L",		LD_IndirectIndexIYFromL, LD_General);

	// Target (Address)
	_INST_FROM (0x32,	3, 13, 13,	"LD ([#2]),A",			LD_AddressFromA, LD_General);
	_INST_FROM (0x22,	3, 16, 16,	"LD ([#2]),HL",			LD_AddressFromHL, LD_General);
	_INST_FROM (0xED63,	4, 20, 20,	"U3LD ([#2]),HL",		U3LD_AddressFromHL, LD_General);		// Undocumented
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
	_INST_FROM (0x2A,	3, 16, 16,	"LD HL,([$2])",			LD_HLFromAddress, LD_General);
	_INST_FROM (0xED6B,	4, 20, 20,	"U3LD HL,([$2])",		U3LD_HLFromAddress, LD_General);		// Undocumented

	// Target IX
	_INST_FROM (0xDD21,	4, 14, 14,	"LD IX,[#2]",			LD_IX, LD_General);
	_INST_FROM (0xDD2A,	4, 20, 20,	"LD IX,([$2])",			LD_IXFromAddress, LD_General);
	_INST_FROM (0xDD26,	3, 11, 11,	"LD IXH,[#1]",			LD_IXH, LD_General);					// Undocumented
	_INST_FROM (0xDD67,	2, 8, 8,	"LD IXH,A",				LD_IXHFromA, LD_General);				// Undocumented
	_INST_FROM (0xDD60,	2, 8, 8,	"LD IXH,B",				LD_IXHFromB, LD_General);				// Undocumented
	_INST_FROM (0xDD61,	2, 8, 8,	"LD IXH,C",				LD_IXHFromC, LD_General);				// Undocumented
	_INST_FROM (0xDD62,	2, 8, 8,	"LD IXH,D",				LD_IXHFromD, LD_General);				// Undocumented
	_INST_FROM (0xDD63,	2, 8, 8,	"LD IXH,E",				LD_IXHFromE, LD_General);				// Undocumented
	_INST_FROM (0xDD64,	2, 8, 8,	"LD IXH,IXH",			LD_IXHFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD65,	2, 8, 8,	"LD IXH,IXL",			LD_IXHFromIXL, LD_General);				// Undocumented
	_INST_FROM (0xDD2E,	3, 11, 11,	"LD IXL,[#1]",			LD_IXL, LD_General);					// Undocumented
	_INST_FROM (0xDD6F,	2, 8, 8,	"LD IXL,A",				LD_IXLFromA, LD_General);				// Undocumented
	_INST_FROM (0xDD68,	2, 8, 8,	"LD IXL,B",				LD_IXLFromB, LD_General);				// Undocumented
	_INST_FROM (0xDD69,	2, 8, 8,	"LD IXL,C",				LD_IXLFromC, LD_General);				// Undocumented
	_INST_FROM (0xDD6A,	2, 8, 8,	"LD IXL,D",				LD_IXLFromD, LD_General);				// Undocumented
	_INST_FROM (0xDD6B,	2, 8, 8,	"LD IXL,E",				LD_IXLFromE, LD_General);				// Undocumented
	_INST_FROM (0xDD6C,	2, 8, 8,	"LD IXL,IXH",			LD_IXLFromIXH, LD_General);				// Undocumented
	_INST_FROM (0xDD6D,	2, 8, 8,	"LD IXL,IXL",			LD_IXLFromIXL, LD_General);				// Undocumented

	// Target IY
	_INST_FROM (0xFD21,	4, 14, 14,	"LD IY,[#2]",			LD_IY, LD_General);
	_INST_FROM (0xFD2A,	4, 20, 20,	"LD IY,([$2])",			LD_IYFromAddress, LD_General);
	_INST_FROM (0xFD26,	3, 11, 11,	"LD IYH,[#1]",			LD_IYH, LD_General);					// Undocumented
	_INST_FROM (0xFD67,	2, 8, 8,	"LD IYH,A",				LD_IYHFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD60,	2, 8, 8,	"LD IYH,B",				LD_IYHFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD61,	2, 8, 8,	"LD IYH,C",				LD_IYHFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD62,	2, 8, 8,	"LD IYH,D",				LD_IYHFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD63,	2, 8, 8,	"LD IYH,E",				LD_IYHFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD64,	2, 8, 8,	"LD IYH,IXH",			LD_IYHFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD65,	2, 8, 8,	"LD IYH,IXL",			LD_IYHFromIYL, LD_General);				// Undocumented
	_INST_FROM (0xFD2E,	3, 11, 11,	"LD IYL,[#1]",			LD_IYL, LD_General);					// Undocumented
	_INST_FROM (0xFD6F,	2, 8, 8,	"LD IYL,A",				LD_IYLFromA, LD_General);				// Undocumented
	_INST_FROM (0xFD68,	2, 8, 8,	"LD IYL,B",				LD_IYLFromB, LD_General);				// Undocumented
	_INST_FROM (0xFD69,	2, 8, 8,	"LD IYL,C",				LD_IYLFromC, LD_General);				// Undocumented
	_INST_FROM (0xFD6A,	2, 8, 8,	"LD IYL,D",				LD_IYLFromD, LD_General);				// Undocumented
	_INST_FROM (0xFD6B,	2, 8, 8,	"LD IYL,E",				LD_IYLFromE, LD_General);				// Undocumented
	_INST_FROM (0xFD6C,	2, 8, 8,	"LD IYL,IYH",			LD_IYLFromIYH, LD_General);				// Undocumented
	_INST_FROM (0xFD6D,	2, 8, 8,	"LD IYL,IYL",			LD_IYLFromIYL, LD_General);				// Undocumented

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
