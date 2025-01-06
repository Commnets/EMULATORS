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
#include <ZX81/Type.hpp>
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

		SinclairZX81 (Memory::Configuration cfg, VisualSystem vS, Type t);

		VisualSystem visualSystem () const
							{ return (_visualSystem); }

		Type type () const
							{ return (_type); }

		// To access the different locations of the memory...
		// The locations depends in some ocassions on the type of computer...
		/** Where the display file starting point is stored. */
		inline MCHEmul::Address D_FILEAddress () const;
		/** The location of the display file. */
		MCHEmul::Address D_FILE () const
							{ return (MCHEmul::Address (memory () -> values (D_FILEAddress (), 2), false)); }
		/** The location of the end of the display.
			In the ZX80 there is a specific variable with that. 
			In the ZX81 there isn't and it lasts until the vars space starts. */
		inline MCHEmul::Address DF_END () const;
		/** Where the variables of the system starting point is stored. */
		inline MCHEmul::Address VARSAddress () const;
		/** The location of the variables of the system */
		MCHEmul::Address VARS () const
							{ return (MCHEmul::Address (memory () -> values (VARSAddress (), 2), false)); }
		/** The location of the chars definition. */
		inline MCHEmul::Address CHARS () const;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** Content of the display file. \n
			The location of the display file varies depending on the program 
			and also depending on the computer being emulated. \n
			Its initial position is located under the variable D_FILE. \n
			In the memory structure of the ZX81 like machines the VARS space follows the D_FILE,
			And where the VARS starts is located in the VARS variable.
			D_FILE and VARS variables location itself varies depending on the version of the computer emulated. 
			We have simulated this under the ULA because it is more or less "graphic", but it could be simple at memory. */
		inline MCHEmul::UBytes displayFileSnapShot () const;
		/** To get the characters. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

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
		void setConfiguration (Memory::Configuration cfg, Type t, bool rs = true);

		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		private:
		/** The video system used by ZX81 emulation. */
		VisualSystem _visualSystem;
		/** Which computer is being emulated indeed. */
		Type _type;

		// Implementation
		/** A reference to the ULA chip. */
		ULA* _ula;
		/** To control the status of the A6. */
		MCHEmul::Pulse _A6;
	};

	// ---
	inline MCHEmul::Address SinclairZX81::D_FILEAddress () const
	{ 
		return (MCHEmul::Address ({ 0x0c, 0x40 }, false)); 
	}

	// ---
	inline MCHEmul::Address SinclairZX81::DF_END () const
	{
		return ((_type == Type::_ZX80)
			? MCHEmul::Address (memory () -> values (MCHEmul::Address 
				({ 0x10, 0x40 } /** DF_ENDAddress. */, false), 2), false)
			: VARS ());
	}

	// ---
	inline MCHEmul::Address SinclairZX81::VARSAddress () const
	{ 
		return ((_type == Type::_ZX80)
			? MCHEmul::Address ({ 0x08, 0x40 }, false)
			: MCHEmul::Address ({ 0x10, 0x40 }, false)); 
	}

	// ---
	inline MCHEmul::Address SinclairZX81::CHARS () const
	{
		return ((_type == Type::_ZX80)
			? MCHEmul::Address ({ 0x00, 0x0e }, false)
			: MCHEmul::Address (MCHEmul::Address ({ 0x00, 0x1e }, false)));
	}

	// ---
	inline MCHEmul::UBytes SinclairZX81::displayFileSnapShot () const
	{ 
		MCHEmul::Address dFI = D_FILE ();
		MCHEmul::Address dFE = DF_END ();

		assert (dFE.value () >= dFI.value ());

		return (memory () -> values (dFI, (size_t) (dFE.value () - dFI.value ())));
	}
}

#endif
  
// End of the file
/*@}*/
