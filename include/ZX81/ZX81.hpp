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

namespace ZX81
{
	class ULA;

	// Port used in the ZX81...
	/**
	  * PORT FE: \n
	  * When reading: \n
	  *		bits 0 - 4	= keyboard row status. \n
	  *		bit 5		= not used. Always at true. \n
	  *		bit 6		= Display refresh rate. 0 = 60Hz, 1 = 50Hz. \n
	  *		bit 7		= Cassette input. 0 = normal. 1 = pulse. \n
	  * When writting: \n
	  *		Whatever value, enables NMI generator. \n
	  *		NMIs (Non maskable interrupts) are used during SLOW mode vertical blanking
	  *		periods to count the number of drawn blank scanlines.
	  * Listen events from the OSIO (@see OSIO)
	  */
	class PortFE final : public FZ80::Z80Port
	{
		public:
		friend ULA;

		static const int _ID = 0xfe;
		static const std::string _NAME;

		PortFE (bool ntsc)
			: FZ80::Z80Port (_ID, _NAME),
			  _NTSC (ntsc),
			  _keyboardStatus (8, MCHEmul::UByte::_0)
							{ initializeInternalValues (); }

		bool NTSC () const
							{ return (_NTSC); }

		virtual MCHEmul::UByte value () const override;
		virtual void setValue (const MCHEmul::UByte& v) override;

		virtual void initialize () override;

		private:
		/** Invoked from ULA. */
		void setKeyboardStatus (size_t r, size_t c, bool v)
							{ _keyboardStatus [r].setBit (c, v); }

		// Implemtation
		void initializeInternalValues ();

		private:
		bool _NTSC;

		// Implemenetation
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;
	};

	class SinclairZX81 final : public SINCLAIR::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed.
			The ZX80/ZX81 manages the video using the CPU, so this is the average speed,
			because the processor always work at 3.5 MHz. */
		static const unsigned int _CLOCK		= 3500000;	// 3.5 MHz
		static const unsigned int _PALCLOCK		= 804600;	// 0.804 MHz
		static const unsigned int _NTSCCLOCK	= 536400;	// 0.536 MHz

		SinclairZX81 (Memory::Configuration cfg,
			VisualSystem vS, unsigned char tc /** The configuration. */);

		virtual bool initialize (bool iM = true) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

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

		protected:
		/** The video system used by the commodore VIC20. */
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
