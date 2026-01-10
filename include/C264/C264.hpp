/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C264.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/05/2023 \n
 *	Description: The global structure defining a C264 series machine.
 *				 Explanation of the history: https://www.zimmers.net/cbmpics/c264s.html
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C264__
#define __C264_C264__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C264/Type.hpp>
#include <C264/Memory.hpp>

namespace C264
{
	/** A generic class for all C264 computers. \n
		This includes: \n
		C16-116 that at the end are the same machine (being 116 the cheapest version of C16),
		and C264 plater named as Plus/4. The intended V364 was never released. */
	class Commodore264 : public COMMODORE::Computer
	{
		public:
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed. \n
			In the original series computer, 
			the speed if defined by the TED (Video/Sound & IO) Chip. \n
			The TED change speed depending on whether the raster is in a visible or nor visible zone of the screen.
			and even if it has been configured to be PAL or NTSC! \n
			So the speed here is just a reference as an average. */
		static const unsigned int _PALCLOCK		= 886723; // 0.886 MHz = TED_PAL::_CLOCK / 20
		static const unsigned int _NTSCCLOCK	= 894886; // 0.894 MHz = TED_NTSC::_CLOCK / 16

		Commodore264 (const MCHEmul::Chips& cps, MCHEmul::Memory* m, const MCHEmul::IODevices& dvs, 
			VisualSystem vS, Type t, unsigned int cfg, const std::string& lng = MCHEmul::_DEFAULTLANGUAGE);

		VisualSystem visualSystem () const
							{ return (_visualSystem); }

		virtual bool initialize (bool iM = true) override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** To change the speed of the CPU. */
		virtual void specificComputerCycle () override;

		// Managing memory configuration...
		/** To get the configuration of the memory. */
		unsigned int configuration () const
							{ return (static_cast <const Memory*> (memory ()) -> configuration ()); }
		/** Change the configuration of the memory. \n
			The parameter rs indicates whether to restart the computer. ºn
			By default it is true. */
		void setConfiguration (unsigned int cfg, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS, VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		protected:
		/** The video system used by the C264 computer. */
		VisualSystem _visualSystem;
		/** The configuration mode of the computer. 
			In some version couldn't make sense. */
		unsigned int _configuration;

		// Implementation
		/** A reference to the TED chip. */
		TED* _ted;
		/** Two additional clocks are used, 
			the active one will depend on the situation of the processor. */
		MCHEmul::Clock _slowClock, _fastClock;
	};

	/** The Commodore 16_116 a specific element of the series 264. \n
		It was released initially with only 16k althought it could be expanded.
		It is known as a simplified version of the Commodore Plus/4. */
	class Commodore16_116 final : public Commodore264
	{
		public:
		Commodore16_116 (VisualSystem vS, 
				unsigned int cfg, const std::string& lg = MCHEmul::_DEFAULTLANGUAGE)
			: Commodore264 (standardChips (lg, vS), 
				new C264::C16_116Memory (cfg, lg), standardDevices (vS), vS, Type::_C16, cfg, lg)
							{ }

		virtual bool initialize (bool iM = true) override;

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS, VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);
	};

	/** Initially named C264. \n
		Includes built in software: A word processor, a spreadsheet and a database. */
	class CommodorePlus4 final : public Commodore264
	{
		public:
		CommodorePlus4 (VisualSystem vS, 
				unsigned int cfg, const std::string& lg = MCHEmul::_DEFAULTLANGUAGE)
			: Commodore264 (standardChips (lg, vS), 
				new C264::CPlus4Memory (cfg, lg), standardDevices (vS), vS, Type::_CPLUS4, cfg, lg)
							{ }

		virtual bool initialize (bool iM = true) override;

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS, VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);
	};
}

#endif
  
// End of the file
/*@}*/
