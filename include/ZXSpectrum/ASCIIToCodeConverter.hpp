/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
*	@file	
*	File: ASCIIToCodeConverter.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 19/04/2026 \n
*	Description: To convert ASCII Codes into specific tables used by the ZXSPECTRUM Computer
*	Versions: 1.0 Initial
*/

#ifndef __ZXSPECTRUM_ASCIICONVERTER__
#define __ZXSPECTRUM_ASCIICONVERTER__

#include <CORE/ASCIIToCodeConverter.hpp>

namespace ZXSPECTRUM
{
	/** The specific version to ZXSPECTRUM.
		Like ASCII but "shifted" in general terms!. */
	class ASCIIToCodeConverter final : public MCHEmul::ASCIIToCodeConverter
	{
		public:
		/** The default sign when something can not be converted. */
		static const MCHEmul::UByte _DEFAULTZXSPECTRUMCODE;
		static const MCHEmul::UByte _DEFAULTASCII;

		ASCIIToCodeConverter ()
			: MCHEmul::ASCIIToCodeConverter ()
						{ }

		/** Convert a simple char from ASCII to ZXSPECTRUMCODE. 
			When something can not be converted a _DEFAULTZXSPECTRUMCODE is returtned */
		virtual MCHEmul::UByte convert (unsigned char c) const override;
		/** The opposite. */
		virtual MCHEmul::UByte inverseConvert (unsigned char c) const override;
	};
}

#endif

// End of the file
/*@}*/
