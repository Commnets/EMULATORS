#include "CRTData.hpp"

VIC20::CRTData::Header::Header ()
	: _signature (""), _name (""),
	  _length (), _version (), _type ()
{ 
	setSignature ("VIC20 Cartridge");
	setName ("");
	setLength (40),
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
		const MCHEmul::UBytes& l, const MCHEmul::UBytes& t,	const MCHEmul::UBytes& b)
	: _signature (""), // Assign just after this...
	  _length (l), _type (t), _bank (b), 
	  _data (dt) // Not adjusted later...
{ 
	setSignature (n); 
	setLength (l);
	setType (t);
	setBank (b);
}

// ---
VIC20::CRTData::Chip::Chip (
		const std::string& n,
		MCHEmul::DataMemoryBlock&& dt,
		const MCHEmul::UBytes& l, const MCHEmul::UBytes& t,	const MCHEmul::UBytes& b)
	: _signature (""), // Assign just after this...
	  _length (l), _type (t), _bank (b), 
	  _data (std::move (dt)) // Not adjusted later...
{ 
	setSignature (n); 
	setLength (l);
	setType (t);
	setBank (b);
}

// --
MCHEmul::UBytes VIC20::CRTData::Header::bytes () const
{
	std::vector <MCHEmul::UByte> dt;

	std::vector <MCHEmul::UByte> sB;
	for (const auto& i : _signature) 
		sB.emplace_back (MCHEmul::UByte (i));
	dt.insert (dt.end (), sB.begin (), sB.end ());

	dt.insert (dt.end (), _length.bytes ().begin (), _length.bytes ().end ());
	dt.insert (dt.end (), _version.bytes ().begin (), _version.bytes ().end ());
	dt.insert (dt.end (), _type.bytes ().begin (), _type.bytes ().end ());

	std::vector <MCHEmul::UByte> fuB (8, MCHEmul::UByte (0));
	dt.insert (dt.end (), fuB.begin (), fuB.end ());

	std::vector <MCHEmul::UByte> nB;
	for (const auto& i : _name) 
		nB.emplace_back (MCHEmul::UByte (i));
	dt.insert (dt.end (), nB.begin (), nB.end ());

	return (dt);
}

// ---
MCHEmul::UBytes VIC20::CRTData::Chip::bytes () const
{
	std::vector <MCHEmul::UByte> dt;

	std::vector <MCHEmul::UByte> sB;
	for (const auto& i : _signature) 
		sB.emplace_back (MCHEmul::UByte (i));
	dt.insert (dt.end (), sB.begin (), sB.end ());

	dt.insert (dt.end (), _length.bytes ().begin (), _length.bytes ().end ());
	dt.insert (dt.end (), _type.bytes ().begin (), _type.bytes ().end ());
	dt.insert (dt.end (), _bank.bytes ().begin (), _bank.bytes ().end ());

	std::vector <MCHEmul::UByte> aB = _data.startAddress ().bytes ();
	dt.insert (dt.begin (), aB.begin (), aB.end ());

	MCHEmul::UInt sz = MCHEmul::UInt::fromUnsignedInt ((unsigned int) _data.size ());
	sz.setMinLength (2);
	dt.insert (dt.end (), sz.bytes ().begin (), sz.bytes ().end ());

	dt.insert (dt.end (), _data.bytes ().begin (), _data.bytes ().end ());

	return (dt);
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
