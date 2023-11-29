#include <FZ80/CZ80.hpp>
#include <FZ80/Instructions.hpp>
#include <FZ80/INTInterrupt.hpp>
#include <FZ80/NMIInterrupt.hpp>

const std::string FZ80::CZ80::_CARRYFLAGNAME = "C";
const std::string FZ80::CZ80::_NEGATIVEFLAGNAME = "N";
const std::string FZ80::CZ80::_PARITYOVERFLOWFLAGNAME = "PO";
const std::string FZ80::CZ80::_BIT3FLAGNAME = "B3";
const std::string FZ80::CZ80::_HALFCARRYFLAGNAME = "H";
const std::string FZ80::CZ80::_BIT5FLAGNAME = "B5";
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
	  _iyRegister  ({ &iyhRegister (), &iylRegister () }),
	  _IFF1 (false), _IFF2 (false)
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
	MCHEmul::Instructions CBInstructions;	// Bit instructions
	MCHEmul::Instructions DDInstructions;	// IX Instructions
	MCHEmul::Instructions DDCBInstructions; // IX Bit Instructions
	MCHEmul::Instructions EDInstructions;	// Miscelaneous Instructions
	MCHEmul::Instructions FDInstructions;	// IY Instructions
	MCHEmul::Instructions FDCBInstructions; // IY Bit Instructions

	// ---
	// LD
	// Target A
	result [0x3E]				= new FZ80::LD_A;
	DDInstructions [0xDD3D]		= new FZ80::LD_A;						// Undocumented
	FDInstructions [0xFD3E]		= new FZ80::LD_A;						// Undocumented
	result [0x7F]				= new FZ80::LD_AFromA;
	DDInstructions [0xDD7F]		= new FZ80::LD_AFromA;					// Undocumented
	FDInstructions [0xFD7F]		= new FZ80::LD_AFromA;					// Undocumented
	result [0x78]				= new FZ80::LD_AFromB;
	DDInstructions [0xDD78]		= new FZ80::LD_AFromB;					// Undocumented
	FDInstructions [0xFD78]		= new FZ80::LD_AFromB;					// Undocumented
	result [0x79]				= new FZ80::LD_AFromC;
	DDInstructions [0xDD79]		= new FZ80::LD_AFromC;					// Undocumented
	FDInstructions [0xFD79]		= new FZ80::LD_AFromC;					// Undocumented
	result [0x7A]				= new FZ80::LD_AFromD;
	DDInstructions [0xDD7A]		= new FZ80::LD_AFromD;					// Undocumented
	FDInstructions [0xFD7A]		= new FZ80::LD_AFromD;					// Undocumented
	result [0x7B]				= new FZ80::LD_AFromE;
	DDInstructions [0xDD7B]		= new FZ80::LD_AFromE;					// Undocumented
	FDInstructions [0xFD7B]		= new FZ80::LD_AFromE;					// Undocumented
	result [0x7C]				= new FZ80::LD_AFromH;
	result [0x7D]				= new FZ80::LD_AFromL;
	EDInstructions [0xED57]		= new FZ80::LD_AFromI;
	EDInstructions [0xED5F]		= new FZ80::LD_AFromR;
	result [0x7E]				= new FZ80::LD_AFromAddressHL;
	result [0x0A]				= new FZ80::LD_AFromAddressBC;
	DDInstructions [0xDD22]		= new FZ80::LD_AddressFromIX;
	FDInstructions [0xFD22]		= new FZ80::LD_AddressFromIY;
	result [0x1A]				= new FZ80::LD_AFromAddressDE;
	DDInstructions [0xDD7E]		= new FZ80::LD_AFromAddressIndexIX;
	FDInstructions [0xFD7E]		= new FZ80::LD_AFromAddressIndexIY;
	result [0x3A]				= new FZ80::LD_AFromAddress;
	DDInstructions [0xDD7C]		= new FZ80::LD_AFromIXH;				// Undocumented
	DDInstructions [0xDD7D]		= new FZ80::LD_AFromIXL;				// Undocumented
	FDInstructions [0xFD7C]		= new FZ80::LD_AFromIYH;				// Undocumented
	FDInstructions [0xFD7D]		= new FZ80::LD_AFromIYL;				// Undocumented

	// B
	result [0x06]				= new FZ80::LD_B;
	DDInstructions [0xDD06]		= new FZ80::LD_B;						// Undocumented
	FDInstructions [0xFD06]		= new FZ80::LD_B;						// Undocumented
	result [0x47]				= new FZ80::LD_BFromA;
	DDInstructions [0xDD47]		= new FZ80::LD_BFromA;					// Undocumented
	FDInstructions [0xFD47]		= new FZ80::LD_BFromA;					// Undocumented
	result [0x40]				= new FZ80::LD_BFromB;
	DDInstructions [0xDD40]		= new FZ80::LD_BFromB;					// Undocumented
	FDInstructions [0xFD40]		= new FZ80::LD_BFromB;					// Undocumented
	result [0x41]				= new FZ80::LD_BFromC;
	DDInstructions [0xDD41]		= new FZ80::LD_BFromC;					// Undocumented
	FDInstructions [0xFD41]		= new FZ80::LD_BFromC;					// Undocumented
	result [0x42]				= new FZ80::LD_BFromD;
	DDInstructions [0xDD42]		= new FZ80::LD_BFromD;					// Undocumented
	FDInstructions [0xFD42]		= new FZ80::LD_BFromD;					// Undocumented
	result [0x43]				= new FZ80::LD_BFromE;
	DDInstructions [0xDD43]		= new FZ80::LD_BFromE;					// Undocumented
	FDInstructions [0xFD43]		= new FZ80::LD_BFromE;					// Undocumented
	result [0x44]				= new FZ80::LD_BFromH;
	result [0x45]				= new FZ80::LD_BFromL;
	result [0x46]				= new FZ80::LD_BFromAddressHL;
	DDInstructions [0xDD46]		= new FZ80::LD_BFromAddressIndexIX;
	FDInstructions [0xFD46]		= new FZ80::LD_BFromAddressIndexIY;
	DDInstructions [0xDD44]		= new FZ80::LD_BFromIXH;				// Undocumented
	DDInstructions [0xDD45]		= new FZ80::LD_BFromIXL;				// Undocumented
	FDInstructions [0xFD44]		= new FZ80::LD_BFromIYH;				// Undocumented
	FDInstructions [0xFD45]		= new FZ80::LD_BFromIYL;				// Undocumented

	// C
	result [0x0E]				= new FZ80::LD_C;
	DDInstructions [0xDD0E]		= new FZ80::LD_C;						// Undocumented
	FDInstructions [0xFD0E]		= new FZ80::LD_C;						// Undocumented
	result [0x4F]				= new FZ80::LD_CFromA;
	DDInstructions [0xDD4F]		= new FZ80::LD_CFromA;					// Undocumented
	FDInstructions [0xFD4F]		= new FZ80::LD_CFromA;					// Undocumented
	result [0x48]				= new FZ80::LD_CFromB;
	DDInstructions [0xDD48]		= new FZ80::LD_CFromB;					// Undocumented
	FDInstructions [0xFD48]		= new FZ80::LD_CFromB;					// Undocumented
	result [0x49]				= new FZ80::LD_CFromC;
	DDInstructions [0xDD49]		= new FZ80::LD_CFromC;					// Undocumented
	FDInstructions [0xFD49]		= new FZ80::LD_CFromC;					// Undocumented
	result [0x4A]				= new FZ80::LD_CFromD;
	DDInstructions [0xDD4A]		= new FZ80::LD_CFromD;					// Undocumented
	FDInstructions [0xFD4A]		= new FZ80::LD_CFromD;					// Undocumented
	result [0x4B]				= new FZ80::LD_CFromE;
	DDInstructions [0xDD4B]		= new FZ80::LD_CFromD;					// Undocumented
	FDInstructions [0xFD4B]		= new FZ80::LD_CFromD;					// Undocumented
	result [0x4C]				= new FZ80::LD_CFromH;
	result [0x4D]				= new FZ80::LD_CFromL;
	result [0x4E]				= new FZ80::LD_CFromAddressHL;
	DDInstructions [0xDD4E]		= new FZ80::LD_CFromAddressIndexIX;
	FDInstructions [0xFD4E]		= new FZ80::LD_CFromAddressIndexIY;
	DDInstructions [0xDD4C]		= new FZ80::LD_CFromIXH;				// Undocumented
	DDInstructions [0xDD4D]		= new FZ80::LD_CFromIXL;				// Undocumented
	FDInstructions [0xFD4C]		= new FZ80::LD_CFromIYH;				// Undocumented
	FDInstructions [0xFD4D]		= new FZ80::LD_CFromIYL;				// Undocumented

	// D
	result [0x16]				= new FZ80::LD_D;
	DDInstructions [0xDD16]		= new FZ80::LD_D;						// Undocumented
	FDInstructions [0xFD16]		= new FZ80::LD_D;						// Undocumented
	result [0x57]				= new FZ80::LD_DFromA;
	DDInstructions [0xDD57]		= new FZ80::LD_DFromA;					// Undocumented
	FDInstructions [0xFD57]		= new FZ80::LD_DFromA;					// Undocumented
	result [0x50]				= new FZ80::LD_DFromB;
	DDInstructions [0xDD50]		= new FZ80::LD_DFromB;					// Undocumented
	FDInstructions [0xFD50]		= new FZ80::LD_DFromB;					// Undocumented
	result [0x51]				= new FZ80::LD_DFromC;
	DDInstructions [0xDD51]		= new FZ80::LD_DFromC;					// Undocumented
	FDInstructions [0xFD51]		= new FZ80::LD_DFromC;					// Undocumented
	result [0x52]				= new FZ80::LD_DFromD;
	DDInstructions [0xDD52]		= new FZ80::LD_DFromD;					// Undocumented
	FDInstructions [0xFD52]		= new FZ80::LD_DFromD;					// Undocumented
	result [0x53]				= new FZ80::LD_DFromE;
	DDInstructions [0xDD53]		= new FZ80::LD_DFromE;					// Undocumented
	FDInstructions [0xFD53]		= new FZ80::LD_DFromE;					// Undocumented
	result [0x54]				= new FZ80::LD_DFromH;
	result [0x55]				= new FZ80::LD_DFromL;
	result [0x56]				= new FZ80::LD_DFromAddressHL;
	DDInstructions [0xDD56]		= new FZ80::LD_DFromAddressIndexIX;
	FDInstructions [0xFD56]		= new FZ80::LD_DFromAddressIndexIY;
	DDInstructions [0xDD54]		= new FZ80::LD_DFromIXH;				// Undocumented
	DDInstructions [0xDD55]		= new FZ80::LD_DFromIXL;				// Undocumented
	FDInstructions [0xFD54]		= new FZ80::LD_DFromIYH;				// Undocumented
	FDInstructions [0xFD55]		= new FZ80::LD_DFromIYL;				// Undocumented

	// E
	result [0x1E]				= new FZ80::LD_E;
	DDInstructions [0xDD1E]		= new FZ80::LD_E;						// Undocumented
	FDInstructions [0xFD1E]		= new FZ80::LD_E;						// Undocumented
	result [0x5F]				= new FZ80::LD_EFromA;
	DDInstructions [0xDD5F]		= new FZ80::LD_EFromA;					// Undocumented
	FDInstructions [0xFD5F]		= new FZ80::LD_EFromA;					// Undocumented
	result [0x58]				= new FZ80::LD_EFromB;
	DDInstructions [0xDD58]		= new FZ80::LD_EFromB;					// Undocumented
	FDInstructions [0xFD58]		= new FZ80::LD_EFromB;					// Undocumented
	result [0x59]				= new FZ80::LD_EFromC;
	DDInstructions [0xDD59]		= new FZ80::LD_EFromC;					// Undocumented
	FDInstructions [0xFD59]		= new FZ80::LD_EFromC;					// Undocumented
	result [0x5A]				= new FZ80::LD_EFromD;
	DDInstructions [0xDD5A]		= new FZ80::LD_EFromD;					// Undocumented
	FDInstructions [0xFD5A]		= new FZ80::LD_EFromD;					// Undocumented
	result [0x5B]				= new FZ80::LD_EFromE;
	DDInstructions [0xDD5B]		= new FZ80::LD_EFromE;					// Undocumented
	FDInstructions [0xFD5B]		= new FZ80::LD_EFromE;					// Undocumented
	result [0x5C]				= new FZ80::LD_EFromH;
	result [0x5D]				= new FZ80::LD_EFromL;
	result [0x5E]				= new FZ80::LD_EFromAddressHL;
	DDInstructions [0xDD5E]		= new FZ80::LD_EFromAddressIndexIX;
	FDInstructions [0xFD5E]		= new FZ80::LD_EFromAddressIndexIY;
	DDInstructions [0xDD5C]		= new FZ80::LD_EFromIXH;				// Undocumented
	DDInstructions [0xDD5D]		= new FZ80::LD_EFromIXL;				// Undocumented
	FDInstructions [0xFD5C]		= new FZ80::LD_EFromIYH;				// Undocumented
	FDInstructions [0xFD5D]		= new FZ80::LD_EFromIYL;				// Undocumented

	// H
	result [0x26]				= new FZ80::LD_H;
	result [0x67]				= new FZ80::LD_HFromA;
	result [0x60]				= new FZ80::LD_HFromB;
	result [0x61]				= new FZ80::LD_HFromC;
	result [0x62]				= new FZ80::LD_HFromD;
	result [0x63]				= new FZ80::LD_HFromE;
	result [0x64]				= new FZ80::LD_HFromH;
	result [0x65]				= new FZ80::LD_HFromL;
	result [0x66]				= new FZ80::LD_HFromAddressHL;
	DDInstructions [0xDD66]		= new FZ80::LD_HFromAddressIndexIX;
	FDInstructions [0xFD66]		= new FZ80::LD_HFromAddressIndexIY;

	// L
	result [0x2E]				= new FZ80::LD_L;
	result [0x6F]				= new FZ80::LD_LFromA;
	result [0x68]				= new FZ80::LD_LFromB;
	result [0x69]				= new FZ80::LD_LFromC;
	result [0x6A]				= new FZ80::LD_LFromD;
	result [0x6B]				= new FZ80::LD_LFromE;
	result [0x6C]				= new FZ80::LD_LFromH;
	result [0x6D]				= new FZ80::LD_LFromL;
	result [0x6E]				= new FZ80::LD_LFromAddressHL;
	DDInstructions [0xDD6E]		= new FZ80::LD_LFromAddressIndexIX;
	FDInstructions [0xFD6E]		= new FZ80::LD_LFromAddressIndexIY;

	// (HL)
	result [0x36]				= new FZ80::LD_IndirectHL;
	result [0x77]				= new FZ80::LD_IndirectHLFromA;
	result [0x70]				= new FZ80::LD_IndirectHLFromB;
	result [0x71]				= new FZ80::LD_IndirectHLFromC;
	result [0x72]				= new FZ80::LD_IndirectHLFromD;
	result [0x73]				= new FZ80::LD_IndirectHLFromE;
	result [0x74]				= new FZ80::LD_IndirectHLFromF;
	result [0x75]				= new FZ80::LD_IndirectHLFromL;

	// (BC)
	result [0x02]				= new FZ80::LD_IndirectBCFromA;

	// (DE)
	result [0x12]				= new FZ80::LD_IndirectDEFromA;

	// (IX) <- Number
	DDInstructions [0xDD36]		= new FZ80::LD_IndirectIndexIX;

	// (IX) -> r
	DDInstructions [0xDD77]		= new FZ80::LD_IndirectIndexIXFromA;
	DDInstructions [0xDD70]		= new FZ80::LD_IndirectIndexIXFromB;
	DDInstructions [0xDD71]		= new FZ80::LD_IndirectIndexIXFromC;
	DDInstructions [0xDD72]		= new FZ80::LD_IndirectIndexIXFromD;
	DDInstructions [0xDD73]		= new FZ80::LD_IndirectIndexIXFromE;
	DDInstructions [0xDD74]		= new FZ80::LD_IndirectIndexIXFromF;
	DDInstructions [0xDD75]		= new FZ80::LD_IndirectIndexIXFromL;

	// (IY) <- NUmber
	FDInstructions [0xFD36] = new FZ80::LD_IndirectIndexIY;

	// (IY)
	FDInstructions [0xFD77]		= new FZ80::LD_IndirectIndexIYFromA;
	FDInstructions [0xFD70]		= new FZ80::LD_IndirectIndexIYFromB;
	FDInstructions [0xFD71]		= new FZ80::LD_IndirectIndexIYFromC;
	FDInstructions [0xFD72]		= new FZ80::LD_IndirectIndexIYFromD;
	FDInstructions [0xFD73]		= new FZ80::LD_IndirectIndexIYFromE;
	FDInstructions [0xFD74]		= new FZ80::LD_IndirectIndexIYFromF;
	FDInstructions [0xFD75]		= new FZ80::LD_IndirectIndexIYFromL;

	// (Addrees)
	result [0x32]				= new FZ80::LD_AddressFromA;
	result [0x22]				= new FZ80::LD_AddressFromHL;
	EDInstructions [0xED63]		= new FZ80::LD_AddressFromHL;			// Undocumented
	DDInstructions [0xDD22]		= new FZ80::LD_AddressFromIX;
	FDInstructions [0xFD22]		= new FZ80::LD_AddressFromIY;
	EDInstructions [0xED43]		= new FZ80::LD_AddressFromBC;
	EDInstructions [0xED53]		= new FZ80::LD_AddressFromDE;
	EDInstructions [0xED73]		= new FZ80::LD_AddressFromSP;

	EDInstructions [0xED47]		= new FZ80::LD_IFromA;
	EDInstructions [0xED4F]		= new FZ80::LD_RFromA;
	result [0x01]				= new FZ80::LD_BC;
	EDInstructions [0xED4B]		= new FZ80::LD_BCFromAddress;
	result [0x11]				= new FZ80::LD_DE;
	EDInstructions [0xED5B]		= new FZ80::LD_DEFromAddress;
	result [0x21]				= new FZ80::LD_HL;
	result [0x2A]				= new FZ80::LD_HLFromAddress;
	EDInstructions [0xED6B]		= new FZ80::LD_HLFromAddress;			// Undocumented
	DDInstructions [0xDD21]		= new FZ80::LD_IX;
	DDInstructions [0xDD2A]		= new FZ80::LD_IXFromAddress;
	FDInstructions [0xFD21]		= new FZ80::LD_IY;
	FDInstructions [0xFD2A]		= new FZ80::LD_IYFromAddress;
	result [0x31]				= new FZ80::LD_SP;
	result [0xF9]				= new FZ80::LD_SPFromHL;
	DDInstructions [0xDDF9]		= new FZ80::LD_SPFromIX;
	FDInstructions [0xFDF9]		= new FZ80::LD_SPFromIY;
	EDInstructions [0xED7B]		= new FZ80::LD_SPFromAddress;

	// STACK
	// PUSH: Always to stack
	result [0xF5]				= new FZ80::PUSH_AF;
	result [0xC5]				= new FZ80::PUSH_BC;
	result [0xD5]				= new FZ80::PUSH_DE;
	result [0xE5]				= new FZ80::PUSH_HL;
	DDInstructions [0xDDE5]		= new FZ80::PUSH_IX;
	FDInstructions [0xFDE5]		= new FZ80::PUSH_IY;
	// PULL: Always from stack
	result [0xF1]				= new FZ80::POP_AF;
	result [0xC1]				= new FZ80::POP_BC;
	result [0xD1]				= new FZ80::POP_DE;
	result [0xE1]				= new FZ80::POP_HL;
	DDInstructions [0xDDE1]		= new FZ80::POP_IX;
	FDInstructions [0xFDE1]		= new FZ80::POP_IY;

	// Blocks Managing
	result [0x08]				= new FZ80::EX_AF;
	result [0xEB]				= new FZ80::EX_DEWithHL;
	result [0xD9]				= new FZ80::EX_X;
	result [0xE3]				= new FZ80::EX_SPWithHL;
	DDInstructions [0xDDE3]		= new FZ80::EX_SPWithIX;
	FDInstructions [0xFDE3]		= new FZ80::EX_SPWithIY;
	EDInstructions [0xEDA0]		= new FZ80::LDI;
	EDInstructions [0xEDB0]		= new FZ80::LDIR;
	EDInstructions [0xEDA8]		= new FZ80::LDD;
	EDInstructions [0xEDB8]		= new FZ80::LDDR;
	EDInstructions [0xEDA1]		= new FZ80::CPI;
	EDInstructions [0xEDB1]		= new FZ80::CPIR;
	EDInstructions [0xEDA9]		= new FZ80::CPD;
	EDInstructions [0xEDB9]		= new FZ80::CPDR;

	// AND: Always with A
	result [0xA7]				= new FZ80::AND_A;
	DDInstructions [0xDDA7]		= new FZ80::AND_A;						// Undocumented
	FDInstructions [0xFDA7]		= new FZ80::AND_A;						// Undocumented
	result [0xA0]				= new FZ80::AND_B;
	DDInstructions [0xDDA0]		= new FZ80::AND_B;						// Undocumented
	FDInstructions [0xFDA0]		= new FZ80::AND_B;						// Undocumented
	result [0xA1]				= new FZ80::AND_C;
	DDInstructions [0xDDA1]		= new FZ80::AND_C;						// Undocumented
	FDInstructions [0xFDA1]		= new FZ80::AND_C;						// Undocumented
	result [0xA2]				= new FZ80::AND_D;
	DDInstructions [0xDDA2]		= new FZ80::AND_D;						// Undocumented
	FDInstructions [0xFDA2]		= new FZ80::AND_D;						// Undocumented
	result [0xA3]				= new FZ80::AND_E;
	DDInstructions [0xDDA3]		= new FZ80::AND_E;						// Undocumented
	FDInstructions [0xFDA3]		= new FZ80::AND_E;						// Undocumented
	result [0xA4]				= new FZ80::AND_H;
	result [0xA5]				= new FZ80::AND_L;
	result [0xA6]				= new FZ80::AND_IndirectHL;
	DDInstructions [0xDDA6]		= new FZ80::AND_IndirectIndexIX;
	FDInstructions [0xFDA6]		= new FZ80::AND_IndirectIndexIY;
	result [0xE6]				= new FZ80::AND;
	DDInstructions [0xDDA4]		= new FZ80::AND_IXH;					// Undocumented
	DDInstructions [0xDDA5]		= new FZ80::AND_IXL;					// Undocumented
	FDInstructions [0xFDA4]		= new FZ80::AND_IYH;					// Undocumented
	FDInstructions [0xFDA5]		= new FZ80::AND_IYL;					// Undocumented

	// OR: Always with A
	result [0xB7]				= new FZ80::OR_A;
	DDInstructions [0xDDB7]		= new FZ80::OR_A;						// Undocumented
	FDInstructions [0xFDB7]		= new FZ80::OR_A;						// Undocumented
	result [0xB0]				= new FZ80::OR_B;
	DDInstructions [0xDDB0]		= new FZ80::OR_B;						// Undocumented
	FDInstructions [0xFDB0]		= new FZ80::OR_B;						// Undocumented
	result [0xB1]				= new FZ80::OR_C;
	DDInstructions [0xDDB1]		= new FZ80::OR_C;						// Undocumented
	FDInstructions [0xFDB1]		= new FZ80::OR_C;						// Undocumented
	result [0xB2]				= new FZ80::OR_D;
	DDInstructions [0xDDB2]		= new FZ80::OR_D;						// Undocumented
	FDInstructions [0xFDB2]		= new FZ80::OR_D;						// Undocumented
	result [0xB3]				= new FZ80::OR_E;
	DDInstructions [0xDDB3]		= new FZ80::OR_E;						// Undocumented
	FDInstructions [0xFDB3]		= new FZ80::OR_E;						// Undocumented
	result [0xB4]				= new FZ80::OR_H;
	result [0xB5]				= new FZ80::OR_L;
	result [0xB6]				= new FZ80::OR_IndirectHL;
	DDInstructions [0xDDB6]		= new FZ80::OR_IndirectIndexIX;
	FDInstructions [0xFDB6]		= new FZ80::OR_IndirectIndexIY;
	result [0xF6]				= new FZ80::OR;
	DDInstructions [0xDDB4]		= new FZ80::OR_IXH;					// Undocumented
	DDInstructions [0xDDB5]		= new FZ80::OR_IXL;					// Undocumented
	FDInstructions [0xFDB4]		= new FZ80::OR_IYH;					// Undocumented
	FDInstructions [0xFDB5]		= new FZ80::OR_IYL;					// Undocumented

	// XOR: Always with A
	result [0xAF]				= new FZ80::XOR_A;
	DDInstructions [0xDDAF]		= new FZ80::XOR_A;					// Undocumented
	FDInstructions [0xFDAF]		= new FZ80::XOR_A;					// Undocumented
	result [0xA8]				= new FZ80::XOR_B;
	DDInstructions [0xDDA8]		= new FZ80::XOR_B;					// Undocumented
	FDInstructions [0xFDA8]		= new FZ80::XOR_B;					// Undocumented
	result [0xA9]				= new FZ80::XOR_C;
	DDInstructions [0xDDA9]		= new FZ80::XOR_C;					// Undocumented
	FDInstructions [0xFDA9]		= new FZ80::XOR_C;					// Undocumented
	result [0xAA]				= new FZ80::XOR_D;
	DDInstructions [0xDDAA]		= new FZ80::XOR_D;					// Undocumented
	FDInstructions [0xFDAA]		= new FZ80::XOR_D;					// Undocumented
	result [0xAB]				= new FZ80::XOR_E;
	DDInstructions [0xDDAB]		= new FZ80::XOR_E;					// Undocumented
	FDInstructions [0xFDAB]		= new FZ80::XOR_E;					// Undocumented
	result [0xAC]				= new FZ80::XOR_H;
	result [0xAD]				= new FZ80::XOR_L;
	result [0xAE]				= new FZ80::XOR_IndirectHL;
	DDInstructions [0xDDAE]		= new FZ80::XOR_IndirectIndexIX;
	FDInstructions [0xFDAE]		= new FZ80::XOR_IndirectIndexIY;
	result [0xEE]				= new FZ80::XOR;
	DDInstructions [0xDDAC]		= new FZ80::XOR_IXH;				// Undocumented
	DDInstructions [0xDDAD]		= new FZ80::XOR_IXL;				// Undocumented
	FDInstructions [0xFDAC]		= new FZ80::XOR_IYH;				// Undocumented
	FDInstructions [0xFDAD]		= new FZ80::XOR_IYL;				// Undocumented

	// ADD without carry
	result [0x87]				= new FZ80::ADD_AWithA;
	DDInstructions [0xDD87]		= new FZ80::ADD_AWithA;				// Undocumented
	FDInstructions [0xFD87]		= new FZ80::ADD_AWithA;				// Undocumented
	result [0x80]				= new FZ80::ADD_AWithB;
	DDInstructions [0xDD80]		= new FZ80::ADD_AWithB;				// Undocumented
	FDInstructions [0xFD80]		= new FZ80::ADD_AWithB;				// Undocumented
	result [0x81]				= new FZ80::ADD_AWithC;
	DDInstructions [0xDD81]		= new FZ80::ADD_AWithC;				// Undocumented
	FDInstructions [0xFD81]		= new FZ80::ADD_AWithC;				// Undocumented
	result [0x82]				= new FZ80::ADD_AWithD;
	DDInstructions [0xDD82]		= new FZ80::ADD_AWithD;				// Undocumented
	FDInstructions [0xFD82]		= new FZ80::ADD_AWithD;				// Undocumented
	result [0x83]				= new FZ80::ADD_AWithE;
	DDInstructions [0xDD83]		= new FZ80::ADD_AWithE;				// Undocumented
	FDInstructions [0xFD83]		= new FZ80::ADD_AWithE;				// Undocumented
	result [0x84]				= new FZ80::ADD_AWithH;
	result [0x85]				= new FZ80::ADD_AWithL;
	result [0x86]				= new FZ80::ADD_AWithIndirectHL;
	DDInstructions [0xDD86]		= new FZ80::ADD_AWithIndirectIndexIX;
	FDInstructions [0xFD86]		= new FZ80::ADD_AWithIndirectIndexIY;
	result [0xC6]				= new FZ80::ADD_A;
	result [0x09]				= new FZ80::ADD_HLWithBC;
	result [0x19]				= new FZ80::ADD_HLWithDE;
	result [0x29]				= new FZ80::ADD_HLWithHL;
	result [0x39]				= new FZ80::ADD_HLWithSP;
	DDInstructions [0xDD84]		= new FZ80::ADD_AWithIXH;			// Undocumented
	DDInstructions [0xDD85]		= new FZ80::ADD_AWithIXL;			// Undocumented
	FDInstructions [0xFD84]		= new FZ80::ADD_AWithIYH;			// Undocumented
	FDInstructions [0xFD85]		= new FZ80::ADD_AWithIYL;			// Undocumented
	DDInstructions [0xDD09]		= new FZ80::ADD_IXWithBC;
	DDInstructions [0xDD19]		= new FZ80::ADD_IXWithDE;
	DDInstructions [0xDD29]		= new FZ80::ADD_IXWithIX;
	DDInstructions [0xDD39]		= new FZ80::ADD_IXWithSP;
	FDInstructions [0xFD09]		= new FZ80::ADD_IYWithBC;
	FDInstructions [0xFD19]		= new FZ80::ADD_IYWithDE;
	FDInstructions [0xFD29]		= new FZ80::ADD_IYWithIY;
	FDInstructions [0xFD39]		= new FZ80::ADD_IYWithSP;
	
	// ADC with carry
	result [0x8F]				= new FZ80::ADC_AWithA;
	DDInstructions [0xDD8F]		= new FZ80::ADC_AWithA;				// Undocumented
	FDInstructions [0xFD8F]		= new FZ80::ADC_AWithA;				// Undocumented
	result [0x88]				= new FZ80::ADC_AWithB;
	DDInstructions [0xDD88]		= new FZ80::ADC_AWithB;				// Undocumented
	FDInstructions [0xFD88]		= new FZ80::ADC_AWithB;				// Undocumented
	result [0x89]				= new FZ80::ADC_AWithC;
	DDInstructions [0xDD89]		= new FZ80::ADC_AWithC;				// Undocumented
	FDInstructions [0xFD89]		= new FZ80::ADC_AWithC;				// Undocumented
	result [0x8A]				= new FZ80::ADC_AWithD;
	DDInstructions [0xDD8A]		= new FZ80::ADC_AWithD;				// Undocumented
	FDInstructions [0xFD8A]		= new FZ80::ADC_AWithD;				// Undocumented
	result [0x8B]				= new FZ80::ADC_AWithE;
	DDInstructions [0xDD8B]		= new FZ80::ADC_AWithE;				// Undocumented
	FDInstructions [0xFD8B]		= new FZ80::ADC_AWithE;				// Undocumented
	result [0x8C]				= new FZ80::ADC_AWithH;
	result [0x8D]				= new FZ80::ADC_AWithL;
	result [0x8E]				= new FZ80::ADC_AWithIndirectHL;
	DDInstructions [0xDD8E]		= new FZ80::ADC_AWithIndirectIndexIX;
	FDInstructions [0xFD8E]		= new FZ80::ADC_AWithIndirectIndexIY;
	result [0xCE]				= new FZ80::ADC_A;
	EDInstructions [0xED4A]		= new FZ80::ADC_HLWithBC;
	EDInstructions [0xED5A]		= new FZ80::ADC_HLWithDE;
	EDInstructions [0xED6A]		= new FZ80::ADC_HLWithHL;
	EDInstructions [0xED7A]		= new FZ80::ADC_HLWithSP;
	DDInstructions [0xDD8C]		= new FZ80::ADC_AWithIXH;				// Undocumented
	DDInstructions [0xDD8D]		= new FZ80::ADC_AWithIXL;				// Undocumented
	FDInstructions [0xFD8C]		= new FZ80::ADC_AWithIYH;				// Undocumented
	FDInstructions [0xFD8D]		= new FZ80::ADC_AWithIYL;				// Undocumented

	// SUB without carry
	result [0x97]				= new FZ80::SUB_AWithA;
	DDInstructions [0xDD97]		= new FZ80::SUB_AWithA;					// Undocumented
	FDInstructions [0xFD97]		= new FZ80::SUB_AWithA;					// Undocumented
	result [0x90]				= new FZ80::SUB_AWithB;
	DDInstructions [0xDD90]		= new FZ80::SUB_AWithB;					// Undocumented
	FDInstructions [0xFD90]		= new FZ80::SUB_AWithB;					// Undocumented
	result [0x91]				= new FZ80::SUB_AWithC;
	DDInstructions [0xDD91]		= new FZ80::SUB_AWithC;					// Undocumented
	FDInstructions [0xFD91]		= new FZ80::SUB_AWithC;					// Undocumented
	result [0x92]				= new FZ80::SUB_AWithD;
	DDInstructions [0xDD92]		= new FZ80::SUB_AWithD;					// Undocumented
	FDInstructions [0xFD92]		= new FZ80::SUB_AWithD;					// Undocumented
	result [0x93]				= new FZ80::SUB_AWithE;
	DDInstructions [0xDD93]		= new FZ80::SUB_AWithE;					// Undocumented
	FDInstructions [0xFD93]		= new FZ80::SUB_AWithE;					// Undocumented
	result [0x94]				= new FZ80::SUB_AWithH;
	result [0x95]				= new FZ80::SUB_AWithL;
	result [0x96]				= new FZ80::SUB_AWithIndirectHL;
	DDInstructions [0xDD96]		= new FZ80::SUB_AWithIndirectIndexIX;
	FDInstructions [0xFD96]		= new FZ80::SUB_AWithIndirectIndexIY;
	result [0xD6]				= new FZ80::SUB_A;
	DDInstructions [0xDD94]		= new FZ80::SUB_AWithIXH;				// Undocumented
	DDInstructions [0xDD95]		= new FZ80::SUB_AWithIXL;				// Undocumented
	FDInstructions [0xFD94]		= new FZ80::SUB_AWithIYH;				// Undocumented
	FDInstructions [0xFD95]		= new FZ80::SUB_AWithIYL;				// Undocumented
	
	// SBC with carry
	result [0x9F]				= new FZ80::SBC_AWithA;
	DDInstructions [0xDD9F]		= new FZ80::SBC_AWithA;					// Undocumented
	FDInstructions [0xFD9F]		= new FZ80::SBC_AWithA;					// Undocumented
	result [0x98]				= new FZ80::SBC_AWithB;
	DDInstructions [0xDD98]		= new FZ80::SBC_AWithB;					// Undocumented
	FDInstructions [0xFD98]		= new FZ80::SBC_AWithB;					// Undocumented
	result [0x99]				= new FZ80::SBC_AWithC;
	DDInstructions [0xDD99]		= new FZ80::SBC_AWithC;					// Undocumented
	FDInstructions [0xFD99]		= new FZ80::SBC_AWithC;					// Undocumented
	result [0x9A]				= new FZ80::SBC_AWithD;
	DDInstructions [0xDD9A]		= new FZ80::SBC_AWithD;					// Undocumented
	FDInstructions [0xFD9A]		= new FZ80::SBC_AWithD;					// Undocumented
	result [0x9B]				= new FZ80::SBC_AWithE;
	DDInstructions [0xDD9B]		= new FZ80::SBC_AWithE;					// Undocumented
	FDInstructions [0xFD9B]		= new FZ80::SBC_AWithE;					// Undocumented
	result [0x9C]				= new FZ80::SBC_AWithH;
	result [0x9D]				= new FZ80::SBC_AWithL;
	result [0x9E]				= new FZ80::SBC_AWithIndirectHL;
	DDInstructions [0xDD9E]		= new FZ80::SBC_AWithIndirectIndexIX;
	FDInstructions [0xFD9E]		= new FZ80::SBC_AWithIndirectIndexIY;
	result [0xDE]				= new FZ80::SBC_A;
	EDInstructions [0xED42]		= new FZ80::SBC_HLWithBC;
	EDInstructions [0xED52]		= new FZ80::SBC_HLWithDE;
	EDInstructions [0xED62]		= new FZ80::SBC_HLWithHL;
	EDInstructions [0xED72]		= new FZ80::SBC_HLWithSP;
	DDInstructions [0xDD9C]		= new FZ80::SBC_AWithIXH;				// Undocumented
	DDInstructions [0xDD9D]		= new FZ80::SBC_AWithIXL;				// Undocumented
	FDInstructions [0xFD9C]		= new FZ80::SBC_AWithIYH;				// Undocumented
	FDInstructions [0xFD9D]		= new FZ80::SBC_AWithIYL;				// Undocumented

	// INC
	result [0x3C]				= new FZ80::INC_A;
	DDInstructions [0xDD3C]		= new FZ80::INC_A;						// Undocumented
	FDInstructions [0xFD3C]		= new FZ80::INC_A;						// Undocumented
	result [0x04]				= new FZ80::INC_B;
	DDInstructions [0xDD04]		= new FZ80::INC_B;						// Undocumented
	FDInstructions [0xFD04]		= new FZ80::INC_B;						// Undocumented
	result [0x0C]				= new FZ80::INC_C;
	DDInstructions [0xDD0C]		= new FZ80::INC_C;						// Undocumented
	FDInstructions [0xFD0C]		= new FZ80::INC_C;						// Undocumented
	result [0x14]				= new FZ80::INC_D;
	DDInstructions [0xDD14]		= new FZ80::INC_D;						// Undocumented
	FDInstructions [0xFD14]		= new FZ80::INC_D;						// Undocumented
	result [0x1C]				= new FZ80::INC_E;
	DDInstructions [0xDD1C]		= new FZ80::INC_E;						// Undocumented
	FDInstructions [0xFD1C]		= new FZ80::INC_E;						// Undocumented
	result [0x24]				= new FZ80::INC_H;
	result [0x2C]				= new FZ80::INC_L;
	result [0x34]				= new FZ80::INC_IndirectHL;
	DDInstructions [0xDD34]		= new FZ80::INC_IndirectIndexIX;
	FDInstructions [0xFD34]		= new FZ80::INC_IndirectIndexIY;
	result [0x03]				= new FZ80::INC_BC;
	result [0x13]				= new FZ80::INC_DE;
	result [0x23]				= new FZ80::INC_HL;
	result [0x33]				= new FZ80::INC_SP;
	DDInstructions [0xDD23]		= new FZ80::INC_IX;
	FDInstructions [0xFD23]		= new FZ80::INC_IY;
	DDInstructions [0xDD24]		= new FZ80::INC_IXH;					// Undocumented
	DDInstructions [0xDD2C]		= new FZ80::INC_IXL;					// Undocumented
	FDInstructions [0xFD24]		= new FZ80::INC_IYH;					// Undocumented
	FDInstructions [0xFD2C]		= new FZ80::INC_IYL;					// Undocumented

	// DEC
	result [0x3D]				= new FZ80::DEC_A;
	DDInstructions [0xDD3D]		= new FZ80::DEC_A;						// Undocumented
	FDInstructions [0xFD3D]		= new FZ80::DEC_A;						// Undocumented
	result [0x05]				= new FZ80::DEC_B;
	DDInstructions [0xDD05]		= new FZ80::DEC_B;						// Undocumented
	FDInstructions [0xFD05]		= new FZ80::DEC_B;						// Undocumented
	result [0x0D]				= new FZ80::DEC_C;
	DDInstructions [0xDD0D]		= new FZ80::DEC_C;						// Undocumented
	FDInstructions [0xFD0D]		= new FZ80::DEC_C;						// Undocumented
	result [0x15]				= new FZ80::DEC_D;
	DDInstructions [0xDD15]		= new FZ80::DEC_D;						// Undocumented
	FDInstructions [0xFD15]		= new FZ80::DEC_D;						// Undocumented
	result [0x1D]				= new FZ80::DEC_E;
	DDInstructions [0xDD1D]		= new FZ80::DEC_E;						// Undocumented
	FDInstructions [0xFD1D]		= new FZ80::DEC_E;						// Undocumented
	result [0x25]				= new FZ80::DEC_H;
	result [0x2D]				= new FZ80::DEC_L;
	result [0x35]				= new FZ80::DEC_IndirectHL;
	DDInstructions [0xDD35]		= new FZ80::DEC_IndirectIndexIX;
	FDInstructions [0xFD35]		= new FZ80::DEC_IndirectIndexIY;
	result [0x0B]				= new FZ80::DEC_BC;
	result [0x1B]				= new FZ80::DEC_DE;
	result [0x2B]				= new FZ80::DEC_HL;
	result [0x3B]				= new FZ80::DEC_SP;
	DDInstructions [0xDD2B]		= new FZ80::DEC_IX;
	FDInstructions [0xFD2B]		= new FZ80::DEC_IY;
	DDInstructions [0xDD25]		= new FZ80::DEC_IXH;					// Undocumented
	DDInstructions [0xDD2D]		= new FZ80::DEC_IXL;					// Undocumented
	FDInstructions [0xFD25]		= new FZ80::DEC_IYH;					// Undocumented
	FDInstructions [0xFD2D]		= new FZ80::DEC_IYL;					// Undocumented

	// CP
	result [0xBF]				= new FZ80::CP_WithA;
	DDInstructions [0xDDBF]		= new FZ80::CP_WithA;					// Undocumented
	FDInstructions [0xFDBF]		= new FZ80::CP_WithA;					// Undocumented
	result [0xB8]				= new FZ80::CP_WithB;
	DDInstructions [0xDDB8]		= new FZ80::CP_WithB;					// Undocumented
	FDInstructions [0xFDB8]		= new FZ80::CP_WithB;					// Undocumented
	result [0xB9]				= new FZ80::CP_WithC;
	DDInstructions [0xDDB9]		= new FZ80::CP_WithC;					// Undocumented
	FDInstructions [0xFDB9]		= new FZ80::CP_WithC;					// Undocumented
	result [0xBA]				= new FZ80::CP_WithD;
	DDInstructions [0xDDBA]		= new FZ80::CP_WithD;					// Undocumented
	FDInstructions [0xFDBA]		= new FZ80::CP_WithD;					// Undocumented
	result [0xBB]				= new FZ80::CP_WithE;
	DDInstructions [0xDDBB]		= new FZ80::CP_WithE;					// Undocumented
	FDInstructions [0xFDBB]		= new FZ80::CP_WithE;					// Undocumented
	result [0xBC]				= new FZ80::CP_WithH;			
	result [0xBD]				= new FZ80::CP_WithL;			
	result [0xBE]				= new FZ80::CP_WithIndirectHL;
	DDInstructions [0xDDBE]		= new FZ80::CP_WithIndirectIndexIX;
	FDInstructions [0xFDBE]		= new FZ80::CP_WithIndirectIndexIY;
	result [0xFE]				= new FZ80::CP;
	DDInstructions [0xDDBC]		= new FZ80::CP_WithIXH;					// Undocumented
	DDInstructions [0xDDBD]		= new FZ80::CP_WithIXL;					// Undocumented
	FDInstructions [0xFDBC]		= new FZ80::CP_WithIYH;					// Undocumented
	FDInstructions [0xFDBD]		= new FZ80::CP_WithIYL;					// Undocumented

	// ALU Generic Instructions
	result [0x27]				= new FZ80::DAA;
	result [0x2F]				= new FZ80::CPL;
	EDInstructions [0xED44]		= new FZ80::NEG;
	result [0x3F]				= new FZ80::CCF;
	result [0x37]				= new FZ80::SCF;

	// RCL
	result [0x07] = new FZ80::RLC_A;
	CBInstructions [0xCB07]		= new FZ80::RLC_ASlower;
	CBInstructions [0xCB00]		= new FZ80::RLC_B;
	CBInstructions [0xCB01]		= new FZ80::RLC_C;
	CBInstructions [0xCB02]		= new FZ80::RLC_D;
	CBInstructions [0xCB03]		= new FZ80::RLC_E;
	CBInstructions [0xCB04]		= new FZ80::RLC_H;
	CBInstructions [0xCB05]		= new FZ80::RLC_L;
	CBInstructions [0xCB06]		= new FZ80::RLC_IndirectHL;
	DDCBInstructions [0xDDCB06] = new FZ80::RLC_IndirectIndexIX;
	DDCBInstructions [0xDDCB07] = new FZ80::RLC_IndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB00] = new FZ80::RLC_IndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB01] = new FZ80::RLC_IndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB02] = new FZ80::RLC_IndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB03] = new FZ80::RLC_IndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB04] = new FZ80::RLC_IndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB05] = new FZ80::RLC_IndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB06] = new FZ80::RLC_IndirectIndexIY;
	FDCBInstructions [0xFDCB07] = new FZ80::RLC_IndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB00] = new FZ80::RLC_IndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB01] = new FZ80::RLC_IndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB02] = new FZ80::RLC_IndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB03] = new FZ80::RLC_IndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB04] = new FZ80::RLC_IndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB05] = new FZ80::RLC_IndirectIndexIYCopyL;	// Undocumented

	// RL
	result [0x17] = new FZ80::RL_A;
	CBInstructions [0xCB17]		= new FZ80::RL_ASlower;
	CBInstructions [0xCB10]		= new FZ80::RL_B;
	CBInstructions [0xCB11]		= new FZ80::RL_C;
	CBInstructions [0xCB12]		= new FZ80::RL_D;
	CBInstructions [0xCB13]		= new FZ80::RL_E;
	CBInstructions [0xCB14]		= new FZ80::RL_H;
	CBInstructions [0xCB15]		= new FZ80::RL_L;
	CBInstructions [0xCB16]		= new FZ80::RL_IndirectHL;
	DDCBInstructions [0xDDCB06] = new FZ80::RL_IndirectIndexIX;
	DDCBInstructions [0xDDCB07] = new FZ80::RL_IndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB00] = new FZ80::RL_IndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB01] = new FZ80::RL_IndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB02] = new FZ80::RL_IndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB03] = new FZ80::RL_IndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB04] = new FZ80::RL_IndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB05] = new FZ80::RL_IndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB06] = new FZ80::RL_IndirectIndexIY;
	FDCBInstructions [0xFDCB07] = new FZ80::RL_IndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB00] = new FZ80::RL_IndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB01] = new FZ80::RL_IndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB02] = new FZ80::RL_IndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB03] = new FZ80::RL_IndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB04] = new FZ80::RL_IndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB05] = new FZ80::RL_IndirectIndexIYCopyL;	// Undocumented

	// RLD
	EDInstructions [0xED6F]		= new RLD;

	// SLA
	CBInstructions [0xCB27]		= new SLA_A;
	CBInstructions [0xCB20]		= new SLA_B;
	CBInstructions [0xCB21]		= new SLA_C;
	CBInstructions [0xCB22]		= new SLA_D;
	CBInstructions [0xCB23]		= new SLA_E;
	CBInstructions [0xCB24]		= new SLA_H;
	CBInstructions [0xCB25]		= new SLA_L;
	CBInstructions [0xCB26]		= new SLA_IndirectHL;
	DDCBInstructions [0xDDCB26] = new SLA_IndirectIndexIX;
	DDCBInstructions [0xDDCB27] = new SLA_IndirectIndexIXCopyA;			// Undocumented
	DDCBInstructions [0xDDCB20] = new SLA_IndirectIndexIXCopyB;			// Undocumented
	DDCBInstructions [0xDDCB21] = new SLA_IndirectIndexIXCopyC;			// Undocumented
	DDCBInstructions [0xDDCB22] = new SLA_IndirectIndexIXCopyD;			// Undocumented
	DDCBInstructions [0xDDCB23] = new SLA_IndirectIndexIXCopyE;			// Undocumented
	DDCBInstructions [0xDDCB24] = new SLA_IndirectIndexIXCopyH;			// Undocumented
	DDCBInstructions [0xDDCB25] = new SLA_IndirectIndexIXCopyL;			// Undocumented
	FDCBInstructions [0xFDCB26] = new SLA_IndirectIndexIY;
	FDCBInstructions [0xFDCB27] = new SLA_IndirectIndexIYCopyA;			// Undocumented
	FDCBInstructions [0xFDCB20] = new SLA_IndirectIndexIYCopyB;			// Undocumented
	FDCBInstructions [0xFDCB21] = new SLA_IndirectIndexIYCopyC;			// Undocumented
	FDCBInstructions [0xFDCB22] = new SLA_IndirectIndexIYCopyD;			// Undocumented
	FDCBInstructions [0xFDCB23] = new SLA_IndirectIndexIYCopyE;			// Undocumented
	FDCBInstructions [0xFDCB24] = new SLA_IndirectIndexIYCopyH;			// Undocumented
	FDCBInstructions [0xFDCB25] = new SLA_IndirectIndexIYCopyL;			// Undocumented

	// SLL
	CBInstructions [0xCB37]		= new SLL_A;
	CBInstructions [0xCB30]		= new SLL_B;
	CBInstructions [0xCB31]		= new SLL_C;
	CBInstructions [0xCB32]		= new SLL_D;
	CBInstructions [0xCB33]		= new SLL_E;
	CBInstructions [0xCB34]		= new SLL_H;
	CBInstructions [0xCB35]		= new SLL_L;
	CBInstructions [0xCB36]		= new SLL_IndirectHL;
	DDCBInstructions [0xDDCB36] = new SLL_IndirectIndexIX;
	DDCBInstructions [0xDDCB37] = new SLL_IndirectIndexIXCopyA;			// Undocumented
	DDCBInstructions [0xDDCB30] = new SLL_IndirectIndexIXCopyB;			// Undocumented
	DDCBInstructions [0xDDCB31] = new SLL_IndirectIndexIXCopyC;			// Undocumented
	DDCBInstructions [0xDDCB32] = new SLL_IndirectIndexIXCopyD;			// Undocumented
	DDCBInstructions [0xDDCB33] = new SLL_IndirectIndexIXCopyE;			// Undocumented
	DDCBInstructions [0xDDCB34] = new SLL_IndirectIndexIXCopyH;			// Undocumented
	DDCBInstructions [0xDDCB35] = new SLL_IndirectIndexIXCopyL;			// Undocumented
	FDCBInstructions [0xFDCB36] = new SLL_IndirectIndexIY;
	FDCBInstructions [0xFDCB37] = new SLL_IndirectIndexIYCopyA;			// Undocumented
	FDCBInstructions [0xFDCB30] = new SLL_IndirectIndexIYCopyB;			// Undocumented
	FDCBInstructions [0xFDCB31] = new SLL_IndirectIndexIYCopyC;			// Undocumented
	FDCBInstructions [0xFDCB32] = new SLL_IndirectIndexIYCopyD;			// Undocumented
	FDCBInstructions [0xFDCB33] = new SLL_IndirectIndexIYCopyE;			// Undocumented
	FDCBInstructions [0xFDCB34] = new SLL_IndirectIndexIYCopyH;			// Undocumented
	FDCBInstructions [0xFDCB35] = new SLL_IndirectIndexIYCopyL;			// Undocumented

	// RRC
	result [0x0F] = new FZ80::RRC_A;
	CBInstructions [0xCB0F]		= new FZ80::RRC_ASlower;
	CBInstructions [0xCB08]		= new FZ80::RRC_B;
	CBInstructions [0xCB09]		= new FZ80::RRC_C;
	CBInstructions [0xCB0A]		= new FZ80::RRC_D;
	CBInstructions [0xCB0B]		= new FZ80::RRC_E;
	CBInstructions [0xCB0C]		= new FZ80::RRC_F;
	CBInstructions [0xCB0D]		= new FZ80::RRC_L;
	CBInstructions [0xCB0E]		= new FZ80::RRC_IndirectHL;
	DDCBInstructions [0xDDCB0E] = new FZ80::RRC_IndirectIndexIX;
	DDCBInstructions [0xDDCB0F] = new FZ80::RRC_IndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB08] = new FZ80::RRC_IndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB09] = new FZ80::RRC_IndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB0A] = new FZ80::RRC_IndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB0B] = new FZ80::RRC_IndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB0C] = new FZ80::RRC_IndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB0D] = new FZ80::RRC_IndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB0E] = new FZ80::RRC_IndirectIndexIY;
	FDCBInstructions [0xFDCB0F] = new FZ80::RRC_IndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB08] = new FZ80::RRC_IndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB09] = new FZ80::RRC_IndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB0A] = new FZ80::RRC_IndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB0B] = new FZ80::RRC_IndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB0C] = new FZ80::RRC_IndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB0D] = new FZ80::RRC_IndirectIndexIYCopyL;	// Undocumented

	// RR
	result [0x0F] = new FZ80::RR_A;
	CBInstructions [0xCB0F]		= new FZ80::RR_ASlower;
	CBInstructions [0xCB08]		= new FZ80::RR_B;
	CBInstructions [0xCB09]		= new FZ80::RR_C;
	CBInstructions [0xCB0A]		= new FZ80::RR_D;
	CBInstructions [0xCB0B]		= new FZ80::RR_E;
	CBInstructions [0xCB0C]		= new FZ80::RR_F;
	CBInstructions [0xCB0D]		= new FZ80::RR_L;
	CBInstructions [0xCB0E]		= new FZ80::RR_IndirectHL;
	DDCBInstructions [0xDDCB0E] = new FZ80::RR_IndirectIndexIX;
	DDCBInstructions [0xDDCB0F] = new FZ80::RR_IndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB08] = new FZ80::RR_IndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB09] = new FZ80::RR_IndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB0A] = new FZ80::RR_IndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB0B] = new FZ80::RR_IndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB0C] = new FZ80::RR_IndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB0D] = new FZ80::RR_IndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB0E] = new FZ80::RR_IndirectIndexIY;
	FDCBInstructions [0xFDCB0F] = new FZ80::RR_IndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB08] = new FZ80::RR_IndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB09] = new FZ80::RR_IndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB0A] = new FZ80::RR_IndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB0B] = new FZ80::RR_IndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB0C] = new FZ80::RR_IndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB0D] = new FZ80::RR_IndirectIndexIYCopyL;	// Undocumented

	// RRD
	EDInstructions [0xED67]		= new RRD;

	// There are two levels of abstraction!
	// Add the bit instruction groups into their parent groups...
	DDInstructions [0xDDCB]		= new FZ80::Byte4InstructionCode (0xDDCB, DDCBInstructions);
	FDInstructions [0xFDCB]		= new FZ80::Byte4InstructionCode (0xFDCB, FDCBInstructions);
	// ...and then add the groups intoain set of instructions...
	result [0xCB]				= new FZ80::Byte2InstructionCode (0xCB, CBInstructions);
	result [0xDD]				= new FZ80::Byte2InstructionCode (0xDD, DDInstructions);
	result [0xED]				= new FZ80::Byte2InstructionCode (0xED, EDInstructions);
	result [0xFD]				= new FZ80::Byte2InstructionCode (0xFD, FDInstructions);

	return (result);
}
