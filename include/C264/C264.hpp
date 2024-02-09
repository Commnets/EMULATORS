/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C264.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/05/2023 \n
 *	Description: The global structure defining a C264 series machine.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C264__
#define __C264_C264__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C264/Memory.hpp>

namespace C264
{
	/** A generic class for all C264 computers. */
	class Commodore264 : public COMMODORE::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed. \n
			In the original series computer, 
			the speed if defined by the TED (Video/Sound & IO) Chip. \n
			The TED change speed depending on whether the raster is in a visible or nor visible zone of the screen.
			So the speed here is just a reference as an average. */
		static const unsigned int _PALCLOCK		= 886000;  // 0.886 MHz
		static const unsigned int _NTSCCLOCK	= 1100000; // 1.100 MHz

		Commodore264 (unsigned int cfg, 
			VisualSystem vS, const std::string& lg,
			const MCHEmul::Chips& cps, const MCHEmul::IODevices& dvs);

		virtual bool initialize (bool iM = true) override;

		// TODO

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Managing memory configuration...
		/** To get the configuration. */
		unsigned int configuration () const
							{ return (static_cast <const Memory*> (memory ()) -> configuration ()); }
		/** Adapt the C264 to the memory configuration. \n
			The parameter rs indicates whether to restart the computer. ºn
			By default it is true. */
		void setConfiguration (unsigned int cfg, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (Commodore264::VisualSystem vS, const std::string& sS);
		static MCHEmul::IODevices standardDevices (Commodore264::VisualSystem vS);

		protected:
		/** The video system used by the commodore 64. */
		VisualSystem _visualSystem;
	};

	/** The Commodore 16 a specific element of the series 264.
		It is known as a simplified version of the Commodore Plus/4. */
	class Commodore16 final : public Commodore264
	{
		public:
		Commodore16 (unsigned int cfg, 
			VisualSystem vS, const std::string& lg)
			: Commodore264 (cfg, vS, lg, standardChips (vS, lg), standardDevices (vS))
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (Commodore264::VisualSystem vS, const std::string& sS);
		static MCHEmul::IODevices standardDevices (Commodore264::VisualSystem vS);
	};

	/** The Commodore 116 a specific element of the series 264.
		It is know as a simplified version of the Commodore Plus/4 with the same look & feel than this. */
	class Commodore116 final : public Commodore264
	{
		public:
		Commodore116 (unsigned int cfg, 
			VisualSystem vS, const std::string& lg)
			: Commodore264 (cfg, vS, lg, standardChips (vS, lg), standardDevices (vS))
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (Commodore264::VisualSystem vS, const std::string& sS);
		static MCHEmul::IODevices standardDevices (Commodore264::VisualSystem vS);
	};

	/** The most advanced computer of the series,
		included even built in software. */
	class CommodorePlus4 final : public Commodore264
	{
		public:
		CommodorePlus4 (unsigned int cfg, 
			VisualSystem vS, const std::string& lg)
			: Commodore264 (cfg, vS, lg, standardChips (vS, lg), standardDevices (vS))
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (Commodore264::VisualSystem vS, const std::string& sS);
		static MCHEmul::IODevices standardDevices (Commodore264::VisualSystem vS);
	};
}

#endif
  
// End of the file
/*@}*/
