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
MCHEmul::UByte FZ80::Z80Port::value () const
{
	return (_cpu -> port (_id));
}

// ---
void FZ80::Z80Port::setValue (const MCHEmul::UByte& v)
{
	_cpu -> setPort (_id, v);
}

// ---
MCHEmul::InfoStructure FZ80::Z80Port::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID", _id);
	result.add ("NAME", _name);
	result.add ("VALUE", value ());

	return (result);
}

// ---
FZ80::CZ80::CZ80 (const Z80PortsMap& pts, const MCHEmul::CPUArchitecture& a)
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
	  _IFF1 (false), _IFF2 (false),
	  _ports (pts), _portsRaw (256, nullptr)
{
	// The reference to the memory has not set still here...
	// It is linked to the CPU at computer (class) level!

	//This type of CPU admites two types of interruptions...
	addInterrupt (new FZ80::INTInterrupt);
	addInterrupt (new FZ80::NMIInterrupt);

	// The map of ports is converted into a list (raw) of ports...
	// The non defined ports will be set as Basic ones!
	FZ80::Z80PortsMap::const_iterator i;
	for (unsigned short j = 0; j < 256; j++)
		_portsRaw [(size_t) j] = ((i = _ports.find ((unsigned char) j)) == _ports.end ()) 
			? new FZ80::Z80BasicPort ((unsigned char) j, "-") : (*i).second;
}

// ---
FZ80::CZ80::~CZ80 ()
{
	for (auto i : _portsRaw)
		delete i;
}

// ---
bool FZ80::CZ80::initialize ()
{
	if (!MCHEmul::CPU::initialize ())
		return (false);

	_INTMode = 0;
	_IFF1 = true;
	_IFF2 = false;

	for (auto& i : _portsRaw)
		i -> initialize ();

	return (true);
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
				{ { _CARRYFLAGNAME, _CARRYFLAG /** bit number */ },			// Carry flag
				  { _NEGATIVEFLAGNAME, _NEGATIVEFLAG },						// Negative flag
				  { _PARITYOVERFLOWFLAGNAME, _PARITYOVERFLOWFLAG },			// Parity Overflow flag
				  { "-", 3 },												// Not used...
				  { _HALFCARRYFLAGNAME, _HALFCARRYFLAG },					// Half Carry flag
				  { "-", 5 },												// Not used...
				  { _ZEROFLAGNAME, _ZEROFLAG },								// Zero flag
				  { _SIGNFLAGNAME, _SIGNFLAG } }));							// Sign flag
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
	result [0x3E]				= new FZ80::LD_A;
	DDInstructions [0xDD3E]		= new FZ80::U1LD_A;							// Undocumented
	FDInstructions [0xFD3E]		= new FZ80::U2LD_A;							// Undocumented
	result [0x7F]				= new FZ80::LD_AFromA;
	DDInstructions [0xDD7F]		= new FZ80::U1LD_AFromA;					// Undocumented
	FDInstructions [0xFD7F]		= new FZ80::U2LD_AFromA;					// Undocumented
	result [0x78]				= new FZ80::LD_AFromB;
	DDInstructions [0xDD78]		= new FZ80::U1LD_AFromB;					// Undocumented
	FDInstructions [0xFD78]		= new FZ80::U2LD_AFromB;					// Undocumented
	result [0x79]				= new FZ80::LD_AFromC;
	DDInstructions [0xDD79]		= new FZ80::U1LD_AFromC;					// Undocumented
	FDInstructions [0xFD79]		= new FZ80::U2LD_AFromC;					// Undocumented
	result [0x7A]				= new FZ80::LD_AFromD;
	DDInstructions [0xDD7A]		= new FZ80::U1LD_AFromD;					// Undocumented
	FDInstructions [0xFD7A]		= new FZ80::U2LD_AFromD;					// Undocumented
	result [0x7B]				= new FZ80::LD_AFromE;
	DDInstructions [0xDD7B]		= new FZ80::U1LD_AFromE;					// Undocumented
	FDInstructions [0xFD7B]		= new FZ80::U2LD_AFromE;					// Undocumented
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
	DDInstructions [0xDD7C]		= new FZ80::LD_AFromIXH;					// Undocumented
	DDInstructions [0xDD7D]		= new FZ80::LD_AFromIXL;					// Undocumented
	FDInstructions [0xFD7C]		= new FZ80::LD_AFromIYH;					// Undocumented
	FDInstructions [0xFD7D]		= new FZ80::LD_AFromIYL;					// Undocumented
	result [0x06]				= new FZ80::LD_B;
	DDInstructions [0xDD06]		= new FZ80::U1LD_B;							// Undocumented
	FDInstructions [0xFD06]		= new FZ80::U2LD_B;							// Undocumented
	result [0x47]				= new FZ80::LD_BFromA;
	DDInstructions [0xDD47]		= new FZ80::U1LD_BFromA;					// Undocumented
	FDInstructions [0xFD47]		= new FZ80::U2LD_BFromA;					// Undocumented
	result [0x40]				= new FZ80::LD_BFromB;
	DDInstructions [0xDD40]		= new FZ80::U1LD_BFromB;					// Undocumented
	FDInstructions [0xFD40]		= new FZ80::U2LD_BFromB;					// Undocumented
	result [0x41]				= new FZ80::LD_BFromC;
	DDInstructions [0xDD41]		= new FZ80::U1LD_BFromC;					// Undocumented
	FDInstructions [0xFD41]		= new FZ80::U2LD_BFromC;					// Undocumented
	result [0x42]				= new FZ80::LD_BFromD;
	DDInstructions [0xDD42]		= new FZ80::U1LD_BFromD;					// Undocumented
	FDInstructions [0xFD42]		= new FZ80::U2LD_BFromD;					// Undocumented
	result [0x43]				= new FZ80::LD_BFromE;
	DDInstructions [0xDD43]		= new FZ80::U1LD_BFromE;					// Undocumented
	FDInstructions [0xFD43]		= new FZ80::U2LD_BFromE;					// Undocumented
	result [0x44]				= new FZ80::LD_BFromH;
	result [0x45]				= new FZ80::LD_BFromL;
	result [0x46]				= new FZ80::LD_BFromAddressHL;
	DDInstructions [0xDD46]		= new FZ80::LD_BFromAddressIndexIX;
	FDInstructions [0xFD46]		= new FZ80::LD_BFromAddressIndexIY;
	DDInstructions [0xDD44]		= new FZ80::LD_BFromIXH;					// Undocumented
	DDInstructions [0xDD45]		= new FZ80::LD_BFromIXL;					// Undocumented
	FDInstructions [0xFD44]		= new FZ80::LD_BFromIYH;					// Undocumented
	FDInstructions [0xFD45]		= new FZ80::LD_BFromIYL;					// Undocumented
	result [0x0E]				= new FZ80::LD_C;
	DDInstructions [0xDD0E]		= new FZ80::U1LD_C;							// Undocumented
	FDInstructions [0xFD0E]		= new FZ80::U2LD_C;							// Undocumented
	result [0x4F]				= new FZ80::LD_CFromA;
	DDInstructions [0xDD4F]		= new FZ80::U1LD_CFromA;					// Undocumented
	FDInstructions [0xFD4F]		= new FZ80::U2LD_CFromA;					// Undocumented
	result [0x48]				= new FZ80::LD_CFromB;
	DDInstructions [0xDD48]		= new FZ80::U1LD_CFromB;					// Undocumented
	FDInstructions [0xFD48]		= new FZ80::U2LD_CFromB;					// Undocumented
	result [0x49]				= new FZ80::LD_CFromC;
	DDInstructions [0xDD49]		= new FZ80::U1LD_CFromC;					// Undocumented
	FDInstructions [0xFD49]		= new FZ80::U2LD_CFromC;					// Undocumented
	result [0x4A]				= new FZ80::LD_CFromD;
	DDInstructions [0xDD4A]		= new FZ80::U1LD_CFromD;					// Undocumented
	FDInstructions [0xFD4A]		= new FZ80::U2LD_CFromD;					// Undocumented
	result [0x4B]				= new FZ80::LD_CFromE;
	DDInstructions [0xDD4B]		= new FZ80::U1LD_CFromE;					// Undocumented
	FDInstructions [0xFD4B]		= new FZ80::U2LD_CFromE;					// Undocumented
	result [0x4C]				= new FZ80::LD_CFromH;
	result [0x4D]				= new FZ80::LD_CFromL;
	result [0x4E]				= new FZ80::LD_CFromAddressHL;
	DDInstructions [0xDD4E]		= new FZ80::LD_CFromAddressIndexIX;
	FDInstructions [0xFD4E]		= new FZ80::LD_CFromAddressIndexIY;
	DDInstructions [0xDD4C]		= new FZ80::LD_CFromIXH;					// Undocumented
	DDInstructions [0xDD4D]		= new FZ80::LD_CFromIXL;					// Undocumented
	FDInstructions [0xFD4C]		= new FZ80::LD_CFromIYH;					// Undocumented
	FDInstructions [0xFD4D]		= new FZ80::LD_CFromIYL;					// Undocumented
	result [0x16]				= new FZ80::LD_D;
	DDInstructions [0xDD16]		= new FZ80::U1LD_D;							// Undocumented
	FDInstructions [0xFD16]		= new FZ80::U2LD_D;							// Undocumented
	result [0x57]				= new FZ80::LD_DFromA;
	DDInstructions [0xDD57]		= new FZ80::U1LD_DFromA;					// Undocumented
	FDInstructions [0xFD57]		= new FZ80::U2LD_DFromA;					// Undocumented
	result [0x50]				= new FZ80::LD_DFromB;
	DDInstructions [0xDD50]		= new FZ80::U1LD_DFromB;					// Undocumented
	FDInstructions [0xFD50]		= new FZ80::U2LD_DFromB;					// Undocumented
	result [0x51]				= new FZ80::LD_DFromC;
	DDInstructions [0xDD51]		= new FZ80::U1LD_DFromC;					// Undocumented
	FDInstructions [0xFD51]		= new FZ80::U2LD_DFromC;					// Undocumented
	result [0x52]				= new FZ80::LD_DFromD;
	DDInstructions [0xDD52]		= new FZ80::U1LD_DFromD;					// Undocumented
	FDInstructions [0xFD52]		= new FZ80::U2LD_DFromD;					// Undocumented
	result [0x53]				= new FZ80::LD_DFromE;
	DDInstructions [0xDD53]		= new FZ80::U1LD_DFromE;					// Undocumented
	FDInstructions [0xFD53]		= new FZ80::U2LD_DFromE;					// Undocumented
	result [0x54]				= new FZ80::LD_DFromH;
	result [0x55]				= new FZ80::LD_DFromL;
	result [0x56]				= new FZ80::LD_DFromAddressHL;
	DDInstructions [0xDD56]		= new FZ80::LD_DFromAddressIndexIX;
	FDInstructions [0xFD56]		= new FZ80::LD_DFromAddressIndexIY;
	DDInstructions [0xDD54]		= new FZ80::LD_DFromIXH;					// Undocumented
	DDInstructions [0xDD55]		= new FZ80::LD_DFromIXL;					// Undocumented
	FDInstructions [0xFD54]		= new FZ80::LD_DFromIYH;					// Undocumented
	FDInstructions [0xFD55]		= new FZ80::LD_DFromIYL;					// Undocumented
	result [0x1E]				= new FZ80::LD_E;
	DDInstructions [0xDD1E]		= new FZ80::U1LD_E;							// Undocumented
	FDInstructions [0xFD1E]		= new FZ80::U2LD_E;							// Undocumented
	result [0x5F]				= new FZ80::LD_EFromA;
	DDInstructions [0xDD5F]		= new FZ80::U1LD_EFromA;					// Undocumented
	FDInstructions [0xFD5F]		= new FZ80::U2LD_EFromA;					// Undocumented
	result [0x58]				= new FZ80::LD_EFromB;
	DDInstructions [0xDD58]		= new FZ80::U1LD_EFromB;					// Undocumented
	FDInstructions [0xFD58]		= new FZ80::U2LD_EFromB;					// Undocumented
	result [0x59]				= new FZ80::LD_EFromC;
	DDInstructions [0xDD59]		= new FZ80::U1LD_EFromC;					// Undocumented
	FDInstructions [0xFD59]		= new FZ80::U2LD_EFromC;					// Undocumented
	result [0x5A]				= new FZ80::LD_EFromD;
	DDInstructions [0xDD5A]		= new FZ80::U1LD_EFromD;					// Undocumented
	FDInstructions [0xFD5A]		= new FZ80::U2LD_EFromD;					// Undocumented
	result [0x5B]				= new FZ80::LD_EFromE;
	DDInstructions [0xDD5B]		= new FZ80::U1LD_EFromE;					// Undocumented
	FDInstructions [0xFD5B]		= new FZ80::U2LD_EFromE;					// Undocumented
	result [0x5C]				= new FZ80::LD_EFromH;
	result [0x5D]				= new FZ80::LD_EFromL;
	result [0x5E]				= new FZ80::LD_EFromAddressHL;
	DDInstructions [0xDD5E]		= new FZ80::LD_EFromAddressIndexIX;
	FDInstructions [0xFD5E]		= new FZ80::LD_EFromAddressIndexIY;
	DDInstructions [0xDD5C]		= new FZ80::LD_EFromIXH;					// Undocumented
	DDInstructions [0xDD5D]		= new FZ80::LD_EFromIXL;					// Undocumented
	FDInstructions [0xFD5C]		= new FZ80::LD_EFromIYH;					// Undocumented
	FDInstructions [0xFD5D]		= new FZ80::LD_EFromIYL;					// Undocumented
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
	result [0x36]				= new FZ80::LD_IndirectHL;
	result [0x77]				= new FZ80::LD_IndirectHLFromA;
	result [0x70]				= new FZ80::LD_IndirectHLFromB;
	result [0x71]				= new FZ80::LD_IndirectHLFromC;
	result [0x72]				= new FZ80::LD_IndirectHLFromD;
	result [0x73]				= new FZ80::LD_IndirectHLFromE;
	result [0x74]				= new FZ80::LD_IndirectHLFromF;
	result [0x75]				= new FZ80::LD_IndirectHLFromL;
	result [0x02]				= new FZ80::LD_IndirectBCFromA;
	result [0x12]				= new FZ80::LD_IndirectDEFromA;
	DDInstructions [0xDD36]		= new FZ80::LD_IndirectIndexIX;
	DDInstructions [0xDD77]		= new FZ80::LD_IndirectIndexIXFromA;
	DDInstructions [0xDD70]		= new FZ80::LD_IndirectIndexIXFromB;
	DDInstructions [0xDD71]		= new FZ80::LD_IndirectIndexIXFromC;
	DDInstructions [0xDD72]		= new FZ80::LD_IndirectIndexIXFromD;
	DDInstructions [0xDD73]		= new FZ80::LD_IndirectIndexIXFromE;
	DDInstructions [0xDD74]		= new FZ80::LD_IndirectIndexIXFromH;
	DDInstructions [0xDD75]		= new FZ80::LD_IndirectIndexIXFromL;
	FDInstructions [0xFD36]		= new FZ80::LD_IndirectIndexIY;
	FDInstructions [0xFD77]		= new FZ80::LD_IndirectIndexIYFromA;
	FDInstructions [0xFD70]		= new FZ80::LD_IndirectIndexIYFromB;
	FDInstructions [0xFD71]		= new FZ80::LD_IndirectIndexIYFromC;
	FDInstructions [0xFD72]		= new FZ80::LD_IndirectIndexIYFromD;
	FDInstructions [0xFD73]		= new FZ80::LD_IndirectIndexIYFromE;
	FDInstructions [0xFD74]		= new FZ80::LD_IndirectIndexIYFromH;
	FDInstructions [0xFD75]		= new FZ80::LD_IndirectIndexIYFromL;
	result [0x32]				= new FZ80::LD_AddressFromA;
	result [0x22]				= new FZ80::LD_AddressFromHL;
	EDInstructions [0xED63]		= new FZ80::U3LD_AddressFromHL;				// Undocumented
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
	EDInstructions [0xED6B]		= new FZ80::U3LD_HLFromAddress;				// Undocumented
	DDInstructions [0xDD21]		= new FZ80::LD_IX;
	DDInstructions [0xDD2A]		= new FZ80::LD_IXFromAddress;
	DDInstructions [0xDD26]		= new FZ80::LD_IXH;							// Undocumented
	DDInstructions [0xDD67]		= new FZ80::LD_IXHFromA;					// Undocumented
	DDInstructions [0xDD60]		= new FZ80::LD_IXHFromB;					// Undocumented
	DDInstructions [0xDD61]		= new FZ80::LD_IXHFromC;					// Undocumented
	DDInstructions [0xDD62]		= new FZ80::LD_IXHFromD;					// Undocumented
	DDInstructions [0xDD63]		= new FZ80::LD_IXHFromE;					// Undocumented
	DDInstructions [0xDD64]		= new FZ80::LD_IXHFromIXH;					// Undocumented
	DDInstructions [0xDD65]		= new FZ80::LD_IXHFromIXL;					// Undocumented
	DDInstructions [0xDD2E]		= new FZ80::LD_IXL;							// Undocumented
	DDInstructions [0xDD6F]		= new FZ80::LD_IXLFromA;					// Undocumented
	DDInstructions [0xDD68]		= new FZ80::LD_IXLFromB;					// Undocumented
	DDInstructions [0xDD69]		= new FZ80::LD_IXLFromC;					// Undocumented
	DDInstructions [0xDD6A]		= new FZ80::LD_IXLFromD;					// Undocumented
	DDInstructions [0xDD6B]		= new FZ80::LD_IXLFromE;					// Undocumented
	DDInstructions [0xDD6C]		= new FZ80::LD_IXLFromIXH;					// Undocumented
	DDInstructions [0xDD6D]		= new FZ80::LD_IXLFromIXL;					// Undocumented
	FDInstructions [0xFD21]		= new FZ80::LD_IY;
	FDInstructions [0xFD2A]		= new FZ80::LD_IYFromAddress;
	FDInstructions [0xFD26]		= new FZ80::LD_IYH;							// Undocumented
	FDInstructions [0xFD67]		= new FZ80::LD_IYHFromA;					// Undocumented
	FDInstructions [0xFD60]		= new FZ80::LD_IYHFromB;					// Undocumented
	FDInstructions [0xFD61]		= new FZ80::LD_IYHFromC;					// Undocumented
	FDInstructions [0xFD62]		= new FZ80::LD_IYHFromD;					// Undocumented
	FDInstructions [0xFD63]		= new FZ80::LD_IYHFromE;					// Undocumented
	FDInstructions [0xFD64]		= new FZ80::LD_IYHFromIYH;					// Undocumented
	FDInstructions [0xFD65]		= new FZ80::LD_IYHFromIYL;					// Undocumented
	FDInstructions [0xFD2E]		= new FZ80::LD_IYL;							// Undocumented
	FDInstructions [0xFD6F]		= new FZ80::LD_IYLFromA;					// Undocumented
	FDInstructions [0xFD68]		= new FZ80::LD_IYLFromB;					// Undocumented
	FDInstructions [0xFD69]		= new FZ80::LD_IYLFromC;					// Undocumented
	FDInstructions [0xFD6A]		= new FZ80::LD_IYLFromD;					// Undocumented
	FDInstructions [0xFD6B]		= new FZ80::LD_IYLFromE;					// Undocumented
	FDInstructions [0xFD6C]		= new FZ80::LD_IYLFromIYH;					// Undocumented
	FDInstructions [0xFD6D]		= new FZ80::LD_IYLFromIYL;					// Undocumented
	result [0x31]				= new FZ80::LD_SP;
	result [0xF9]				= new FZ80::LD_SPFromHL;
	DDInstructions [0xDDF9]		= new FZ80::LD_SPFromIX;
	FDInstructions [0xFDF9]		= new FZ80::LD_SPFromIY;
	EDInstructions [0xED7B]		= new FZ80::LD_SPFromAddress;

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
	DDInstructions [0xDDA7]		= new FZ80::U1AND_A;						// Undocumented
	FDInstructions [0xFDA7]		= new FZ80::U2AND_A;						// Undocumented
	result [0xA0]				= new FZ80::AND_B;
	DDInstructions [0xDDA0]		= new FZ80::U1AND_B;						// Undocumented
	FDInstructions [0xFDA0]		= new FZ80::U2AND_B;						// Undocumented
	result [0xA1]				= new FZ80::AND_C;
	DDInstructions [0xDDA1]		= new FZ80::U1AND_C;						// Undocumented
	FDInstructions [0xFDA1]		= new FZ80::U2AND_C;						// Undocumented
	result [0xA2]				= new FZ80::AND_D;
	DDInstructions [0xDDA2]		= new FZ80::U1AND_D;						// Undocumented
	FDInstructions [0xFDA2]		= new FZ80::U2AND_D;						// Undocumented
	result [0xA3]				= new FZ80::AND_E;
	DDInstructions [0xDDA3]		= new FZ80::U1AND_E;						// Undocumented
	FDInstructions [0xFDA3]		= new FZ80::U2AND_E;						// Undocumented
	result [0xA4]				= new FZ80::AND_H;
	result [0xA5]				= new FZ80::AND_L;
	result [0xA6]				= new FZ80::AND_IndirectHL;
	DDInstructions [0xDDA6]		= new FZ80::AND_IndirectIndexIX;
	FDInstructions [0xFDA6]		= new FZ80::AND_IndirectIndexIY;
	result [0xE6]				= new FZ80::AND;
	DDInstructions [0xDDA4]		= new FZ80::AND_IXH;						// Undocumented
	DDInstructions [0xDDA5]		= new FZ80::AND_IXL;						// Undocumented
	FDInstructions [0xFDA4]		= new FZ80::AND_IYH;						// Undocumented
	FDInstructions [0xFDA5]		= new FZ80::AND_IYL;						// Undocumented

	// OR: Always with A
	result [0xB7]				= new FZ80::OR_A;
	DDInstructions [0xDDB7]		= new FZ80::U1OR_A;							// Undocumented
	FDInstructions [0xFDB7]		= new FZ80::U2OR_A;							// Undocumented
	result [0xB0]				= new FZ80::OR_B;
	DDInstructions [0xDDB0]		= new FZ80::U1OR_B;							// Undocumented
	FDInstructions [0xFDB0]		= new FZ80::U2OR_B;							// Undocumented
	result [0xB1]				= new FZ80::OR_C;
	DDInstructions [0xDDB1]		= new FZ80::U1OR_C;							// Undocumented
	FDInstructions [0xFDB1]		= new FZ80::U2OR_C;							// Undocumented
	result [0xB2]				= new FZ80::OR_D;
	DDInstructions [0xDDB2]		= new FZ80::U1OR_D;							// Undocumented
	FDInstructions [0xFDB2]		= new FZ80::U2OR_D;							// Undocumented
	result [0xB3]				= new FZ80::OR_E;
	DDInstructions [0xDDB3]		= new FZ80::U1OR_E;							// Undocumented
	FDInstructions [0xFDB3]		= new FZ80::U2OR_E;							// Undocumented
	result [0xB4]				= new FZ80::OR_H;
	result [0xB5]				= new FZ80::OR_L;
	result [0xB6]				= new FZ80::OR_IndirectHL;
	DDInstructions [0xDDB6]		= new FZ80::OR_IndirectIndexIX;
	FDInstructions [0xFDB6]		= new FZ80::OR_IndirectIndexIY;
	result [0xF6]				= new FZ80::OR;
	DDInstructions [0xDDB4]		= new FZ80::OR_IXH;							// Undocumented
	DDInstructions [0xDDB5]		= new FZ80::OR_IXL;							// Undocumented
	FDInstructions [0xFDB4]		= new FZ80::OR_IYH;							// Undocumented
	FDInstructions [0xFDB5]		= new FZ80::OR_IYL;							// Undocumented

	// XOR: Always with A
	result [0xAF]				= new FZ80::XOR_A;
	DDInstructions [0xDDAF]		= new FZ80::U1XOR_A;						// Undocumented
	FDInstructions [0xFDAF]		= new FZ80::U2XOR_A;						// Undocumented
	result [0xA8]				= new FZ80::XOR_B;
	DDInstructions [0xDDA8]		= new FZ80::U1XOR_B;						// Undocumented
	FDInstructions [0xFDA8]		= new FZ80::U2XOR_B;						// Undocumented
	result [0xA9]				= new FZ80::XOR_C;
	DDInstructions [0xDDA9]		= new FZ80::U1XOR_C;						// Undocumented
	FDInstructions [0xFDA9]		= new FZ80::U2XOR_C;						// Undocumented
	result [0xAA]				= new FZ80::XOR_D;
	DDInstructions [0xDDAA]		= new FZ80::U1XOR_D;						// Undocumented
	FDInstructions [0xFDAA]		= new FZ80::U2XOR_D;						// Undocumented
	result [0xAB]				= new FZ80::XOR_E;
	DDInstructions [0xDDAB]		= new FZ80::U1XOR_E;						// Undocumented
	FDInstructions [0xFDAB]		= new FZ80::U2XOR_E;						// Undocumented
	result [0xAC]				= new FZ80::XOR_H;
	result [0xAD]				= new FZ80::XOR_L;
	result [0xAE]				= new FZ80::XOR_IndirectHL;
	DDInstructions [0xDDAE]		= new FZ80::XOR_IndirectIndexIX;
	FDInstructions [0xFDAE]		= new FZ80::XOR_IndirectIndexIY;
	result [0xEE]				= new FZ80::XOR;
	DDInstructions [0xDDAC]		= new FZ80::XOR_IXH;						// Undocumented
	DDInstructions [0xDDAD]		= new FZ80::XOR_IXL;						// Undocumented
	FDInstructions [0xFDAC]		= new FZ80::XOR_IYH;						// Undocumented
	FDInstructions [0xFDAD]		= new FZ80::XOR_IYL;						// Undocumented

	// ADD without carry
	result [0x87]				= new FZ80::ADD_AWithA;
	DDInstructions [0xDD87]		= new FZ80::U1ADD_AWithA;					// Undocumented
	FDInstructions [0xFD87]		= new FZ80::U2ADD_AWithA;					// Undocumented
	result [0x80]				= new FZ80::ADD_AWithB;
	DDInstructions [0xDD80]		= new FZ80::U1ADD_AWithB;					// Undocumented
	FDInstructions [0xFD80]		= new FZ80::U2ADD_AWithB;					// Undocumented
	result [0x81]				= new FZ80::ADD_AWithC;
	DDInstructions [0xDD81]		= new FZ80::U1ADD_AWithC;					// Undocumented
	FDInstructions [0xFD81]		= new FZ80::U2ADD_AWithC;					// Undocumented
	result [0x82]				= new FZ80::ADD_AWithD;
	DDInstructions [0xDD82]		= new FZ80::U1ADD_AWithD;					// Undocumented
	FDInstructions [0xFD82]		= new FZ80::U2ADD_AWithD;					// Undocumented
	result [0x83]				= new FZ80::ADD_AWithE;
	DDInstructions [0xDD83]		= new FZ80::U1ADD_AWithE;					// Undocumented
	FDInstructions [0xFD83]		= new FZ80::U2ADD_AWithE;					// Undocumented
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
	DDInstructions [0xDD84]		= new FZ80::ADD_AWithIXH;					// Undocumented
	DDInstructions [0xDD85]		= new FZ80::ADD_AWithIXL;					// Undocumented
	FDInstructions [0xFD84]		= new FZ80::ADD_AWithIYH;					// Undocumented
	FDInstructions [0xFD85]		= new FZ80::ADD_AWithIYL;					// Undocumented
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
	DDInstructions [0xDD8F]		= new FZ80::U1ADC_AWithA;					// Undocumented
	FDInstructions [0xFD8F]		= new FZ80::U2ADC_AWithA;					// Undocumented
	result [0x88]				= new FZ80::ADC_AWithB;
	DDInstructions [0xDD88]		= new FZ80::U1ADC_AWithB;					// Undocumented
	FDInstructions [0xFD88]		= new FZ80::U2ADC_AWithB;					// Undocumented
	result [0x89]				= new FZ80::ADC_AWithC;
	DDInstructions [0xDD89]		= new FZ80::U1ADC_AWithC;					// Undocumented
	FDInstructions [0xFD89]		= new FZ80::U2ADC_AWithC;					// Undocumented
	result [0x8A]				= new FZ80::ADC_AWithD;
	DDInstructions [0xDD8A]		= new FZ80::U1ADC_AWithD;					// Undocumented
	FDInstructions [0xFD8A]		= new FZ80::U2ADC_AWithD;					// Undocumented
	result [0x8B]				= new FZ80::ADC_AWithE;
	DDInstructions [0xDD8B]		= new FZ80::U1ADC_AWithE;					// Undocumented
	FDInstructions [0xFD8B]		= new FZ80::U2ADC_AWithE;					// Undocumented
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
	DDInstructions [0xDD8C]		= new FZ80::ADC_AWithIXH;					// Undocumented
	DDInstructions [0xDD8D]		= new FZ80::ADC_AWithIXL;					// Undocumented
	FDInstructions [0xFD8C]		= new FZ80::ADC_AWithIYH;					// Undocumented
	FDInstructions [0xFD8D]		= new FZ80::ADC_AWithIYL;					// Undocumented

	// SUB without carry
	result [0x97]				= new FZ80::SUB_AWithA;
	DDInstructions [0xDD97]		= new FZ80::U1SUB_AWithA;					// Undocumented
	FDInstructions [0xFD97]		= new FZ80::U2SUB_AWithA;					// Undocumented
	result [0x90]				= new FZ80::SUB_AWithB;
	DDInstructions [0xDD90]		= new FZ80::U1SUB_AWithB;					// Undocumented
	FDInstructions [0xFD90]		= new FZ80::U2SUB_AWithB;					// Undocumented
	result [0x91]				= new FZ80::SUB_AWithC;
	DDInstructions [0xDD91]		= new FZ80::U1SUB_AWithC;					// Undocumented
	FDInstructions [0xFD91]		= new FZ80::U2SUB_AWithC;					// Undocumented
	result [0x92]				= new FZ80::SUB_AWithD;
	DDInstructions [0xDD92]		= new FZ80::U1SUB_AWithD;					// Undocumented
	FDInstructions [0xFD92]		= new FZ80::U2SUB_AWithD;					// Undocumented
	result [0x93]				= new FZ80::SUB_AWithE;
	DDInstructions [0xDD93]		= new FZ80::U1SUB_AWithE;					// Undocumented
	FDInstructions [0xFD93]		= new FZ80::U2SUB_AWithE;					// Undocumented
	result [0x94]				= new FZ80::SUB_AWithH;
	result [0x95]				= new FZ80::SUB_AWithL;
	result [0x96]				= new FZ80::SUB_AWithIndirectHL;
	DDInstructions [0xDD96]		= new FZ80::SUB_AWithIndirectIndexIX;
	FDInstructions [0xFD96]		= new FZ80::SUB_AWithIndirectIndexIY;
	result [0xD6]				= new FZ80::SUB_A;
	DDInstructions [0xDD94]		= new FZ80::SUB_AWithIXH;					// Undocumented
	DDInstructions [0xDD95]		= new FZ80::SUB_AWithIXL;					// Undocumented
	FDInstructions [0xFD94]		= new FZ80::SUB_AWithIYH;					// Undocumented
	FDInstructions [0xFD95]		= new FZ80::SUB_AWithIYL;					// Undocumented
	
	// SBC with carry
	result [0x9F]				= new FZ80::SBC_AWithA;
	DDInstructions [0xDD9F]		= new FZ80::U1SBC_AWithA;					// Undocumented
	FDInstructions [0xFD9F]		= new FZ80::U2SBC_AWithA;					// Undocumented
	result [0x98]				= new FZ80::SBC_AWithB;
	DDInstructions [0xDD98]		= new FZ80::U1SBC_AWithB;					// Undocumented
	FDInstructions [0xFD98]		= new FZ80::U2SBC_AWithB;					// Undocumented
	result [0x99]				= new FZ80::SBC_AWithC;
	DDInstructions [0xDD99]		= new FZ80::U1SBC_AWithC;					// Undocumented
	FDInstructions [0xFD99]		= new FZ80::U2SBC_AWithC;					// Undocumented
	result [0x9A]				= new FZ80::SBC_AWithD;
	DDInstructions [0xDD9A]		= new FZ80::U1SBC_AWithD;					// Undocumented
	FDInstructions [0xFD9A]		= new FZ80::U2SBC_AWithD;					// Undocumented
	result [0x9B]				= new FZ80::SBC_AWithE;
	DDInstructions [0xDD9B]		= new FZ80::U1SBC_AWithE;					// Undocumented
	FDInstructions [0xFD9B]		= new FZ80::U2SBC_AWithE;					// Undocumented
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
	DDInstructions [0xDD9C]		= new FZ80::SBC_AWithIXH;					// Undocumented
	DDInstructions [0xDD9D]		= new FZ80::SBC_AWithIXL;					// Undocumented
	FDInstructions [0xFD9C]		= new FZ80::SBC_AWithIYH;					// Undocumented
	FDInstructions [0xFD9D]		= new FZ80::SBC_AWithIYL;					// Undocumented

	// INC
	result [0x3C]				= new FZ80::INC_A;
	DDInstructions [0xDD3C]		= new FZ80::U1INC_A;						// Undocumented
	FDInstructions [0xFD3C]		= new FZ80::U2INC_A;						// Undocumented
	result [0x04]				= new FZ80::INC_B;
	DDInstructions [0xDD04]		= new FZ80::U1INC_B;						// Undocumented
	FDInstructions [0xFD04]		= new FZ80::U2INC_B;						// Undocumented
	result [0x0C]				= new FZ80::INC_C;
	DDInstructions [0xDD0C]		= new FZ80::U1INC_C;						// Undocumented
	FDInstructions [0xFD0C]		= new FZ80::U2INC_C;						// Undocumented
	result [0x14]				= new FZ80::INC_D;
	DDInstructions [0xDD14]		= new FZ80::U1INC_D;						// Undocumented
	FDInstructions [0xFD14]		= new FZ80::U2INC_D;						// Undocumented
	result [0x1C]				= new FZ80::INC_E;
	DDInstructions [0xDD1C]		= new FZ80::U1INC_E;						// Undocumented
	FDInstructions [0xFD1C]		= new FZ80::U2INC_E;						// Undocumented
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
	DDInstructions [0xDD24]		= new FZ80::INC_IXH;						// Undocumented
	DDInstructions [0xDD2C]		= new FZ80::INC_IXL;						// Undocumented
	FDInstructions [0xFD24]		= new FZ80::INC_IYH;						// Undocumented
	FDInstructions [0xFD2C]		= new FZ80::INC_IYL;						// Undocumented

	// DEC
	result [0x3D]				= new FZ80::DEC_A;
	DDInstructions [0xDD3D]		= new FZ80::U1DEC_A;						// Undocumented
	FDInstructions [0xFD3D]		= new FZ80::U2DEC_A;						// Undocumented
	result [0x05]				= new FZ80::DEC_B;
	DDInstructions [0xDD05]		= new FZ80::U1DEC_B;						// Undocumented
	FDInstructions [0xFD05]		= new FZ80::U2DEC_B;						// Undocumented
	result [0x0D]				= new FZ80::DEC_C;
	DDInstructions [0xDD0D]		= new FZ80::U1DEC_C;						// Undocumented
	FDInstructions [0xFD0D]		= new FZ80::U2DEC_C;						// Undocumented
	result [0x15]				= new FZ80::DEC_D;
	DDInstructions [0xDD15]		= new FZ80::U1DEC_D;						// Undocumented
	FDInstructions [0xFD15]		= new FZ80::U2DEC_D;						// Undocumented
	result [0x1D]				= new FZ80::DEC_E;
	DDInstructions [0xDD1D]		= new FZ80::U1DEC_E;						// Undocumented
	FDInstructions [0xFD1D]		= new FZ80::U2DEC_E;						// Undocumented
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
	DDInstructions [0xDD25]		= new FZ80::DEC_IXH;						// Undocumented
	DDInstructions [0xDD2D]		= new FZ80::DEC_IXL;						// Undocumented
	FDInstructions [0xFD25]		= new FZ80::DEC_IYH;						// Undocumented
	FDInstructions [0xFD2D]		= new FZ80::DEC_IYL;						// Undocumented

	// CP
	result [0xBF]				= new FZ80::CP_WithA;
	DDInstructions [0xDDBF]		= new FZ80::U1CP_WithA;						// Undocumented
	FDInstructions [0xFDBF]		= new FZ80::U2CP_WithA;						// Undocumented
	result [0xB8]				= new FZ80::CP_WithB;
	DDInstructions [0xDDB8]		= new FZ80::U1CP_WithB;						// Undocumented
	FDInstructions [0xFDB8]		= new FZ80::U2CP_WithB;						// Undocumented
	result [0xB9]				= new FZ80::CP_WithC;
	DDInstructions [0xDDB9]		= new FZ80::U1CP_WithC;						// Undocumented
	FDInstructions [0xFDB9]		= new FZ80::U2CP_WithC;						// Undocumented
	result [0xBA]				= new FZ80::CP_WithD;
	DDInstructions [0xDDBA]		= new FZ80::U1CP_WithD;						// Undocumented
	FDInstructions [0xFDBA]		= new FZ80::U2CP_WithD;						// Undocumented
	result [0xBB]				= new FZ80::CP_WithE;
	DDInstructions [0xDDBB]		= new FZ80::U1CP_WithE;						// Undocumented
	FDInstructions [0xFDBB]		= new FZ80::U2CP_WithE;						// Undocumented
	result [0xBC]				= new FZ80::CP_WithH;			
	result [0xBD]				= new FZ80::CP_WithL;			
	result [0xBE]				= new FZ80::CP_WithIndirectHL;
	DDInstructions [0xDDBE]		= new FZ80::CP_WithIndirectIndexIX;
	FDInstructions [0xFDBE]		= new FZ80::CP_WithIndirectIndexIY;
	result [0xFE]				= new FZ80::CP;
	DDInstructions [0xDDBC]		= new FZ80::CP_WithIXH;						// Undocumented
	DDInstructions [0xDDBD]		= new FZ80::CP_WithIXL;						// Undocumented
	FDInstructions [0xFDBC]		= new FZ80::CP_WithIYH;						// Undocumented
	FDInstructions [0xFDBD]		= new FZ80::CP_WithIYL;						// Undocumented

	// Generic Instructions
	result [0x27]				= new FZ80::DAA;
	result [0x2F]				= new FZ80::CPL;
	EDInstructions [0xED44]		= new FZ80::NEG;
	result [0x3F]				= new FZ80::CCF;
	result [0x37]				= new FZ80::SCF;
	result [0x00]				= new FZ80::NOP;
	result [0x76]				= new FZ80::HALT;
	result [0xF3]				= new FZ80::DI;
	result [0xFB]				= new FZ80::EI;
	EDInstructions [0xED46]		= new FZ80::IM_0;
	EDInstructions [0xED56]		= new FZ80::IM_1;
	EDInstructions [0xED5E]		= new FZ80::IM_2;

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
	DDCBInstructions [0xDDCB07] = new FZ80::RLC_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB00] = new FZ80::RLC_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB01] = new FZ80::RLC_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB02] = new FZ80::RLC_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB03] = new FZ80::RLC_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB04] = new FZ80::RLC_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB05] = new FZ80::RLC_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB06] = new FZ80::RLC_IndirectIndexIY;
	FDCBInstructions [0xFDCB07] = new FZ80::RLC_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB00] = new FZ80::RLC_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB01] = new FZ80::RLC_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB02] = new FZ80::RLC_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB03] = new FZ80::RLC_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB04] = new FZ80::RLC_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB05] = new FZ80::RLC_IndirectIndexIYCopyL;		// Undocumented

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
	DDCBInstructions [0xDDCB16] = new FZ80::RL_IndirectIndexIX;
	DDCBInstructions [0xDDCB17] = new FZ80::RL_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB10] = new FZ80::RL_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB11] = new FZ80::RL_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB12] = new FZ80::RL_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB13] = new FZ80::RL_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB14] = new FZ80::RL_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB15] = new FZ80::RL_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB16] = new FZ80::RL_IndirectIndexIY;
	FDCBInstructions [0xFDCB17] = new FZ80::RL_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB10] = new FZ80::RL_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB11] = new FZ80::RL_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB12] = new FZ80::RL_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB13] = new FZ80::RL_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB14] = new FZ80::RL_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB15] = new FZ80::RL_IndirectIndexIYCopyL;		// Undocumented

	// RLD
	EDInstructions [0xED6F]		= new FZ80::RLD;

	// SLA
	CBInstructions [0xCB27]		= new FZ80::SLA_A;
	CBInstructions [0xCB20]		= new FZ80::SLA_B;
	CBInstructions [0xCB21]		= new FZ80::SLA_C;
	CBInstructions [0xCB22]		= new FZ80::SLA_D;
	CBInstructions [0xCB23]		= new FZ80::SLA_E;
	CBInstructions [0xCB24]		= new FZ80::SLA_H;
	CBInstructions [0xCB25]		= new FZ80::SLA_L;
	CBInstructions [0xCB26]		= new FZ80::SLA_IndirectHL;
	DDCBInstructions [0xDDCB26] = new FZ80::SLA_IndirectIndexIX;
	DDCBInstructions [0xDDCB27] = new FZ80::SLA_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB20] = new FZ80::SLA_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB21] = new FZ80::SLA_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB22] = new FZ80::SLA_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB23] = new FZ80::SLA_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB24] = new FZ80::SLA_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB25] = new FZ80::SLA_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB26] = new FZ80::SLA_IndirectIndexIY;
	FDCBInstructions [0xFDCB27] = new FZ80::SLA_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB20] = new FZ80::SLA_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB21] = new FZ80::SLA_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB22] = new FZ80::SLA_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB23] = new FZ80::SLA_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB24] = new FZ80::SLA_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB25] = new FZ80::SLA_IndirectIndexIYCopyL;		// Undocumented

	// SLL
	CBInstructions [0xCB37]		= new FZ80::SLL_A;
	CBInstructions [0xCB30]		= new FZ80::SLL_B;
	CBInstructions [0xCB31]		= new FZ80::SLL_C;
	CBInstructions [0xCB32]		= new FZ80::SLL_D;
	CBInstructions [0xCB33]		= new FZ80::SLL_E;
	CBInstructions [0xCB34]		= new FZ80::SLL_H;
	CBInstructions [0xCB35]		= new FZ80::SLL_L;
	CBInstructions [0xCB36]		= new FZ80::SLL_IndirectHL;
	DDCBInstructions [0xDDCB36] = new FZ80::SLL_IndirectIndexIX;
	DDCBInstructions [0xDDCB37] = new FZ80::SLL_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB30] = new FZ80::SLL_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB31] = new FZ80::SLL_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB32] = new FZ80::SLL_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB33] = new FZ80::SLL_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB34] = new FZ80::SLL_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB35] = new FZ80::SLL_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB36] = new FZ80::SLL_IndirectIndexIY;
	FDCBInstructions [0xFDCB37] = new FZ80::SLL_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB30] = new FZ80::SLL_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB31] = new FZ80::SLL_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB32] = new FZ80::SLL_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB33] = new FZ80::SLL_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB34] = new FZ80::SLL_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB35] = new FZ80::SLL_IndirectIndexIYCopyL;		// Undocumented

	// RRC
	result [0x0F]				= new FZ80::RRC_A;
	CBInstructions [0xCB0F]		= new FZ80::RRC_ASlower;
	CBInstructions [0xCB08]		= new FZ80::RRC_B;
	CBInstructions [0xCB09]		= new FZ80::RRC_C;
	CBInstructions [0xCB0A]		= new FZ80::RRC_D;
	CBInstructions [0xCB0B]		= new FZ80::RRC_E;
	CBInstructions [0xCB0C]		= new FZ80::RRC_F;
	CBInstructions [0xCB0D]		= new FZ80::RRC_L;
	CBInstructions [0xCB0E]		= new FZ80::RRC_IndirectHL;
	DDCBInstructions [0xDDCB0E] = new FZ80::RRC_IndirectIndexIX;
	DDCBInstructions [0xDDCB0F] = new FZ80::RRC_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB08] = new FZ80::RRC_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB09] = new FZ80::RRC_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB0A] = new FZ80::RRC_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB0B] = new FZ80::RRC_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB0C] = new FZ80::RRC_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB0D] = new FZ80::RRC_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB0E] = new FZ80::RRC_IndirectIndexIY;
	FDCBInstructions [0xFDCB0F] = new FZ80::RRC_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB08] = new FZ80::RRC_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB09] = new FZ80::RRC_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB0A] = new FZ80::RRC_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB0B] = new FZ80::RRC_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB0C] = new FZ80::RRC_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB0D] = new FZ80::RRC_IndirectIndexIYCopyL;		// Undocumented

	// RR
	result [0x1F]				= new FZ80::RR_A;
	CBInstructions [0xCB1F]		= new FZ80::RR_ASlower;
	CBInstructions [0xCB18]		= new FZ80::RR_B;
	CBInstructions [0xCB19]		= new FZ80::RR_C;
	CBInstructions [0xCB1A]		= new FZ80::RR_D;
	CBInstructions [0xCB1B]		= new FZ80::RR_E;
	CBInstructions [0xCB1C]		= new FZ80::RR_F;
	CBInstructions [0xCB1D]		= new FZ80::RR_L;
	CBInstructions [0xCB1E]		= new FZ80::RR_IndirectHL;
	DDCBInstructions [0xDDCB1E] = new FZ80::RR_IndirectIndexIX;
	DDCBInstructions [0xDDCB1F] = new FZ80::RR_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB18] = new FZ80::RR_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB19] = new FZ80::RR_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB1A] = new FZ80::RR_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB1B] = new FZ80::RR_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB1C] = new FZ80::RR_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB1D] = new FZ80::RR_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB1E] = new FZ80::RR_IndirectIndexIY;
	FDCBInstructions [0xFDCB1F] = new FZ80::RR_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB18] = new FZ80::RR_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB19] = new FZ80::RR_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB1A] = new FZ80::RR_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB1B] = new FZ80::RR_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB1C] = new FZ80::RR_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB1D] = new FZ80::RR_IndirectIndexIYCopyL;		// Undocumented

	// RRD
	EDInstructions [0xED67]		= new FZ80::RRD;

	// SRA
	CBInstructions [0xCB2F]		= new FZ80::SRA_A;
	CBInstructions [0xCB28]		= new FZ80::SRA_B;
	CBInstructions [0xCB29]		= new FZ80::SRA_C;
	CBInstructions [0xCB2A]		= new FZ80::SRA_D;
	CBInstructions [0xCB2B]		= new FZ80::SRA_E;
	CBInstructions [0xCB2C]		= new FZ80::SRA_H;
	CBInstructions [0xCB2D]		= new FZ80::SRA_L;
	CBInstructions [0xCB2E]		= new FZ80::SRA_IndirectHL;
	DDCBInstructions [0xDDCB2E] = new FZ80::SRA_IndirectIndexIX;
	DDCBInstructions [0xDDCB2F] = new FZ80::SRA_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB28] = new FZ80::SRA_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB29] = new FZ80::SRA_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB2A] = new FZ80::SRA_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB2B] = new FZ80::SRA_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB2C] = new FZ80::SRA_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB2D] = new FZ80::SRA_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB2E] = new FZ80::SRA_IndirectIndexIY;
	FDCBInstructions [0xFDCB2F] = new FZ80::SRA_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB28] = new FZ80::SRA_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB29] = new FZ80::SRA_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB2A] = new FZ80::SRA_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB2B] = new FZ80::SRA_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB2C] = new FZ80::SRA_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB2D] = new FZ80::SRA_IndirectIndexIYCopyL;		// Undocumented

	// SRL
	CBInstructions [0xCB3F]		= new FZ80::SRL_A;
	CBInstructions [0xCB38]		= new FZ80::SRL_B;
	CBInstructions [0xCB39]		= new FZ80::SRL_C;
	CBInstructions [0xCB3A]		= new FZ80::SRL_D;
	CBInstructions [0xCB3B]		= new FZ80::SRL_E;
	CBInstructions [0xCB3C]		= new FZ80::SRL_H;
	CBInstructions [0xCB3D]		= new FZ80::SRL_L;
	CBInstructions [0xCB3E]		= new FZ80::SRL_IndirectHL;
	DDCBInstructions [0xDDCB3E] = new FZ80::SRL_IndirectIndexIX;
	DDCBInstructions [0xDDCB3F] = new FZ80::SRL_IndirectIndexIXCopyA;		// Undocumented
	DDCBInstructions [0xDDCB38] = new FZ80::SRL_IndirectIndexIXCopyB;		// Undocumented
	DDCBInstructions [0xDDCB39] = new FZ80::SRL_IndirectIndexIXCopyC;		// Undocumented
	DDCBInstructions [0xDDCB3A] = new FZ80::SRL_IndirectIndexIXCopyD;		// Undocumented
	DDCBInstructions [0xDDCB3B] = new FZ80::SRL_IndirectIndexIXCopyE;		// Undocumented
	DDCBInstructions [0xDDCB3C] = new FZ80::SRL_IndirectIndexIXCopyH;		// Undocumented
	DDCBInstructions [0xDDCB3D] = new FZ80::SRL_IndirectIndexIXCopyL;		// Undocumented
	FDCBInstructions [0xFDCB3E] = new FZ80::SRL_IndirectIndexIY;
	FDCBInstructions [0xFDCB3F] = new FZ80::SRL_IndirectIndexIYCopyA;		// Undocumented
	FDCBInstructions [0xFDCB38] = new FZ80::SRL_IndirectIndexIYCopyB;		// Undocumented
	FDCBInstructions [0xFDCB39] = new FZ80::SRL_IndirectIndexIYCopyC;		// Undocumented
	FDCBInstructions [0xFDCB3A] = new FZ80::SRL_IndirectIndexIYCopyD;		// Undocumented
	FDCBInstructions [0xFDCB3A] = new FZ80::SRL_IndirectIndexIYCopyE;		// Undocumented
	FDCBInstructions [0xFDCB3B] = new FZ80::SRL_IndirectIndexIYCopyH;		// Undocumented
	FDCBInstructions [0xFDCB3C] = new FZ80::SRL_IndirectIndexIYCopyL;		// Undocumented

	// BIT
	CBInstructions [0xCB47]		= new FZ80::BIT_0WithA;
	CBInstructions [0xCB40]		= new FZ80::BIT_0WithB;
	CBInstructions [0xCB41]		= new FZ80::BIT_0WithC;
	CBInstructions [0xCB42]		= new FZ80::BIT_0WithD;
	CBInstructions [0xCB43]		= new FZ80::BIT_0WithE;
	CBInstructions [0xCB44]		= new FZ80::BIT_0WithH;
	CBInstructions [0xCB45]		= new FZ80::BIT_0WithL;
	CBInstructions [0xCB46]		= new FZ80::BIT_0WithIndirectHL;
	DDCBInstructions [0xDDCB46] = new FZ80::BIT_0WithIndirectIndexIX; 
	DDCBInstructions [0xDDCB40] = new FZ80::U1BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB41] = new FZ80::U2BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB42] = new FZ80::U3BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB43] = new FZ80::U4BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB44] = new FZ80::U5BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB45] = new FZ80::U6BIT_0WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB47] = new FZ80::U7BIT_0WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB46] = new FZ80::BIT_0WithIndirectIndexIY; 
	FDCBInstructions [0xFDCB40] = new FZ80::U1BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB41] = new FZ80::U2BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB42] = new FZ80::U3BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB43] = new FZ80::U4BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB44] = new FZ80::U5BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB45] = new FZ80::U6BIT_0WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB47] = new FZ80::U7BIT_0WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB4F]		= new FZ80::BIT_1WithA;
	CBInstructions [0xCB48]		= new FZ80::BIT_1WithB;
	CBInstructions [0xCB49]		= new FZ80::BIT_1WithC;
	CBInstructions [0xCB4A]		= new FZ80::BIT_1WithD;
	CBInstructions [0xCB4B]		= new FZ80::BIT_1WithE;
	CBInstructions [0xCB4C]		= new FZ80::BIT_1WithH;
	CBInstructions [0xCB4D]		= new FZ80::BIT_1WithL;
	CBInstructions [0xCB4E]		= new FZ80::BIT_1WithIndirectHL;
	DDCBInstructions [0xDDCB4E] = new FZ80::BIT_1WithIndirectIndexIX;
	DDCBInstructions [0xDDCB48] = new FZ80::U1BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB49] = new FZ80::U2BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB4A] = new FZ80::U3BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB4B] = new FZ80::U4BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB4C] = new FZ80::U5BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB4D] = new FZ80::U6BIT_1WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB4F] = new FZ80::U7BIT_1WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB4E] = new FZ80::BIT_1WithIndirectIndexIY;
	FDCBInstructions [0xFDCB48] = new FZ80::U1BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB49] = new FZ80::U2BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB4A] = new FZ80::U3BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB4B] = new FZ80::U4BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB4C] = new FZ80::U5BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB4D] = new FZ80::U6BIT_1WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB4F] = new FZ80::U7BIT_1WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB57]		= new FZ80::BIT_2WithA;
	CBInstructions [0xCB50]		= new FZ80::BIT_2WithB;
	CBInstructions [0xCB51]		= new FZ80::BIT_2WithC;
	CBInstructions [0xCB52]		= new FZ80::BIT_2WithD;
	CBInstructions [0xCB53]		= new FZ80::BIT_2WithE;
	CBInstructions [0xCB54]		= new FZ80::BIT_2WithH;
	CBInstructions [0xCB55]		= new FZ80::BIT_2WithL;
	CBInstructions [0xCB56]		= new FZ80::BIT_2WithIndirectHL;
	DDCBInstructions [0xDDCB56] = new FZ80::BIT_2WithIndirectIndexIX;
	DDCBInstructions [0xDDCB50] = new FZ80::U1BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB51] = new FZ80::U2BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB52] = new FZ80::U3BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB53] = new FZ80::U4BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB54] = new FZ80::U5BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB55] = new FZ80::U6BIT_2WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB57] = new FZ80::U7BIT_2WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB56] = new FZ80::BIT_2WithIndirectIndexIY;
	FDCBInstructions [0xFDCB50] = new FZ80::U1BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB51] = new FZ80::U2BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB52] = new FZ80::U3BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB53] = new FZ80::U4BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB54] = new FZ80::U5BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB55] = new FZ80::U6BIT_2WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB57] = new FZ80::U7BIT_2WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB5F]		= new FZ80::BIT_3WithA;
	CBInstructions [0xCB58]		= new FZ80::BIT_3WithB;
	CBInstructions [0xCB59]		= new FZ80::BIT_3WithC;
	CBInstructions [0xCB5A]		= new FZ80::BIT_3WithD;
	CBInstructions [0xCB5B]		= new FZ80::BIT_3WithE;
	CBInstructions [0xCB5C]		= new FZ80::BIT_3WithH;
	CBInstructions [0xCB5D]		= new FZ80::BIT_3WithL;
	CBInstructions [0xCB5E]		= new FZ80::BIT_3WithIndirectHL;
	DDCBInstructions [0xDDCB5E] = new FZ80::BIT_3WithIndirectIndexIX;
	DDCBInstructions [0xDDCB58] = new FZ80::U1BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB59] = new FZ80::U2BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB5A] = new FZ80::U3BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB5B] = new FZ80::U4BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB5C] = new FZ80::U5BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB5D] = new FZ80::U6BIT_3WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB5F] = new FZ80::U7BIT_3WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB5E] = new FZ80::BIT_3WithIndirectIndexIY;
	FDCBInstructions [0xFDCB58] = new FZ80::U1BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB59] = new FZ80::U2BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB5A] = new FZ80::U3BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB5B] = new FZ80::U4BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB5C] = new FZ80::U5BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB5D] = new FZ80::U6BIT_3WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB5F] = new FZ80::U7BIT_3WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB67]		= new FZ80::BIT_4WithA;
	CBInstructions [0xCB60]		= new FZ80::BIT_4WithB;
	CBInstructions [0xCB61]		= new FZ80::BIT_4WithC;
	CBInstructions [0xCB62]		= new FZ80::BIT_4WithD;
	CBInstructions [0xCB63]		= new FZ80::BIT_4WithE;
	CBInstructions [0xCB64]		= new FZ80::BIT_4WithH;
	CBInstructions [0xCB65]		= new FZ80::BIT_4WithL;
	CBInstructions [0xCB66]		= new FZ80::BIT_4WithIndirectHL;
	DDCBInstructions [0xDDCB66] = new FZ80::BIT_4WithIndirectIndexIX;
	DDCBInstructions [0xDDCB60] = new FZ80::U1BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB61] = new FZ80::U2BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB62] = new FZ80::U3BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB63] = new FZ80::U4BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB64] = new FZ80::U5BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB65] = new FZ80::U6BIT_4WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB67] = new FZ80::U7BIT_4WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB66] = new FZ80::BIT_4WithIndirectIndexIY;
	FDCBInstructions [0xFDCB60] = new FZ80::U1BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB61] = new FZ80::U2BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB62] = new FZ80::U3BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB63] = new FZ80::U4BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB64] = new FZ80::U5BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB65] = new FZ80::U6BIT_4WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB67] = new FZ80::U7BIT_4WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB6F]		= new FZ80::BIT_5WithA;
	CBInstructions [0xCB68]		= new FZ80::BIT_5WithB;
	CBInstructions [0xCB69]		= new FZ80::BIT_5WithC;
	CBInstructions [0xCB6A]		= new FZ80::BIT_5WithD;
	CBInstructions [0xCB6B]		= new FZ80::BIT_5WithE;
	CBInstructions [0xCB6C]		= new FZ80::BIT_5WithH;
	CBInstructions [0xCB6D]		= new FZ80::BIT_5WithL;
	CBInstructions [0xCB6E]		= new FZ80::BIT_5WithIndirectHL;
	DDCBInstructions [0xDDCB6E] = new FZ80::BIT_5WithIndirectIndexIX;
	DDCBInstructions [0xDDCB68] = new FZ80::U1BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB69] = new FZ80::U2BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB6A] = new FZ80::U3BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB6B] = new FZ80::U4BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB6C] = new FZ80::U5BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB6D] = new FZ80::U6BIT_5WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB6F] = new FZ80::U7BIT_5WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB6E] = new FZ80::BIT_5WithIndirectIndexIY;
	FDCBInstructions [0xFDCB68] = new FZ80::U1BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB69] = new FZ80::U2BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB6A] = new FZ80::U3BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB6B] = new FZ80::U4BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB6C] = new FZ80::U5BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB6D] = new FZ80::U6BIT_5WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB6F] = new FZ80::U7BIT_5WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB77]		= new FZ80::BIT_6WithA;
	CBInstructions [0xCB70]		= new FZ80::BIT_6WithB;
	CBInstructions [0xCB71]		= new FZ80::BIT_6WithC;
	CBInstructions [0xCB72]		= new FZ80::BIT_6WithD;
	CBInstructions [0xCB73]		= new FZ80::BIT_6WithE;
	CBInstructions [0xCB74]		= new FZ80::BIT_6WithH;
	CBInstructions [0xCB75]		= new FZ80::BIT_6WithL;
	CBInstructions [0xCB76]		= new FZ80::BIT_6WithIndirectHL;
	DDCBInstructions [0xDDCB76] = new FZ80::BIT_6WithIndirectIndexIX;
	DDCBInstructions [0xDDCB70] = new FZ80::U1BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB71] = new FZ80::U2BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB72] = new FZ80::U3BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB73] = new FZ80::U4BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB74] = new FZ80::U5BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB75] = new FZ80::U6BIT_6WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB77] = new FZ80::U7BIT_6WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB76] = new FZ80::BIT_6WithIndirectIndexIY;
	FDCBInstructions [0xFDCB70] = new FZ80::U1BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB71] = new FZ80::U2BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB72] = new FZ80::U3BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB73] = new FZ80::U4BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB74] = new FZ80::U5BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB75] = new FZ80::U6BIT_6WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB77] = new FZ80::U7BIT_6WithIndirectIndexIY;		// Undocumented
	CBInstructions [0xCB7F]		= new FZ80::BIT_7WithA;
	CBInstructions [0xCB78]		= new FZ80::BIT_7WithB;
	CBInstructions [0xCB79]		= new FZ80::BIT_7WithC;
	CBInstructions [0xCB7A]		= new FZ80::BIT_7WithD;
	CBInstructions [0xCB7B]		= new FZ80::BIT_7WithE;
	CBInstructions [0xCB7C]		= new FZ80::BIT_7WithH;
	CBInstructions [0xCB7D]		= new FZ80::BIT_7WithL;
	CBInstructions [0xCB7E]		= new FZ80::BIT_7WithIndirectHL;
	DDCBInstructions [0xDDCB7E] = new FZ80::BIT_7WithIndirectIndexIX;
	DDCBInstructions [0xDDCB78] = new FZ80::U1BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB79] = new FZ80::U2BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB7A] = new FZ80::U3BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB7B] = new FZ80::U4BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB7C] = new FZ80::U5BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB7D] = new FZ80::U6BIT_7WithIndirectIndexIX;		// Undocumented
	DDCBInstructions [0xDDCB7F] = new FZ80::U7BIT_7WithIndirectIndexIX;		// Undocumented
	FDCBInstructions [0xFDCB7E] = new FZ80::BIT_7WithIndirectIndexIY;
	FDCBInstructions [0xFDCB78] = new FZ80::U1BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB79] = new FZ80::U2BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB7A] = new FZ80::U3BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB7B] = new FZ80::U4BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB7C] = new FZ80::U5BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB7D] = new FZ80::U6BIT_7WithIndirectIndexIY;		// Undocumented
	FDCBInstructions [0xFDCB7F] = new FZ80::U7BIT_7WithIndirectIndexIY;		// Undocumented

	// SET
	CBInstructions [0xCBC7]		= new FZ80::SET_0WithA;
	CBInstructions [0xCBC0]		= new FZ80::SET_0WithB;
	CBInstructions [0xCBC1]		= new FZ80::SET_0WithC;
	CBInstructions [0xCBC2]		= new FZ80::SET_0WithD;
	CBInstructions [0xCBC3]		= new FZ80::SET_0WithE;
	CBInstructions [0xCBC4]		= new FZ80::SET_0WithH;
	CBInstructions [0xCBC5]		= new FZ80::SET_0WithL;
	CBInstructions [0xCBC6]		= new FZ80::SET_0WithIndirectHL;
	DDCBInstructions [0xDDCBC6] = new FZ80::SET_0WithIndirectIndexIX; 
	DDCBInstructions [0xDDCBC7] = new FZ80::SET_0WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBC0] = new FZ80::SET_0WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBC1] = new FZ80::SET_0WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBC2] = new FZ80::SET_0WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBC3] = new FZ80::SET_0WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBC4] = new FZ80::SET_0WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBC5] = new FZ80::SET_0WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBC6] = new FZ80::SET_0WithIndirectIndexIY; 
	FDCBInstructions [0xFDCBC7] = new FZ80::SET_0WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBC0] = new FZ80::SET_0WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBC1] = new FZ80::SET_0WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBC2] = new FZ80::SET_0WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBC3] = new FZ80::SET_0WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBC4] = new FZ80::SET_0WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBC5] = new FZ80::SET_0WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBCF]		= new FZ80::SET_1WithA;
	CBInstructions [0xCBC8]		= new FZ80::SET_1WithB;
	CBInstructions [0xCBC9]		= new FZ80::SET_1WithC;
	CBInstructions [0xCBCA]		= new FZ80::SET_1WithD;
	CBInstructions [0xCBCB]		= new FZ80::SET_1WithE;
	CBInstructions [0xCBCC]		= new FZ80::SET_1WithH;
	CBInstructions [0xCBCD]		= new FZ80::SET_1WithL;
	CBInstructions [0xCBCE]		= new FZ80::SET_1WithIndirectHL;
	DDCBInstructions [0xDDCBCE] = new FZ80::SET_1WithIndirectIndexIX;
	DDCBInstructions [0xDDCBCF] = new FZ80::SET_1WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBC8] = new FZ80::SET_1WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBC9] = new FZ80::SET_1WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBCA] = new FZ80::SET_1WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBCB] = new FZ80::SET_1WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBCC] = new FZ80::SET_1WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBCD] = new FZ80::SET_1WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBCE] = new FZ80::SET_1WithIndirectIndexIY;
	FDCBInstructions [0xFDCBCF] = new FZ80::SET_1WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBC8] = new FZ80::SET_1WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBC9] = new FZ80::SET_1WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBCA] = new FZ80::SET_1WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBCB] = new FZ80::SET_1WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBCC] = new FZ80::SET_1WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBCD] = new FZ80::SET_1WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBD7]		= new FZ80::SET_2WithA;
	CBInstructions [0xCBD0]		= new FZ80::SET_2WithB;
	CBInstructions [0xCBD1]		= new FZ80::SET_2WithC;
	CBInstructions [0xCBD2]		= new FZ80::SET_2WithD;
	CBInstructions [0xCBD3]		= new FZ80::SET_2WithE;
	CBInstructions [0xCBD4]		= new FZ80::SET_2WithH;
	CBInstructions [0xCBD5]		= new FZ80::SET_2WithL;
	CBInstructions [0xCBD6]		= new FZ80::SET_2WithIndirectHL;
	DDCBInstructions [0xDDCBD6] = new FZ80::SET_2WithIndirectIndexIX;
	DDCBInstructions [0xDDCBD7] = new FZ80::SET_2WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBD0] = new FZ80::SET_2WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBD1] = new FZ80::SET_2WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBD2] = new FZ80::SET_2WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBD3] = new FZ80::SET_2WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBD4] = new FZ80::SET_2WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBD5] = new FZ80::SET_2WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBD6] = new FZ80::SET_2WithIndirectIndexIY;
	FDCBInstructions [0xFDCBD7] = new FZ80::SET_2WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBD0] = new FZ80::SET_2WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBD1] = new FZ80::SET_2WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBD2] = new FZ80::SET_2WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBD3] = new FZ80::SET_2WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBD4] = new FZ80::SET_2WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBD5] = new FZ80::SET_2WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBDF]		= new FZ80::SET_3WithA;
	CBInstructions [0xCBD8]		= new FZ80::SET_3WithB;
	CBInstructions [0xCBD9]		= new FZ80::SET_3WithC;
	CBInstructions [0xCBDA]		= new FZ80::SET_3WithD;
	CBInstructions [0xCBDB]		= new FZ80::SET_3WithE;
	CBInstructions [0xCBDC]		= new FZ80::SET_3WithH;
	CBInstructions [0xCBDD]		= new FZ80::SET_3WithL;
	CBInstructions [0xCBDE]		= new FZ80::SET_3WithIndirectHL;
	DDCBInstructions [0xDDCBDE] = new FZ80::SET_3WithIndirectIndexIX;
	DDCBInstructions [0xDDCBDF] = new FZ80::SET_3WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBD8] = new FZ80::SET_3WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBD9] = new FZ80::SET_3WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBDA] = new FZ80::SET_3WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBDB] = new FZ80::SET_3WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBDC] = new FZ80::SET_3WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBDD] = new FZ80::SET_3WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBDE] = new FZ80::SET_3WithIndirectIndexIY;
	FDCBInstructions [0xFDCBDF] = new FZ80::SET_3WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBD8] = new FZ80::SET_3WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBD9] = new FZ80::SET_3WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBDA] = new FZ80::SET_3WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBDB] = new FZ80::SET_3WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBDC] = new FZ80::SET_3WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBDD] = new FZ80::SET_3WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBE7]		= new FZ80::SET_4WithA;
	CBInstructions [0xCBE0]		= new FZ80::SET_4WithB;
	CBInstructions [0xCBE1]		= new FZ80::SET_4WithC;
	CBInstructions [0xCBE2]		= new FZ80::SET_4WithD;
	CBInstructions [0xCBE3]		= new FZ80::SET_4WithE;
	CBInstructions [0xCBE4]		= new FZ80::SET_4WithH;
	CBInstructions [0xCBE5]		= new FZ80::SET_4WithL;
	CBInstructions [0xCBE6]		= new FZ80::SET_4WithIndirectHL;
	DDCBInstructions [0xDDCBE6] = new FZ80::SET_4WithIndirectIndexIX;
	DDCBInstructions [0xDDCBE7] = new FZ80::SET_4WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBE0] = new FZ80::SET_4WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBE1] = new FZ80::SET_4WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBE2] = new FZ80::SET_4WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBE3] = new FZ80::SET_4WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBE4] = new FZ80::SET_4WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBE5] = new FZ80::SET_4WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBE6] = new FZ80::SET_4WithIndirectIndexIY;
	FDCBInstructions [0xFDCBE7] = new FZ80::SET_4WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBE0] = new FZ80::SET_4WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBE1] = new FZ80::SET_4WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBE2] = new FZ80::SET_4WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBE3] = new FZ80::SET_4WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBE4] = new FZ80::SET_4WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBE5] = new FZ80::SET_4WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBEF]		= new FZ80::SET_5WithA;
	CBInstructions [0xCBE8]		= new FZ80::SET_5WithB;
	CBInstructions [0xCBE9]		= new FZ80::SET_5WithC;
	CBInstructions [0xCBEA]		= new FZ80::SET_5WithD;
	CBInstructions [0xCBEB]		= new FZ80::SET_5WithE;
	CBInstructions [0xCBEC]		= new FZ80::SET_5WithH;
	CBInstructions [0xCBED]		= new FZ80::SET_5WithL;
	CBInstructions [0xCBEE]		= new FZ80::SET_5WithIndirectHL;
	DDCBInstructions [0xDDCBEE] = new FZ80::SET_5WithIndirectIndexIX;
	DDCBInstructions [0xDDCBEF] = new FZ80::SET_5WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBE8] = new FZ80::SET_5WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBE9] = new FZ80::SET_5WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBEA] = new FZ80::SET_5WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBEB] = new FZ80::SET_5WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBEC] = new FZ80::SET_5WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBED] = new FZ80::SET_5WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBEE] = new FZ80::SET_5WithIndirectIndexIY;
	FDCBInstructions [0xFDCBEF] = new FZ80::SET_5WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBE8] = new FZ80::SET_5WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBE9] = new FZ80::SET_5WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBEA] = new FZ80::SET_5WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBEB] = new FZ80::SET_5WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBEC] = new FZ80::SET_5WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBED] = new FZ80::SET_5WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBF7]		= new FZ80::SET_6WithA;
	CBInstructions [0xCBF0]		= new FZ80::SET_6WithB;
	CBInstructions [0xCBF1]		= new FZ80::SET_6WithC;
	CBInstructions [0xCBF2]		= new FZ80::SET_6WithD;
	CBInstructions [0xCBF3]		= new FZ80::SET_6WithE;
	CBInstructions [0xCBF4]		= new FZ80::SET_6WithH;
	CBInstructions [0xCBF5]		= new FZ80::SET_6WithL;
	CBInstructions [0xCBF6]		= new FZ80::SET_6WithIndirectHL;
	DDCBInstructions [0xDDCBF6] = new FZ80::SET_6WithIndirectIndexIX;
	DDCBInstructions [0xDDCBF7] = new FZ80::SET_6WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBF0] = new FZ80::SET_6WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBF1] = new FZ80::SET_6WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBF2] = new FZ80::SET_6WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBF3] = new FZ80::SET_6WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBF4] = new FZ80::SET_6WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBF5] = new FZ80::SET_6WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBF6] = new FZ80::SET_6WithIndirectIndexIY;
	FDCBInstructions [0xFDCBF7] = new FZ80::SET_6WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBF0] = new FZ80::SET_6WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBF1] = new FZ80::SET_6WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBF2] = new FZ80::SET_6WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBF3] = new FZ80::SET_6WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBF4] = new FZ80::SET_6WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBF5] = new FZ80::SET_6WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBFF]		= new FZ80::SET_7WithA;
	CBInstructions [0xCBF8]		= new FZ80::SET_7WithB;
	CBInstructions [0xCBF9]		= new FZ80::SET_7WithC;
	CBInstructions [0xCBFA]		= new FZ80::SET_7WithD;
	CBInstructions [0xCBFB]		= new FZ80::SET_7WithE;
	CBInstructions [0xCBFC]		= new FZ80::SET_7WithH;
	CBInstructions [0xCBFD]		= new FZ80::SET_7WithL;
	CBInstructions [0xCBFE]		= new FZ80::SET_7WithIndirectHL;
	DDCBInstructions [0xDDCBFE] = new FZ80::SET_7WithIndirectIndexIX;
	DDCBInstructions [0xDDCBFF] = new FZ80::SET_7WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBF8] = new FZ80::SET_7WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBF9] = new FZ80::SET_7WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBFA] = new FZ80::SET_7WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBFB] = new FZ80::SET_7WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBFC] = new FZ80::SET_7WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBFD] = new FZ80::SET_7WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBFE] = new FZ80::SET_7WithIndirectIndexIY;
	FDCBInstructions [0xFDCBFF] = new FZ80::SET_7WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBF8] = new FZ80::SET_7WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBF9] = new FZ80::SET_7WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBFA] = new FZ80::SET_7WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBFB] = new FZ80::SET_7WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBFC] = new FZ80::SET_7WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBFD] = new FZ80::SET_7WithIndirectIndexIYCopyL;	// Undocumented

	// RES
	CBInstructions [0xCB87]		= new FZ80::RES_0WithA;
	CBInstructions [0xCB80]		= new FZ80::RES_0WithB;
	CBInstructions [0xCB81]		= new FZ80::RES_0WithC;
	CBInstructions [0xCB82]		= new FZ80::RES_0WithD;
	CBInstructions [0xCB83]		= new FZ80::RES_0WithE;
	CBInstructions [0xCB84]		= new FZ80::RES_0WithH;
	CBInstructions [0xCB85]		= new FZ80::RES_0WithL;
	CBInstructions [0xCB86]		= new FZ80::RES_0WithIndirectHL;
	DDCBInstructions [0xDDCB86] = new FZ80::RES_0WithIndirectIndexIX; 
	DDCBInstructions [0xDDCB87] = new FZ80::RES_0WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB80] = new FZ80::RES_0WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB81] = new FZ80::RES_0WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB82] = new FZ80::RES_0WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB83] = new FZ80::RES_0WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB84] = new FZ80::RES_0WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB85] = new FZ80::RES_0WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB86] = new FZ80::RES_0WithIndirectIndexIY; 
	FDCBInstructions [0xFDCB87] = new FZ80::RES_0WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB80] = new FZ80::RES_0WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB81] = new FZ80::RES_0WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB82] = new FZ80::RES_0WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB83] = new FZ80::RES_0WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB84] = new FZ80::RES_0WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB85] = new FZ80::RES_0WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCB8F]		= new FZ80::RES_1WithA;
	CBInstructions [0xCB88]		= new FZ80::RES_1WithB;
	CBInstructions [0xCB89]		= new FZ80::RES_1WithC;
	CBInstructions [0xCB8A]		= new FZ80::RES_1WithD;
	CBInstructions [0xCB8B]		= new FZ80::RES_1WithE;
	CBInstructions [0xCB8C]		= new FZ80::RES_1WithH;
	CBInstructions [0xCB8D]		= new FZ80::RES_1WithL;
	CBInstructions [0xCB8E]		= new FZ80::RES_1WithIndirectHL;
	DDCBInstructions [0xDDCB8E] = new FZ80::RES_1WithIndirectIndexIX;
	DDCBInstructions [0xDDCB8F] = new FZ80::RES_1WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB88] = new FZ80::RES_1WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB89] = new FZ80::RES_1WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB8A] = new FZ80::RES_1WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB8B] = new FZ80::RES_1WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB8C] = new FZ80::RES_1WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB8D] = new FZ80::RES_1WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB8E] = new FZ80::RES_1WithIndirectIndexIY;
	FDCBInstructions [0xFDCB8F] = new FZ80::RES_1WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB88] = new FZ80::RES_1WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB89] = new FZ80::RES_1WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB8A] = new FZ80::RES_1WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB8B] = new FZ80::RES_1WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB8C] = new FZ80::RES_1WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB8D] = new FZ80::RES_1WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCB97]		= new FZ80::RES_2WithA;
	CBInstructions [0xCB90]		= new FZ80::RES_2WithB;
	CBInstructions [0xCB91]		= new FZ80::RES_2WithC;
	CBInstructions [0xCB92]		= new FZ80::RES_2WithD;
	CBInstructions [0xCB93]		= new FZ80::RES_2WithE;
	CBInstructions [0xCB94]		= new FZ80::RES_2WithH;
	CBInstructions [0xCB95]		= new FZ80::RES_2WithL;
	CBInstructions [0xCB96]		= new FZ80::RES_2WithIndirectHL;
	DDCBInstructions [0xDDCB96] = new FZ80::RES_2WithIndirectIndexIX;
	DDCBInstructions [0xDDCB97] = new FZ80::RES_2WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB90] = new FZ80::RES_2WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB91] = new FZ80::RES_2WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB92] = new FZ80::RES_2WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB93] = new FZ80::RES_2WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB94] = new FZ80::RES_2WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB95] = new FZ80::RES_2WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB96] = new FZ80::RES_2WithIndirectIndexIY;
	FDCBInstructions [0xFDCB97] = new FZ80::RES_2WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB90] = new FZ80::RES_2WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB91] = new FZ80::RES_2WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB92] = new FZ80::RES_2WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB93] = new FZ80::RES_2WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB94] = new FZ80::RES_2WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB95] = new FZ80::RES_2WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCB9F]		= new FZ80::RES_3WithA;
	CBInstructions [0xCB98]		= new FZ80::RES_3WithB;
	CBInstructions [0xCB99]		= new FZ80::RES_3WithC;
	CBInstructions [0xCB9A]		= new FZ80::RES_3WithD;
	CBInstructions [0xCB9B]		= new FZ80::RES_3WithE;
	CBInstructions [0xCB9C]		= new FZ80::RES_3WithH;
	CBInstructions [0xCB9D]		= new FZ80::RES_3WithL;
	CBInstructions [0xCB9E]		= new FZ80::RES_3WithIndirectHL;
	DDCBInstructions [0xDDCB9E] = new FZ80::RES_3WithIndirectIndexIX;
	DDCBInstructions [0xDDCB9F] = new FZ80::RES_3WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCB98] = new FZ80::RES_3WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCB99] = new FZ80::RES_3WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCB9A] = new FZ80::RES_3WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCB9B] = new FZ80::RES_3WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCB9C] = new FZ80::RES_3WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCB9D] = new FZ80::RES_3WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCB9E] = new FZ80::RES_3WithIndirectIndexIY;
	FDCBInstructions [0xFDCB9F] = new FZ80::RES_3WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCB98] = new FZ80::RES_3WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCB99] = new FZ80::RES_3WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCB9A] = new FZ80::RES_3WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCB9B] = new FZ80::RES_3WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCB9C] = new FZ80::RES_3WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCB9D] = new FZ80::RES_3WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBA7]		= new FZ80::RES_4WithA;
	CBInstructions [0xCBA0]		= new FZ80::RES_4WithB;
	CBInstructions [0xCBA1]		= new FZ80::RES_4WithC;
	CBInstructions [0xCBA2]		= new FZ80::RES_4WithD;
	CBInstructions [0xCBA3]		= new FZ80::RES_4WithE;
	CBInstructions [0xCBA4]		= new FZ80::RES_4WithH;
	CBInstructions [0xCBA5]		= new FZ80::RES_4WithL;
	CBInstructions [0xCBA6]		= new FZ80::RES_4WithIndirectHL;
	DDCBInstructions [0xDDCBA6] = new FZ80::RES_4WithIndirectIndexIX;
	DDCBInstructions [0xDDCBA7] = new FZ80::RES_4WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBA0] = new FZ80::RES_4WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBA1] = new FZ80::RES_4WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBA2] = new FZ80::RES_4WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBA3] = new FZ80::RES_4WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBA4] = new FZ80::RES_4WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBA5] = new FZ80::RES_4WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBA6] = new FZ80::RES_4WithIndirectIndexIY;
	FDCBInstructions [0xFDCBA7] = new FZ80::RES_4WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBA0] = new FZ80::RES_4WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBA1] = new FZ80::RES_4WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBA2] = new FZ80::RES_4WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBA3] = new FZ80::RES_4WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBA4] = new FZ80::RES_4WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBA5] = new FZ80::RES_4WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBAF]		= new FZ80::RES_5WithA;
	CBInstructions [0xCBA8]		= new FZ80::RES_5WithB;
	CBInstructions [0xCBA9]		= new FZ80::RES_5WithC;
	CBInstructions [0xCBAA]		= new FZ80::RES_5WithD;
	CBInstructions [0xCBAB]		= new FZ80::RES_5WithE;
	CBInstructions [0xCBAC]		= new FZ80::RES_5WithH;
	CBInstructions [0xCBAD]		= new FZ80::RES_5WithL;
	CBInstructions [0xCBAE]		= new FZ80::RES_5WithIndirectHL;
	DDCBInstructions [0xDDCBAE] = new FZ80::RES_5WithIndirectIndexIX;
	DDCBInstructions [0xDDCBAF] = new FZ80::RES_5WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBA8] = new FZ80::RES_5WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBA9] = new FZ80::RES_5WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBAA] = new FZ80::RES_5WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBAB] = new FZ80::RES_5WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBAC] = new FZ80::RES_5WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBAD] = new FZ80::RES_5WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBAE] = new FZ80::RES_5WithIndirectIndexIY;
	FDCBInstructions [0xFDCBAF] = new FZ80::RES_5WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBA8] = new FZ80::RES_5WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBA9] = new FZ80::RES_5WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBAA] = new FZ80::RES_5WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBAB] = new FZ80::RES_5WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBAC] = new FZ80::RES_5WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBAD] = new FZ80::RES_5WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBB7]		= new FZ80::RES_6WithA;
	CBInstructions [0xCBB0]		= new FZ80::RES_6WithB;
	CBInstructions [0xCBB1]		= new FZ80::RES_6WithC;
	CBInstructions [0xCBB2]		= new FZ80::RES_6WithD;
	CBInstructions [0xCBB3]		= new FZ80::RES_6WithE;
	CBInstructions [0xCBB4]		= new FZ80::RES_6WithH;
	CBInstructions [0xCBB5]		= new FZ80::RES_6WithL;
	CBInstructions [0xCBB6]		= new FZ80::RES_6WithIndirectHL;
	DDCBInstructions [0xDDCBB6] = new FZ80::RES_6WithIndirectIndexIX;
	DDCBInstructions [0xDDCBB7] = new FZ80::RES_6WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBB0] = new FZ80::RES_6WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBB1] = new FZ80::RES_6WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBB2] = new FZ80::RES_6WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBB3] = new FZ80::RES_6WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBB4] = new FZ80::RES_6WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBB5] = new FZ80::RES_6WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBB6] = new FZ80::RES_6WithIndirectIndexIY;
	FDCBInstructions [0xFDCBB7] = new FZ80::RES_6WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBB0] = new FZ80::RES_6WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBB1] = new FZ80::RES_6WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBB2] = new FZ80::RES_6WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBB3] = new FZ80::RES_6WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBB4] = new FZ80::RES_6WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBB5] = new FZ80::RES_6WithIndirectIndexIYCopyL;	// Undocumented
	CBInstructions [0xCBBF]		= new FZ80::RES_7WithA;
	CBInstructions [0xCBB8]		= new FZ80::RES_7WithB;
	CBInstructions [0xCBB9]		= new FZ80::RES_7WithC;
	CBInstructions [0xCBBA]		= new FZ80::RES_7WithD;
	CBInstructions [0xCBBB]		= new FZ80::RES_7WithE;
	CBInstructions [0xCBBC]		= new FZ80::RES_7WithH;
	CBInstructions [0xCBBD]		= new FZ80::RES_7WithL;
	CBInstructions [0xCBBE]		= new FZ80::RES_7WithIndirectHL;
	DDCBInstructions [0xDDCBBE] = new FZ80::RES_7WithIndirectIndexIX;
	DDCBInstructions [0xDDCBBF] = new FZ80::RES_7WithIndirectIndexIXCopyA;	// Undocumented
	DDCBInstructions [0xDDCBB8] = new FZ80::RES_7WithIndirectIndexIXCopyB;	// Undocumented
	DDCBInstructions [0xDDCBB9] = new FZ80::RES_7WithIndirectIndexIXCopyC;	// Undocumented
	DDCBInstructions [0xDDCBBA] = new FZ80::RES_7WithIndirectIndexIXCopyD;	// Undocumented
	DDCBInstructions [0xDDCBBB] = new FZ80::RES_7WithIndirectIndexIXCopyE;	// Undocumented
	DDCBInstructions [0xDDCBBC] = new FZ80::RES_7WithIndirectIndexIXCopyH;	// Undocumented
	DDCBInstructions [0xDDCBBD] = new FZ80::RES_7WithIndirectIndexIXCopyL;	// Undocumented
	FDCBInstructions [0xFDCBBE] = new FZ80::RES_7WithIndirectIndexIY;
	FDCBInstructions [0xFDCBBF] = new FZ80::RES_7WithIndirectIndexIYCopyA;	// Undocumented
	FDCBInstructions [0xFDCBB8] = new FZ80::RES_7WithIndirectIndexIYCopyB;	// Undocumented
	FDCBInstructions [0xFDCBB9] = new FZ80::RES_7WithIndirectIndexIYCopyC;	// Undocumented
	FDCBInstructions [0xFDCBBA] = new FZ80::RES_7WithIndirectIndexIYCopyD;	// Undocumented
	FDCBInstructions [0xFDCBBB] = new FZ80::RES_7WithIndirectIndexIYCopyE;	// Undocumented
	FDCBInstructions [0xFDCBBC] = new FZ80::RES_7WithIndirectIndexIYCopyH;	// Undocumented
	FDCBInstructions [0xFDCBBD] = new FZ80::RES_7WithIndirectIndexIYCopyL;	// Undocumented

	// JP
	result [0xC3]				= new FZ80::JP;
	result [0xC2]				= new FZ80::JP_NZ;
	result [0xCA]				= new FZ80::JP_Z;
	result [0xD2]				= new FZ80::JP_NC;
	result [0xDA]				= new FZ80::JP_C;
	result [0xE2]				= new FZ80::JP_PO;
	result [0xEA]				= new FZ80::JP_PE;
	result [0xF2]				= new FZ80::JP_P;
	result [0xFA]				= new FZ80::JP_M;
	result [0xE9]				= new FZ80::JP_IndirectHL;
	DDInstructions [0xDDE9]		= new FZ80::JP_IndirectIX;
	FDInstructions [0xFDE9]		= new FZ80::JP_IndirectIY;

	// JR
	result [0x18]				= new FZ80::JR;
	result [0x20]				= new FZ80::JR_NZ;
	result [0x28]				= new FZ80::JR_Z;
	result [0x30]				= new FZ80::JR_NC;
	result [0x38]				= new FZ80::JR_C;

	// DJNZ
	result [0x10]				= new FZ80::DJNZ;

	// CALL
	result [0xCD]				= new FZ80::CALL;
	result [0xC4]				= new FZ80::CALL_NZ;
	result [0xCC]				= new FZ80::CALL_Z;
	result [0xD4]				= new FZ80::CALL_NC;
	result [0xDC]				= new FZ80::CALL_C;
	result [0xE4]				= new FZ80::CALL_PO;
	result [0xEC]				= new FZ80::CALL_PE;
	result [0xF4]				= new FZ80::CALL_P;
	result [0xFC]				= new FZ80::CALL_M;

	// RST
	result [0xC7]				= new FZ80::RST_00;
	result [0xCF]				= new FZ80::RST_08;
	result [0xD7]				= new FZ80::RST_10;
	result [0xDF]				= new FZ80::RST_18;
	result [0xE7]				= new FZ80::RST_20;
	result [0xEF]				= new FZ80::RST_28;
	result [0xF7]				= new FZ80::RST_30;
	result [0xFF]				= new FZ80::RST_38;

	// RET
	result [0xC9]				= new FZ80::RET;
	result [0xC0]				= new FZ80::RET_NZ;
	result [0xC8]				= new FZ80::RET_Z;
	result [0xD0]				= new FZ80::RET_NC;
	result [0xD8]				= new FZ80::RET_C;
	result [0xE0]				= new FZ80::RET_PO;
	result [0xE8]				= new FZ80::RET_PE;
	result [0xF0]				= new FZ80::RET_P;
	result [0xF8]				= new FZ80::RET_M;
	EDInstructions [0xED4D]		= new FZ80::RET_I;
	EDInstructions [0xED45]		= new FZ80::RET_N;

	// IN: Read the value from a port!
	result [0xDB]				= new FZ80::IN_A;
	EDInstructions [0xED78]		= new FZ80::IN_AFromPort;
	EDInstructions [0xED40]		= new FZ80::IN_BFromPort;
	EDInstructions [0xED48]		= new FZ80::IN_CFromPort;
	EDInstructions [0xED50]		= new FZ80::IN_DFromPort;
	EDInstructions [0xED58]		= new FZ80::IN_EFromPort;
	EDInstructions [0xED60]		= new FZ80::IN_HFromPort;
	EDInstructions [0xED68]		= new FZ80::IN_LFromPort;
	EDInstructions [0xED70]		= new FZ80::IN_FFromPort;					// Undocumented

	// IN Block
	EDInstructions [0xEDA2]		= new FZ80::INI;
	EDInstructions [0xEDB2]		= new FZ80::INIR;	
	EDInstructions [0xEDAA]		= new FZ80::IND;
	EDInstructions [0xEDBA]		= new FZ80::INDR;	

	// OUT: Move the value to a port!
	result [0xD3]				= new FZ80::OUT_A;
	EDInstructions [0xED79]		= new FZ80::OUT_AToPort;
	EDInstructions [0xED41]		= new FZ80::OUT_BToPort;
	EDInstructions [0xED49]		= new FZ80::OUT_CToPort;
	EDInstructions [0xED51]		= new FZ80::OUT_DToPort;
	EDInstructions [0xED59]		= new FZ80::OUT_EToPort;
	EDInstructions [0xED61]		= new FZ80::OUT_HToPort;
	EDInstructions [0xED69]		= new FZ80::OUT_LToPort;
	EDInstructions [0xED71]		= new FZ80::OUT_0ToPort;					// Undocumented

	// OUT Block
	EDInstructions [0xEDA3]		= new FZ80::OUTI;
	EDInstructions [0xEDB3]		= new FZ80::OTIR;	
	EDInstructions [0xEDAB]		= new FZ80::OUTD;
	EDInstructions [0xEDBB]		= new FZ80::OTDR;	

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
