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
		/** Macros for the speed. \n
			In the original series computer, 
			the speed if defined by the TED (Video/Sound & IO) Chip. \n
			The TED change speed depending on whether the raster is in a visible or nor visible zone of the screen.
			and even if it has been configured to be PAL or NTSC! ºn
			So the speed here is just a reference as an average. */
		static const unsigned int _CLOCK = 886000;  // 0.886 MHz

		Commodore264 (unsigned int cfg, const std::string& lg,
			const MCHEmul::Chips& cps, const MCHEmul::IODevices& dvs);

		virtual bool initialize (bool iM = true) override;

		// TODO

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Managing memory configuration...
		/** To get the type of machine. */
		unsigned int machineType () const
							{ return (static_cast <const Memory*> (memory ()) -> machineType ()); }
		/** Change the type of machine. \n
			The parameter rs indicates whether to restart the computer. ºn
			By default it is true. */
		void setMachineType (unsigned int mT, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS);
		static MCHEmul::IODevices standardDevices ();
	};

	/** The Commodore 16_116 a specific element of the series 264. \n
		It was released initially with only 16k althought it could be expanded.
		It is known as a simplified version of the Commodore Plus/4. */
	class Commodore16_116 final : public Commodore264
	{
		public:
		Commodore16_116 (const std::string& lg)
			: Commodore264 (0, lg, standardChips (lg), standardDevices ())
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS);
		static MCHEmul::IODevices standardDevices ();
	};

	/** Initially named C264. \n
		Includes built in software: A word processor, a spreadsheet and a database. */
	class CommodorePlus4 final : public Commodore264
	{
		public:
		CommodorePlus4 (const std::string& lg)
			: Commodore264 (1, lg, standardChips (lg), standardDevices ())
							{ }

		protected:
		// Implementation
		static MCHEmul::Chips standardChips (const std::string& sS);
		static MCHEmul::IODevices standardDevices ();
	};
}

#endif
  
// End of the file
/*@}*/
