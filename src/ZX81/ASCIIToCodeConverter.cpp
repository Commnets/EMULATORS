#include <ZX81/ASCIIToCodeConverter.hpp>

// The question mark is the default sign when something can not be converted...
const MCHEmul::UByte ZX81::ASCIIToCodeConverter::_DEFAULTZX81CODE = MCHEmul::UByte (0x3f);
const MCHEmul::UByte ZX81::ASCIIToCodeConverter::_DEFAULTASCII = MCHEmul::UByte ('?');

// ---
MCHEmul::UByte ZX81::ASCIIToCodeConverter::convert (unsigned char c) const
{
	unsigned char result = _DEFAULTZX81CODE.value();
	
	if (c < 0x20)
		return MCHEmul::UByte (result);
	
	if (c >= '0' && c <= '9')
		result = 0x1c + (c - '0');
	else if (c >= 'A' && c <= 'Z')
		result = 0x26 + (c - 'A');
	else if (c >= 'a' && c <= 'z')
		result = 0x26 + (c - 'a'); // ZX81 has no lowercase.
	else
	{
		switch (c)
		{
			case ' ': result = 0x00; break;
			case '"': result = 0x0b; break;
			case '#': result = 0x0c; break; // ASCII fallback for ZX81 pound sign.
			case '$': result = 0x0d; break;
			case ':': result = 0x0e; break;
			case '?': result = 0x0f; break;
			case '(': result = 0x10; break;
			case ')': result = 0x11; break;
			case '>': result = 0x12; break;
			case '<': result = 0x13; break;
			case '=': result = 0x14; break;
			case '+': result = 0x15; break;
			case '-': result = 0x16; break;
			case '*': result = 0x17; break;
			case '/': result = 0x18; break;
			case ';': result = 0x19; break;
			case ',': result = 0x1a; break;
			case '.': result = 0x1b; break;
			default: break;
		}
	}

	return (MCHEmul::UByte (result));
}

// ---
MCHEmul::UByte ZX81::ASCIIToCodeConverter::inverseConvert (unsigned char c) const
{
	// By default what to print out is _DEFAULTASCII...
	// unless the char received has an quivalent in "printable" ascii.
	unsigned char result = _DEFAULTASCII.value ();

	// On the ZX81, the reverse video characters are in block 128..191
	// and correspond to codes 0..63 with bit 7 set to 1.
	// They are treated the same as their normal version.
 	if (c >= 0x80 && c <= 0xbf)
		c &= 0x3f;
	// Digits 0..9
	if (c >= 0x1c && c <= 0x25) 
		result = '0' + (c - 0x1c);
	// Letters A..Z
	else 
	if (c >= 0x26 && c <= 0x3f) 
		result = 'A' + (c - 0x26);
	else
	{
		switch (c)
		{
			case 0x00: result = ' '; break;
			case 0x0b: result = '"'; break;
			case 0x0c: result = '#'; break; // ZX81 pound sign fallback...
			case 0x0d: result = '$'; break;
			case 0x0e: result = ':'; break;
			case 0x0f: result = '?'; break;
			case 0x10: result = '('; break;
			case 0x11: result = ')'; break;
			case 0x12: result = '>'; break;
			case 0x13: result = '<'; break;
			case 0x14: result = '='; break;
			case 0x15: result = '+'; break;
			case 0x16: result = '-'; break;
			case 0x17: result = '*'; break;
			case 0x18: result = '/'; break;
			case 0x19: result = ';'; break;
			case 0x1a: result = ','; break;
			case 0x1b: result = '.'; break;
			default: break;
		}
	}

	// No other type of letter is managed...
	return (MCHEmul::UByte (result));
}
