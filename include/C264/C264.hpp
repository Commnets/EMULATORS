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
			and even if it has been configured to be PAL or NTSC! ºn
			So the speed here is just a reference as an average. */
		static const unsigned int _PALCLOCK		= 889200; // 0.889 MHz
		static const unsigned int _NTSCCLOCK	= 896040; // 0.896 MHz

		Commodore264 (const MCHEmul::Chips& cps, MCHEmul::Memory* m, const MCHEmul::IODevices& dvs, 
			VisualSystem vS, const std::string& lng = MCHEmul::_DEFAULTLANGUAGE);

		virtual bool initialize (bool iM = true) override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Managing memory configuration...
		/** To get the configuration of the memory. */
		const MCHEmul::UByte& configuration () const
							{ return (static_cast <const Memory*> (memory ()) -> configuration ()); }
		/** Change the configuration of the memory. \n
			The parameter rs indicates whether to restart the computer. ºn
			By default it is true. */
		void setConfiguration (const MCHEmul::UByte& cfg, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS, VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		protected:
		/** The video system used by the C264 computer. */
		VisualSystem _visualSystem;
		/** The configuration mode of the computer. 
			In some version couldn't make sense. */
		MCHEmul::UByte _configuration;
	};

	/** The Commodore 16_116 a specific element of the series 264. \n
		It was released initially with only 16k althought it could be expanded.
		It is known as a simplified version of the Commodore Plus/4. */
	class Commodore16_116 final : public Commodore264
	{
		public:
		Commodore16_116 (VisualSystem vS, const std::string& lg = MCHEmul::_DEFAULTLANGUAGE)
			: Commodore264 (standardChips (lg, vS), new C264::C16_116Memory (lg), standardDevices (vS), vS, lg)
							{ }

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
		CommodorePlus4 (VisualSystem vS, const std::string& lg = MCHEmul::_DEFAULTLANGUAGE)
			: Commodore264 (standardChips (lg, vS), new C264::CPlus4Memory (lg), standardDevices (vS), vS, lg)
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS, VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);
	};
}

#endif
  
// End of the file
/*@}*/
