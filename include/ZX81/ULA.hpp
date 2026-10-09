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
#include <ZX81/Type.hpp>
#include <ZX81/ULARegisters.hpp>

namespace ZX81
{
	class MemoryVideoCode;
	class PortManager;

	/** Video generation shared by ZX81 and the discrete ZX80 video circuitry. \n
		CPU clock is 3.25 MHz; the ULA shifts two pixels per CPU T-state. \n
		The ROM schedules display-file execution and blank intervals; the ULA does not
		render a framebuffer directly from D_FILE. \n
		\n
		A qualifying opcode fetch with A15 high and data bit 6 clear captures the byte
		and returns NOP. Operand, data and inspection reads preserve the byte;
		HALT cycles do not capture characters. \n
		Each capture retains I:R before this M1 increments R. Pattern loading follows
		8 pixels later on ZX80 and 7 on ZX81; the previous pattern survives until then. \n
		With refresh-capable 16K RAM, the captured I:R selects the pattern when it
		addresses the expansion or its ULA-view mirror. Otherwise the character path
		combines I bits 7-1, character bits 5-0 and the current LINECNTRL value. \n
		An accepted load also installs character bit 7 as inverse-video polarity. \n
		\n
		ZX81 has a horizontal generator independent of presentation: its compatibility
		baseline is 414 ULA clocks (207 T), with HSYNC active at counts 32 through 63. \n
		HSYNC start advances LINECNTRL unless blocked and requests NMI if enabled.
		Terminal wrap alone creates no HSYNC edge. An accepted CPU INT response
		schedules a counter reset two T later, distinct from subsequent HSYNC. \n
		ZX80 retains a coarse logical-line model driven by accepted INT or the
		presentation-tail position, with duplicate suppression and no NMI generator. \n
		\n
		The presentation raster determines window coordinates and cropping. INT response
		aligns it to the trailing interval; horizontal wrap advances the presentation
		row. On ZX81 these actions are separate from hardware-model HSYNC. \n
		An actual even-port read starts VSYNC when NMI generation is disabled and VSYNC
		is inactive. Entry restarts presentation and the coarse horizontal phase,
		blocking LINECNTRL at 0 on ZX80 or 7 on ZX81. Any output ends VSYNC and releases
		that block. Port decoding controls NMI separately; see PortManager. \n
		\n
		CPU port accesses are released at transaction boundaries; chip execution can
		be batched. Prefix captures use nominal M1 offsets; per-cycle delivery
		can still occur after their attributed times. HALT/NMI compensation
		extends the CPU response, without pin-level WAIT or ordinary-instruction WAIT. \n
		In PerCycle mode, prefixed captures can arrive after their scheduled load;
		the ULA does not replay previously simulated pixels or state transitions. \n
		Keyboard and joystick input is stored in ULARegisters for PortManager;
		screen event markers are diagnostic overlays. Hardware references and the
		implementation limits are documented in the ZX80/ZX81 video audit skills. \n
		\n
		The structure of a line is the following: \n
		-----------------------------------------------------------------------------\n
		|Zone				|Time aprox	|ULA Cicles (at 6.5?MHz, double than CPU)	|\n
		|------------------	|-----------|-------------------------------------------|\n
		|**HSYNC**			|\~4.7?µs	|4.7?µs × 6.5?MHz ? **30.6 ? 31 cycles**		|\n
		|**Back Porch**		|\~5.7?µs	|5.7?µs × 6.5?MHz ? **37.1 ? 36 cycles**		|\n
		|**Video Active**	|\~52?µs		|52?µs × 6.5?MHz ? **338 cycles**				|\n
		|**Front Porch**	|\~1.5?µs	|1.5?µs × 6.5?MHz ? **9.75 ? 9 cycles**		|\n
		|------------------	|-----------|-------------------------------------------|\n
		|**TOTAL**			|\~63.9?µs	|**414 cycles** (rounded)					|\n
		-----------------------------------------------------------------------------\n 
		Standard PAL requires 312 lines to draw a frame. \n
		The structure of the frame is the following: \n
		-----------------------------------------------------------------\n 
		|Zone							|Lines aprox.	|Time aprox.	|\n
		|-------------------------------|--------------	|-------------	|\n
		|**Vertical sync pulse** (VSYNC)|\~5 lines		|\~320?µs		|\n
		|**Vertical back porch**		|\~25 lines		|\~1.6?ms		|\n
		|**Zona de imagen activa**		|\~284 lines	|\~18.2?ms		|\n
		|**Vertical front porch**		|\~3 lines		|\~192?µs		|\n
		|-------------------------------|--------------	|-------------	|\n
		|**TOTAL**						|**312 líneas**	|**20?ms**		|\n
		-----------------------------------------------------------------\n 
		\n
	*/
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		friend MemoryVideoCode;
		friend PortManager;

		static const unsigned int _ID = 210;

		/** Receives vertical/horizontal presentation geometry, machine type, ULA memory
			view and attributes. PAL/NTSC subclasses supply the default geometry. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, Type t, 
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
		void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2)
							{ _raster.displayPositions (x1, y1, x2, y2); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }
		MCHEmul::Raster& raster ()
							{ return (_raster); }

		/** To activate or desactivate the visualization of events. */
		void setShowEvents (bool sE)
							{ _showEvents = sE; }

		/** Records CPU INT response start; ZX81 applies the bus delay in simulate. */
		void setINTack (unsigned int c)
							{ _ULARegisters -> setINTack (c); }
		/** Predicts with NMI enable unchanged and no new CPU/port events. */
		bool aboutToGenerateNMIAfterCycles (unsigned int nC);
		/** Additional response cycles for the coarse HALT/NMI model. \n
			The caller identifies a NMI from this ULA accepted while HALTed. */
		unsigned int haltNMIWaitCycles
			(unsigned int requestClock, unsigned int acceptanceClock) const;

		virtual bool initialize () override;

		/** Simulates cycles in the ULA. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * ULARegisters	= InfoStructire: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		/** Pattern-bus value exposed to odd-port reads; see ULARegisters::lastVRAMByteRead */
		const MCHEmul::UByte& lastVRAMByteRead () const
							{ return (_ULARegisters -> lastVRAMByteRead ()); }

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
		/** A display byte awaiting its pattern-load phase. */
		struct PendingCharacter
		{
			unsigned int _captureClock;
			unsigned int _loadClock;
			unsigned char _loadPhase;
			unsigned char _i;
			/** CPU refresh address for this M1, before its R increment. */
			unsigned short _refreshAddress;
			MCHEmul::UByte _code;
		};

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Draw in the visible zone if there were something to be drawn. \n
			This method is executed in every ULA clock. The ULA shifts left the SHIFT register 
			until there were nothing. The bit shifted is drawn taking into account 
			whether the code read (from the video memory) had the bit 7 set. \n
			Returns true when the raster was in the visible zona and false in other circunstance. */
		bool drawInVisibleZone (MCHEmul::CPU* cpu);

		/** Captures a character without changing the active shift register. */
		void captureCharData (MCHEmul::CPU* cpu, const MCHEmul::UByte& dt);
		/** Loads pending patterns before shifting the corresponding pixel. */
		void loadPendingCharData (unsigned int c, unsigned char p);

		/** Restarts presentation coordinates and the coarse horizontal-generator phase. */
		void restartRaster ();

		private:
		/** Processes the bus acknowledge before character loading at this pixel. */
		void processHorizontalSync (MCHEmul::CPU* cpu, unsigned int c, unsigned char p);
		/** Advances the generator by one ULA clock, independently of presentation. \n
			Terminal count only wraps the generator; HSYNC begins at its own phase.*/
		void advanceHorizontalCounter ()
							{ if (++_horizontalCounter == _HORIZONTALPERIOD) 
								_horizontalCounter = 0; }

		/** Restores the initial correspondence with presentation coordinates. */
		void initializeHorizontalTiming ();

		//-----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugULACycle (MCHEmul::CPU* cpu, unsigned int i);
		/** Logs the returned port value and the state after the read. \n
			The timestamp is the last CPU cycle processed by the ULA. \n
			It is not the exact I/O sampling cycle. */
		void debugPortRead (unsigned short ab, unsigned char id,
			const MCHEmul::UByte& v, bool ms) const;
		/** Logs the logical fetch time and the CPU clock when it was reported. */
		void debugCharCapture
			(const PendingCharacter& ch, unsigned int observedClock) const;
		/** Records the actual pattern load, including late or rejected loads. */
		void debugCharLoad (const PendingCharacter& ch,
			unsigned int c, unsigned char p, const MCHEmul::Address& a,
			const MCHEmul::UByte& pattern, bool ramRefresh, bool after, bool accepted) const;
		/** Records logical line synchronization independently of raster wrap. */
		void debugLineSync (unsigned int c, unsigned char p,
			bool external, bool internal, bool wasActive, bool applied,
			unsigned short hB, unsigned char lB) const;
		/** Records horizontal wrap and the resulting vertical advance. */
		void debugLineAdvance (unsigned int c, unsigned char p,
			unsigned short hB, unsigned short vB) const;
		/** Records generator events separately from presentation row advances. */
		void debugHorizontalTiming (unsigned int c, unsigned char p,
			const char* event, const char* cause, unsigned short before,
			unsigned char lB, bool nmiRequested) const;
		//-----

		protected:
		/** A reference to the ULA registers. */
		ULARegisters* _ULARegisters;
		/** The type of model. */
		Type _type;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To show or no the main events that affects the visualization. */
		bool _showEvents;

		// Character capture and delayed pattern load.
		/** Nominal delay from the character-capturing M1 to pattern load, in pixels. */
		const unsigned char _charLoadDelayPixels;
		std::vector <PendingCharacter> _pendingCharacters;
		size_t _nextPendingCharacter;

		// Presentation alignment; only ZX80 also uses this as logical sync.
		const unsigned short _lineSyncPosition;
		bool _lineSyncActive;

		// ZX81 timing baseline: 207 T period and HSYNC at counts 16 through 31.
		// These values are independent of character-load latency and raster geometry.
		static const unsigned short _HORIZONTALPERIOD	= 414;
		static const unsigned short _HSYNCSTART			= 32;
		static const unsigned short _HSYNCEND			= 64;
		static const unsigned int _INTACKDELAY			= 2;

		unsigned short _horizontalCounter;
		bool _hSyncActive;
		bool _horizontalResetPending;
		unsigned int _horizontalResetClock;

		// Implementation
		bool _simulationStarted;
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;

		// To draw situations...
		bool _HALTBefore; // Just to identify the first HALT of many!
		MCHEmul::OBool _INTActive, _NMIActive, _HALTActive;
		MCHEmul::OBool _LINECNTRLTo0;
		unsigned char _LINECNTRLTo0Draw; // Alternative (when 0) to draw the LNCTRL = 1 situation...
		MCHEmul::OBool _writePort, _readPortFE, _NMIGeneratorOn, _NMIGeneratorOff;
	};

	/** The version para PAL systems. */
	class ULA_PAL final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_PAL (Type t, int vV);
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_NTSC (Type t, int vV);
	};
}

#endif
  
// End of the file
/*@}*/
