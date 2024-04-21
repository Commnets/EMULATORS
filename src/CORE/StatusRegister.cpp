#include <CORE/StatusRegister.hpp>

// ---
MCHEmul::StatusRegister::StatusRegister (size_t nB, const BitNames& bN)
	: MCHEmul::Register (-2 /** always */, "ST", UBytes (std::vector <UByte> (nB, UByte::_0))), 
	  _bitNames (bN),
	  _bitPosName () // Filled up below...
{ 
	for (const auto& i : _bitNames) 
		_bitPosName.insert (std::map <size_t, const std::string>::value_type (i.second, i.first)); 
}

// ---
MCHEmul::UBytes MCHEmul::StatusRegister::valuesWithout (const MCHEmul::Strings& bN)
{
	MCHEmul::UBytes result = values ();
	for (const auto& i : _bitNames)
		if (std::find (bN.begin (), bN.end (), i.first) != bN.end ())
			result.setBit (i.second, false);
	return (result);
}

// ---
std::string MCHEmul::StatusRegister::asString () const
{
	std::string result = _name + ":$" + 
		MCHEmul::removeAll0 (values ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')) + "-";
	for (int i = (int) ((size () * MCHEmul::UByte::sizeBits ()) - 1); i >= 0; i--)
		result += bitName ((size_t) i) + "(" + (bitStatus ((size_t) i) ? "1" : "0") + ")";
	return (result);
}
