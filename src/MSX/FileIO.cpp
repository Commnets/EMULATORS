#include <MSX/FileIO.hpp>

namespace
{
	bool isCASHeaderAt (const std::vector <MCHEmul::UByte>& dt, size_t p)
	{
		if ((p + MSX::CASFileData::_SIGNATURE.size ()) > dt.size ())
			return (false);

		bool result = true;
		for (size_t i = 0; i < MSX::CASFileData::_SIGNATURE.size () && result; i++)
			result = dt [p + i] == MSX::CASFileData::_SIGNATURE [i];

		return (result);
	}

	// ---
	MSX::CASFileData::BlockType blockTypeFor (
		const std::vector <MCHEmul::UByte>& dt)
	{
		if (dt.size () < 16)
			return (MSX::CASFileData::BlockType::_DATA);

		unsigned char marker = dt [0].value ();
		if (marker != 0xd0 && marker != 0xd3 && marker != 0xea)
			return (MSX::CASFileData::BlockType::_DATA);

		bool header = true;
		for (size_t i = 1; i < 10 && header; i++)
			header = dt [i].value () == marker;
		if (!header)
			return (MSX::CASFileData::BlockType::_DATA);

		return (marker == 0xd0
			? MSX::CASFileData::BlockType::_BINARYHEADER
			: (marker == 0xd3
				? MSX::CASFileData::BlockType::_BASICHEADER
				: MSX::CASFileData::BlockType::_ASCIIHEADER));
	}

	// ---
	std::string blockTypeName (MSX::CASFileData::BlockType t)
	{
		std::string result = "DATA";

		switch (t)
		{
			case MSX::CASFileData::BlockType::_BINARYHEADER:
				result = "BINARYHEADER";
				break;

			case MSX::CASFileData::BlockType::_BASICHEADER:
				result = "BASICHEADER";
				break;

			case MSX::CASFileData::BlockType::_ASCIIHEADER:
				result = "ASCIIHEADER";
				break;

			default:
				break;
		}

		return (result);
	}

	// ---
	std::string fileNameFor (const std::vector <MCHEmul::UByte>& dt,
		MSX::CASFileData::BlockType t)
	{
		if (t == MSX::CASFileData::BlockType::_DATA)
			return ("");

		std::string result;
		for (size_t i = 10; i < 16; i++)
			result += (char) dt [i].value ();

		while (!result.empty () &&
			(result.back () == ' ' || result.back () == '\0'))
			result.pop_back ();

		return (result);
	}
}

const std::vector <MCHEmul::UByte> MSX::CASFileData::_SIGNATURE =
	{ 0x1f, 0xa6, 0xde, 0xba, 0xcc, 0x13, 0x7d, 0x74 };

// ---
MCHEmul::Strings MSX::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the token received is just "complex"
	// because the combination of the keys was done...
	if (t.length () > 1)
		return (MCHEmul::Strings { t }); // The keystroke is already the token!

	MCHEmul::Strings result;

	// In the MSX system there is upper case letter and normal case ones...
	// so, if a upper case one is detected the LSHIFT keystroke must be selected!
	if (std::isalpha (t [0]) && std::isupper (t [0]))
		result.emplace_back ("LSHIFT+" + std::string ({ t [0] }));
	else
	{
		std::map <char, MCHEmul::Strings>::const_iterator sp;
		if ((sp = _SPECIALKEYS.find (t [0])) != _SPECIALKEYS.end ()) result = (*sp).second;
		else result = MCHEmul::Strings { t };
	}

	return (result);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks MSX::CASFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = "MSXCAS";
	result._attributes = { { "FORMAT", "CAS" } };

	for (size_t i = 0; i < _blocks.size (); i++)
	{
		const MSX::CASFileData::Block& block = _blocks [i];
		MCHEmul::DataMemoryBlock dataBlock (MCHEmul::Address (), block._bytes);

		dataBlock.setName ("CASBLOCK_" + std::to_string (i));
		dataBlock.setAttribute ("TYPE", blockTypeName (block._type));
		if (!block._fileName.empty ())
			dataBlock.setAttribute ("FILENAME", block._fileName);

		result._data.emplace_back (std::move (dataBlock));
	}

	return (result);
}

// ---
std::string MSX::CASFileData::asString () const
{
	return ("MSX CAS (Blocks: " + std::to_string (_blocks.size ()) + ")");
}

// ---
bool MSX::CASFileTypeIO::canRead (const std::string& fN) const
{
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length () ||
		MCHEmul::upper (fN.substr (pp + 1)) != "CAS")
		return (false);

	std::ifstream file (fN, std::ios::in | std::ios::binary);
	if (!file)
		return (false);

	file.seekg (0, std::ios::end);
	std::streamoff fileSize = file.tellg ();
	if (fileSize < (std::streamoff) MSX::CASFileData::_SIGNATURE.size ())
		return (false);

	file.seekg (0, std::ios::beg);
	std::vector <MCHEmul::UByte> data;
	data.reserve ((size_t) fileSize);
	for (std::streamoff i = 0; i < fileSize; i++)
	{
		char value = 0;
		file.read (&value, 1);
		if (!file)
			return (false);
		data.emplace_back ((unsigned char) value);
	}

	bool result = false;
	for (size_t i = 0; i < data.size () && !result; i++)
		result = isCASHeaderAt (data, i);

	return (result);
}

// ---
MCHEmul::FileData* MSX::CASFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	(void) bE;

	std::ifstream file (fN, std::ios::in | std::ios::binary);
	if (!file)
		return (nullptr);

	file.seekg (0, std::ios::end);
	std::streamoff fileSize = file.tellg ();
	if (fileSize < (std::streamoff) MSX::CASFileData::_SIGNATURE.size ())
		return (nullptr);
	file.seekg (0, std::ios::beg);

	std::vector <MCHEmul::UByte> data;
	data.reserve ((size_t) fileSize);
	for (std::streamoff i = 0; i < fileSize; i++)
	{
		char value = 0;
		file.read (&value, 1);
		if (!file)
			return (nullptr);
		data.emplace_back ((unsigned char) value);
	}

	std::vector <size_t> headers;
	for (size_t i = 0; i < data.size (); i++)
		if (isCASHeaderAt (data, i))
			headers.emplace_back (i);
	if (headers.empty ())
		return (nullptr);

	MSX::CASFileData* result = new MSX::CASFileData;
	for (size_t i = 0; i < headers.size (); i++)
	{
		size_t first = headers [i] + MSX::CASFileData::_SIGNATURE.size ();
		size_t last = ((i + 1) < headers.size ()) ? headers [i + 1] : data.size ();

		MSX::CASFileData::Block block;
		block._bytes = std::vector <MCHEmul::UByte>
			(data.begin () + first, data.begin () + last);
		block._type = blockTypeFor (block._bytes);
		block._fileName = fileNameFor (block._bytes, block._type);
		result -> _blocks.emplace_back (std::move (block));
	}

	return (result);
}

// ---
bool MSX::CASFileTypeIO::writeFile (
	MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	(void) bE;

	MSX::CASFileData* casData = dynamic_cast <MSX::CASFileData*> (fD);
	if (casData == nullptr)
		return (false);

	std::ofstream file (fN, std::ios::out | std::ios::binary);
	if (!file)
		return (false);

	for (const auto& block : casData -> _blocks)
	{
		for (const auto& value : MSX::CASFileData::_SIGNATURE)
			file.put ((char) value.value ());

		for (const auto& value : block._bytes)
			file.put ((char) value.value ());

		if (!file)
			return (false);
	}

	return (true);
}
