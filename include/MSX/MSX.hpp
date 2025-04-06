/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: MSX.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: The global structure defining a MSX machine.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_MSX__
#define __MSX_MSX__

#include <CORE/incs.hpp>
#include <MSX/Model.hpp>
#include <MSX/Memory.hpp>

namespace MSX
{
	/** The computer. */
	class MSXComputer final : public MCHEmul::Computer
	{
		public:
		MSXComputer (MSXModel* m, unsigned int cfg,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		const MSXModel* model () const
							{ return (_model); }
		MSXModel* model ()
							{ return (_model); }

		// To get snapshots of the memory...
		// They are used in some commands...
		/** Content of the screen memory. \n
			The position of the screen memory in the MSX is fixed. It is within the graphical chip. */
		MCHEmul::UBytes screenMemorySnapShot () const
							{ return (_model -> vdp () -> screenMemorySnapShot ()); }
		/** Content of the color memory. \n
			The position of the color memory in the ZX Spectrum memory is fixed. */
		MCHEmul::UBytes colorMemorySnapShot () const
							{ return (_model -> vdp () -> colorMemorySnapShot ()); }

		virtual bool initialize (bool iM = true) override;

		private:
		/** To process the events related with the insertion or extraction of a cartridge. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Managing memory configuration...
		/** To get the configuration. */
		unsigned int configuration () const
							{ return (static_cast <const MSX::Memory*> (memory ()) -> configuration ()); }
		/** Adapt the MSX to the memory configuration. \n
			The parameter rs indicates whether to restart the computer. \n
			By default it is true. */
		void setConfiguration (unsigned int cfg, bool rs = true);

		private:
		/** Which computer is being emulated indeed. */
		MSXModel* _model;
	};
}

#endif
  
// End of the file
/*@}*/
