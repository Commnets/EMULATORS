#include "CRTData.hpp"

VIC20::CRTData::Header::Header ()
	: _signature (""), _name (""),
	  _length (), _version (), _type ()
{ 
	setSignature ("VIC20 Cartridge");
	setName ("");
	setLength (64),
	setVersion (MCHEmul::UBytes ({ 2, 0 }, true)),
	setType (0);
}

// ---
VIC20::CRTData::Header::Header (const std::string& s, const std::string& n,
		unsigned int l, unsigned short v, unsigned short t)
	: _signature (""), _name (""),
	  _length (), _version (), _type ()
{ 
	setSignature (s);
	setName (n);
	setLength (l);
	setVersion (v);
	setType (t);
}

// ---
VIC20::CRTData::Chip::Chip (
		const std::string& n,
		const MCHEmul::DataMemoryBlock& dt,
		const MCHEmul::UBytes& l, const MCHEmul::UBytes& t)
	: _signature (""), // Assign just after this...
	  _length (l), _type (t), 
	  _data (dt) // Not adjusted later...
{ 
	setSignature (n); 
	setLength (l);
	setType (t);
}

// ---
VIC20::CRTData::Chip::Chip (
		const std::string& n,
		MCHEmul::DataMemoryBlock&& dt,
		const MCHEmul::UBytes& l, const MCHEmul::UBytes& t)
	: _signature (""), // Assign just after this...
	  _length (l), _type (t), 
	  _data (std::move (dt)) // Not adjusted later...
{ 
	setSignature (n); 
	setLength (l);
	setType (t);
}

// --
MCHEmul::UBytes VIC20::CRTData::Header::bytes () const
{
	std::vector <MCHEmul::UByte> result;

	// The elements of the signature are converted into UBytes
	// ...and then added in the data result...
	std::vector <MCHEmul::UByte> sB;
	for (const auto& i : _signature) 
		sB.emplace_back (MCHEmul::UByte (i));
	result.insert (result.end (), sB.begin (), sB.end ());

	// ...then the length (in little - endian format)
	result.insert (result.end (), _length.bytes ().begin (), _length.bytes ().end ());
	// ...then the version (normal order)
	result.insert (result.end (), _version.bytes ().begin (), _version.bytes ().end ());
	// ..then the type
	result.insert (result.end (), _type.bytes ().begin (), _type.bytes ().end ());

	// There are then 8 bytes not usefull for a VIC20 Cartridge:
	// 0x18 = EXROM = 0
	// 0x19 = GAME = 0
	// 0x1a = Revision = 0
	// 0x1b - 0x1f = Future use = 0
	std::vector <MCHEmul::UByte> fuB (8, MCHEmul::UByte (0));
	result.insert (result.end (), fuB.begin (), fuB.end ());

	// Then the name of the cartridge...
	std::vector <MCHEmul::UByte> nB;
	for (const auto& i : _name) 
		nB.emplace_back (MCHEmul::UByte (i));
	result.insert (result.end (), nB.begin (), nB.end ());

	return (result);
}

// ---
MCHEmul::UBytes VIC20::CRTData::Chip::bytes () const
{
	std::vector <MCHEmul::UByte> result;

	// The signature elements are converted into UBytes!
	// ...and then all are added in the result!
	std::vector <MCHEmul::UByte> sB;
	for (const auto& i : _signature) 
		sB.emplace_back (MCHEmul::UByte (i));
	result.insert (result.end (), sB.begin (), sB.end ());

	// ...then the length (in little - endian format)
	result.insert (result.end (), _length.bytes ().begin (), _length.bytes ().end ());
	// ...then the type (in the normal format)
	result.insert (result.end (), _type.bytes ().begin (), _type.bytes ().end ());

	// There are then 2 bytes not usefull for a VIC20 Cartridge:
	// 0x0a - 0x0b = Bank = 0x0000
	std::vector <MCHEmul::UByte> fuB (2, MCHEmul::UByte (0));
	result.insert (result.end (), fuB.begin (), fuB.end ());

	// ...the the start address...
	std::vector <MCHEmul::UByte> aB = _data.startAddress ().bytes (); // high - low format (big endian already). */
	result.insert (result.end (), aB.begin (), aB.end ());

	// ...the the size in bytes
	MCHEmul::UInt sz = MCHEmul::UInt::fromUnsignedInt ((unsigned int) _data.size ());
	sz.setMinLength (2); // Already in format big - endian!
	result.insert (result.end (), sz.bytes ().begin (), sz.bytes ().end ());

	// ...and finally the data info
	result.insert (result.end (), _data.bytes ().begin (), _data.bytes ().end ());

	return (result);
}

// ---
bool VIC20::CRTData::saveTo (const std::string& fN, const VIC20::Parameters& prms, bool& e)
{
	std::vector <MCHEmul::UByte> dt;

	// ...the header...
	std::vector <MCHEmul::UByte> hB (_header.bytes ().bytes ());
	dt.insert (dt.end (), hB.begin (), hB.end ());

	// ...and the chips...
	for (const auto& i : _chips)
	{ 
		std::vector <MCHEmul::UByte> cB (i.bytes ().bytes ());
		dt.insert (dt.end (), cB.begin (), cB.end ());
	}

	return (MCHEmul::UBytes::saveBytesTo (fN, MCHEmul::UBytes (dt)));
}
