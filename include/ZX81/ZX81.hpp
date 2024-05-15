/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: ZX81.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: The global structure defining a ZX81 machine.
 *	Versions: 1.0 Initial
 *	Based on: https://problemkaputt.de/zxdocs.htm#zx80zx81iomap
 */

#ifndef __ZX81_ZX81__
#define __ZX81_ZX81__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>
#include <ZX81/Memory.hpp>
#include <ZX81/ULA.hpp>

namespace ZX81
{
	/** The computer. */
	class SinclairZX81 final : public SINCLAIR::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed.
			The ZX80/ZX81 manages the video using the CPU, so this is the average speed,
			because the processor always work at 3.25 MHz. */
		static const unsigned int _CLOCK		= 3250000;	// 3.25 MHz
		static const unsigned int _PALCLOCK		= 804600;	// 0.804 MHz
		static const unsigned int _NTSCCLOCK	= 536400;	// 0.536 MHz

		SinclairZX81 (Memory::Configuration cfg,
			VisualSystem vS, unsigned char tc /** The configuration. */);

		/** To get a reference to the ULA chip. */
		const ULA* ula () const
							{ return (_ula); }
		ULA* ula ()
							{ return (_ula); }

		virtual bool initialize (bool iM = true) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** In the ZX81 the A6 (address bus is connected with the INT. */
		virtual void specificComputerCycle () override;

		// Managing memory configuration...
		/** To get the configuration. */
		Memory::Configuration configuration () const
							{ return (static_cast <const ZX81::Memory*> (memory ()) -> configuration ()); }
		/** Adapt the ZX81 to the memory configuration. \n
			The parameter rs indicates whether to restart the computer. \n
			By default it is true. */
		void setConfiguration (Memory::Configuration cfg, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		private:
		/** The video system used by the commodore VIC20. */
		VisualSystem _visualSystem;

		// Implementation
		/** A reference to the ULA chip. */
		ULA* _ula;
		/** To control the status of the A6. */
		MCHEmul::Pulse _A6;
	};
}

#endif
  
// End of the file
/*@}*/
