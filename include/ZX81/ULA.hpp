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

namespace ZX81
{
	class SinclairZX81;
	class ULARegisters;

	/** 
		The chip that takes care of anything around the graphics in ZX81. \n
		It is also accountable for reading the status of the keyboard. \n
		@see GraphicalChip. \n
		\n
		How it works! (read carefully) \n
		Based on https://problemkaputt.de/zxdocs.htm#zx80zx81ioports (ZX80/ZX81 Video Display Times). \n
		and on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/ \n
		\n
		General concepts: \n
		The CPU/ULA speed is the same for either a PAL or a NTSC system. \n
		There is a pin in the ULA that = 1 (not connected) when NTSC and 0 (connected to ground) when PAL. \n
		CPU clock = 3,25MHz; 1 cycle CPU = 0,307692us. \n
		ULA clock = 6,5MHZ; 1 cyle ULA = 0,153846us. \n
		64us per raster line including Horizontal retrace = 416 cycles ULA (64/0,153846) or 208 cycles CPU (64/0,307692). \n
		Every ULA cycle a pixel is drawn. \n
		So 2 pixels are drawn in visible zone per CPU cycle. \n
		And 8 pixels are drawn in 4 CPU cycles (8 cycles ULA), that it is what a NOP execution takes. \n
		\n
		I register holds 0x1E from the beginning (important to determine where to read the cahr info). \n
		The char definition is from 0x1E00 onwards located. \n
		There is a 3 bits internal counter (0 - 7) in ULA (LNCTRL) than is moved any time HSYNC is detected.
		\n
		The program to draw the screen passes throught out 4 different blocks: \n
		1.- Block 1: VSYNC + frame count + keyboard read. \n
			NMI generator off. \n
			0 - 5 (6) ray trace lines (let's say out of the visible screen). \n
			(64us ray trace per line * 6) + 15us (retrace) = 399us ray trace spends in this zone. \n
			CPU in 400us = 1.300 (400/0,307692) instruction cycles. \n
		2.- Block 2: Top blank lines / Application code: \n
			NMI generator on. \n
			6 - 61 (56) ray trace lines (in PAL visible screen). \n
			At the end of every line, a NMI interrupt is generated to count the lines. \n
			Is the ULA the one generating the interrupt when detects HSYNC. \n
			The application code is executed but is is interrupted by the NMI. \n
			54 out of 56 (PAL) lines available for execution = 3456us = 11.232 instruction cycles. \n
			1 line waiting for the first HALT, and the other generating a collapsed DFILE. \n
		3.- Display zone: \n
			NMI generator off. \n
			INT type 1 on (executed always at 0x0038 position). \n
			62 - 253 (192 = 24 lines * 8) ray trace lines (in display). \n
			Register R is loaded with FC (bit 6 - 0 = 125). \n
			Program jumps to (DFILE) and start to "execute". \n
		\n
			CPU starts to read the opcode (whatever), M1 signal is marked as up. \n
			ULA also does it, and if bit 6 = false, put down all bits in data bus, and then CPU read NOP actually. \n
			But if bit 6 were not 0, ULA wouldn't do anything, and CPU then would read the normal code. \n
			It then should be 0x76 (HALT) that will stops the CPU. Otherwise a crash will be likely generated. \n
			The HALT instruction is unblocked when INT comes. \n
			From this point onwards ULA draws blank. \n
		\n
			The R register is incremented anytime a NOP or the HALT instruction is executed. \n
			At T4 of execution the IR value is moved into the address bus. \n
			The ULA overrides the address bus, using the IR info that is already in, to look for the char info (@see balow). \n
			The A6 is wired with INT (R counts from 0 to 127. A6 = 0). \n
			With A6 passes from 1 to 0 (negative edge) INT is launched. \n
			INT actualice the counter line (LNCTRL). \n
		4.- Bottom blank line: \n
			Idem that Top blank lines. \n
		\n
		How char info is read: \n
		1.- The Address of the char info is made up of: I register (A15 - A9) + Char code (A8 - A3) + LNCTRL. \n
		2.- The bit 7 of the Char code determine whether it is drawn in inverse mode. \n
	*/
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		friend SinclairZX81;

		static const unsigned int _ID = 210;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 414;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		/** To get a reference to the ULARegisters. */
		const ULARegisters* registers () const
							{ return (_ULARegisters); }
		ULARegisters* registers ()
							{ return (_ULARegisters); }

		// Size & position of the scren...
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2);

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** Simulates cycles in the VICI. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * Raster			= InfoStructure: Info about the raster.
		  * Sound			= InfoStructire: Info about the sound.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Different actions are taken attending the raster cycle. */
		void treatRasterCycle ();
		/** Read and draw the graphics. */
		void readGraphicsAndDrawVisibleZone (MCHEmul::CPU* cpu);

		protected:
		/** A reference to the ULA registers. */
		ULARegisters* _ULARegisters;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "_cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
	};

	// ---
	inline void ULA::screenPositions (unsigned short& x1, unsigned short& y1, 
		unsigned short& x2, unsigned short& y2)
	{
		_raster.displayPositions (x1, y1, x2, y2);
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
