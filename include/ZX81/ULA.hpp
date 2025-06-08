/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: ULA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: The "ULA" Chip.
 *				 This chip is used to produce the signal video and to read the keyboard.
 *	Versions: 1.0 Initial
 *	Based on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/
 */

#ifndef __ZX81_ULA__
#define __ZX81_ULA__

#include <CORE/incs.hpp>
#include <ZX81/ULARegisters.hpp>

namespace ZX81
{
	class MemoryVideoCode;

	/** 
		The chip that takes care of anything around the graphics in ZX81. \n
		It is also accountable for reading the status of the keyboard. \n
		@see GraphicalChip. \n
		\n
		How it works! (read carefully) \n
		Based on: https://problemkaputt.de/zxdocs.htm#zx80zx81ioports (ZX80/ZX81 Video Display Times). \n
		and on: https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/ \n
		and on: http://blog.tynemouthsoftware.co.uk/2023/10/how-the-zx80-generates-video.html \n
		and on: http://blog.tynemouthsoftware.co.uk/2023/10/how-the-zx81-generates-video.html \n
		\n
		General concepts: \n
		The CPU/ULA speed is the same for either a PAL or a NTSC system. \n
		There is a pin in the ULA that = 1 (not connected) when NTSC and 0 (connected to ground) when PAL. \n
		CPU clock = 3,25MHz; 1 cycle CPU = 0,307692us. \n
		ULA clock = 6,5MHZ; 1 cyle ULA = 0,153846us. \n
		64us per raster line including Horizontal retrace = 416 cycles ULA (64/0,153846) or 208 cycles CPU (64/0,307692). \n
		Every ULA cycle a pixel is drawn. \n
		So 2 pixels are drawn in visible zone per CPU cycle. \n
		And 8 pixels are drawn in 4 CPU cycles (8 cycles ULA), that it is what a NOP/HALT execution takes. \n
		\n
		Phases:
		1.- VSYC: \n
		Increments the frame counter. \n
		Reads the keyboard. \n
		First time the keyboard is read (IN FE,A), the VSYNC signal is sent to the TV. \n
		Until VSYNC finishes no other intruction to read the keyboard will launch VSYNC again. \n
		This instruction also put LNCTRL = 0, clamps the video output signal to 0 (drawing always in white), and
		put the NMI internal ULA generator = false (No NMI generation). \n
		The internal counter for horizontal cycles (ICFHC) is working (counting from 0 to 207 including = 208). \n
		... \n
		Executes OUT FF,A \n
		release LNCTRL reset. \n
		put video output signal to normal. \n
		NMI internal ULA generator = true (NMI generation after internal line counter = 208). \n
		ICFHC still works...
		A' will have the base number of blank lines...
		\n
		2.- TOP Blank lines / App Code: \n
		When ICFHC = 208 a HSYNC pulse is generated and sent to the TV. \n
		Also a NMI interrupt is launched and the application code is then executed.
		The NMI increments a blank line counter (A').
		When the A' counter = 0, starts the VIDEO DIPLAY routine, and stops the ULA internal NMI generator = false.
		\n
		3.- VIDEO DISPLAY:
		The register I = 1e (A9 - A12 of the address of char data fixed).
		The register R = e0...see why later!
		INT interrupts are allowed (type 1).
		BC to count down 192 lines (in two blocks B from 24 to 0 = 25 lines, and C from 8 to 1 = 8 per line).
		Z80 jumps to C0D2 location.
		How it works: \n
		\n
		1 loop:
		CPU read the opcode (whatever), M1 signal is marked as up. \n
		ULA also does it, and if bit 6 of the opcode = false, put down all bits in data bus, and then CPU read NOP actually. \n
		But, if bit 6 were not 0, ULA wouldn't do anything, and CPU then would read the normal code. \n
		the NOP instruction takes 4 cycles. \n
		In cycles 3 CPU put IR register on the address bus. \n
		In cycle 4 the ULA forms the address to look for char info = I + OpCode (6 bits) + LNCTRL. \n
		Bit 7 of the Opcode is to determine whether it will draw as it is or in reverse mode. \n
		Put the value in the address bus and read the char info. \n
		The value read is put into the SHIFT REGISTER. \n
		Register R is incremented (after executing any instruction, and NOP is one of them). \n
		Move to following OpCode. \n
		2 loop: \n
		The value is the SHIFT REGISTER is out in 4 CPU cycles, (ULA cycles at double speed remember!). \n
		Repeat what was told in loop 1. \n
		So chars are drawn from loop 2 to 33 (32 at a maximum). \n
		... \n
		34 loop (or maybe before). \n
		HALT is found. \n
		The SHIFT REGISTER is put to 0, and the video output signal is clapped = 0,
		R register increments and reaches 82. At this point A6 = 0 and INT is launched. \n
		INT decrements C and aldo B every 8. \n
		When INT is launched a HSYNC is also launched, video output signal is then free. \n
		When B reaches 0 the ULA NMI internal generator = true, and jumps to do the same than TOP blank lines, 
		but at the BOTTOM. \n
		... \n
		4.- BOTTOM Blank lines / App Code: \n
		Same than above... \n
		When finishes swiches back NMI = false and back to TOP. \n
	*/
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		friend MemoryVideoCode;

		static const unsigned int _ID = 210;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		/** To get a reference to the ULARegisters. */
		const ULARegisters* registers () const
							{ return (_ULARegisters); }
		ULARegisters* registers ()
							{ return (_ULARegisters); }

		// Size & position of the screen...
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2);

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }
		MCHEmul::Raster& raster ()
							{ return (_raster); }

		/** To activate or desactivate the visualization of events. */
		void setShowEvents (bool sE)
							{ _showEvents = sE; }

		/** To inform about the INT ack. \n
			see @ZX81::SinclairZX81 class for more details. 
			The method received the clock situation (CPU point of view) when happened. */
		void setINTack (unsigned int c)
							{ _ULARegisters -> setINTack (c); }
		inline bool aboutToGenerateNMIAfterCycles (unsigned int nC);

		virtual bool initialize () override;

		/** Simulates cycles in the ULA. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * ULARegisters	= InfoStructire: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// Port events...
		/** To reflect the event related with reading/writting in a port,
			related with the management of the display: That's it :
			writting in anyone and reading from anyone ending in 0, usually $FE. */
		void markWritePortAction ()
							{ _writePort = true; }
		void markReadPortFEAction ()
							{ _readPortFE = true; }
		/** Specifically events related with the NMI generator. */
		void markNMIGeneratorOn ()
							{ _NMIGeneratorOn = true; }
		void markNMIGeneratorOff ()
							{ _NMIGeneratorOff = true; }

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Draw in the visible zone if there were something to be drawn. \n
			This method is executed in every ULA clock. The ULA shifts left the SHIFT register 
			until there were nothing. The bit shifted is drawn taking into account 
			whether the code read (from the video memory) had the bit 7 set. */
		void drawInVisibleZone (MCHEmul::CPU* cpu);

		// Invoked from memory ZX81::MemoryVideoCode::readCharData!
		/** To load data into the SHIFT Registers after readind char. 
			This method is invoked from the memory. \n
			It takes into account the LNCTRL, the character read and the value of the IR register.
			This is the way the ULA builds up where the info to load is. */
		bool readCharData (MCHEmul::CPU* cpu, const MCHEmul::UByte& dt);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugULACycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** A reference to the ULA registers. */
		ULARegisters* _ULARegisters;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To show or no the main events that affects the visualization. */
		bool _showEvents;

		// Implementation
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;

		// To draw situations...
		bool _HALTBefore; // Just to identify the first HALT of many!
		MCHEmul::OBool _INTActive, _NMIActive, _HALTActive;
		MCHEmul::OBool _writePort, _readPortFE, _NMIGeneratorOn, _NMIGeneratorOff;
	};

	// ---
	inline void ULA::screenPositions (unsigned short& x1, unsigned short& y1, 
		unsigned short& x2, unsigned short& y2)
	{
		_raster.displayPositions (x1, y1, x2, y2);
	}

	// ---
	inline bool ULA::aboutToGenerateNMIAfterCycles (unsigned int nC)
	{
		bool rP;
		return (_raster.simulateMoveCycles (nC << 1 /** double cycles in ULA. */, rP) &&
			_ULARegisters -> NMIGenerator ()); 
		// It means that in the next execution for nC cycles, 
		// a NMI interrupt will be generated...
	}

	/** The version para PAL systems. */
	class ULA_PAL final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_PAL (int vV);
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_NTSC (int vV);
	};
}

#endif
  
// End of the file
/*@}*/
