/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: ASCIIToCodeConverter.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 29/12/2021 \n
*	Description: To convert ASCII Codes into specific tables used by the computers (PET,...)
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_ASCIICONVERTER__
#define __MCHEMUL_ASCIICONVERTER__

#include <CORE/UByte.hpp>

namespace MCHEmul
{
	/** To convert the ASCII codes into the specific ones recognized by a computer. 
		The opposite opertion is also possible. \n
		The method convert and "inverseConvert" are the important ones. */
	class ASCIIToCodeConverter
	{
		public:
		ASCIIToCodeConverter ()
						{ }

		ASCIIToCodeConverter (const ASCIIToCodeConverter&) = delete;

		ASCIIToCodeConverter& operator = (const ASCIIToCodeConverter&) = delete;

		virtual ~ASCIIToCodeConverter  ()
						{ }

		ASCIIToCodeConverter (ASCIIToCodeConverter&&) = delete;

		ASCIIToCodeConverter& operator = (ASCIIToCodeConverter&&) = delete;

		/** Convert a simple char from ASCII into the computer's code.
			By default it doesn't translate anything. */
		virtual UByte convert (unsigned char c) const
						{ return (UByte (c)); }
		/** The opposite: From the Computer's code into ASCII. \n
			By default it doesn't translate anything. */
		virtual UByte inverseConvert (unsigned char c) const
						{ return (UByte (c)); }

		/** Convert a string into a set of bytes using the conversion table. 
			By default it doesn't translate anything. */
		std::vector <UByte> convert (const std::string& str) const;
		/** The inverse. */
		std::vector <UByte> inverseConvert (const std::string& str) const;
		/** Convert a vector of UByte. */
		std::vector <UByte> convert (const std::vector <UByte>& lst) const;
		/** The inverse. */
		std::vector <UByte> inverseConvert (const std::vector <UByte>& lst) const;
	};
}

#endif

// End of the file
/*@}*/
