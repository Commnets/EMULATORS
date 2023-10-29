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
			   { "Speed Range", "2.5-10 MHz" } }));
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
		_hlpRegister ({ nullptr, nullptr })
{
	// Create the union of several registers...
	// ...to speed up later the access..
	_afRegister  = { &aRegister (),  &fRegister ()  };
	_bcRegister  = { &bRegister (),  &cRegister ()  };
	_deRegister  = { &dRegister (),  &eRegister ()  };
	_hlRegister  = { &hRegister (),  &lRegister ()  };
	_afpRegister = { &apRegister (), &fpRegister () };
	_bcpRegister = { &bpRegister (), &cpRegister () };
	_depRegister = { &dpRegister (), &epRegister () };
	_hlpRegister = { &hpRegister (), &lpRegister () };
}

// ---
MCHEmul::Registers FZ80::CZ80::createInternalRegisters ()
{
	return (MCHEmul::Registers
			({ 
			   // Main Registers...
			   MCHEmul::Register ((int) _AREGISTER,		"A",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _FREGISTER,		"F",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _BREGISTER,		"B",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _CREGISTER,		"C",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _DREGISTER,		"D",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _EREGISTER,		"E",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _HREGISTER,		"H",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _LREGISTER,		"L",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // Main Registers Copy...
			   MCHEmul::Register ((int) _APREGISTER,	"AP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _APREGISTER,	"FP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _BPREGISTER,	"BP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _CPREGISTER,	"CP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _DPREGISTER,	"DP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _EPREGISTER,	"EP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _HPREGISTER,	"HP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   MCHEmul::Register ((int) _LPREGISTER,	"LP",	{ MCHEmul::UByte::_0 } /** 1 byte long */),
			   // Index Registers...
			   MCHEmul::Register ((int) _IXREGISTER,	"IX",	{ MCHEmul::UByte::_0, MCHEmul::UByte::_0 } /** 2 byte long */),
			   MCHEmul::Register ((int) _IYREGISTER,	"IY",	{ MCHEmul::UByte::_0, MCHEmul::UByte::_0 } /** 2 byte long */)
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
				  { "-", 3 },											// Not used
				  { _HALFCARRYFLAGNAME, _HALFCARRYFLAG },				// Half Carry flag
				  { "-", 5 },											// Not used
				  { _ZEROFLAGNAME, _ZEROFLAG },							// Zero flag
				  { _SIGNFLAGNAME, _SIGNFLAG } }));						// Sign flag
}

// ---
MCHEmul::Instructions FZ80::CZ80::createInstructions ()
{
	// TODO

	return (MCHEmul::Instructions ());
}
