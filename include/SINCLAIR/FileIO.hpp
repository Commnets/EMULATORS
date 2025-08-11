/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/08/2025 \n
 *	Description:	The specific file readers for SINCLAIR emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_FILEIO__
#define __SINCLAIR_FILEIO__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** In a SINCLAIR machine the way the tokens are read and 
		the keystrokes associated are managed is totally different,
		but those TOKENS vary per type of machine, and also the key to press to generate them. */
	class KeystrokeTypeIO : public MCHEmul::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO (const MCHEmul::Strings& t,
			const std::map <std::string, MCHEmul::Strings>& k)
			: MCHEmul::KeystrokeTypeIO (),
			  _TOKENS (t), _KEYSTROKES (k)
							{ }

		protected:
		virtual MCHEmul::Strings generateTokensFor (const std::string& str) const override;
		virtual MCHEmul::Strings generateKeystrokeForToken (const std::string& t) const override;

		protected:
		virtual bool isARealToken (char u, const std::string& l, size_t i) const = 0;
		
		protected:
		/** Received as parameter, at construction time. */
		const MCHEmul::Strings _TOKENS;
		const std::map <std::string, MCHEmul::Strings> _KEYSTROKES;
	};
}

#endif
  
// End of the file
/*@}*/
