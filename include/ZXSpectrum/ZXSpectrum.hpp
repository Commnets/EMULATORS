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
			Type t, const std::string& lang = "ENG");

		VisualSystem visualSystem () const
							{ return (_visualSystem); }

		Type type () const
							{ return (_type); }

		/** To get a reference to the ULA chip. */
		const ULA* ula () const
							{ return (_ula); }
		ULA* ula ()
							{ return (_ula); }

		virtual bool initialize (bool iM = true) override;

		private:
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
		/** To control the status of the A6. */
		MCHEmul::Pulse _A6;
	};
}

#endif
  
// End of the file
/*@}*/
