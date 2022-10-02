#include <C64/ChipRegisters.hpp>

// ---
std::ostream& C64::operator << (std::ostream& o, const C64::ChipRegisters& vr)
{
	bool fB = true;
	for (size_t i = 0; i < vr.numberRegisters (); i += 0x10, fB = false)
	{
		if (!fB) o << std::endl;

		bool fV = true;
		for (size_t j = i; j < (i + 0x010) && j < 0x040; j++, fV = false)
			o << (fV ? "" : " ") << MCHEmul::UByte ((unsigned char) j) << ":" << vr.readValue (j);
	}

	return (o);
}
