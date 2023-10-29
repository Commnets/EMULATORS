#include <F6500/C6500.hpp>
#include <F6500/Instructions.hpp>
#include <F6500/IRQInterrupt.hpp>
#include <F6500/NMIInterrupt.hpp>

const std::string F6500::C6500::_CARRYFLAGNAME = "C";
const std::string F6500::C6500::_ZEROFLAGNAME = "Z";
const std::string F6500::C6500::_IRQFLAGNAME = "I";
const std::string F6500::C6500::_DECIMALFLAGNAME = "D";
const std::string F6500::C6500::_BREAKFLAGNAME = "B";
const std::string F6500::C6500::_OVERFLOWFLAGNAME = "V";
const std::string F6500::C6500::_NEGATIVEFLAGNAME = "N";

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
	if (!MCHEmul::CPU::initialize ())
		return (false);

	// After 6 clock sycles, everything starts!
	addClockCycles (6);
	// Initially the interruptions are disabled!
	statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);
	// Sets the point where the execution starts!
	programCounter ().setAddress (MCHEmul::Address 
		(memoryRef () -> values (ResetVectorAddress (), 2), false /** Little - endian */));

	return (true);
}

// ---
MCHEmul::Registers F6500::C6500::createInternalRegisters ()
{
	return (MCHEmul::Registers
			({ 
			   MCHEmul::Register ((int) _ACCUMULATOR,	"A",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _XREGISTER,		"X",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _YREGISTER,		"Y",	{ MCHEmul::UByte::_0 } /** 1 byte long */) 
			 }));
}

// ---
MCHEmul::StatusRegister F6500::C6500::createStatusRegister ()
{
	return (MCHEmul::StatusRegister 
			(
				1 /** 1 byte long */,
				{ { _CARRYFLAGNAME, _CARRYFLAG /** bit number */ },	// Carry flag
				  { _ZEROFLAGNAME, _ZEROFLAG },						// Zero flag
				  { _IRQFLAGNAME, _IRQFLAG },						// Interrupt disable flag
				  { _DECIMALFLAGNAME, _DECIMALFLAG },				// Decimal mode flag
				  { _BREAKFLAGNAME, _BREAKFLAG },					// Break flag
				  { "-", 5 },										// Not used
				  { _OVERFLOWFLAGNAME, _OVERFLOWFLAG },				// Overflow flag
				  { _NEGATIVEFLAGNAME, _NEGATIVEFLAG } }));			// Negative flag
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

	// ALR: AND + LSR
	// Instructions totally undocumented...
	result [0x4B] = new ALR_Inmediate;

	// ANC: AND + Carry Flag
	// Instructions totally undocumented...
	result [0x0B] = new ANC_Inmediate;
	result [0x2B] = new ANC_Inmediate;

	// AND
	result [0x29] = new AND_Inmediate;
	result [0x2D] = new AND_Absolute;
	result [0x25] = new AND_ZeroPage;
	result [0x21] = new AND_ZeroPageIndirectX;
	result [0x31] = new AND_ZeroPageIndirectY;
	result [0x35] = new AND_ZeroPageX;
	result [0x3D] = new AND_AbsoluteX;
	result [0x39] = new AND_AbsoluteY;

	// ARR: AND + ROR
	// Instructions totally undocumented...
	result [0x6B] = new ARR_Inmediate;

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

	// DCP: DEC + CMP
	// Instructions totally undocumented...
	result [0xC7] = new DCP_ZeroPage;
	result [0xD7] = new DCP_ZeroPageX;
	result [0xC3] = new DCP_ZeroPageIndirectX;
	result [0xD3] = new DCP_ZeroPageIndirectY;
	result [0xCF] = new DCP_Absolute;
	result [0xDF] = new DCP_AbsoluteX;
	result [0xDB] = new DCP_AbsoluteY;

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

	// ISC: INC + SBC
	// Instructions totally undocumented...
	result [0xE7] = new ISC_ZeroPage;
	result [0xF7] = new ISC_ZeroPageX;
	result [0xE3] = new ISC_ZeroPageIndirectX;
	result [0xF3] = new ISC_ZeroPageIndirectY;
	result [0xEf] = new ISC_Absolute;
	result [0xFF] = new ISC_AbsoluteX;
	result [0xFB] = new ISC_AbsoluteY;

	// JAM: Restart the CPU
	// Totally undocumented
	result [0x02] = new JAM;
	result [0x12] = new JAM;
	result [0x22] = new JAM;
	result [0x32] = new JAM;
	result [0x42] = new JAM;
	result [0x52] = new JAM;
	result [0x62] = new JAM;
	result [0x72] = new JAM;
	result [0x92] = new JAM;
	result [0xB2] = new JAM;
	result [0xD2] = new JAM;
	result [0xF2] = new JAM;

	// JMP
	result [0x4C] = new JMP_Absolute;
	result [0x6C] = new JMP_Indirect;

	// JSR
	result [0x20] = new JSR_Absolute;

	// LAX: LDA + TAX
	// Instructions totally undocumented...
	result [0xA7] = new LAX_ZeroPage;
	result [0xB7] = new LAX_ZeroPageY;
	result [0xA3] = new LAX_ZeroPageIndirectX;
	result [0xB3] = new LAX_ZeroPageIndirectY;
	result [0xAF] = new LAX_Absolute;
	result [0xBF] = new LAX_AbsoluteY;

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
	// The official one...
	result [0xEA] = new NOP;
	// The un official ones...
	result [0x1A] = new NOP;
	result [0x3A] = new NOP;
	result [0x5A] = new NOP;
	result [0x7A] = new NOP;
	result [0xDA] = new NOP;
	result [0xFA] = new NOP;
	result [0x80] = new NOP_Inmediate;
	// The inestable versions are not created...
	result [0x89] = new NOP_Inmediate;
	result [0x04] = new NOP_ZeroPage;
	result [0x44] = new NOP_ZeroPage;
	result [0x64] = new NOP_ZeroPage;
	result [0x14] = new NOP_ZeroPageX;
	result [0x34] = new NOP_ZeroPageX;
	result [0x54] = new NOP_ZeroPageX;
	result [0x74] = new NOP_ZeroPageX;
	result [0xD4] = new NOP_ZeroPageX;
	result [0xF4] = new NOP_ZeroPageX;
	result [0x0C] = new NOP_Absolute;
	result [0x1C] = new NOP_AbsoluteX;
	result [0x3C] = new NOP_AbsoluteX;
	result [0x5C] = new NOP_AbsoluteX;
	result [0x7C] = new NOP_AbsoluteX;
	result [0xDC] = new NOP_AbsoluteX;
	result [0xFC] = new NOP_AbsoluteX;

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

	// RLA: ROL + AND
	// Instructions totally undocumented...
	result [0x27] = new RLA_ZeroPage;
	result [0x37] = new RLA_ZeroPageX;
	result [0x23] = new RLA_ZeroPageIndirectX;
	result [0x33] = new RLA_ZeroPageIndirectY;
	result [0x2F] = new RLA_Absolute;
	result [0x3F] = new RLA_AbsoluteX;
	result [0x3B] = new RLA_AbsoluteY;

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

	// RRA: ROR + ADC
	// Instructions totally undocumented...
	result [0x67] = new RRA_ZeroPage;
	result [0x77] = new RRA_ZeroPageX;
	result [0x63] = new RRA_ZeroPageIndirectX;
	result [0x73] = new RRA_ZeroPageIndirectY;
	result [0x6F] = new RRA_Absolute;
	result [0x7F] = new RRA_AbsoluteX;
	result [0x7B] = new RRA_AbsoluteY;

	// RTI
	result [0x40] = new RTI;

	// RTS
	result [0x60] = new RTS;

	// SAX: PHP + PHA + STX + AND + STA + PLA + PLP
	// Instructions totally undocumented...
	result [0x87] = new SAX_ZeroPage;
	result [0x97] = new SAX_ZeroPageY;
	result [0x83] = new SAX_ZeroPageIndirectX;
	result [0x8F] = new SAX_Absolute;

	// SBC
	result [0xE9] = new SBC_Inmediate;
	result [0xED] = new SBC_Absolute;
	result [0xE5] = new SBC_ZeroPage;
	result [0xE1] = new SBC_ZeroPageIndirectX;
	result [0xF1] = new SBC_ZeroPageIndirectY;
	result [0xF5] = new SBC_ZeroPageX;
	result [0xFD] = new SBC_AbsoluteX;
	result [0xF9] = new SBC_AbsoluteY;
	// SBC Not documented...
	result [0xEB] = new SBC_Inmediate;

	// SBX: STA + TXA + AND + CMP + PHP + SEC + CLD + SBC + TAX + LDA + PLP
	// Instructions totally undocumented...
	result [0xCB] = new SBX_Inmediate;

	// SEC
	result [0x38] = new SEC;

	// SED
	result [0xF8] = new SED;

	// SEI
	result [0x78] = new SEI;

	// SLO: ASL + ORA
	// Instructions totally undocumented...
	result [0x07] = new SLO_ZeroPage;
	result [0x17] = new SLO_ZeroPageX;
	result [0x03] = new SLO_ZeroPageIndirectX;
	result [0x13] = new SLO_ZeroPageIndirectY;
	result [0x0F] = new SLO_Absolute;
	result [0x1F] = new SLO_AbsoluteX;
	result [0x1B] = new SLO_AbsoluteY;

	// SRE: LSR + EOR
	// Instructions totally undocumented...
	result [0x47] = new SRE_ZeroPage;
	result [0x57] = new SRE_ZeroPageX;
	result [0x43] = new SRE_ZeroPageIndirectX;
	result [0x53] = new SRE_ZeroPageIndirectY;
	result [0x4F] = new SRE_Absolute;
	result [0x5F] = new SRE_AbsoluteX;
	result [0x5B] = new SRE_AbsoluteY;

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

	assert (result.size () == 245); 

	return (result);
}
