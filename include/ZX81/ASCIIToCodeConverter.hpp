/** \ingroup ZX81 */
/*@{*/

/**	
*	@file	
*	File: ASCIIToCodeConverter.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 19/04/2026 \n
*	Description: To convert ASCII Codes into specific tables used by the ZX81 Computer
*	Versions: 1.0 Initial
*/

#ifndef __ZX81_ASCIICONVERTER__
#define __ZX81_ASCIICONVERTER__

#include <CORE/ASCIIToCodeConverter.hpp>

namespace ZX81
{
	/** Adapted to the ZX81.
		It follows their own conventions. */
	class ASCIIToCodeConverter final : public MCHEmul::ASCIIToCodeConverter
	{
		public:
		/** The default sign when something can not be converted. */
		static const MCHEmul::UByte _DEFAULTZX81CODE;
		static const MCHEmul::UByte _DEFAULTASCII;

		ASCIIToCodeConverter ()
			: MCHEmul::ASCIIToCodeConverter ()
						{ }

		/** Convert a simple char from ASCII to ZX81CODE. 
			When something can not be converted a _DEFAULTZX81CODE is returtned */
		virtual MCHEmul::UByte convert (unsigned char c) const override;
		/** The opposite. */
		virtual MCHEmul::UByte inverseConvert (unsigned char c) const override;
	};
}

#endif

// End of the file
/*@}*/
