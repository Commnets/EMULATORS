/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ADD.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of ADD instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ADDINSTRUCTIONS__
#define __FZX80_ADDINSTRUCTIONS__

namespace FZ80
{
	/** ADD is always over the value of A in 8 bits operations. \n
		The flags are affected in a different way depending on whether the operation is 8 bits ot 16 bits. */
	class ADD_General : public Instruction
	{
		public:
		ADD_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a value. \n
			The 8 bits operations are always done using the A destination. \n
			The value of the carry is or not taken into account according with the parameter c. \n
			No carry is taken into account by default. */
		bool executeWith (const MCHEmul::UByte& v, bool c = false);
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
}

#endif
  
// End of the file
/*@}*/
