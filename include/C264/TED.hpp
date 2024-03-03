/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: TED.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: The TED in the C264 series.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_TED__
#define __C264_TED__

#include <COMMODORE/incs.hpp>
#include <C264/C6529B1.hpp>
#include <C264/Memory.hpp>

namespace C264
{
	class TEDRegisters;

	/** The TED keyboard port is connected against: \n
		1.- The Keyboard matrix, which is affected with what C6529B1 chip latch (@see C6529B1 chip): \n
			When $fd30 position is written, the keyboard matrix changes and the outcome is put into 
			the TED keyboard port ready to be read once the $ff08 position (TED register number 0x08) 
			is finally written. \n
			So how to read the keyboard in C264 series?: \n
			lda $xx			\n
			sta $fd30		// The value of the keyboard matrix (of the row selected) \n
							// is moved into the keyboard port \n
			lda $ff			\n
			sta $ff08		// The TED latches the current value in the keyboard port. \n
			lds $ff08		// Read the value latched \n
		2.- The Joystick ports (1 and 2). 
			The pin number 5 (select) of the joysticks port are connected with 
			the D1 and D2 pins respectively of the data bus (entering the data pins of the TED). \n
			To detect a movement in the joystick the pin 5 must be down, 
			then the value of the joystick is moved to the keyboard port that
			then can be red to determine the movement. */
	class TED : public COMMODORE::TED
	{
		public:
		static const unsigned int _ROMACCESSCHANGED = 300;

		TED (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
			int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { })
			: COMMODORE::TED (vd, hd, vV, sW, attrs),
			  _TEDRegisters (nullptr)
							{ }

		virtual bool initialize () override;

		/** When there is changes in the configuration of the memory, 
			those are notify to the computer that is observing this chip. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		virtual bool ROMActiveToFetchCharData () const override // The memory is C264 type...
							{ return (static_cast <const C264::Memory*> (memoryRef ()) -> ROMActive ()); }
		virtual void activeROMtoFecthCharData (bool a) override //The memory is C264 type...
							{ static_cast <C264::Memory*> (memoryRef ()) -> activeROM (a); }

		private:
		TEDRegisters* _TEDRegisters;
	};

	/** The version para PAL systems. */
	class TED_PAL final : public TED
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TED_PAL (int vV, MCHEmul::SoundLibWrapper* wS);
	};

	/** The version para NTSC systems. */
	class TED_NTSC final : public TED
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		TED_NTSC (int vV, MCHEmul::SoundLibWrapper* wS);
	};
}

#endif
  
// End of the file
/*@}*/
