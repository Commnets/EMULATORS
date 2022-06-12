/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6510.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The main CPU of the C64.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6510__
#define __F6500_C6510__

#include <F6500/C6500.hpp>

namespace F6500
{
	/** The Chip CPU 6510 */
	class C6510 : public C6500
	{
		public:
		C6510 ()
			: C6500 (createArchitecture ())
							{ }

		virtual MCHEmul::Address IRQVectorAddress () const override
							{ return (MCHEmul::Address ({ 0xfe, 0xff }, false /** Little - endian */)); }
		virtual MCHEmul::Address NMIVectorAddress () const override
							{ return (MCHEmul::Address ({ 0xfa, 0xff }, false /** Little - endian */)); }
		virtual MCHEmul::Address ResetVectorAddress () const override
							{ return (MCHEmul::Address ({ 0xfc, 0xff }, false /** Little - endian */)); }

		private:
		// Implementation
		static MCHEmul::CPUArchitecture createArchitecture ();
	};
}

#endif
  
// End of the file
/*@}*/
