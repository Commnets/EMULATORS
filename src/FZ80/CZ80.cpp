#include <FZ80/CZ80.hpp>
#include <FZ80/Instructions.hpp>

const std::string FZ80::CZ80::_CARRYFLAGNAME = "C";
const std::string FZ80::CZ80::_NEGATIVEFLAGNAME = "N";
const std::string FZ80::CZ80::_PARITYOVERFLOWFLAGNAME = "PO";
const std::string FZ80::CZ80::_HALFCARRYFLAGNAME = "H";
const std::string FZ80::CZ80::_ZEROFLAGNAME = "Z";
const std::string FZ80::CZ80::_SIGNFLAGNAME = "S";

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
FZ80::CZ80::CZ80 (const MCHEmul::CPUArchitecture& a)
	: MCHEmul::CPU (a,
		FZ80::CZ80::createInternalRegisters (), 
		FZ80::CZ80::createStatusRegister (),
		FZ80::CZ80::createInstructions ()),
		_afRegister  ({ nullptr, nullptr }),
		_bcRegister  ({ nullptr, nullptr }),
		_deRegister  ({ nullptr, nullptr }),
		_hlRegister  ({ nullptr, nullptr }),
		_afpRegister ({ nullptr, nullptr }),
		_bcpRegister ({ nullptr, nullptr }),
		_depRegister ({ nullptr, nullptr }),
		_hlpRegister ({ nullptr, nullptr }),
		_ixRegister  ({ nullptr, nullptr }),
		_iyRegister  ({ nullptr, nullptr })
{
	// Create the union of several registers...
	// ...to speed up later the access..
	_afRegister  = { &aRegister   (), &fRegister   () };
	_bcRegister  = { &bRegister   (), &cRegister   () };
	_deRegister  = { &dRegister   (), &eRegister   () };
	_hlRegister  = { &hRegister   (), &lRegister   () };
	_afpRegister = { &apRegister  (), &fpRegister  () };
	_bcpRegister = { &bpRegister  (), &cpRegister  () };
	_depRegister = { &dpRegister  (), &epRegister  () };
	_hlpRegister = { &hpRegister  (), &lpRegister  () };
	_ixRegister	 = { &ixhRegister (), &ixlRegister () };
	_iyRegister	 = { &iyhRegister (), &iylRegister () };
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

	// LD
	result [0x3E]	= new LD_A;
	result [0x7F]	= new LD_AFromA;
	result [0x78]	= new LD_AFromB;
	result [0x79]	= new LD_AFromC;
	result [0x7A]	= new LD_AFromD;
	result [0x7B]	= new LD_AFromE;
	result [0x7C]	= new LD_AFromF;
	result [0x7D]	= new LD_AFromL;
	result [0xED57] = new LD_AFromI;
	result [0xED5F] = new LD_AFromR;
	result [0x7E]	= new LD_AFromAddressHL;
	result [0x0A]	= new LD_AFromAddressBC;
	result [0x1A]	= new LD_AFromAddressDE;
	result [0xDD7E]	= new LD_AFromAddressIndexIX;
	result [0xFD7E]	= new LD_AFromAddressIndexIY;
	result [0x3A]	= new LD_AFromAddress;
	result [0x06]	= new LD_B;
	result [0x47]	= new LD_BFromA;
	result [0x40]	= new LD_BFromB;
	result [0x41]	= new LD_BFromC;
	result [0x42]	= new LD_BFromD;
	result [0x43]	= new LD_BFromE;
	result [0x44]	= new LD_BFromF;
	result [0x45]	= new LD_BFromL;
	result [0xDD46]	= new LD_BFromAddressIndexIX;
	result [0xFD46]	= new LD_BFromAddressIndexIY;
	result [0x0E]	= new LD_C;
	result [0x4F]	= new LD_CFromA;
	result [0x48]	= new LD_CFromB;
	result [0x49]	= new LD_CFromC;
	result [0x4A]	= new LD_CFromD;
	result [0x4B]	= new LD_CFromE;
	result [0x4C]	= new LD_CFromF;
	result [0x4D]	= new LD_CFromL;
	result [0xDD4E]	= new LD_CFromAddressIndexIX;
	result [0xFD4E]	= new LD_CFromAddressIndexIY;
	result [0x16]	= new LD_D;
	result [0x57]	= new LD_DFromA;
	result [0x50]	= new LD_DFromB;
	result [0x51]	= new LD_DFromC;
	result [0x52]	= new LD_DFromD;
	result [0x53]	= new LD_DFromE;
	result [0x54]	= new LD_DFromF;
	result [0x55]	= new LD_DFromL;
	result [0xDD56]	= new LD_DFromAddressIndexIX;
	result [0xFD56]	= new LD_DFromAddressIndexIY;
	result [0x1E]	= new LD_E;
	result [0x5F]	= new LD_EFromA;
	result [0x58]	= new LD_EFromB;
	result [0x59]	= new LD_EFromC;
	result [0x5A]	= new LD_EFromD;
	result [0x5B]	= new LD_EFromE;
	result [0x5C]	= new LD_EFromF;
	result [0x5D]	= new LD_EFromL;
	result [0xDD5E]	= new LD_EFromAddressIndexIX;
	result [0xFD5E]	= new LD_EFromAddressIndexIY;
	result [0x26]	= new LD_H;
	result [0x67]	= new LD_HFromA;
	result [0x60]	= new LD_HFromB;
	result [0x61]	= new LD_HFromC;
	result [0x62]	= new LD_HFromD;
	result [0x63]	= new LD_HFromE;
	result [0x64]	= new LD_HFromF;
	result [0x65]	= new LD_HFromL;
	result [0xDD66]	= new LD_HFromAddressIndexIX;
	result [0xFD66]	= new LD_HFromAddressIndexIY;
	result [0x2E]	= new LD_L;
	result [0x6F]	= new LD_LFromA;
	result [0x68]	= new LD_LFromB;
	result [0x69]	= new LD_LFromC;
	result [0x6A]	= new LD_LFromD;
	result [0x6B]	= new LD_LFromE;
	result [0x6C]	= new LD_LFromF;
	result [0x6D]	= new LD_LFromL;
	result [0xDD6E]	= new LD_LFromAddressIndexIX;
	result [0xFD6E]	= new LD_LFromAddressIndexIY;

	return (result);
}
