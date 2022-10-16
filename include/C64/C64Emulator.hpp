/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: C64Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/04/2021 \n
 *	Description: The root class to define any C64 emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_C64EMULATOR__
#define __C64_C64EMULATOR__

#include <EMULATORS/Emulator.hpp>
#include <C64/global.hpp>
#include <C64/C64.hpp>
#include <C64/IOPBuilder.hpp>

namespace C64
{
	class C64Emulator : public MCHEmul::Emulator
	{
		public:
		/** The possible additional parameters of the C64 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const std::string _NTSC;

		/**
		  * Constructor:
		  * @param argc		: Número de parámetros.
		  * @param argv		: The parameters.\n
		  *	The basic parameters for any C64 emulator are (apart of the ones defined by the parent: \n
		  *	/n				: To indicate if the visualization system is NTSC. PAL by default.
		  */
		C64Emulator (const MCHEmul::Strings& argv);

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return ((_attributes.find (_NTSC) != _attributes.end ()) ? true : false); }

		/** To add the peripherals linked to the computer, according to the parameters. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new C64::Commodore64 (NTSCSystem () 
								? C64::Commodore64::VisualSystem::_NTSC : C64::Commodore64::VisualSystem::_PAL)); }

		virtual MCHEmul::IOPeripheralBuilder* createPeripheralBuilder () const override
							{ return (new C64::IOPeripheralBuilder); }

		/** Just to actualize the global time of the C64. */
		virtual bool additionalRunCycle () override
				{ C64::actualizeGlobalTime ();  return (true); }
	};
}

#endif
  
// End of the file
/*@}*/
