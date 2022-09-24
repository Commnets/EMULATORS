/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: Instructions.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The list of instructions used by any F6500 family.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_INSTRUCTIONS__
#define __F6500_INSTRUCTIONS__

#include <global.hpp>
#include <core/Address.hpp>
#include <core/Memory.hpp>
#include <language/Instruction.hpp>

namespace F6500
{
	/** Most of 6500 instruction inherits from this class
		because it includes diffrent access for any addrss mode. 
		Always little-endian format and no more than 2 bytes for and address are values taken. */
	class Instruction : public MCHEmul::Instruction
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: MCHEmul::Instruction (c, mp, cc, t)
							{ }
	
		protected:
		// To interpret the parameters of the instruction as an address 
		/** 2 parameters representing an address. */
		MCHEmul::Address address_absolute ();
		/** 1 parameter representing an address in the page 0. */
		MCHEmul::Address address_zeroPage ();
		/** 2 parameters representing a base address. 
			The final one is got adding the X register to the base. */
		MCHEmul::Address address_absoluteX ();
		/** 2 parameters representing a base address. 
			The final one is got adding the Y register to the base. */
		MCHEmul::Address address_absoluteY ();
		/** 1 parameter representing a base address in page 0. 
			The final one is got adding the X register to the base. */
		MCHEmul::Address address_zeroPageX ();
		/** 1 parameter representing a base address in page 0. 
			The final one is got adding the Y register to the base. */
		MCHEmul::Address address_zeroPageY ();
		/** 1 parameter representing a base address in page 0. 
			The final one is got from the position in page 0 result of adding the X register to the base. */
		MCHEmul::Address address_indirectZeroPageX ();
		/** 1 parameter representing a base address in page 0. 
			The final one is got from the position in page 0 result of adding the X register to position found in the base. */
		MCHEmul::Address address_indirectZeroPageY ();
		/** 2 parameters representint a base addreess. 
			The final one is got at the position of that first base address. */
		MCHEmul::Address address_indirect ();

		// To interpret the parameters of the instruction as a a value
		/** 1 parameter with the value. */
		MCHEmul::UByte value_inmediate ();
		MCHEmul::UByte value_absolute ()
							{ return (memory () -> value (address_absolute ())); }
		MCHEmul::UByte value_zeroPage ()
							{ return (memory () -> value (address_zeroPage ())); }
		MCHEmul::UByte value_absoluteX ()
							{ return (memory () -> value (address_absoluteX ())); }
		MCHEmul::UByte value_absoluteY ()
							{ return (memory () -> value (address_absoluteY ())); }
		/** 1 parameter that can be negative value. It us used in jumps. */
		MCHEmul::UByte value_relative ();
		MCHEmul::UByte value_zeroPageX ()
							{ return (memory () -> value (address_zeroPageX ())); }
		MCHEmul::UByte value_zeroPageY ()
							{ return (memory () -> value (address_zeroPageY ())); }
		MCHEmul::UByte value_indirectZeroPageX ()
							{ return (memory () -> value (address_indirectZeroPageX ())); }
		MCHEmul::UByte value_indirectZeroPageY ()
							{ return (memory () -> value (address_indirectZeroPageY ())); }
	};

	// ADC
	/** ADC_General: To aggregate common steps in every ADC instruction. */
	class ADC_General : public Instruction
	{
		public:
		ADC_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0x69, 2, 2, "ADC#[#1]",		ADC_Inmediate, ADC_General);
	_INST_FROM (0x6d, 3, 4, "ADC[$2]",		ADC_Absolute, ADC_General);
	_INST_FROM (0x65, 2, 3, "ADC[$1]",		ADC_ZeroPage, ADC_General);
	_INST_FROM (0x61, 2, 6, "ADC([$1],X)",	ADC_ZeroPageIndirectX, ADC_General);
	_INST_FROM (0x71, 2, 5, "ADC([$1]),Y",	ADC_ZeroPageIndirectY, ADC_General);
	_INST_FROM (0x75, 2, 4, "ADC[$1],X",	ADC_ZeroPageX, ADC_General);
	_INST_FROM (0x7d, 3, 4, "ADC[$2],X",	ADC_AbsoluteX, ADC_General);
	_INST_FROM (0x79, 3, 4, "ADC[$2],Y",	ADC_AbsoluteY, ADC_General);

	// AND
	/** AND_General: To aggregate common steps in every AND instruction. */
	class AND_General : public Instruction
	{
		public:
		AND_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0x29, 2, 2, "AND#[#1]",		AND_Inmediate, AND_General);
	_INST_FROM (0x2d, 3, 4, "AND[$2]",		AND_Absolute, AND_General);
	_INST_FROM (0x25, 2, 3, "AND[$1]",		AND_ZeroPage, AND_General);
	_INST_FROM (0x21, 2, 6, "AND([$1],X)",	AND_ZeroPageIndirectX, AND_General);
	_INST_FROM (0x31, 2, 5, "AND([$1]),Y",	AND_ZeroPageIndirectY, AND_General);
	_INST_FROM (0x35, 2, 4, "AND[$1],X",	AND_ZeroPageX, AND_General);
	_INST_FROM (0x3d, 3, 4, "AND[$2],X",	AND_AbsoluteX, AND_General);
	_INST_FROM (0x39, 3, 4, "AND[$2],Y",	AND_AbsoluteY, AND_General);

	// ASL
	/** ASL_General: To aggregate common steps in every ASL instruction. */
	class ASL_General : public Instruction
	{
		public:
		ASL_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x0e, 3, 6, "ASL[$2]",	ASL_Absolute, ASL_General);
	_INST_FROM (0x06, 2, 5, "ASL[$1]",		ASL_ZeroPage, ASL_General);
	_INST_FROM (0x0a, 1, 2, "ASL",			ASL_Accumulator, ASL_General);
	_INST_FROM (0x16, 2, 6, "ASL[$1],X",	ASL_ZeroPageX, ASL_General);
	_INST_FROM (0x1e, 3, 7, "ASL[$2],X",	ASL_AbsoluteX, ASL_General);

	/** BXX_General: To aggregate common steps in every Branch instruction. */
	class BXX_General : public Instruction
	{
		public:
		BXX_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		void executeBranch ();
	};

	// BCC 
	_INST_FROM (0x90, 2, 2, "BCC[&1]",		BCC, BXX_General);

	// BCS 
	_INST_FROM (0xb0, 2, 2, "BCS[&1]",		BCS, BXX_General);

	// BEQ
	_INST_FROM (0xf0, 2, 2, "BEQ[&1]",		BEQ, BXX_General);

	// BIT
	_INST_FROM (0x2c, 3, 4, "BIT[$2]",		BIT_Absolute, Instruction);
	_INST_FROM (0x24, 2, 3, "BIT[$1]",		BIT_ZeroPage, Instruction);

	// BMI
	_INST_FROM (0x30, 2, 2, "BMI[&1]",		BMI, BXX_General);

	// BNE
	_INST_FROM (0xd0, 2, 2, "BNE[&1]",		BNE, BXX_General);

	// BPL
	_INST_FROM (0x10, 2, 2, "BPL[&1]",		BPL, BXX_General);

	// BRK
	_INST_FROM (0x00, 1, 7, "BRK",			BRK, Instruction);

	// BVC
	_INST_FROM (0x50, 2, 2, "BVC[&1]",		BVC, BXX_General);

	// BVS
	_INST_FROM (0x70, 2, 2, "BVS[&1]",		BVS, BXX_General);

	// CLC
	_INST_FROM (0x18, 1, 2, "CLC",			CLC, Instruction);

	// CLD
	_INST_FROM (0xd8, 1, 2, "CLD",			CLD, Instruction);

	// CLI
	_INST_FROM (0x58, 1, 2, "CLI",			CLI, Instruction);

	// CLV
	_INST_FROM (0xb8, 1, 2, "CLV",			CLV, Instruction);

	// CMP
	/** CMP_General: To aggregate common steps in every CMP instruction. */
	class CMP_General : public Instruction
	{
		public:
		CMP_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xc9, 2, 2, "CMP#[#1]",		CMP_Inmediate, CMP_General);
	_INST_FROM (0xcd, 3, 4, "CMP[$2]",	CMP_Absolute, CMP_General);
	_INST_FROM (0xc5, 2, 3, "CMP[$1]",		CMP_ZeroPage, CMP_General);
	_INST_FROM (0xc1, 2, 6, "CMP([$1],X)",	CMP_ZeroPageIndirectX, CMP_General);
	_INST_FROM (0xd1, 2, 5, "CMP([$1]),Y",	CMP_ZeroPageIndirectY, CMP_General);
	_INST_FROM (0xd5, 2, 4, "CMP[$1],X",	CMP_ZeroPageX, CMP_General);
	_INST_FROM (0xdd, 3, 4, "CMP[$2],X",	CMP_AbsoluteX, CMP_General);
	_INST_FROM (0xd9, 3, 4, "CMP[$2],Y",	CMP_AbsoluteY, CMP_General);

	// CPX
	/** CPX_General: To aggregate common steps in every CPX instruction. */
	class CPX_General : public Instruction
	{
		public:
		CPX_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xe0, 2, 2, "CPX#[#1]",		CPX_Inmediate, CPX_General);
	_INST_FROM (0xec, 3, 4, "CPX[$2]",	CPX_Absolute, CPX_General);
	_INST_FROM (0xe4, 2, 3, "CPX[$1]",		CPX_ZeroPage, CPX_General);

	// CPY
	/** CPY_General: To aggregate common steps in every CPY instruction. */
	class CPY_General : public Instruction
	{
		public:
		CPY_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xc0, 2, 2, "CPY#[#1]",		CPY_Inmediate, CPY_General);
	_INST_FROM (0xcc, 3, 4, "CPY[$2]",	CPY_Absolute, CPY_General);
	_INST_FROM (0xc4, 2, 3, "CPY[$1]",		CPY_ZeroPage, CPY_General);

	// DEC
	/** DEC_General: To aggregate common steps in every DEC instruction. */
	class DEC_General : public Instruction
	{
		public:
		DEC_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0xce, 3, 6, "DEC[$2]",	DEC_Absolute, DEC_General);
	_INST_FROM (0xc6, 2, 5, "DEC[$1]",		DEC_ZeroPage, DEC_General);
	_INST_FROM (0xd6, 2, 6, "DEC[$1],X",	DEC_ZeroPageX, DEC_General);
	_INST_FROM (0xde, 3, 7, "DEC[$2],X",	DEC_AbsoluteX, DEC_General);

	// DEX
	_INST_FROM (0xca, 1, 2, "DEX",			DEX, Instruction);

	// DEY
	_INST_FROM (0x88, 1, 2, "DEY",			DEY, Instruction);

	// EOR
	/** EOR_General: To aggregate common steps in every EOR instruction. */
	class EOR_General : public Instruction
	{
		public:
		EOR_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0x49, 2, 2, "EOR#[#1]",		EOR_Inmediate, EOR_General);
	_INST_FROM (0x4d, 3, 4, "EOR[$2]",		EOR_Absolute, EOR_General);
	_INST_FROM (0x45, 2, 3, "EOR[$1]",		EOR_ZeroPage, EOR_General);
	_INST_FROM (0x41, 2, 6, "EOR([$1],X)",	EOR_ZeroPageIndirectX, EOR_General);
	_INST_FROM (0x51, 2, 5, "EOR([$1]),Y",	EOR_ZeroPageIndirectY, EOR_General);
	_INST_FROM (0x55, 2, 4, "EOR[$1],X",	EOR_ZeroPageX, EOR_General);
	_INST_FROM (0x5d, 3, 4, "EOR[$2],X",	EOR_AbsoluteX, EOR_General);
	_INST_FROM (0x59, 3, 4, "EOR[$2],Y",	EOR_AbsoluteY, EOR_General);

	// INC
	/** INC_General: To aggregate common steps in every INC instruction. */
	class INC_General : public Instruction
	{
		public:
		INC_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0xee, 3, 6, "INC[$2]",		INC_Absolute, INC_General);
	_INST_FROM (0xe6, 2, 5, "INC[$1]",		INC_ZeroPage, INC_General);
	_INST_FROM (0xf6, 2, 6, "INC[$1],X",	INC_ZeroPageX, INC_General);
	_INST_FROM (0xfe, 3, 7, "INC[$2],X",	INC_AbsoluteX, INC_General);

	// INX
	_INST_FROM (0xe8, 1, 2, "INX",			INX, Instruction);

	// INY
	_INST_FROM (0xc8, 1, 2, "INY",			INY, Instruction);

	// JMP
	_INST_FROM (0x4c, 3, 3, "JMP[$2]",		JMP_Absolute, Instruction);
	_INST_FROM (0x6c, 3, 5, "JMP([$2])",	JMP_Indirect, Instruction);

	// JSR
	_INST_FROM (0x20, 3, 6, "JSR[$2]",		JSR_Absolute, Instruction);

	// LDA
	/** LDA_General: To aggregate common steps in every LDA instruction. */
	class LDA_General : public Instruction
	{
		public:
		LDA_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xa9, 2, 2, "LDA#[#1]",		LDA_Inmediate, LDA_General);
	_INST_FROM (0xad, 3, 4, "LDA[$2]",		LDA_Absolute, LDA_General);
	_INST_FROM (0xa5, 2, 3, "LDA[$1]",		LDA_ZeroPage, LDA_General);
	_INST_FROM (0xa1, 2, 6, "LDA([$1],X)",	LDA_ZeroPageIndirectX, LDA_General);
	_INST_FROM (0xb1, 2, 5, "LDA([$1]),Y",	LDA_ZeroPageIndirectY, LDA_General);
	_INST_FROM (0xb5, 2, 4, "LDA[$1],X",	LDA_ZeroPageX, LDA_General);
	_INST_FROM (0xbd, 3, 4, "LDA[$2],X",	LDA_AbsoluteX, LDA_General);
	_INST_FROM (0xb9, 3, 4, "LDA[$2],Y",	LDA_AbsoluteY, LDA_General);

	// LDX
	/** LDX_General: To aggregate common steps in every LDX instruction. */
	class LDX_General : public Instruction
	{
		public:
		LDX_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xa2, 2, 2, "LDX#[#1]",		LDX_Inmediate, LDX_General);
	_INST_FROM (0xae, 3, 4, "LDX[$2]",		LDX_Absolute, LDX_General);
	_INST_FROM (0xa6, 2, 3, "LDX[$1]",		LDX_ZeroPage, LDX_General);
	_INST_FROM (0xbe, 3, 4, "LDX[$2],Y",	LDX_AbsoluteY, LDX_General);
	_INST_FROM (0xb6, 2, 4, "LDX[$1],Y",	LDX_ZeroPageY, LDX_General);

	// LDY
	/** LDY_General: To aggregate common steps in every LDY instruction. */
	class LDY_General : public Instruction
	{
		public:
		LDY_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xa0, 2, 2, "LDY#[#1]",		LDY_Inmediate, LDY_General);
	_INST_FROM (0xac, 3, 4, "LDY[$2]",		LDY_Absolute, LDY_General);
	_INST_FROM (0xa4, 2, 3, "LDY[$1]",		LDY_ZeroPage, LDY_General);
	_INST_FROM (0xb4, 2, 4, "LDY[$1],X",	LDY_ZeroPageX, LDY_General);
	_INST_FROM (0xbc, 3, 4, "LDY[$2],X",	LDY_AbsoluteX, LDY_General);

	// LSR
	/** LSR_General: To aggregate common steps in every LSR instruction. */
	class LSR_General : public Instruction
	{
		public:
		LSR_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x4e, 3, 6, "LSR[$2]",		LSR_Absolute, LSR_General);
	_INST_FROM (0x46, 2, 5, "LSR[$1]",		LSR_ZeroPage, LSR_General);
	_INST_FROM (0x4a, 1, 2, "LSR",			LSR_Accumulator, LSR_General);
	_INST_FROM (0x56, 2, 6, "LSR[$1],X",	LSR_ZeroPageX, LSR_General);
	_INST_FROM (0x5e, 3, 7, "LSR[$2],X",	LSR_AbsoluteX, LSR_General);

	// NOP
	_INST_FROM (0xea, 1, 2, "NOP",			NOP, Instruction);

	// ORA
	/** ORA_General: To aggregate common steps in every ORA instruction. */
	class ORA_General : public Instruction
	{
		public:
		ORA_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0x09, 2, 2, "ORA#[#1]",		ORA_Inmediate, ORA_General);
	_INST_FROM (0x0d, 3, 4, "ORA[$2]",		ORA_Absolute, ORA_General);
	_INST_FROM (0x05, 2, 3, "ORA[$1]",		ORA_ZeroPage, ORA_General);
	_INST_FROM (0x01, 2, 6, "ORA([$1],X)",	ORA_ZeroPageIndirectX, ORA_General);
	_INST_FROM (0x11, 2, 5, "ORA([$1]),Y",	ORA_ZeroPageIndirectY, ORA_General);
	_INST_FROM (0x15, 2, 4, "ORA[$1],X",	ORA_ZeroPageX, ORA_General);
	_INST_FROM (0x1d, 3, 4, "ORA[$2],X",	ORA_AbsoluteX, ORA_General);
	_INST_FROM (0x19, 3, 4, "ORA[$2],Y",	ORA_AbsoluteY, ORA_General);

	// PHA
	_INST_FROM (0x48, 1, 3, "PHA",			PHA, Instruction);

	// PHP
	_INST_FROM (0x08, 1, 3, "PHP",			PHP, Instruction);

	// PLA
	_INST_FROM (0x68, 1, 4, "PLA",			PLA, Instruction);

	// PLP
	_INST_FROM (0x28, 1, 4, "PLP",			PLP, Instruction);

	// ROL
	/** ROL_General: To aggregate common steps in every ROL instruction. */
	class ROL_General : public Instruction
	{
		public:
		ROL_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x2e, 3, 6, "ROL[$2]",		ROL_Absolute, ROL_General);
	_INST_FROM (0x26, 2, 5, "ROL[$1]",		ROL_ZeroPage, ROL_General);
	_INST_FROM (0x2a, 1, 2, "ROL",			ROL_Accumulator, ROL_General);
	_INST_FROM (0x36, 2, 6, "ROL[$1],X",	ROL_ZeroPageX, ROL_General);
	_INST_FROM (0x3e, 3, 7, "ROL[$2],X",	ROL_AbsoluteX, ROL_General);

	// ROR
	/** ROR_General: To aggregate common steps in every ROR instruction. */
	class ROR_General : public Instruction
	{
		public:
		ROR_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x6e, 3, 6, "ROR[$2]",		ROR_Absolute, ROR_General);
	_INST_FROM (0x66, 2, 5, "ROR[$1]",		ROR_ZeroPage, ROR_General);
	_INST_FROM (0x6a, 1, 2, "ROR",			ROR_Accumulator, ROR_General);
	_INST_FROM (0x76, 2, 6, "ROR[$1],X",	ROR_ZeroPageX, ROR_General);
	_INST_FROM (0x7e, 3, 7, "ROR[$2],X",	ROR_AbsoluteX, ROR_General);

	// RTI
	_INST_FROM (0x40, 1, 6, "RTI",			RTI, Instruction);

	// RTS
	_INST_FROM (0x60, 1, 6, "RTS",			RTS, Instruction);

	// SBC
	/** SBC_General: To aggregate common steps in every SBC instruction. */
	class SBC_General : public Instruction
	{
		public:
		SBC_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::UByte u);
	};

	_INST_FROM (0xe9, 2, 2, "SBC#[#1]",		SBC_Inmediate, SBC_General);
	_INST_FROM (0xed, 3, 4, "SBC[$2]",		SBC_Absolute, SBC_General);
	_INST_FROM (0xe5, 2, 3, "SBC[$1]",		SBC_ZeroPage, SBC_General);
	_INST_FROM (0xe1, 2, 6, "SBC([$1],X)",	SBC_ZeroPageIndirectX, SBC_General);
	_INST_FROM (0xf1, 2, 5, "SBC([$1]),Y",	SBC_ZeroPageIndirectY, SBC_General);
	_INST_FROM (0xf5, 2, 4, "SBC[$1],X",	SBC_ZeroPageX, SBC_General);
	_INST_FROM (0xfd, 3, 4, "SBC[$2],X",	SBC_AbsoluteX, SBC_General);
	_INST_FROM (0xf9, 3, 4, "SBC[$2],Y",	SBC_AbsoluteY, SBC_General);

	// SEC
	_INST_FROM (0x38, 1, 2, "SEC",			SEC, Instruction);

	// SED
	_INST_FROM (0xf8, 1, 2, "SED",			SED, Instruction);

	// SEI
	_INST_FROM (0x78, 1, 2, "SEI",			SEI, Instruction);

	// STA
	/** STA_General: To aggregate common steps in every STA instruction. */
	class STA_General : public Instruction
	{
		public:
		STA_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x8d, 3, 4, "STA[$2]",		STA_Absolute, STA_General);
	_INST_FROM (0x85, 2, 3, "STA[$1]",		STA_ZeroPage, STA_General);
	_INST_FROM (0x81, 2, 6, "STA([$1],X)",	STA_ZeroPageIndirectX, STA_General);
	_INST_FROM (0x91, 2, 6, "STA([$1]),Y",	STA_ZeroPageIndirectY, STA_General);
	_INST_FROM (0x95, 2, 4, "STA[$1],X",	STA_ZeroPageX, STA_General);
	_INST_FROM (0x9d, 3, 5, "STA[$2],X",	STA_AbsoluteX, STA_General);
	_INST_FROM (0x99, 3, 5, "STA[$2],Y",	STA_AbsoluteY, STA_General);

	// STX
	/** STX_General: To aggregate common steps in every STX instruction. */
	class STX_General : public Instruction
	{
		public:
		STX_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x8e, 3, 4, "STX[$2]",		STX_Absolute, STX_General);
	_INST_FROM (0x86, 2, 3, "STX[$1]",		STX_ZeroPage, STX_General);
	_INST_FROM (0x96, 2, 4, "STX[$1],Y",	STX_ZeroPageY, STX_General);

	// STY
	/** STY_General: To aggregate common steps in every STY instruction. */
	class STY_General : public Instruction
	{
		public:
		STY_General (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
			: Instruction (c, mp, cc, t)
							{ }

		protected:
		bool executeOn (const MCHEmul::Address& a);
	};

	_INST_FROM (0x8c, 3, 4, "STY[$2]",		STY_Absolute, STY_General);
	_INST_FROM (0x84, 2, 3, "STY[$1]",		STY_ZeroPage, STY_General);
	_INST_FROM (0x94, 2, 4, "STY[$1],X",	STY_ZeroPageX, STY_General);

	// TAX
	_INST_FROM (0xaa, 1, 2, "TAX",			TAX, Instruction);

	// TAY
	_INST_FROM (0xa8, 1, 2, "TAY",			TAY, Instruction);

	// TSX
	_INST_FROM (0xba, 1, 2, "TSX",			TSX, Instruction);

	// TXA
	_INST_FROM (0x8a, 1, 2, "TXA",			TXA, Instruction);

	// TXS
	_INST_FROM (0x9a, 1, 2, "TXS",			TXS, Instruction);

	// TYA
	_INST_FROM (0x98, 1, 2, "TYA",			TYA, Instruction);
}

#endif
  
// End of the file
/*@}*/
