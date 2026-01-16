/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: FileIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description:	The specific file readers for MSX emulations.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_FILEIO__
#define __MSX_FILEIO__

#include <CORE/incs.hpp>

namespace MSX
{
	/** In the MSX the keystrokes are generated in a special manner. */
	class KeystrokeTypeIO final : public MCHEmul::KeystrokeTypeIO
	{
		public:
		KeystrokeTypeIO (const std::map <char, MCHEmul::Strings>& sK)
			: MCHEmul::KeystrokeTypeIO (),
			  _SPECIALKEYS (sK)
							{ }

		private:
		/** It is like the standard one, but no lowecase letters are allowed.
			So any letter will be managed as an uiper ase letter. */
		virtual MCHEmul::Strings generateKeystrokeForToken (const std::string& t) const override;

		private:
		const std::map <char, MCHEmul::Strings> _SPECIALKEYS;
		// The default ones...
		static const std::map <char, MCHEmul::Strings> _DEFAULTSPECIALKEYS;
	};
}

#endif
  
// End of the file
/*@}*/
