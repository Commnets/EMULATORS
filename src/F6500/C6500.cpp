#include <F6500/C6500.hpp>
#include <F6500/Instructions.hpp>
#include <F6500/IRQInterrupt.hpp>
#include <F6500/NMIInterrupt.hpp>

// ---
F6500::C6500::C6500 (const MCHEmul::CPUArchitecture& a)
	: MCHEmul::CPU (a,
		F6500::C6500::createInternalRegisters (), 
		F6500::C6500::createStatusRegister (),
		F6500::C6500::createInstructions ())
{
	// The reference to the memory has not set still here...
	// It is linked to the CPU at computer (class) level!

	//This type of CPU admites two types of interruptions...
	addInterrupt (new F6500::IRQInterrupt);
	addInterrupt (new F6500::NMIInterrupt);
}

// ---
bool F6500::C6500::initialize ()
{
	bool result = MCHEmul::CPU::initialize ();

	// Sets thpoint where the execution starts!
	if (result)
		programCounter ().setAddress (MCHEmul::Address 
			(memoryRef () -> values (ResetVectorAddress (), 2), false /** Little - endian */));

	return (result);
}

// ---
MCHEmul::Registers F6500::C6500::createInternalRegisters ()
{
	return (MCHEmul::Registers
			({ MCHEmul::Register ((int) _ACCUMULATOR,	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _XREGISTER,		{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _YREGISTER,		{ MCHEmul::UByte::_0 } /** 1 byte long */) }));
}

// ---
MCHEmul::StatusRegister F6500::C6500::createStatusRegister ()
{
	return (MCHEmul::StatusRegister 
			(
				1 /** 1 byte long */,
				{ { "C", 0 /** bit number */ },		// Carry flag
				  { "Z", 1 },		// Zero flag
				  { "I", 2 },		// Interrupt disable flag
				  { "D", 3 },		// Decimal mode flag
				  { "B", 4 },		// Break flag
				  { "-", 5 },		// Not used
				  { "V", 6 },		// Overflow flag
				  { "N", 7 } }));	// Negative flag
}

// ---
MCHEmul::Instructions F6500::C6500::createInstructions ()
{
	MCHEmul::Instructions result;

	// ADC
	result [0x69] = new ADC_Inmediate;
	result [0x6D] = new ADC_Absolute;
	result [0x65] = new ADC_ZeroPage;
	result [0x61] = new ADC_ZeroPageIndirectX;
	result [0x71] = new ADC_ZeroPageIndirectY;
	result [0x75] = new ADC_ZeroPageX;
	result [0x7D] = new ADC_AbsoluteX;
	result [0x79] = new ADC_AbsoluteY;

	// AND
	result [0x29] = new AND_Inmediate;
	result [0x2D] = new AND_Absolute;
	result [0x25] = new AND_ZeroPage;
	result [0x21] = new AND_ZeroPageIndirectX;
	result [0x31] = new AND_ZeroPageIndirectY;
	result [0x35] = new AND_ZeroPageX;
	result [0x3D] = new AND_AbsoluteX;
	result [0x39] = new AND_AbsoluteY;

	// ASL
	result [0x0E] = new ASL_Absolute;
	result [0x06] = new ASL_ZeroPage;
	result [0x0A] = new ASL_Accumulator;
	result [0x16] = new ASL_ZeroPageX;
	result [0x1E] = new ASL_AbsoluteX;

	// BCC 
	result [0x90] = new BCC;

	// BCS 
	result [0xB0] = new BCS;

	// BEQ
	result [0xF0] = new BEQ;

	// BIT
	result [0x2C] = new BIT_Absolute;
	result [0x24] = new BIT_ZeroPage;

	// BMI
	result [0x30] = new BMI;

	// BNE
	result [0xD0] = new BNE;

	// BPL
	result [0x10] = new BPL;

	// BRK
	result [0x00] = new BRK;

	// BVC
	result [0x50] = new BVC;

	// BVS
	result [0x70] = new BVS;

	// CLC
	result [0x18] = new CLC;

	// CLD
	result [0xD8] = new CLD;

	// CLI
	result [0x58] = new CLI;

	// CLV
	result [0xB8] = new CLV;

	// CMP
	result [0xC9] = new CMP_Inmediate;
	result [0xCD] = new CMP_Absolute;
	result [0xC5] = new CMP_ZeroPage;
	result [0xC1] = new CMP_ZeroPageIndirectX;
	result [0xD1] = new CMP_ZeroPageIndirectY;
	result [0xD5] = new CMP_ZeroPageX;
	result [0xDD] = new CMP_AbsoluteX;
	result [0xD9] = new CMP_AbsoluteY;

	// CPX
	result [0xE0] = new CPX_Inmediate;
	result [0xEC] = new CPX_Absolute;
	result [0xE4] = new CPX_ZeroPage;

	// CPY
	result [0xC0] = new CPY_Inmediate;
	result [0xCC] = new CPY_Absolute;
	result [0xC4] = new CPY_ZeroPage;

	// DEC
	result [0xCE] = new DEC_Absolute;
	result [0xC6] = new DEC_ZeroPage;
	result [0xD6] = new DEC_ZeroPageX;
	result [0xDE] = new DEC_AbsoluteX;

	// DEX
	result [0xCA] = new DEX;

	// DEY
	result [0x88] = new DEY;

	// EOR
	result [0x49] = new EOR_Inmediate;
	result [0x4D] = new EOR_Absolute;
	result [0x45] = new EOR_ZeroPage;
	result [0x41] = new EOR_ZeroPageIndirectX;
	result [0x51] = new EOR_ZeroPageIndirectY;
	result [0x55] = new EOR_ZeroPageX;
	result [0x5D] = new EOR_AbsoluteX;
	result [0x59] = new EOR_AbsoluteY;

	// INC
	result [0xEE] = new INC_Absolute;
	result [0xE6] = new INC_ZeroPage;
	result [0xF6] = new INC_ZeroPageX;
	result [0xFE] = new INC_AbsoluteX;

	// INX
	result [0xE8] = new INX;

	// INY
	result [0xC8] = new INY;

	// JMP
	result [0x4C] = new JMP_Absolute;
	result [0x6C] = new JMP_Indirect;

	// JSR
	result [0x20] = new JSR_Absolute;

	// LDA
	result [0xA9] = new LDA_Inmediate;
	result [0xAD] = new LDA_Absolute;
	result [0xA5] = new LDA_ZeroPage;
	result [0xA1] = new LDA_ZeroPageIndirectX;
	result [0xB1] = new LDA_ZeroPageIndirectY;
	result [0xB5] = new LDA_ZeroPageX;
	result [0xBD] = new LDA_AbsoluteX;
	result [0xB9] = new LDA_AbsoluteY;

	// LDX
	result [0xA2] = new LDX_Inmediate;
	result [0xAE] = new LDX_Absolute;
	result [0xA6] = new LDX_ZeroPage;
	result [0xBE] = new LDX_AbsoluteY;
	result [0xB6] = new LDX_ZeroPageY;

	// LDY
	result [0xA0] = new LDY_Inmediate;
	result [0xAC] = new LDY_Absolute;
	result [0xA4] = new LDY_ZeroPage;
	result [0xB4] = new LDY_ZeroPageX;
	result [0xBC] = new LDY_AbsoluteX;

	// LSR
	result [0x4E] = new LSR_Absolute;
	result [0x46] = new LSR_ZeroPage;
	result [0x4A] = new LSR_Accumulator;
	result [0x56] = new LSR_ZeroPageX;
	result [0x5E] = new LSR_AbsoluteX;

	// NOP
	result [0xEA] = new NOP;

	// ORA
	result [0x09] = new ORA_Inmediate;
	result [0x0D] = new ORA_Absolute;
	result [0x05] = new ORA_ZeroPage;
	result [0x01] = new ORA_ZeroPageIndirectX;
	result [0x11] = new ORA_ZeroPageIndirectY;
	result [0x15] = new ORA_ZeroPageX;
	result [0x1D] = new ORA_AbsoluteX;
	result [0x19] = new ORA_AbsoluteY;

	// PHA
	result [0x48] = new PHA;

	// PHP
	result [0x08] = new PHP;

	// PLA
	result [0x68] = new PLA;

	// PLP
	result [0x28] = new	PLP;

	// ROL
	result [0x2E] = new ROL_Absolute;
	result [0x26] = new ROL_ZeroPage;
	result [0x2A] = new ROL_Accumulator;
	result [0x36] = new ROL_ZeroPageX;
	result [0x3E] = new ROL_AbsoluteX;

	// ROR
	result [0x6E] = new ROR_Absolute;
	result [0x66] = new ROR_ZeroPage;
	result [0x6A] = new ROR_Accumulator;
	result [0x76] = new ROR_ZeroPageX;
	result [0x7E] = new ROR_AbsoluteX;

	// RTI
	result [0x40] = new RTI;

	// RTS
	result [0x60] = new RTS;

	// SBC
	result [0xE9] = new SBC_Inmediate;
	result [0xED] = new SBC_Absolute;
	result [0xE5] = new SBC_ZeroPage;
	result [0xE1] = new SBC_ZeroPageIndirectX;
	result [0xF1] = new SBC_ZeroPageIndirectY;
	result [0xF5] = new SBC_ZeroPageX;
	result [0xFD] = new SBC_AbsoluteX;
	result [0xF9] = new SBC_AbsoluteY;

	// SEC
	result [0x38] = new SEC;

	// SED
	result [0xF8] = new SED;

	// SEI
	result [0x78] = new SEI;

	// STA
	result [0x8D] = new STA_Absolute;
	result [0x85] = new STA_ZeroPage;
	result [0x81] = new STA_ZeroPageIndirectX;
	result [0x91] = new STA_ZeroPageIndirectY;
	result [0x95] = new STA_ZeroPageX;
	result [0x9D] = new STA_AbsoluteX;
	result [0x99] = new STA_AbsoluteY;

	// STX
	result [0x8E] = new STX_Absolute;
	result [0x86] = new STX_ZeroPage;
	result [0x96] = new STX_ZeroPageY;

	// STY
	result [0x8C] = new STY_Absolute;
	result [0x84] = new STY_ZeroPage;
	result [0x94] = new STY_ZeroPageX;

	// TAX
	result [0xAA] = new TAX;

	// TAY
	result [0xA8] = new TAY;

	// TSX
	result [0xBA] = new TSX;

	// TXA
	result [0x8A] = new TXA;

	// TXS
	result [0x9A] = new TXS;

	// TYA
	result [0x98] = new TYA;

	assert (result.size () == 151); 

	return (result);
}
