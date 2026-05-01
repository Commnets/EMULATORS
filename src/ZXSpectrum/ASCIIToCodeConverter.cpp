#include <ZXSPECTRUM/ASCIIToCodeConverter.hpp>

// The question mark is the default sign when something can not be converted...
const MCHEmul::UByte ZXSPECTRUM::ASCIIToCodeConverter::_DEFAULTZXSPECTRUMCODE = MCHEmul::UByte (0x3f);
const MCHEmul::UByte ZXSPECTRUM::ASCIIToCodeConverter::_DEFAULTASCII = MCHEmul::UByte (0x3f);

// ---
MCHEmul::UByte ZXSPECTRUM::ASCIIToCodeConverter::convert (unsigned char c) const
{
	unsigned char result = _DEFAULTZXSPECTRUMCODE.value ();

	if (c < 0x20) 
		return (MCHEmul::UByte (result));;
	
	// Specific exceptions:
	// ZX Spectrum uses 0x60 for the Pound sign (POUND).
	if (c == '#') result = 0x60;
	// ZX Spectrum uses 0x7F for the Copyright symbol (©).
	// Note: In Spectrum, '@' is 0x40. We keep it as 0x40 to avoid 
	// breaking addresses, unless you specifically want '@' to become (C).
 	else if (c == '@') result = 0x7f; 
	// Standard printable range:
	// ZX Spectrum follows standard ASCII almost entirely between 0x20 and 0x7e.
	if (c >= 0x20 && c <= 0x7e) result = c;

	return (MCHEmul::UByte (result));
}

// ---
MCHEmul::UByte ZXSPECTRUM::ASCIIToCodeConverter::inverseConvert (unsigned char c) const
{
	unsigned char result = _DEFAULTASCII.value ();

	// Control codes, with no translation...
	if (c < 0x20)
		return (MCHEmul::UByte (result));

	// 0x20..0x7f: Visible ASCII
	// The base matches with ASCII for these codes except
	// specific SPECTRUM chars like £ (0x60) y © (0x7f).
	if (c <= 0x7f)
	{
		// The pound sign is not in ASCII, so it is common to replace it with the hash symbol.
		if (c == 0x60) result = '#'; 
		// The copyright sign is not in ASCII, so it is common to replace it with the at symbol.
		else if (c == 0x7f) result = '@'; 
		else result = c;
	}

	// The rest of the codes will be printed out like as default...
	return (result);
}
