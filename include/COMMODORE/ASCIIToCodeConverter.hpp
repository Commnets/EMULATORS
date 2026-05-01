/** \ingroup COMMODORE */
/*@{*/

/**	
*	@file	
*	File: ASCIIToCodeConverter.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 19/04/2026 \n
*	Description: To convert ASCII Codes into specific tables used by the COMMODORE Computers
*	Versions: 1.0 Initial
*/

#ifndef __COMMODORE_ASCIICONVERTER__
#define __COMMODORE_ASCIICONVERTER__

#include <CORE/ASCIIToCodeConverter.hpp>

namespace COMMODORE
{
	/** Adapted to COMMODORE machines where they use PETSCII, quite similar to ASCII, BTW. */
	class ASCIIToCodeConverter final : public MCHEmul::ASCIIToCodeConverter
	{
		public:
		/** The default sign when something can not be converted. */
		static const MCHEmul::UByte _DEFAULTPETSCII;
		static const MCHEmul::UByte _DEFAULTASCII;

		ASCIIToCodeConverter ()
			: MCHEmul::ASCIIToCodeConverter ()
						{ }

		/** Convert a simple char from ASCII to PETSCII. 
			When something can not be converted a DEFAULTPETSCII is returtned */
		virtual MCHEmul::UByte convert (unsigned char c) const override;
		/** The opposite. */
		virtual MCHEmul::UByte inverseConvert (unsigned char c) const override;
	};
}

#endif

// End of the file
/*@}*/
