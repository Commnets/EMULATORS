/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: PortManager.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: To manager all ports in a MSX.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_PORTMANAGER__
#define __MSX_PORTMANAGER__

#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>

namespace MSX
{
	/** Generic Port to manage all... */
	class PortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		PortManager ();

		// See that the constructor is private
		// because it can only be invoked from ULA

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, true)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, false)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override;

		virtual void initialize () override;

		private:
		/** ms = true when is is wanted to modify the internal status. */
		MCHEmul::UByte getValue (unsigned short ab, unsigned char id, bool ms = false) const;
	};
}

#endif
  
// End of the file
/*@}*/