/** \ingroup EMULS */
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

#ifndef __MCHEMUL_C64EMULATOR__
#define __MCHEMUL_C64EMULATOR__

#include <EMULATORS/Emulator.hpp>
#include <C64/C64.hpp>

namespace Emuls
{
	class C64Emulator : public Emulator
	{
		public:
		/** The possible additional parameters of the C64 Emulator. */
		static const unsigned char _PARAMNTSC;
		static const std::string _NTSC;

		/**
		  * Constructor:
		  * @param argc		: Número de parámetros.
		  * @param argv		: The parameters.
		  * The basic parameters for any C64 emulator are (apart of th eones defined by the parent: \n
		  * /n				: To indicate if the visualization system is NTSC. PAL by default.
		  */
		C64Emulator (const std::vector <std::string>& argv);

		/** To know whether the visualizacion system is or not NTSC. */
		bool NTSCSystem () const
							{ return ((_attributes.find (_NTSC) != _attributes.end ()) ? true : false); }

		protected:
		virtual MCHEmul::Computer* createComputer () const override
							{ return (new C64::Commodore64 (NTSCSystem () 
								? C64::Commodore64::VisualSystem::_NTSC : C64::Commodore64::VisualSystem::_PAL)); }
	};
}

#endif
  
// End of the file
/*@}*/
