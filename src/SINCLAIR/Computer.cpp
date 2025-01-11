#include <SINCLAIR/Computer.hpp>
#include <fstream>

// ---
MCHEmul::InfoStructure SINCLAIR::Computer::SystemVariable::getInfoStructure (const MCHEmul::Memory* m) const
{
	MCHEmul::InfoStructure result;

	result.add ("NAME", _name);
	result.add ("TYPE", std::to_string ((unsigned int) _type));
	result.add ("ADDRESS", _address);
	// Depending on the type the value is written is the right format...
	result.add ("VALUE", 
		(_type == SINCLAIR::Computer::SystemVariable::Type::_ADDRESS)
			? MCHEmul::removeAll0 (valueAsAddress (m).asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
			: (_type == SINCLAIR::Computer::SystemVariable::Type::_1BYTEVALUE)
				? MCHEmul::removeAll0 (valueAs1Byte (m).asString (MCHEmul::UByte::OutputFormat::_HEXA))
				: (_type == SINCLAIR::Computer::SystemVariable::Type::_2BYTESVALUE)
					? MCHEmul::removeAll0 (valueAs2Bytes (m).asString (MCHEmul::UByte::OutputFormat::_HEXA, ' '))
					: "BLOCK");
	result.add ("BYTES", _bytes);
	result.add ("DESCRIPTION", _description);

	return (result);
}

// ---
MCHEmul::InfoStructure SINCLAIR::Computer::getSystemVariablesAsInfoStructure (const MCHEmul::Memory* m) const
{
	MCHEmul::InfoStructure result;

	size_t ct = 0;
	for (const auto& i : _systemVariables)
		result.add (std::to_string (ct++), i.second.getInfoStructure (m));

	return (result);
}

// ---
SINCLAIR::Computer::SystemVariables SINCLAIR::Computer::loadSystemVariablesFrom (const std::string& fN)
{
	SINCLAIR::Computer::SystemVariables result;

	std::ifstream f (fN);
	if (!f)
		return (result);

	std::string s;
	while (!f.eof ())
	{
		s = "";
		std::getline (f, s);
		MCHEmul::Strings e = MCHEmul::getElementsFrom (s, ';');
		assert (e.size () == 4); // 4 elements always...

		// The number of bytes will also tell us the type of data kept...
		unsigned char b = (unsigned char) std::stoi (e [1].substr (1), nullptr, 16);
		SINCLAIR::Computer::SystemVariable::Type t;
		switch (b)
		{
			case 1:		t = SINCLAIR::Computer::SystemVariable::Type::_1BYTEVALUE; break;
			case 2:		t = SINCLAIR::Computer::SystemVariable::Type::_ADDRESS; break;
			default:	t = SINCLAIR::Computer::SystemVariable::Type::_BLOCK; break;
		}

		result.insert (SINCLAIR::Computer::SystemVariables::value_type 
			(e [0], { e [0], t, b, MCHEmul::Address::fromStr (e [2]), e [3] }));
	}

	return (result);
}
