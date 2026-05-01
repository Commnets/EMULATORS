#include <COMMODORE/ASCIIToCodeConverter.hpp>

// The question mark is the default sign when something can not be converted...
const MCHEmul::UByte COMMODORE::ASCIIToCodeConverter::_DEFAULTPETSCII = MCHEmul::UByte (0x3f);
const MCHEmul::UByte COMMODORE::ASCIIToCodeConverter::_DEFAULTASCII = MCHEmul::UByte ('?');

// ---
MCHEmul::UByte COMMODORE::ASCIIToCodeConverter::convert (unsigned char c) const
{
    unsigned char result = _DEFAULTPETSCII.value ();

	// Control codes, with no translation...
	if (c < 0x20)
		return (MCHEmul::UByte (result));

    // Common range: 
    // Space, uppercase letters, numbers, @, and punctuation marks and some sepcial symbols...
    if (c <= 0x5f) result = (unsigned char) c;
	// Lowercase letters are not directly represented in PETSCII;
	// It is common to use the uppercase letters in PETSCII, 
    // so if a lowercase letter is received,
    // it is normal to convert them to uppercase to make them visible...
    else if (c >= (unsigned char) 'a' && 
             c <= (unsigned char) 'z') result = (unsigned char) (c - 0x20); 

    return (MCHEmul::UByte (result));
}

// ---
MCHEmul::UByte COMMODORE::ASCIIToCodeConverter::inverseConvert (unsigned char c) const
{
    unsigned char result = _DEFAULTASCII.value ();
    
    // Common range: 
    // Space, numbers, punctuation marks, @ and uppercase letters and some special symbols...
    if (c >= 0x20 && c <= 0x5f) result = (unsigned char) c;
    
    return (MCHEmul::UByte (result));
}
