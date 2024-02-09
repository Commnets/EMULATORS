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
F6500::C6500::C6500 (int id,	const MCHEmul::Attributes& attrs)
	: MCHEmul::CPU (id,
		F6500::C6500::createArchitecture (),
		F6500::C6500::createInternalRegisters (), 
		F6500::C6500::createStatusRegister (),
		F6500::C6500::createInstructions (),
		attrs)
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
MCHEmul::CPUArchitecture F6500::C6500::createArchitecture ()
{
	return (
		MCHEmul::CPUArchitecture 
			(2 /** 2 bytes = 16 bites */, 
			 1 /** bytes per instruction */, 
			 false /** Little endian. */,
			 { { "Family", "6500" } }));
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
	result [0x69] = new F6500::ADC_Inmediate;
	result [0x6D] = new F6500::ADC_Absolute;
	result [0x65] = new F6500::ADC_ZeroPage;
	result [0x61] = new F6500::ADC_ZeroPageIndirectX;
	result [0x71] = new F6500::ADC_ZeroPageIndirectY;
	result [0x75] = new F6500::ADC_ZeroPageX;
	result [0x7D] = new F6500::ADC_AbsoluteX;
	result [0x79] = new F6500::ADC_AbsoluteY;

	// ALR: AND + LSR
	// Instructions totally undocumented...
	result [0x4B] = new F6500::ALR_Inmediate;

	// ANC: AND + Carry Flag
	// Instructions totally undocumented...
	result [0x0B] = new F6500::ANC_Inmediate;
	result [0x2B] = new F6500::ANC_Inmediate;

	// AND
	result [0x29] = new F6500::AND_Inmediate;
	result [0x2D] = new F6500::AND_Absolute;
	result [0x25] = new F6500::AND_ZeroPage;
	result [0x21] = new F6500::AND_ZeroPageIndirectX;
	result [0x31] = new F6500::AND_ZeroPageIndirectY;
	result [0x35] = new F6500::AND_ZeroPageX;
	result [0x3D] = new F6500::AND_AbsoluteX;
	result [0x39] = new F6500::AND_AbsoluteY;

	// ARR: AND + ROR
	// Instructions totally undocumented...
	result [0x6B] = new F6500::ARR_Inmediate;

	// ASL
	result [0x0E] = new F6500::ASL_Absolute;
	result [0x06] = new F6500::ASL_ZeroPage;
	result [0x0A] = new F6500::ASL_Accumulator;
	result [0x16] = new F6500::ASL_ZeroPageX;
	result [0x1E] = new F6500::ASL_AbsoluteX;

	// BCC 
	result [0x90] = new F6500::BCC;

	// BCS 
	result [0xB0] = new F6500::BCS;

	// BEQ
	result [0xF0] = new F6500::BEQ;

	// BIT
	result [0x2C] = new F6500::BIT_Absolute;
	result [0x24] = new F6500::BIT_ZeroPage;

	// BMI
	result [0x30] = new F6500::BMI;

	// BNE
	result [0xD0] = new F6500::BNE;

	// BPL
	result [0x10] = new F6500::BPL;

	// BRK
	result [0x00] = new F6500::BRK;

	// BVC
	result [0x50] = new F6500::BVC;

	// BVS
	result [0x70] = new F6500::BVS;

	// CLC
	result [0x18] = new F6500::CLC;

	// CLD
	result [0xD8] = new F6500::CLD;

	// CLI
	result [0x58] = new F6500::CLI;

	// CLV
	result [0xB8] = new F6500::CLV;

	// CMP
	result [0xC9] = new F6500::CMP_Inmediate;
	result [0xCD] = new F6500::CMP_Absolute;
	result [0xC5] = new F6500::CMP_ZeroPage;
	result [0xC1] = new F6500::CMP_ZeroPageIndirectX;
	result [0xD1] = new F6500::CMP_ZeroPageIndirectY;
	result [0xD5] = new F6500::CMP_ZeroPageX;
	result [0xDD] = new F6500::CMP_AbsoluteX;
	result [0xD9] = new F6500::CMP_AbsoluteY;

	// CPX
	result [0xE0] = new F6500::CPX_Inmediate;
	result [0xEC] = new F6500::CPX_Absolute;
	result [0xE4] = new F6500::CPX_ZeroPage;

	// CPY
	result [0xC0] = new F6500::CPY_Inmediate;
	result [0xCC] = new F6500::CPY_Absolute;
	result [0xC4] = new F6500::CPY_ZeroPage;

	// DCP: DEC + CMP
	// Instructions totally undocumented...
	result [0xC7] = new F6500::DCP_ZeroPage;
	result [0xD7] = new F6500::DCP_ZeroPageX;
	result [0xC3] = new F6500::DCP_ZeroPageIndirectX;
	result [0xD3] = new F6500::DCP_ZeroPageIndirectY;
	result [0xCF] = new F6500::DCP_Absolute;
	result [0xDF] = new F6500::DCP_AbsoluteX;
	result [0xDB] = new F6500::DCP_AbsoluteY;

	// DEC
	result [0xCE] = new F6500::DEC_Absolute;
	result [0xC6] = new F6500::DEC_ZeroPage;
	result [0xD6] = new F6500::DEC_ZeroPageX;
	result [0xDE] = new F6500::DEC_AbsoluteX;

	// DEX
	result [0xCA] = new F6500::DEX;

	// DEY
	result [0x88] = new F6500::DEY;

	// EOR
	result [0x49] = new F6500::EOR_Inmediate;
	result [0x4D] = new F6500::EOR_Absolute;
	result [0x45] = new F6500::EOR_ZeroPage;
	result [0x41] = new F6500::EOR_ZeroPageIndirectX;
	result [0x51] = new F6500::EOR_ZeroPageIndirectY;
	result [0x55] = new F6500::EOR_ZeroPageX;
	result [0x5D] = new F6500::EOR_AbsoluteX;
	result [0x59] = new F6500::EOR_AbsoluteY;

	// INC
	result [0xEE] = new F6500::INC_Absolute;
	result [0xE6] = new F6500::INC_ZeroPage;
	result [0xF6] = new F6500::INC_ZeroPageX;
	result [0xFE] = new F6500::INC_AbsoluteX;

	// INX
	result [0xE8] = new F6500::INX;

	// INY
	result [0xC8] = new F6500::INY;

	// ISC: INC + SBC
	// Instructions totally undocumented...
	result [0xE7] = new F6500::ISC_ZeroPage;
	result [0xF7] = new F6500::ISC_ZeroPageX;
	result [0xE3] = new F6500::ISC_ZeroPageIndirectX;
	result [0xF3] = new F6500::ISC_ZeroPageIndirectY;
	result [0xEf] = new F6500::ISC_Absolute;
	result [0xFF] = new F6500::ISC_AbsoluteX;
	result [0xFB] = new F6500::ISC_AbsoluteY;

	// JAM: Restart the CPU
	// Totally undocumented
	result [0x02] = new F6500::JAM;
	result [0x12] = new F6500::JAM;
	result [0x22] = new F6500::JAM;
	result [0x32] = new F6500::JAM;
	result [0x42] = new F6500::JAM;
	result [0x52] = new F6500::JAM;
	result [0x62] = new F6500::JAM;
	result [0x72] = new F6500::JAM;
	result [0x92] = new F6500::JAM;
	result [0xB2] = new F6500::JAM;
	result [0xD2] = new F6500::JAM;
	result [0xF2] = new F6500::JAM;

	// JMP
	result [0x4C] = new F6500::JMP_Absolute;
	result [0x6C] = new F6500::JMP_Indirect;

	// JSR
	result [0x20] = new F6500::JSR_Absolute;

	// LAX: LDA + TAX
	// Instructions totally undocumented...
	result [0xA7] = new F6500::LAX_ZeroPage;
	result [0xB7] = new F6500::LAX_ZeroPageY;
	result [0xA3] = new F6500::LAX_ZeroPageIndirectX;
	result [0xB3] = new F6500::LAX_ZeroPageIndirectY;
	result [0xAF] = new F6500::LAX_Absolute;
	result [0xBF] = new F6500::LAX_AbsoluteY;

	// LDA
	result [0xA9] = new F6500::LDA_Inmediate;
	result [0xAD] = new F6500::LDA_Absolute;
	result [0xA5] = new F6500::LDA_ZeroPage;
	result [0xA1] = new F6500::LDA_ZeroPageIndirectX;
	result [0xB1] = new F6500::LDA_ZeroPageIndirectY;
	result [0xB5] = new F6500::LDA_ZeroPageX;
	result [0xBD] = new F6500::LDA_AbsoluteX;
	result [0xB9] = new F6500::LDA_AbsoluteY;

	// LDX
	result [0xA2] = new F6500::LDX_Inmediate;
	result [0xAE] = new F6500::LDX_Absolute;
	result [0xA6] = new F6500::LDX_ZeroPage;
	result [0xBE] = new F6500::LDX_AbsoluteY;
	result [0xB6] = new F6500::LDX_ZeroPageY;

	// LDY
	result [0xA0] = new F6500::LDY_Inmediate;
	result [0xAC] = new F6500::LDY_Absolute;
	result [0xA4] = new F6500::LDY_ZeroPage;
	result [0xB4] = new F6500::LDY_ZeroPageX;
	result [0xBC] = new F6500::LDY_AbsoluteX;

	// LSR
	result [0x4E] = new F6500::LSR_Absolute;
	result [0x46] = new F6500::LSR_ZeroPage;
	result [0x4A] = new F6500::LSR_Accumulator;
	result [0x56] = new F6500::LSR_ZeroPageX;
	result [0x5E] = new F6500::LSR_AbsoluteX;

	// LXA is not created, very unestable...
	// AB code...

	// NOP
	// The official one...
	result [0xEA] = new F6500::NOP;
	// The un official ones...
	result [0x1A] = new F6500::NOP;
	result [0x3A] = new F6500::NOP;
	result [0x5A] = new F6500::NOP;
	result [0x7A] = new F6500::NOP;
	result [0xDA] = new F6500::NOP;
	result [0xFA] = new F6500::NOP;
	result [0x80] = new F6500::NOP_Inmediate;
	result [0x82] = new F6500::NOP_Inmediate;
	result [0x89] = new F6500::NOP_Inmediate;
	result [0xC2] = new F6500::NOP_Inmediate;
	result [0xE2] = new F6500::NOP_Inmediate;
	result [0x04] = new F6500::NOP_ZeroPage;
	result [0x44] = new F6500::NOP_ZeroPage;
	result [0x64] = new F6500::NOP_ZeroPage;
	result [0x14] = new F6500::NOP_ZeroPageX;
	result [0x34] = new F6500::NOP_ZeroPageX;
	result [0x54] = new F6500::NOP_ZeroPageX;
	result [0x74] = new F6500::NOP_ZeroPageX;
	result [0xD4] = new F6500::NOP_ZeroPageX;
	result [0xF4] = new F6500::NOP_ZeroPageX;
	result [0x0C] = new F6500::NOP_Absolute;
	result [0x1C] = new F6500::NOP_AbsoluteX;
	result [0x3C] = new F6500::NOP_AbsoluteX;
	result [0x5C] = new F6500::NOP_AbsoluteX;
	result [0x7C] = new F6500::NOP_AbsoluteX;
	result [0xDC] = new F6500::NOP_AbsoluteX;
	result [0xFC] = new F6500::NOP_AbsoluteX;

	// ORA
	result [0x09] = new F6500::ORA_Inmediate;
	result [0x0D] = new F6500::ORA_Absolute;
	result [0x05] = new F6500::ORA_ZeroPage;
	result [0x01] = new F6500::ORA_ZeroPageIndirectX;
	result [0x11] = new F6500::ORA_ZeroPageIndirectY;
	result [0x15] = new F6500::ORA_ZeroPageX;
	result [0x1D] = new F6500::ORA_AbsoluteX;
	result [0x19] = new F6500::ORA_AbsoluteY;

	// PHA
	result [0x48] = new F6500::PHA;

	// PHP
	result [0x08] = new F6500::PHP;

	// PLA
	result [0x68] = new F6500::PLA;

	// PLP
	result [0x28] = new	PLP;

	// RLA: ROL + AND
	// Instructions totally undocumented...
	result [0x27] = new F6500::RLA_ZeroPage;
	result [0x37] = new F6500::RLA_ZeroPageX;
	result [0x23] = new F6500::RLA_ZeroPageIndirectX;
	result [0x33] = new F6500::RLA_ZeroPageIndirectY;
	result [0x2F] = new F6500::RLA_Absolute;
	result [0x3F] = new F6500::RLA_AbsoluteX;
	result [0x3B] = new F6500::RLA_AbsoluteY;

	// ROL
	result [0x2E] = new F6500::ROL_Absolute;
	result [0x26] = new F6500::ROL_ZeroPage;
	result [0x2A] = new F6500::ROL_Accumulator;
	result [0x36] = new F6500::ROL_ZeroPageX;
	result [0x3E] = new F6500::ROL_AbsoluteX;

	// ROR
	result [0x6E] = new F6500::ROR_Absolute;
	result [0x66] = new F6500::ROR_ZeroPage;
	result [0x6A] = new F6500::ROR_Accumulator;
	result [0x76] = new F6500::ROR_ZeroPageX;
	result [0x7E] = new F6500::ROR_AbsoluteX;

	// RRA: ROR + ADC
	// Instructions totally undocumented...
	result [0x67] = new F6500::RRA_ZeroPage;
	result [0x77] = new F6500::RRA_ZeroPageX;
	result [0x63] = new F6500::RRA_ZeroPageIndirectX;
	result [0x73] = new F6500::RRA_ZeroPageIndirectY;
	result [0x6F] = new F6500::RRA_Absolute;
	result [0x7F] = new F6500::RRA_AbsoluteX;
	result [0x7B] = new F6500::RRA_AbsoluteY;

	// RTI
	result [0x40] = new F6500::RTI;

	// RTS
	result [0x60] = new F6500::RTS;

	// SAX: PHP + PHA + STX + AND + STA + PLA + PLP
	// Instructions totally undocumented...
	result [0x87] = new F6500::SAX_ZeroPage;
	result [0x97] = new F6500::SAX_ZeroPageY;
	result [0x83] = new F6500::SAX_ZeroPageIndirectX;
	result [0x8F] = new F6500::SAX_Absolute;

	// SBC
	result [0xE9] = new F6500::SBC_Inmediate;
	result [0xED] = new F6500::SBC_Absolute;
	result [0xE5] = new F6500::SBC_ZeroPage;
	result [0xE1] = new F6500::SBC_ZeroPageIndirectX;
	result [0xF1] = new F6500::SBC_ZeroPageIndirectY;
	result [0xF5] = new F6500::SBC_ZeroPageX;
	result [0xFD] = new F6500::SBC_AbsoluteX;
	result [0xF9] = new F6500::SBC_AbsoluteY;
	// SBC Not documented...
	result [0xEB] = new F6500::SBC_Inmediate;

	// SBX: STA + TXA + AND + CMP + PHP + SEC + CLD + SBC + TAX + LDA + PLP
	// Instructions totally undocumented...
	result [0xCB] = new F6500::SBX_Inmediate;

	// SHA is not created, very unestable.
	// Code 9F, 93
	// SHX is not created, very unestable.
	// Code 9E
	// SHY is nor created, very unestable.
	// Code 9C

	// SEC
	result [0x38] = new F6500::SEC;

	// SED
	result [0xF8] = new F6500::SED;

	// SEI
	result [0x78] = new F6500::SEI;

	// SLO: ASL + ORA
	// Instructions totally undocumented...
	result [0x07] = new F6500::SLO_ZeroPage;
	result [0x17] = new F6500::SLO_ZeroPageX;
	result [0x03] = new F6500::SLO_ZeroPageIndirectX;
	result [0x13] = new F6500::SLO_ZeroPageIndirectY;
	result [0x0F] = new F6500::SLO_Absolute;
	result [0x1F] = new F6500::SLO_AbsoluteX;
	result [0x1B] = new F6500::SLO_AbsoluteY;

	// SRE: LSR + EOR
	// Instructions totally undocumented...
	result [0x47] = new F6500::SRE_ZeroPage;
	result [0x57] = new F6500::SRE_ZeroPageX;
	result [0x43] = new F6500::SRE_ZeroPageIndirectX;
	result [0x53] = new F6500::SRE_ZeroPageIndirectY;
	result [0x4F] = new F6500::SRE_Absolute;
	result [0x5F] = new F6500::SRE_AbsoluteX;
	result [0x5B] = new F6500::SRE_AbsoluteY;

	// STA
	result [0x8D] = new F6500::STA_Absolute;
	result [0x85] = new F6500::STA_ZeroPage;
	result [0x81] = new F6500::STA_ZeroPageIndirectX;
	result [0x91] = new F6500::STA_ZeroPageIndirectY;
	result [0x95] = new F6500::STA_ZeroPageX;
	result [0x9D] = new F6500::STA_AbsoluteX;
	result [0x99] = new F6500::STA_AbsoluteY;

	// STX
	result [0x8E] = new F6500::STX_Absolute;
	result [0x86] = new F6500::STX_ZeroPage;
	result [0x96] = new F6500::STX_ZeroPageY;

	// STY
	result [0x8C] = new F6500::STY_Absolute;
	result [0x84] = new F6500::STY_ZeroPage;
	result [0x94] = new F6500::STY_ZeroPageX;

	// TAS not created, very unestable...
	// Code 9B

	// TAX
	result [0xAA] = new F6500::TAX;

	// TAY
	result [0xA8] = new F6500::TAY;

	// TSX
	result [0xBA] = new F6500::TSX;

	// TXA
	result [0x8A] = new F6500::TXA;

	// TXS
	result [0x9A] = new F6500::TXS;

	// TYA
	result [0x98] = new F6500::TYA;

	assert (result.size () == 245); 

	return (result);
}
