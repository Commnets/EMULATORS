#include <CORE/UBytes.hpp>
#include <algorithm>
#include <fstream>

// ---
const MCHEmul::UBytes MCHEmul::UBytes::_E = MCHEmul::UBytes ();

// ---
std::string MCHEmul::UBytes::asString (MCHEmul::UByte::OutputFormat oF, char s, size_t l, size_t sb) const
{
	if (size () == 0)
		return ("");

	std::string result = "";
	for (size_t i = 0; i < size (); i += sb)
	{
		if (i != 0) result += '\n';
		for (size_t j = i; j < (i + sb) && j < size (); j++)
		{
			if (j != i) result += s;
			result += _values [j].asString (oF, l);
		}
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::UBytes::loadBytesFrom (const std::string& fN, bool& e)
{
	e = false;

	std::ifstream file (fN, std::ios::in | std::ios::binary);
	if (!file)
	{ 
		e = true;

		return (std::vector <MCHEmul::UByte> ());
	}

	file.seekg (0, std::ios::end);
	std::streamoff lF = file.tellg (); 
	// The length of the file can not be longer than the limits of the vector...
	// In x64 it doesn't happen but it might be possible in x84 where size_t = unisgned int and std::strea,off = long long
	if (lF > (std::streamoff) std::numeric_limits <size_t>::max ())
	{
		file.close ();

		e = true;

		return (std::vector <MCHEmul::UByte> ());
	}

	size_t lFA = (size_t) lF;
	char* fDT = new char [lFA];
	file.seekg (0, std::ios::beg);
	file.read (fDT, (std::streamsize) lFA); // Reads the info itself

	file.close ();

	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < (size_t) (lFA / MCHEmul::UByte::size ()); i += MCHEmul::UByte::size ())
		result.push_back ((MCHEmul::UByte) (*(fDT + i)));

	delete [] fDT;

	return (result);
}

// ---
bool MCHEmul::UBytes::saveBytesTo (const std::string& fN, const std::vector <MCHEmul::UByte>& u)
{
	std::ofstream file (fN, std::ios::out | std::ios::binary);
	if (!file)
		return (false);

	char* fV = new char [u.size () * MCHEmul::UByte::size ()];
	for (size_t i = 0; i < (u.size () * MCHEmul::UByte::size ()); i += MCHEmul::UByte::size ()) 
		fV [i] = u [i].value ();
	file.write (fV, (std::streamsize) u.size () * (std::streamsize) MCHEmul::UByte::size ()); // If wrong, result = true; 
	bool result = (!file) ? false : true;

	delete [] fV;

	file.close ();

	return (result);
}
