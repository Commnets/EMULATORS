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
				const std::string& t)
			: MCHEmul::Instruction (c, mp, cc, t, false)
							{ }

		// To interpret the parameters of the instruction as an address 
		/** 2 parameters representing an address. */
		inline MCHEmul::Address address_absolute ();
		/** 2 parameters representing a base address. 
			The final one is got adding a value to the IX register. */
		inline MCHEmul::Address address_indexedX ();
		/** 2 parameters representing a base address. 
			The final one is got adding a value to the IY register. */
		inline MCHEmul::Address address_indexedY ();

		/** 1 parameter with the value. */
		inline MCHEmul::UByte value_inmediate ();
		MCHEmul::UByte value_absolute ()
							{ return (memory () -> value (address_absolute ())); }
		MCHEmul::UByte value_registerA ()
							{ return (cpu () -> internalRegister (CZ80::_AREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerB ()
							{ return (cpu () -> internalRegister (CZ80::_BREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerC ()
							{ return (cpu () -> internalRegister (CZ80::_CREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerD ()
							{ return (cpu () -> internalRegister (CZ80::_DREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerE ()
							{ return (cpu () -> internalRegister (CZ80::_EREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerH ()
							{ return (cpu () -> internalRegister (CZ80::_HREGISTER).values ()[0]); }
		MCHEmul::UByte value_registerL ()
							{ return (cpu () -> internalRegister (CZ80::_LREGISTER).values ()[0]); }
		MCHEmul::UByte value_indexedX ()
							{ return (memory () -> value (address_indexedX ())); }
		MCHEmul::UByte value_indexedY ()
							{ return (memory () -> value (address_indexedY ())); }
	};

	// ---
	inline MCHEmul::Address Instruction::address_indexedX ()
	{
		assert (parameters ().size () == 3);

		// The bytes 0 & 1 will identify the instruction...
		return (MCHEmul::Address ({ 
					cpu () -> internalRegister (CZ80::_IXHREGISTER).values ()[0], 
					cpu () -> internalRegister (CZ80::_IXLREGISTER).values ()[0] }, true /** Already in the right order. */)
				+ (size_t) parameters ()[2].value ());
	}

	// ---
	inline MCHEmul::Address Instruction::address_indexedY ()
	{
		assert (parameters ().size () == 3);

		// The bytes 0 & 1 will identify the instruction...
		return (MCHEmul::Address ({ 
					cpu () -> internalRegister (CZ80::_IYHREGISTER).values ()[0], 
					cpu () -> internalRegister (CZ80::_IYLREGISTER).values ()[0] }, true /** Already in the right order. */)
				+ (size_t) parameters ()[2].value ());
	}

	// ---
	inline MCHEmul::UByte Instruction::value_inmediate ()
	{
		assert (parameters ().size () == 2);

		return (parameters ()[1]);
	}

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
		/** To load a register with a byte. */
		bool executeWith (MCHEmul::Register& r, const MCHEmul::UByte& u)
							{ r.set ({ u }); return (true); }
		/** To load a memory position with the value of a byte. */
		bool executeWith (MCHEmul::Address& a, const MCHEmul::UByte& u)
							{ memory () -> set (a, { u }); return (true); }
	};

	// Target Register
	// Target A
	_INST_FROM (0x3E,	2, 7, 7,	"LD A,[#1]",			LD_A, LD_General);
	_INST_FROM (0x7F,	1, 4, 4,	"LD A,A",				LD_AFromA, LD_General);
	_INST_FROM (0x78,	1, 4, 4,	"LD A,B",				LD_AFromB, LD_General);
	_INST_FROM (0x79,	1, 4, 4,	"LD A,C",				LD_AFromC, LD_General);
	_INST_FROM (0x7A,	1, 4, 4,	"LD A,D",				LD_AFromD, LD_General);
	_INST_FROM (0x7B,	1, 4, 4,	"LD A,E",				LD_AFromE, LD_General);
	_INST_FROM (0x7C,	1, 4, 4,	"LD A,F",				LD_AFromF, LD_General);
	_INST_FROM (0x7D,	1, 4, 4,	"LD A,L",				LD_AFromL, LD_General);
	_INST_FROM (0xED57, 1, 4, 4,	"LD A,I",				LD_AFromI, LD_General);
	_INST_FROM (0xED5F, 1, 4, 4,	"LD A,R",				LD_AFromR, LD_General);
	_INST_FROM (0x7E,	1, 7, 7,	"LD A,(HL)",			LD_AFromAddressHL, LD_General);
	_INST_FROM (0x0A,	1, 7, 7,	"LD A,(BC)",			LD_AFromAddressBC, LD_General);
	_INST_FROM (0x1A,	1, 7, 7,	"LD A,(DE)",			LD_AFromAddressDE, LD_General);
	_INST_FROM (0xDD7E,	3, 19, 19,	"LD A,(IX+[#1])",		LD_AFromAddressIndexIX, LD_General);
	_INST_FROM (0xFD7E,	3, 19, 19,	"LD A,(IY+[#1])",		LD_AFromAddressIndexIY, LD_General);
	_INST_FROM (0x3A,	3, 13, 13,	"LD A,([#2])",			LD_AFromAddress, LD_General);
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
}

#endif
  
// End of the file
/*@}*/
