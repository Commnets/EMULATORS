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
#include <MSX/VDP.hpp>

namespace MSX
{
	/** The computer. */
	class MSXComputer final : public MCHEmul::Computer
	{
		public:
		MSXComputer (MSXModel* m, unsigned int cfg,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get the model. */
		const MSXModel* model () const
							{ return (_model); }
		MSXModel* model ()
							{ return (_model); }

		/** To get a refence to the vdp chip. */
		const VDP* vdp () const
							{ return (_model -> vdp ()); }
		VDP* vdp ()
							{ return (_model -> vdp ()); }

		// To get snapshots of the memory...
		// They are used in some commands...
		/** To get the content of the pattern name table. \n
			The information finally retrieved will depend on the active graphical mode. */
		std::vector <MCHEmul::UByte> patternNameTableSnapShot () const
							{ return (_model -> vdp () -> patternNameTableSnapShot ()); }
		/** To get the information about the pattern generation table. \n
			The information finally collected will depend on the active graphical mode . */
		std::vector <MCHEmul::UByte> patternGenerationTableSnapShot () const
							{ return (_model -> vdp () -> patternGenerationTableSnapShot ()); }
		/** To get the information about the color name table. \n
			The information finally collected will depend on the active graphical mode . \n
			In the TextMode the color are not used, and in the multicolor mode the table 
			defining the colort is the pattern generation one... */
		std::vector <MCHEmul::UByte> colorNameTableSnapShot () const
							{ return (_model -> vdp () -> colorNameTableSnapShot ()); }
		/** To get an snapshot of the sprite definition. 
			No boundaries limits are done. */
		MCHEmul::Strings spriteDrawSnapShot (size_t nS) const
							{ return (_model -> vdp () -> spriteDrawSnapShot (nS)); }
		/** To get a snapshot of a set of sprites.
			When parameter = empty means all sprites. */
		MCHEmul::Strings spritesDrawSnapShot (const std::vector <size_t>& nS = { }) const
							{ return (_model -> vdp () -> spritesDrawSnapShot (nS)); }

		virtual bool initialize (bool iM = true) override;

		private:
		/** To process the events related with the insertion or extraction of a cartridge. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		virtual void specificComputerCycle () override;

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
