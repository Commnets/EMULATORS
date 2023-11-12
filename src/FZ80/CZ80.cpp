#include <FZ80/CZ80.hpp>
#include <FZ80/Instructions.hpp>
#include <FZ80/INTInterrupt.hpp>
#include <FZ80/NMIInterrupt.hpp>

const std::string FZ80::CZ80::_CARRYFLAGNAME = "C";
const std::string FZ80::CZ80::_NEGATIVEFLAGNAME = "N";
const std::string FZ80::CZ80::_PARITYOVERFLOWFLAGNAME = "PO";
const std::string FZ80::CZ80::_HALFCARRYFLAGNAME = "H";
const std::string FZ80::CZ80::_ZEROFLAGNAME = "Z";
const std::string FZ80::CZ80::_SIGNFLAGNAME = "S";

// ---
FZ80::CZ80::CZ80 (const MCHEmul::CPUArchitecture& a)
	: MCHEmul::CPU (a,
		FZ80::CZ80::createInternalRegisters (), 
		FZ80::CZ80::createStatusRegister (),
		FZ80::CZ80::createInstructions ()),
	  _afRegister  ({ &aRegister   (), &fRegister   () }),
	  _bcRegister  ({ &bRegister   (), &cRegister   () }),
	  _deRegister  ({ &dRegister   (), &eRegister   () }),
	  _hlRegister  ({ &hRegister   (), &lRegister   () }),
	  _afpRegister ({ &apRegister  (), &fpRegister  () }),
	  _bcpRegister ({ &bpRegister  (), &cpRegister  () }),
	  _depRegister ({ &dpRegister  (), &epRegister  () }),
	  _hlpRegister ({ &hpRegister  (), &lpRegister  () }),
	  _ixRegister  ({ &ixhRegister (), &ixlRegister () }),
	  _iyRegister  ({ &iyhRegister (), &iylRegister () })
{
	// The reference to the memory has not set still here...
	// It is linked to the CPU at computer (class) level!

	//This type of CPU admites two types of interruptions...
	addInterrupt (new FZ80::INTInterrupt);
	addInterrupt (new FZ80::NMIInterrupt);
}

// ---
MCHEmul::CPUArchitecture FZ80::CZ80::createArchitecture ()
{
	return (
		MCHEmul::CPUArchitecture 
			(2 /** 2 bytes = 16 bites */, 
			 1 /** bytes per instruction */, 
			 false /** Little endian. */, 
			 { { "Code", "Z80" },
			   { "Manufacturer", "Zilog"},
			   { "Year", "1976" },
			   { "Speed Range", "2.5 - 10 MHz" } }));
}

// ---
MCHEmul::Registers FZ80::CZ80::createInternalRegisters ()
{
	return (MCHEmul::Registers
			({ 
			   // Main Registers...
			   MCHEmul::Register ((int) _AREGISTER,		"A",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // F is the status register...
			   MCHEmul::Register ((int) _BREGISTER,		"B",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _CREGISTER,		"C",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _DREGISTER,		"D",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _EREGISTER,		"E",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _HREGISTER,		"H",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _LREGISTER,		"L",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // Main Registers Copy...
			   MCHEmul::Register ((int) _APREGISTER,	"AP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _FPREGISTER,	"FP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _BPREGISTER,	"BP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _CPREGISTER,	"CP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _DPREGISTER,	"DP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _EPREGISTER,	"EP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _HPREGISTER,	"HP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _LPREGISTER,	"LP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // Index Registers...
			   MCHEmul::Register ((int) _IXHREGISTER,	"IXH",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _IXLREGISTER,	"IXL",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _IYHREGISTER,	"IYH",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _IYLREGISTER,	"IYL",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // Special Registers...
			   MCHEmul::Register ((int) _IREGISTER,		"I",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _RREGISTER,		"R",	{ MCHEmul::UByte::_0 } /** 1 byte long */)
			 }));
}

// ---
MCHEmul::StatusRegister FZ80::CZ80::createStatusRegister ()
{
	return (MCHEmul::StatusRegister 
			(
				1 /** 1 byte long */,
				{ { _CARRYFLAGNAME, _CARRYFLAG /** bit number */ },		// Carry flag
				  { _NEGATIVEFLAGNAME, _NEGATIVEFLAG },					// Negative flag
				  { _PARITYOVERFLOWFLAGNAME, _PARITYOVERFLOWFLAG },		// Parity Overflow flag
				  { "-", 3 },											// Not used...
				  { _HALFCARRYFLAGNAME, _HALFCARRYFLAG },				// Half Carry flag
				  { "-", 5 },											// Not used...
				  { _ZEROFLAGNAME, _ZEROFLAG },							// Zero flag
				  { _SIGNFLAGNAME, _SIGNFLAG } }));						// Sign flag
}

// ---
MCHEmul::Instructions FZ80::CZ80::createInstructions ()
{
	MCHEmul::Instructions result;

	// First creates, the groups of instructions....
	MCHEmul::Instructions DDInstructions;
	MCHEmul::Instructions EDInstructions;
	MCHEmul::Instructions FDInstructions;

	// LD
	result [0x3E]			= new LD_A;
	result [0x7F]			= new LD_AFromA;
	result [0x78]			= new LD_AFromB;
	result [0x79]			= new LD_AFromC;
	result [0x7A]			= new LD_AFromD;
	result [0x7B]			= new LD_AFromE;
	result [0x7C]			= new LD_AFromF;
	result [0x7D]			= new LD_AFromL;
	EDInstructions [0xED57] = new LD_AFromI;
	EDInstructions [0xED5F] = new LD_AFromR;
	result [0x7E]			= new LD_AFromAddressHL;
	result [0x0A]			= new LD_AFromAddressBC;
	result [0x1A]			= new LD_AFromAddressDE;
	DDInstructions [0xDD7E]	= new LD_AFromAddressIndexIX;
	FDInstructions [0xFD7E]	= new LD_AFromAddressIndexIY;
	result [0x3A]			= new LD_AFromAddress;
	result [0x06]			= new LD_B;
	result [0x47]			= new LD_BFromA;
	result [0x40]			= new LD_BFromB;
	result [0x41]			= new LD_BFromC;
	result [0x42]			= new LD_BFromD;
	result [0x43]			= new LD_BFromE;
	result [0x44]			= new LD_BFromF;
	result [0x45]			= new LD_BFromL;
	DDInstructions [0xDD46]	= new LD_BFromAddressIndexIX;
	FDInstructions [0xFD46]	= new LD_BFromAddressIndexIY;
	result [0x0E]			= new LD_C;
	result [0x4F]			= new LD_CFromA;
	result [0x48]			= new LD_CFromB;
	result [0x49]			= new LD_CFromC;
	result [0x4A]			= new LD_CFromD;
	result [0x4B]			= new LD_CFromE;
	result [0x4C]			= new LD_CFromF;
	result [0x4D]			= new LD_CFromL;
	DDInstructions [0xDD4E]	= new LD_CFromAddressIndexIX;
	FDInstructions [0xFD4E]	= new LD_CFromAddressIndexIY;
	result [0x16]			= new LD_D;
	result [0x57]			= new LD_DFromA;
	result [0x50]			= new LD_DFromB;
	result [0x51]			= new LD_DFromC;
	result [0x52]			= new LD_DFromD;
	result [0x53]			= new LD_DFromE;
	result [0x54]			= new LD_DFromF;
	result [0x55]			= new LD_DFromL;
	DDInstructions [0xDD56]	= new LD_DFromAddressIndexIX;
	FDInstructions [0xFD56]	= new LD_DFromAddressIndexIY;
	result [0x1E]			= new LD_E;
	result [0x5F]			= new LD_EFromA;
	result [0x58]			= new LD_EFromB;
	result [0x59]			= new LD_EFromC;
	result [0x5A]			= new LD_EFromD;
	result [0x5B]			= new LD_EFromE;
	result [0x5C]			= new LD_EFromF;
	result [0x5D]			= new LD_EFromL;
	DDInstructions [0xDD5E]	= new LD_EFromAddressIndexIX;
	FDInstructions [0xFD5E]	= new LD_EFromAddressIndexIY;
	result [0x26]			= new LD_H;
	result [0x67]			= new LD_HFromA;
	result [0x60]			= new LD_HFromB;
	result [0x61]			= new LD_HFromC;
	result [0x62]			= new LD_HFromD;
	result [0x63]			= new LD_HFromE;
	result [0x64]			= new LD_HFromF;
	result [0x65]			= new LD_HFromL;
	DDInstructions [0xDD66]	= new LD_HFromAddressIndexIX;
	FDInstructions [0xFD66]	= new LD_HFromAddressIndexIY;
	result [0x2E]			= new LD_L;
	result [0x6F]			= new LD_LFromA;
	result [0x68]			= new LD_LFromB;
	result [0x69]			= new LD_LFromC;
	result [0x6A]			= new LD_LFromD;
	result [0x6B]			= new LD_LFromE;
	result [0x6C]			= new LD_LFromF;
	result [0x6D]			= new LD_LFromL;
	DDInstructions [0xDD6E]	= new LD_LFromAddressIndexIX;
	FDInstructions [0xFD6E]	= new LD_LFromAddressIndexIY;
	result [0x36]			= new LD_IndirectHL;
	result [0x77]			= new LD_IndirectHLFromA;
	result [0x70]			= new LD_IndirectHLFromB;
	result [0x71]			= new LD_IndirectHLFromC;
	result [0x72]			= new LD_IndirectHLFromD;
	result [0x73]			= new LD_IndirectHLFromE;
	result [0x74]			= new LD_IndirectHLFromF;
	result [0x75]			= new LD_IndirectHLFromL;
	result [0x02]			= new LD_IndirectBCFromA;
	result [0x12]			= new LD_IndirectDEFromA;
	DDInstructions [0xDD36] = new LD_IndirectIndexIX;
	DDInstructions [0xDD77] = new LD_IndirectIndexIXFromA;
	DDInstructions [0xDD70] = new LD_IndirectIndexIXFromB;
	DDInstructions [0xDD71] = new LD_IndirectIndexIXFromC;
	DDInstructions [0xDD72] = new LD_IndirectIndexIXFromD;
	DDInstructions [0xDD73] = new LD_IndirectIndexIXFromE;
	DDInstructions [0xDD74] = new LD_IndirectIndexIXFromF;
	DDInstructions [0xDD75] = new LD_IndirectIndexIXFromL;
	FDInstructions [0xFD36] = new LD_IndirectIndexIY;
	FDInstructions [0xFD77] = new LD_IndirectIndexIYFromA;
	FDInstructions [0xFD70] = new LD_IndirectIndexIYFromB;
	FDInstructions [0xFD71] = new LD_IndirectIndexIYFromC;
	FDInstructions [0xFD72] = new LD_IndirectIndexIYFromD;
	FDInstructions [0xFD73] = new LD_IndirectIndexIYFromE;
	FDInstructions [0xFD74] = new LD_IndirectIndexIYFromF;
	FDInstructions [0xFD75] = new LD_IndirectIndexIYFromL;
	result [0x32]			= new LD_AddressFromA;
	EDInstructions [0xED47] = new LD_IFromA;
	EDInstructions [0xED4F] = new LD_RFromA;
	result [0x01]			= new LD_BC;
	EDInstructions [0xED48] = new LD_BCFromAddress;
	result [0x11]			= new LD_DE;
	EDInstructions [0xED5B] = new LD_DEFromAddress;
	result [0x21]			= new LD_HL;
	result [0x2A]			= new LD_HLFromAddress;
	DDInstructions [0xDD21] = new LD_IX;
	DDInstructions [0xDD2A] = new LD_IXFromAddress;
	FDInstructions [0xFD21] = new LD_IY;
	FDInstructions [0xFD2A] = new LD_IYFromAddress;
	result [0x31]			= new LD_SP;
	result [0xF9]			= new LD_SPFromHL;
	DDInstructions [0xDDF9] = new LD_SPFromIX;
	FDInstructions [0xFDF9] = new LD_SPFromIY;
	EDInstructions [0xED7B] = new LD_SPFromAddress;

	// STACK
	// PUSH: Always to stack
	result [0xF6]			= new PUSH_AF;
	result [0xC6]			= new PUSH_BC;
	result [0xD6]			= new PUSH_DE;
	result [0xE6]			= new PUSH_HL;
	DDInstructions [0xDDE6] = new PUSH_IX;
	FDInstructions [0xFDE6] = new PUSH_IY;
	// PULL: Always from stack
	result [0xF6]			= new POP_AF;
	result [0xC6]			= new POP_BC;
	result [0xD6]			= new POP_DE;
	result [0xE6]			= new POP_HL;
	DDInstructions [0xDDE6] = new POP_IX;
	FDInstructions [0xFDE6] = new POP_IY;

	// Blocks Managing
	result [0xAF]			= new EX_AF;
	result [0xEB]			= new EX_DEWithHL;
	result [0xD9]			= new EX_X;
	result [0xE3]			= new EX_SPWithHL;
	DDInstructions [0xDDE3]	= new EX_SPWithIX;
	FDInstructions [0xFDE3]	= new EX_SPWithIY;
	EDInstructions [0xEDA0]	= new LDI;
	EDInstructions [0xEDB0]	= new LDIR;
	EDInstructions [0xEDA8]	= new LDD;
	EDInstructions [0xEDB8]	= new LDDR;
	EDInstructions [0xEDA1]	= new CPI;
	EDInstructions [0xEDB1]	= new CPIR;
	EDInstructions [0xEDA9]	= new CPD;
	EDInstructions [0xEDB9]	= new CPDR;

	// AND: Always with A
	result [0xA7]			= new AND_A;
	result [0xA0]			= new AND_B;
	result [0xA1]			= new AND_C;
	result [0xA2]			= new AND_D;
	result [0xA3]			= new AND_E;
	result [0xA4]			= new AND_F;
	result [0xA5]			= new AND_L;
	result [0xA6]			= new AND_IndirectHL;
	DDInstructions [0xDDA6] = new AND_IndirectIndexIX;
	FDInstructions [0xFDA6]	= new AND_IndirectIndexIY;
	result [0xE6]			= new AND;

	// OR: Always with A
	result [0xB7]			= new OR_A;
	result [0xB0]			= new OR_B;
	result [0xB1]			= new OR_C;
	result [0xB2]			= new OR_D;
	result [0xB3]			= new OR_E;
	result [0xB4]			= new OR_F;
	result [0xB5]			= new OR_L;
	result [0xB6]			= new OR_IndirectHL;
	DDInstructions [0xDDB6]	= new OR_IndirectIndexIX;
	FDInstructions [0xFDB6]	= new OR_IndirectIndexIY;
	result [0xF6]			= new OR;

	// XOR: Always with A
	result [0xAF]			= new XOR_A;
	result [0xA8]			= new XOR_B;
	result [0xA9]			= new XOR_C;
	result [0xAA]			= new XOR_D;
	result [0xAB]			= new XOR_E;
	result [0xAC]			= new XOR_F;
	result [0xAD]			= new XOR_L;
	result [0xAE]			= new XOR_IndirectHL;
	DDInstructions [0xDDAE]	= new XOR_IndirectIndexIX;
	FDInstructions [0xFDAE]	= new XOR_IndirectIndexIY;
	result [0xEE]			= new XOR;

	// ADD Always with A and without carry
	result [0x87]			= new ADD_AWithA;
	result [0x80]			= new ADD_AWithB;
	result [0x81]			= new ADD_AWithC;
	result [0x82]			= new ADD_AWithD;
	result [0x83]			= new ADD_AWithE;
	result [0x84]			= new ADD_AWithF;
	result [0x85]			= new ADD_AWithL;
	result [0x86]			= new ADD_AWithIndirectHL;
	DDInstructions [0xDD86] = new ADD_AWithIndirectIndexIX;
	FDInstructions [0xFD86] = new ADD_AWithIndirectIndexIY;
	result [0xC6]			= new ADD_A;
	
	// ADC Always with Q and with carry
	result [0x8F]			= new ADC_AWithA;
	result [0x88]			= new ADC_AWithB;
	result [0x89]			= new ADC_AWithC;
	result [0x8A]			= new ADC_AWithD;
	result [0x8B]			= new ADC_AWithE;
	result [0x8C]			= new ADC_AWithF;
	result [0x8D]			= new ADC_AWithL;
	result [0x8E]			= new ADC_AWithIndirectHL;
	DDInstructions [0xDD8E] = new ADC_AWithIndirectIndexIX;
	FDInstructions [0xFD8E] = new ADC_AWithIndirectIndexIY;
	result [0xCE]			= new ADC_A;

	// SUB Always with A and without carry
	result [0x97]			= new SUB_AWithA;
	result [0x90]			= new SUB_AWithB;
	result [0x91]			= new SUB_AWithC;
	result [0x92]			= new SUB_AWithD;
	result [0x93]			= new SUB_AWithE;
	result [0x94]			= new SUB_AWithF;
	result [0x95]			= new SUB_AWithL;
	result [0x96]			= new SUB_AWithIndirectHL;
	DDInstructions [0xDD96] = new SUB_AWithIndirectIndexIX;
	FDInstructions [0xFD96] = new SUB_AWithIndirectIndexIY;
	result [0xD6]			= new SUB_A;
	
	// SBC Always with Q and with carry
	result [0x9F]			= new SBC_AWithA;
	result [0x98]			= new SBC_AWithB;
	result [0x99]			= new SBC_AWithC;
	result [0x9A]			= new SBC_AWithD;
	result [0x9B]			= new SBC_AWithE;
	result [0x9C]			= new SBC_AWithF;
	result [0x9D]			= new SBC_AWithL;
	result [0x9E]			= new SBC_AWithIndirectHL;
	DDInstructions [0xDD9E] = new SBC_AWithIndirectIndexIX;
	FDInstructions [0xFD9E] = new SBC_AWithIndirectIndexIY;
	result [0xDE]			= new SBC_A;

	// INC
	result [0x3C]			= new INC_A;
	result [0x04]			= new INC_B;
	result [0x0C]			= new INC_C;
	result [0x14]			= new INC_D;
	result [0x1C]			= new INC_E;
	result [0x24]			= new INC_F;
	result [0x2C]			= new INC_L;
	result [0x34]			= new INC_IndirectHL;
	DDInstructions [0xDD34]	= new INC_IndirectIndexIX;
	FDInstructions [0xFD34]	= new INC_IndirectIndexIY;

	// DEC
	result [0x3C]			= new DEC_A;
	result [0x04]			= new DEC_B;
	result [0x0C]			= new DEC_C;
	result [0x14]			= new DEC_D;
	result [0x1C]			= new DEC_E;
	result [0x24]			= new DEC_F;
	result [0x2C]			= new DEC_L;
	result [0x34]			= new DEC_IndirectHL;
	DDInstructions [0xDD34]	= new DEC_IndirectIndexIX;
	FDInstructions [0xFD34]	= new DEC_IndirectIndexIY;

	// ALU Generic Instructions
	result [0x27]			= new DAA;
	result [0x2F]			= new CPL;
	EDInstructions [0xED44]	= new NEG;
	result [0x3F]			= new CCF;
	result [0x37]			= new SCF;

	// Creates the groups...
	result [0xDD]			= new MCHEmul::InstructionsGroup (0xdd, 1, 1, DDInstructions, "GroupDD");
	result [0xED]			= new MCHEmul::InstructionsGroup (0xed, 1, 1, EDInstructions, "GroupED");
	result [0xFD]			= new MCHEmul::InstructionsGroup (0xfd, 1, 1, FDInstructions, "GroupFD");

	return (result);
}
