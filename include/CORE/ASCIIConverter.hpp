/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: ASCIIConverter.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Forniés (EMULATORS library) \n
*	Creation Date: 29/12/2021 \n
*	Description: To convert ASCII Codes into specific tables used by the computers (PET,...)
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_ASCIICONVERTER__
#define __MCHEMUL_ASCIICONVERTER__

#include <CORE/UByte.hpp>

namespace MCHEmul
{
	/** To convert the ASCII codes into other specific table recognized by the computer. */
	class ASCIIConverter
	{
		public:
		ASCIIConverter ()
						{ }

		ASCIIConverter (const ASCIIConverter&) = default;

		ASCIIConverter& operator = (const ASCIIConverter&) = default;

		virtual ~ASCIIConverter  ()
						{ }

		/** Convert a string into a set of bytes using the conversion table. 
			By default it doesn't translate anything. */
		virtual std::vector <UByte> convert (const std::string& str) const;
		/** Convert a simple char. By default it doesn't transalte anything. */
		virtual UByte convert (unsigned char c) const
						{ return (UByte (c)); }
	};
}

#endif

// End of the file
/*@}*/
