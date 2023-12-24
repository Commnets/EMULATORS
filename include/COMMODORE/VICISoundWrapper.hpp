/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICISoundWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 24/12/2023 \n
 *	Description: VICI Sound Wrapper Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VICISOUNDWRAPPER__
#define __COMMODORE_VICISOUNDWRAPPER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** The generic LibWrapper for VIC20. \n
		Nothing special to do. */
	class VICISoundLibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		VICISoundLibWrapper ()
			: MCHEmul::SoundLibWrapper (),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		virtual void setValue (size_t p, const MCHEmul::UByte& v)
							{ /** do nothing. */ }
		virtual const MCHEmul::UByte& readValue (size_t p) const
							{ return (_lastValueRead); }
		/** Like the previous one but not affecting the value. */
		virtual const MCHEmul::UByte& peekValue (size_t p) const
							{ return (readValue (p)); }

		virtual bool getData (MCHEmul::CPU*, MCHEmul::UBytes&) override;

		/** To get information about the voices from then wrapper. \n
			That infomation is not neccesary stored in the registers. */
		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const;

		protected:
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

}

#endif
  
// End of the file
/*@}*/
