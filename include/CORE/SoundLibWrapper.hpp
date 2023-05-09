/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundLibWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 24/03/2023 \n
 *	Description: Wrapper over a Sound Library that can be managed from the Emulators.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SOUNDLIBWRAPPER__
#define __MCHEMUL_SOUNDLIBWRAPPER__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class CPU;

	/** To create the "sound" many external tools could be even used.
		So a general class to create the next frame of sound is defined. */
	class SoundLibWrapper : public InfoClass
	{
		public:
		SoundLibWrapper (const Attributes& attrs = { })
			: InfoClass	("SoundLibWrapper"),
			  _attributes (attrs)
							{ }

		virtual ~SoundLibWrapper ()
							{ }

		/** The warpper can accept changes receiving a list of parameters. */
		virtual void setParameters (const Attributes& attrs)
							{ _attributes = attrs; }
		const Attributes& parameters () const
							{ return (_attributes); }

		virtual void initialize ()
							{ }

		/** To get the next data considering the status of the CPU. \n
			The method returns true when there is data "available" and false if it is not. \n
			In case te data is available the parameter "UBytes" will have been filled up with the value. */
		virtual bool getData (CPU*, UBytes&) = 0;

		virtual InfoStructure getInfoStructure () const override;

		protected:
		Attributes _attributes;
	};
}

#endif
  
// End of the file
/*@}*/

