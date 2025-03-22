/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ZX81.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The global structure defining a ZXSPECTRUM machine.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_ZXSPECTRUM__
#define __ZXSPECTRUM_ZXSPECTRUM__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/Type.hpp>
#include <ZXSpectrum/Memory.hpp>

namespace ZXSPECTRUM
{
	class ULA;

	/** The computer. */
	class SinclairZXSpectrum final : public SINCLAIR::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		// The speed of the processor...
		static const unsigned int _CLOCK		= 3500000;	// 3.5 MHz

		SinclairZXSpectrum (Memory::Configuration cfg, VisualSystem vS, 
			Type t, const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		VisualSystem visualSystem () const
							{ return (_visualSystem); }

		Type type () const
							{ return (_type); }

		// To get snapshots of the memory...
		// They are used in some commands...
		/** Content of the screen memory. \n
			The position of the screen memory in the ZX Spectrum memory is fixed. */
		MCHEmul::UBytes screenMemorySnapShot () const
							{ return (memory () -> values (MCHEmul::Address ({ 0x00, 0x40 }, false), (size_t) 0x1800)); }
		/** Content of the color memory. \n
			The position of the color memory in the ZX Spectrum memory is fixed. */
		MCHEmul::UBytes colorMemorySnapShot () const
							{ return (memory () -> values (MCHEmul::Address ({ 0x00, 0x58 }, false), (size_t) 0x0300)); }
		/** To get the characters. \n
			The position of the chars in the ZX Spectrum memory is fixed. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

		/** To get a reference to the ULA chip. */
		const ULA* ula () const
							{ return (_ula); }
		ULA* ula ()
							{ return (_ula); }

		virtual bool initialize (bool iM = true) override;

		private:
		/** To identify whether the CPU tries to access the 16k of then screen memory
			to then activate the ULA contention access if needed. */
		virtual void specificComputerCycle () override;

		/** To process the events related with the insertion or extraction of a cartridge. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Managing memory configuration...
		/** To get the configuration. */
		Memory::Configuration configuration () const
							{ return (static_cast <const ZXSPECTRUM::Memory*> (memory ()) -> configuration ()); }
		/** Adapt the ZXSpectrum to the memory configuration. \n
			The parameter rs indicates whether to restart the computer. \n
			By default it is true. */
		void setConfiguration (Memory::Configuration cfg, bool rs = true);

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
	};
}

#endif
  
// End of the file
/*@}*/
