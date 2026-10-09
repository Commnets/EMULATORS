/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: PortManager.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 26/03/2024 \n
 *	Description: To manager all ports in ZX81.
 *	Versions: 1.0 Initial
 *	Based on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/
 */

#ifndef __ZX81_PORTMANAGER__
#define __ZX81_PORTMANAGER__

#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
{
	class ULARegisters;
	class ULA;

	// Generic Port to manage all...
	/** Shared ZX80/ZX81 port decoding. \n
	  * Even-port reads return keyboard columns on D0-D4, 1 on D5, PAL/NTSC selection
	  * on D6 (1 = 50 Hz) and EAR on D7. A8-A15 select keyboard rows, active low. \n
	  * An actual even-port read drives MIC low. With NMI disabled it also starts
	  * VSYNC if inactive, restarting presentation and the coarse horizontal phase,
	  * and blocking LINECNTRL at 0 on ZX80 or 7 on ZX81. PEEK preserves hardware
	  * state and event markers. Odd-port reads return the ULA pattern-bus value. \n
	  * Every output ends VSYNC and releases LINECNTRL without resetting its value.
	  * On ZX81, A1 = 0 disables NMI, then A0 = 0 enables it; enable wins when both
	  * conditions hold. ZX80 has no NMI generator. Odd-port outputs drive MIC high. \n
	  * The CPU releases prepared IN/OUT accesses at transaction boundaries, invoking
	  * value/setValue here. This is not exact intra-instruction I/O timing.
	  * Port-read debug timestamps still use the last ULA simulation clock.
	  */
	class PortManager final : public FZ80::Z80Port
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		PortManager (Type t);

		virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, true)); }
		virtual MCHEmul::UByte peekValue (unsigned short ab, unsigned char id) const override
							{ return (getValue (ab, id, false)); }
		virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override;

		/** To link to the different elements. */
		void linkToULA (ULA* ula);

		virtual void initialize () override;

		private:
		/** ms = true applies the side effects of an actual port read. \n
			ms = false inspects the value without changing hardware state or screen event markers. */
		MCHEmul::UByte getValue (unsigned short ab, unsigned char id, bool ms = false) const;

		private:
		Type _type;
		ULA* _ULA;
		ULARegisters* _ULARegisters;
	};
}

#endif
  
// End of the file
/*@}*/