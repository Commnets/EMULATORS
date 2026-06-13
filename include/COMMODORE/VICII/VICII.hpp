/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICII.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: The VICII Chip.
 *	Versions: 2.0 Powered by the information in the documentation and the tests of the emulation of the VICII in CMB64. \n
 *	Based on https://www.cebix.net/VIC-Article.txt.
 */

#ifndef __COMMODORE_VICII__
#define __COMMODORE_VICII__

#include <CORE/incs.hpp>
#include <COMMODORE/VICII/VICIIRegisters.hpp>

namespace COMMODORE
{
	/** 
		The chip that takes care of anything around the graphics in computers like 
		Commodore 64 or Commodore 128 when emulating CMB64. \n
		@see GraphicalChip. \n
		How it works in detail can be found in: https://www.cebix.net/VIC-Article.txt. \n
		\n
		Any case: \n
		Every horizontal raster line takes 64us (including horizontal retrace = 15us as PAL/NTSC standard definition). \n
		1/985.248 = 1,01497us per cycle in PAL. 64us/1,01497 = 63 cycles per raster line in PAL. \n
		1/1.023.000 = 0,977517us per cycle in NTSC. 64us/0,977517us = 64 cycles per raster line in NTSC. \n
		VICII speed is = CPU speed. \n
		So in every CPU cycle 8 pixels are drawn (if possible). \n
		In every VICII cycle two actions are done: 
		In low mode the byte is read, and in the high one the byte is drawn. \n
		In VICII simulation, the cycles (that are different per type of VICII (PAL/NTSC)) 
		x 8 are used to represent the resolution of the VICII.
		\n
		VICII has a LP pin that latches the current raster beam position on a
		negative edge. The emulation maps this behaviour to the mouse: while the
		left mouse button is pressed, the mouse position is treated as the light-pen
		target position. The latch happens when the emulated raster beam reaches
		that visible mouse position, and only once per frame. \n
		The latched position is stored in VIC-II registers $13 and $14.
	*/
	class VICII : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 104;

		// Related with badlines...
		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE							= 0x30;
		static const unsigned short _LASTBADLINE							= 0xf7;
		/** Bauer bad-line BA/c-access start window. */
		static const unsigned short _BADLINE_START_FIRST_CYCLE				= 12;
		static const unsigned short _BADLINE_START_LAST_CYCLE				= 54;
		/** Late Bad Line Condition window that can prevent idle entry at cycle 58. */
		static const unsigned short _BADLINE_IDLE_PREVENT_FIRST_CYCLE		= 54;
		static const unsigned short _BADLINE_IDLE_PREVENT_LAST_CYCLE		= 57;
		/** Effective c-access window used by this emulator model. */
		static const unsigned short _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE	= 16;
		static const unsigned short _BADLINE_EFFECTIVE_CACCESS_LAST_CYCLE	= 55;
		/** Effective graphics access window used by this emulator model. */
		static const unsigned short _GRAPHIC_ACCESS_FIRST_CYCLE				= 16;
		static const unsigned short _GRAPHIC_ACCESS_LAST_CYCLE				= 55;

		/** VC and VCBASE are 10-bit video matrix counters. */
		static const unsigned short _VCMASK = 0x03ff;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25;	// Not taking into account reductions in the size!
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320;	// Not taking into account double coulors!
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		// Some events.
		/** As the VICII only addresses 16k and some computers where it might be connected to, admits up to 64k,
			there is the possibility to change the bank. \n
			The unsigned ints associated to each must be consecutive for everything to work properly. \n
			NOTE: Don't tounch these values! */
		static const unsigned int _BANK0SET = 200;
		static const unsigned int _BANK1SET = 201;
		static const unsigned int _BANK2SET = 202;
		static const unsigned int _BANK3SET = 203;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICII constructor receives a refeence to the the Color RAM, to simulate the access to the 
			color at the same time it acceses to the char info. \n
			The VICII constructor receives also info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		VICII (int intId, MCHEmul::PhysicalStorageSubset* cR, const MCHEmul::Address& cRA,
			const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~VICII () override;

		// Managing lightpen
		/** To activate/desactivate the lightpen. */
		void setLightPenActive (bool a)
							{ _VICIIRegisters -> setLigthPenActive (a); 
							  _eventStatus._lightPenPositionLatched = false;}

		/** To draw or not to draw raster interrupt positions. */
		void setDrawRasterInterruptPositions (bool d)
							{ _drawRasterInterruptPositions = d; }
		/** To draw or not to draw an square rounding the sprites. 
			It is drawn when the sprite is active in the raster line where the VICII is. */
		void setDrawSpritesBorder (bool d)
							{ _drawSpritesBorder = d; }
		/** Whether to draw other events related with the raster of VICII. 
			Borders, bad lines mainly. */
		void setDrawOtherEvents (bool d)
							{ _drawOtherEvents = d; }

		/** To assign or to know the color RAM. 
			The VICII needs to know where the color RAM is, to access the nibbles with the color. */
		const MCHEmul::PhysicalStorageSubset* colorRAM () const
							{ return (_colorRAM); }
		MCHEmul::PhysicalStorageSubset* colorRAM ()
							{ return (_colorRAM); }
		const MCHEmul::Address& colorRAMAddreess () const
							{ return (_colorRAMAddress); }
		void setColorRAM (MCHEmul::PhysicalStorageSubset* cR, const MCHEmul::Address& cRA)
							{ _colorRAM = cR; _colorRAMAddress = cRA; }

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To change and get the bank. */
		unsigned short bank () const
							{ return (_VICIIRegisters -> bank ()); }
		void setBank (unsigned char bk)
							{ _VICIIRegisters -> setBank (bk); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** The raster is a critical thing in the behaviour of the VICII.
			The content of the register 0x12 (and the 0x11 bacause it contains high bit) is "real time".
			Any read instruction could read different values depending on the position of the raster 
			when that instruction happens. */
		virtual void CPUAboutToExecute (MCHEmul::CPU* cpu, MCHEmul::Instruction* inst) override
							{ _VICIIRegisters -> setNumberPositionsNextInstruction 
								(inst -> clockCycles
									(cpu -> memoryRef (), cpu -> programCounter ().asAddress ())); }

		/** Simulates cycles in the VICII. \n
			It draws the border AFTER once graphics info has been drawn within the display zone. \n
			So sprites can be drawn behing the border and collisions could take place out of the visible zone. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters					= InfoStructure: Info about the registers. \n
		  * Raster							= InfoStructure: Info about the raster. \n
		  * VICIIInternal					= InfoStructure: Info about the internal registers of the VICII. \n
		  * DENSeenAtLine30					= Attribute: Whether the DEN signal has been seen at line 30. \n
		  * BadlineCondition				= Attribute: Whether the raster is or not in a bad line. \n
		  * BadlinePreventedIdleThisLine	= Attribute: Whether a bad line has prevented idle in the current line. \n
		  * BadlineDetected                 = Attribute: Whether a bad-line condition has already been accepted in the current raster line. \n
		  * BadlineCAccess                  = Attribute: Whether a bad-line c-access sequence is latched for the current raster line. \n
		  * BadlineDetected					= Attribute: Whether a bad line has been detected in the current cycle. \n
		  * BadlineCAccess					= Attribute: Whether the VICII is accessing to the character data. \n
		  * BadlineBARequested				= Attribute: Whether the VICII has requested the bus to access to the character data. \n
		  * BadlineBARequestCycle			= Attribute: Number of the VICII internal cycle where the bus 
		  *		request to access to the character data happens. \n
		  * BadlineFirstCAccessCycle		= Attribute: Number of the VICII internal cycle where the first 
		  *		access to the character data happens. \n
		  * BadlineCAccessAllowed			= Attribute: Whether the latched c-access sequence is allowed 
		  *		to perform normal Video Matrix / Color RAM reads in this raster line. \n
		  * BadlineCAccessStartCycle		= Attribute: Number of the VICII internal cycle where the access to the character data starts. \n
		  * Cycle							= Attribute: Number of the VICII internal cycle where the raster beam is. \n
		  * LastVICDataRead					= Attribute: The last byte read by the VICII. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** content of the screen. */
		MCHEmul::UBytes screenMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the color memory (always in the same position). */
		MCHEmul::UBytes colorMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the bitmap memory. */
		MCHEmul::UBytes bitmapMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The data of the sprites memory. */
		MCHEmul::UBytes spritesMemorySnapShot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& sprs = { }) const;

		// To print out the form of the different graphic elements
		// taking into account the graphic mode active
		/** The sprites. */
		MCHEmul::Strings spritesDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& sprs = { }) const;
		/** The characters. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics & Sprites in detail...
		/** A couple of variables defining the drawing context. \n
			Tjose are to simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _SC;		// Scroll X
			unsigned short _RC;		// Raster X position not adjusted
			unsigned short _RCA;	// Raster X position adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed because how the C64 screen is defined...
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The data used to detect the collisions with the graphics. */
			MCHEmul::UByte _collisionGraphicData;
			/** The data used to detect collisions with the sprites. */
			MCHEmul::UByte _collisionSpritesData [8];
			/** The color of the each pixel considered as background. */
			unsigned int _backgroundColorData [8];
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];
			/** The color of each pixel if there was sprites behind!,
				and who is the owner (sprite number) of that pixel! */
			unsigned int _spriteColor [8];
			size_t _spriteColorOwner [8];
			/** To indicate whether it is or not consecuencia of a "bad mode",
				and the has to be everything draw in black, but taking into account the real graphics behaind. */
			bool _invalid;

			DrawResult ()
				: _collisionGraphicData (MCHEmul::UByte::_0),
				  _collisionSpritesData
						{ MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, 
						  MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0 },
				  _backgroundColorData 
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _foregroundColorData 
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _spriteColor
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _spriteColorOwner
						{ MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, 
						  MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0 },
				  _invalid (false)
							{ }
		};

		// -----
		// Support methods used by the VIC-II simulation pipeline...
		// --------------------------------------------------------------------
		// Per-cycle simulation pipeline.
		// These methods are directly invoked from simulate(), or are the main
		// entry points for each phase of the VIC-II cycle.
		// --------------------------------------------------------------------
		// Bad-line state management...
		/** To update the complete bad-line state for the current VIC-II cycle. */
		inline void treatBadLineStateAtCurrentCycle ();
		/** To reset the per-raster-line bad-line state. */
		inline void resetBadLineStateForNewRasterLine ();
		/** To update the DEN latch used to enable bad lines in the current frame. */
		inline void updateDENSeenAtLine30 ();
		/** To determine whether a bad line is possible in the current cycle. */
		inline bool isBadLineCondition () const;

		// Idle/screen state management...
		/** To determine whether the VIC-II internal graphics sequencer is in idle state. */
		bool idleStateActive () const
							{ return (_vicGraphicInfo._idleState); }
		/** To determine whether the VIC-II internal graphics sequencer is in screen/display state. */
		bool screenStateActive () const
							{ return (!idleStateActive ()); }
		/** To switch the VIC-II internal graphics sequencer to screen/display state. */
		void enterScreenState ()
							{ _vicGraphicInfo._idleState = false; }
		/** To switch the VIC-II internal graphics sequencer back to idle state. */
		void enterIdleState ()
							{ _vicGraphicInfo._idleState = true; }

		// Bus arbitration/BA-like stop requests...
		/** To request a BA-like CPU stop when the VIC-II is about to need the bus. */
		inline void requestCPUStopForDMAIfNeeded (MCHEmul::CPU* cpu, unsigned int cC);
		/** To determine whether the VIC-II is about to read sprite info. */
		inline bool isAboutToReadSpriteInfo () const;
		/** To determine whether the VIC-II is about to read character/color info. */
		inline bool isAboutToReadCharacterInfo () const;

		// Raster-cycle execution...
		/** To treat the current raster cycle and request a full CPU stop if the VIC-II
			has actually stolen bus cycles during this cycle. */
		inline void treatRasterCycleAndRequestCPUStopIfNeeded (MCHEmul::CPU* cpu, unsigned int cC);
		/** Different actions are taken depending on the raster cycle. \n
			Returns the number of CPU cycles that have to be fully stopped as a
			consequence of VIC-II bus usage. \n
			The way raster cycles are treated depends on the concrete VIC-II variant. */
		virtual unsigned int treatRasterCycle ();
		/** To treat the VIC-II cycle where VC is loaded from VCBASE and
			the graphic access indexes are reset for the current line. */
		inline void treatGraphicFetchStartCycle ();
		/** To treat the VIC-II cycle where RC is advanced, VCBASE can be updated,
			and the idle state can be restored. */
		inline void treatGraphicRowEndCycle ();

		/** Last byte read by the VIC-II from the 8-bit memory data bus. */
		const MCHEmul::UByte& lastVICDataRead () const
							{ return (_lastVICDataRead); }

		// Graphics and bad-line accesses performed during raster-cycle execution...
		/** To treat a graphics access cycle.
			Returns the number of CPU cycles that have to be fully stolen. */
		inline unsigned int treatGraphicAccessCycle ();
		/** To treat a bad-line c-access cycle.
			Returns the number of CPU cycles that have to be fully stolen. */
		inline unsigned int treatBadLineCAccessCycle ();
		/** To determine the first attempted c-access cycle for the current bad-line sequence. \n
			The first attempts can be invalid DMA-delay/FLI accesses returning $ff.
			0 means that no c-access sequence has been defined. */
		inline unsigned short firstBadLineCAccessCycle () const;
		/** To determine whether the VIC-II is doing a bad-line c-access in this cycle. */
		inline bool isBadLineCAccessCycle () const;
		/** To determine whether the VIC-II performs a graphics data access
			in the current raster cycle. */
		bool isGraphicAccessCycle () const
							{ return (_cycleInRasterLine >= _GRAPHIC_ACCESS_FIRST_CYCLE &&
									  _cycleInRasterLine <= _GRAPHIC_ACCESS_LAST_CYCLE); }

		// Graphic access counters...
		/** To reset the graphics access counters at the beginning of a graphics fetch line. */
		inline void resetGraphicAccessCountersForCurrentLine ();
		/** To reset the video counters at the beginning of a new frame. */
		inline void resetGraphicCountersForNewFrame ();
		/** To advance the VIC-II video counters after a graphics access cycle. */
		inline void advanceGraphicAccessCounters ();

		// Raster movement and position-dependent events...
		/** To advance the internal raster position one VIC-II cycle.
			It also handles the transition to a new raster line. */
		inline void advanceRasterPosition ();
		/** To activate the raster IRQ flag if the current raster position matches
			the programmed raster interrupt position. */
		inline void treatRasterIRQAtCurrentPosition ();
		/** To request a CPU IRQ if any VIC-II IRQ reason is currently active. */
		inline void requestIRQIfNeeded (MCHEmul::CPU* cpu, unsigned int cC);
		/** To notify once when the raster enters the first VBlank zone. */
		inline void notifyGraphicsReadyIfNeeded ();

		// Light pen...
		/** To treat light-pen detection at the current raster position. */
		inline void treatLightPenAtCurrentRasterPosition ();

		// --------------------------------------------------------------------
		// Drawing pipeline.
		// These methods are reached from drawVisibleZone().
		// --------------------------------------------------------------------
		/** Treat the visible zone.
			Draws the graphics, detects collisions, and finally draws the border. */
		void drawVisibleZone (MCHEmul::CPU* cpu);
		/** To determine whether the visualization of the current frame is active. */
		bool displayEnabledForCurrentFrame () const
							{ return (_DENSeenAtLine30); }

		// Border management.
		/** To manage the main border status for the current visible slice. */
		inline void actualizeMainBorderStatus (unsigned short cav, unsigned short rv);
		/** To manage the vertical border status within VICGraphicInfo. */
		inline void actualizeVerticalBorderStatus ();

		// Graphics, sprites and collision composition.
		/** Invoked from drawVisibleZone() to draw graphics/sprites and detect collisions. \n
		  *	The parameter is the drawing context. \n
		  *	@see DrawContext and DrawResult. */
		void drawGraphicsSpritesAndDetectCollisions (const DrawContext& dC);
		/** To draw any text or bitmap graphic mode. \n
			The method receives the drawing context and returns a DrawResult. */
		DrawResult drawGraphics (const DrawContext& dC);

		// Character and bitmap drawing modes.
		/** Draws a monochrome character. */
		DrawResult drawMonoColorChar (int cb);
		/** Draws a multicolor character. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMultiColorChar (int cb, bool inv = false);
		/** Draws an extended-background character. */
		DrawResult drawMultiColorExtendedChar (int cb);
		/** Draws a monochrome bitmap. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMonoColorBitMap (int cb, bool inv = false);
		/** Draws a multicolor bitmap. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMultiColorBitMap (int cb, bool inv = false);

		// Sprite drawing.
		/** Draws one sprite over the already computed graphics result. */
		MCHEmul::UByte drawSpriteOver (size_t spr, unsigned int* d, size_t* dO);
		/** Draws a monochrome sprite line. */
		MCHEmul::UByte drawMonoColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d, size_t* dO);
		/** Draws a multicolor sprite line. */
		MCHEmul::UByte drawMultiColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d, size_t* dO);

		// Final drawing output and collision status.
		/** To move the computed graphics/sprite result to screen memory. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);
		/** To detect collisions between graphics and sprites, and between sprites. 
			The second parameter is a byte which bits points out which sprites were or not drawn. */
		void detectCollisions (const DrawResult& cT, const MCHEmul::UByte& sD);

		// Optional event visualization.
		/** To draw debug/event markers if _drawOtherEvents is active. */
		void drawOtherEvents (unsigned short cav, unsigned short rv);

		// --------------------------------------------------------------------
		// Memory reads performed by the raster-cycle pipeline.
		// --------------------------------------------------------------------
		// Character/color and graphics data.
		/** To get the emulator-side index used to access the 40-byte graphics buffers. */
		inline size_t graphicAccessIndex () const;
		/** To read Video Matrix and Color RAM during an effective normal bad-line c-access. */
		inline void readVideoMatrixAndColorRAM ();
		/** To read graphic data using the information previously fetched from
			Video Matrix / Color RAM, or from idle-state fixed addresses. \n
			_VC determines the real VIC-II memory position when needed.
			_GAccessIndex determines the emulator-side 40-byte buffer position. */
		inline void readGraphicalInfo ();

		// Sprite data.
		/** To read graphical data for all active sprites. */
		inline void readSpritesData ();
		/** To read graphical data for one sprite only. */
		inline bool readSpriteData (size_t nS);

		// --------------------------------------------------------------------
		// Different debug methods to simplify the internal code
		// --------------------------------------------------------------------
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugDisconnected (MCHEmul::CPU* cpu);
		void debugVICIICycle (MCHEmul::CPU* cpu, unsigned int i);
		void debugBadLine ();
		void debugReadingSpriteInfo (size_t nS);
		void debugSpriteDrawFinishes (size_t nS);
		void debugSpriteDrawToStart (size_t nS);
		void debugReadingVideoMatrix ();
		void debugReadingGraphics ();
		void debugVideoNoActiveAt (unsigned short cav);
		void debugDrawPixelAt (unsigned short cav);
		void debugDrawSpriteAt (size_t nS, unsigned short x, unsigned short r);
		// -----

		protected:
		/** The type of interrupt that are launched from this chip. */
		int _interruptId;
		/** A reference to the color RAM. */
		MCHEmul::PhysicalStorageSubset* _colorRAM;
		/** With the address it belongs to. */
		MCHEmul::Address _colorRAMAddress;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The position in the line when the IRQ should be launched in case it is active. 
			Depends on the implementation of the VICII. */
		unsigned short _IRQrasterPosition;
		/** The difference with the PAL System. */
		unsigned short _incCyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To draw or not to draw lines at the positions where the raster interruptions are generated. */
		bool _drawRasterInterruptPositions;
		/** To draw or not to draw a border around the sprites active at the line where the raster is in. */
		bool _drawSpritesBorder;
		/** To draw or not other different events. */
		bool _drawOtherEvents;

		// Implementation
		/** When the CPU is not stopped (sometimes the VIC requires to stop it). \n 
			and a instruction is executed, the number of cycles that that instruction required, has to be taken into account
			to define what the VICII has to do. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** Last byte read by the VIC-II from the 8-bit memory data bus. \n
			It is updated by matrix, graphics, sprite and invalid DMA-delay/FLI
			accesses. It is not yet connected to CPU open-bus reads. */
		MCHEmul::UByte _lastVICDataRead;
		/** True when DEN has been active at least once during raster line $30. */
		bool _DENSeenAtLine30;
		/** True when a bad line has already been accepted in the current raster line. \n
			This avoids detecting the same bad line multiple times because simulate ()
			runs once per VIC-II cycle. */
		 bool _badLineAlreadyDetectedThisLine;
		/** True when the bad line condition is active in the current VIC - II cycle. */
		bool _badLineConditionActive;
		/** True when a Bad Line Condition has been seen in cycles 54-57 while
			RC == 7 in the current raster line. \n
			In that case the graphics sequencer must not return to idle state
			at cycle 58, even if the Bad Line Condition is no longer active
			exactly at cycle 58. */
		bool _badLinePreventedIdleThisLine;
		/** True when the BA-like CPU stop request for the current bad line
			has already been issued. */
		bool _badLineBAAlreadyRequested;
		/** Raster cycle where the BA-like CPU stop request for the current bad line
			was issued. 0 means that no BA request has been issued for this line. */
		unsigned short _badLineBARequestCycle;
		/** True when a bad-line c-access sequence has been latched for the current raster line. \n
			This does not necessarily mean that real Video Matrix / Color RAM reads
			will be performed: some attempts can be invalid DMA-delay/FLI accesses
			returning $ff, and aborted sequences can be blocked by
			_badLineCAccessAllowedThisLine. */
		bool _badLineCAccessActive;
		/** True when the current bad-line c-access sequence is allowed to perform
			matrix/color access attempts in this raster line. \n
			Some of those attempts can be invalid DMA-delay/FLI accesses and therefore
			write $ff instead of reading real Video Matrix data. */
		bool _badLineCAccessAllowedThisLine;
		/** Number of initial c-access attempts that must read $ff instead of real
			Video Matrix data. \n
			This models the AEC/BA delay effect used by FLI / DMA delay / VSP. */
		unsigned short _badLineInvalidCAccessCycles;
		/** Raster cycle where the current bad-line c-access sequence started.
			0 means that no c-access sequence is active in the current line. */
		unsigned short _badLineCAccessStartCycle;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
		/** A temporal variable to indicate whether a lightpen position
			has already been latched in the current frame.
			This variable is reset at the beginning of every frame. */
		bool _lightPenFrameLatched;
		/** A very temporal variable to keep when the button is pressed. */
		bool _lightPenButtonPressed;

		/** Structure used to control how the graphics sequencer displays text and bitmap data. \n
		  *	Reference: https://www.cebix.net/VIC-Article.txt, sections 3.7.1 and 3.7.2. \n
		  *	The VIC-II graphics sequencer can be in either idle state or screen/display state. \n
		  *
		  * In display state:
		  * - g-accesses read character or bitmap graphics data;
		  * - VC and the logical VMLI/VLMI advance after each g-access;
		  * - bad-line c-access attempts can fill the internal 40-byte Video Matrix /
		  *   Color RAM line buffer either with real matrix/color data or, in DMA-delay /
		  *   FLI cases, with initial invalid $ff screen-code data and provisional color
		  *   data before normal matrix/color reads resume.
		  *
		  *	In idle state:
		  *	- no new Video Matrix / Color RAM c-accesses are performed;
		  *	- g-accesses still occur, but they read from the idle address:
		  *	  $3fff normally, or $39ff when ECM affects the address lines;
		  *	- the display output is produced from the idle graphics data and uses color 0.
		  *
		  *	This emulator separates two concepts:
		  *	- _VLMI is the logical Video Matrix Line Index;
		  *	- _GAccessIndex is an emulator-side 0..39 buffer index used to address
		  *	  _screenCodeData, _colorData and _graphicData.
		  *
		  *	Relevant raster-cycle rules in this implementation:
		  * - At cycle 14, VC is loaded from VCBASE and the line access indexes are reset.
		  *   If a Bad Line Condition is active, RC is reset to 0 and c-access attempts
		  *   are allowed for this line. If the sequence was first latched at cycle 14,
		  *   the first attempts are treated as invalid FLI/DMA-delay accesses.
		  *   If the condition was latched earlier
		  *   but is no longer active at cycle 14, the c-access sequence is cancelled. 
		  *	- Effective graphics accesses are grouped in cycles 16..55 in this emulator.
		  *	  During these accesses, _GAccessIndex always advances. VC and _VLMI advance
		  *	  only while the sequencer is in display state.
		  *	- At cycle 58, if RC == 7, VCBASE is loaded from VC. The sequencer enters
		  *	  idle state only if there is no current Bad Line Condition and no late
		  *	  Bad Line Condition has prevented idle entry for this line.
		  *	- RC is incremented only if the sequencer remains in display state after
		  *	  the cycle-58 decision. */
		struct VICGraphicInfo
		{
			VICGraphicInfo ()
				: _VCBASE (0), _VC (0),
				  _RC (0),
				  _ROW (0), // This is the row where the raster is once it has been recognized by the VICII
				  _VLMI (0),
				  _GAccessIndex (0),
				  _idleState (true),
				  _ffVBorder (false),
				  _ffMBorder (false),
				  _ffLBorder (false), _ffRBorder (false),
				  _ffMBorderBegin (0), _ffMBorderPixels (0),
				  _screenCodeData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _graphicData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _lastScreenCodeDataRead (MCHEmul::UByte::_0),
				  _lastGraphicDataRead (MCHEmul::UByte::_0),
				  _lastColorDataRead (MCHEmul::UByte::_0)
							{ }

			void emptyVideoMatrixAndColorRAMData ()
							{ _screenCodeData	= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _colorData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }
			
			void emptyGraphicData ()
							{ _graphicData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }


			/** 
				The name of the fields are: \n
				VCBASE			= Attribute: 10-bit video position counter base for the current character/bitmap row.
				VC				= Attribute: 10-bit video position counter. It advances by 1 during display-state g-accesses.
				VLMI			= Attribute: Logical Video Matrix Line Index. \n
				GAccessIndex	= Attribute: Emulator-side graphics access index within the 40-byte line buffer. \n
				RC				= Attribute: Line position within the graphics memory. From 0 to 7. \n
				ROW				= Attribute: Row number. Depending on the VICII type. \n
				IDLE			= Attribute: Whether the VICII is or not in idle state.
			  */
			MCHEmul::InfoStructure getInfoStructure () const;

			// Internal elements used to read the memory from the VICII
			// Read what they do at the header of the class. Very important counters...
			unsigned short _VCBASE, _VC;
			/** Logical Video Matrix Line Index. \n
				It advances only when the VIC-II is in screen/display state. \n
				It can diverge from _GAccessIndex when the VIC-II is in idle state. */
			unsigned short _VLMI;
			/** Graphics access index within the current 40-column graphics fetch line. \n
				This is not a real VIC-II register. It is an emulator-side index used to
				separate the horizontal graphics buffer position from the logical VLMI/VMLI. \n
				It is now the index used to access the emulator-side graphics buffers.
				It can diverge from _VLMI when the VIC-II is in idle state. */
			unsigned short _GAccessIndex;
			unsigned char _RC;
			/** Where the raster is from the VICII perspective. \n
				This counter gets updated just when the first cycle of the VICII happens. */
			unsigned short _ROW;
			/** Whether the VIC-II graphics sequencer is in idle state. \n
				The sequencer leaves idle state when a new Bad Line Condition is accepted. \n
				It can return to idle state at cycle 58 when RC == 7, provided that no
				current Bad Line Condition is active and no late Bad Line Condition in
				cycles 54..57 has prevented the idle transition for this raster line. \n
				Even in idle state, the graphics sequencer still produces display data
				from the idle g-access address, using color 0. */
			bool _idleState;

			// Internal elements used to manage the border...
			bool _ffVBorder; 
			bool _ffMBorder;
			/** These two ones are temporal variables due to the VICII simulation doesn't execute cycle by clcle. 
				When the border is partially detected (either in the right or the left), the border has to be "drawn",
				and just after that the main border effect is either dissactivated (at the left) or activated (at the right). \n
				These variables are not part of the internal VICII registers actually. */
			bool _ffLBorder, _ffRBorder;
			unsigned short _ffMBorderBegin;
			unsigned char _ffMBorderPixels;

			// Implementation...
			// This one doesn't actually exist "in" the VICII chip, 
			// but is used when he left border has to be partially drawn.
			// After doing so, the _ffMBorder will become false...
			mutable MCHEmul::UBytes _screenCodeData;
			mutable MCHEmul::UBytes _graphicData; 
			mutable MCHEmul::UBytes _colorData;
			/** The last info read. */
			mutable MCHEmul::UByte _lastScreenCodeDataRead;
			mutable MCHEmul::UByte _lastGraphicDataRead;
			mutable MCHEmul::UByte _lastColorDataRead;
		};

		VICGraphicInfo _vicGraphicInfo;

		/** 
		  * Structure used in controlling how sprites are managed: \n
		  *	Important things that happen during the raster line and affects the sprites... \n
		  *	As the raster moves the VICII has to decide	which sprite info to draw.\n
		  *	That is based on info gather in three major variables. \n
		  *	The rules to manipulate every value are decribed below attending to the cycle in the raster. \n
		  *	https://www.cebix.net/VIC-Article.txt. (point 3.8.1): \n
		  *	The vertical flipflop is set as long as 
		  *	the bit in the resgiter $d017 (vertical expansion) is cleared. \n
		  *	CYCLE 55 (PAL):			If the bit in the register $d017 is set, the _FF is inverted. \n
		  *	CYCLE 55 & 56 (PAL):	The VICII checks whether every sprite is on (bit at $d015) 
		  *							and the Y coordinate of the sprite matches the lower 8 bits of the raster. \n
		  *							If _DMA is still off, the is it is activated, _MCBASE is cleared,
		  *							and if the bit in the register $d017 is set the _FF is reset (put back to true). \n
		  *	CYCLE 58 (PAL):			_MCBASE is moved to MC. \n
		  *							If _DMA is on and (again) the Y coordinate matches the lower 8 bits of the raster 
		  *							the visualization of the Sprite is swithed on!. \n
		  *	CYCLEs ss:				If _DMA is switched the sprite data is accesed and the _MC is incremented in 3. \n
		  *	CYCLE visible:			If _visible is true, the sprite info is drawn. \n
		  *							The rules to dowble the X size are taken as the visualization of each comes. \n
		  *	CYCLE 15:				If the _FF is set, _MCBASE is incremented in 2. \n
		  *	CYCLE 16:				If the _FF is set, _MCBASE is incremented in 1. \n
		  *							If _MCBASE is 63 then _DMA and _visible are set to off. \n
		  * In this simulation this behaviour has been simplified:
		  * At cycle 15 the info _line is incremented. 
		  * At cycle 52 sprites situation is actualized.
		  */
		struct VICSpriteInfo
		{
			VICSpriteInfo ()
				: _active (false), _line (0), _expansionY (false),
				  _spriteBaseAddress ({ 0x00, 0x00 }, true), // it is the same than using false and quicker...
				  _graphicsLineSprites (MCHEmul::UBytes::_E),
				  _drawing (false), _xS (0)
							{ }

			VICSpriteInfo (bool a, unsigned char l, bool e)
				: _active (a), _line (l), _expansionY (e),
				  _spriteBaseAddress ({ 0x00, 0x00 }, true), // it is the same than using false and quicker...
				  _graphicsLineSprites (MCHEmul::UBytes::_E),
				  _drawing (false), _xS (0)
							{ }

			bool _active; // Temporary combined sprite DMA/display flag in the current simplified sprite model.
			unsigned char _line; // Line of the sprite to be drawn (from 0 to 21). 
			// This is like MCBASE in the documentation. MC is not simulated, 
			// because the read of the info is done 3 mytes simultaneosuly....
			bool _expansionY; // True when the sprite is expanded in the Y axis
			mutable MCHEmul::Address _spriteBaseAddress;
			mutable MCHEmul::UBytes _graphicsLineSprites; // 3 bytes line info each
			/** Once the sprite reaches the coordinate x, the draw action continues until everyting is done. */
			bool _drawing; // Is the sprite being drawn?
			unsigned short _xS; // x coordinate from which the sprite is drawn!
		};

		VICSpriteInfo _vicSpriteInfo [8];

		/** The events that can be drawn
			associated to the movement of the raster line. */
		struct EventsStatus
		{
			/** When the internal variable to indicate the status of the main border, changes. */
			MCHEmul::Pulse _ffVBorderChange;
			/** Same when what change is an auxiliar variable to support changes in the border. */
			MCHEmul::Pulse _ffMBorderChange;
			/** The bad line to hightlight. */
			unsigned short _badLine;
			// Managing the lightpen related events...
			/** A lightpen position has been latched. */
			bool _lightPenPositionLatched;
			/** The lightpen position latched is different than the previous one. */
			MCHEmul::OBool _lightPenPositionChanged;
		};

		mutable EventsStatus _eventStatus;

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	// ---
	inline void VICII::treatBadLineStateAtCurrentCycle ()
	{
		updateDENSeenAtLine30 ();

		// Current Bad Line Condition for this exact VIC-II cycle.
		_badLineConditionActive = isBadLineCondition ();

		// Bauer: a Bad Line Condition in cycles 54..57 while RC == 7 prevents
		// the graphics sequencer from returning to idle state at cycle 58.
		if (_badLineConditionActive &&
			_vicGraphicInfo._RC == 7 &&
			_cycleInRasterLine >= _BADLINE_IDLE_PREVENT_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_IDLE_PREVENT_LAST_CYCLE)
			_badLinePreventedIdleThisLine = true;

		// The first accepted Bad Line Condition in a raster line puts the graphics
		// sequencer into display/screen state. It is latched once per line.
		if (_badLineConditionActive &&
			!_badLineAlreadyDetectedThisLine)
		{
			_badLineAlreadyDetectedThisLine = true;

			enterScreenState ();

			_eventStatus._badLine =
				_raster.vData ().currentVisiblePosition ();

			_IFDEBUG debugBadLine ();
		}

		// Latch a bad-line c-access sequence if the condition appears inside
		// Bauer's BA/c-access start window. This preserves the timing/state effect.
		// If the sequence is already valid at cycle 14, normal/FLI-like c-access
		// handling is decided there. If it appears after cycle 14, it is treated as
		// a late DMA-delay/VSP-like sequence: c-access attempts are allowed, but the
		// first ones return invalid $ff data.
		if (!_badLineCAccessActive &&
			_badLineConditionActive &&
			_cycleInRasterLine >= _BADLINE_START_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_START_LAST_CYCLE)
		{
			_badLineCAccessActive = true;
			_badLineCAccessStartCycle = _cycleInRasterLine;

			// If the Bad Line Condition appears after cycle 14, this is a late
			// DMA-delay/VSP-like sequence. Cycle 14 has already passed, so the line
			// will not be validated by treatGraphicFetchStartCycle(). Allow c-access
			// attempts here and mark the first three as invalid.
			if (_cycleInRasterLine > 14)
			{
				_badLineCAccessAllowedThisLine = true;
				_badLineInvalidCAccessCycles = 3;
			}
		}
	}

	// ---
	inline void VICII::resetBadLineStateForNewRasterLine ()
	{
		_badLineAlreadyDetectedThisLine = false;
		_badLineConditionActive = false;
		_badLinePreventedIdleThisLine = false;
		_badLineBAAlreadyRequested = false;
		_badLineBARequestCycle = 0;
		_badLineCAccessActive = false;
		_badLineCAccessAllowedThisLine = false;
		_badLineInvalidCAccessCycles = 0;
		_badLineCAccessStartCycle = 0;
	}

	// ---
	inline void VICII::updateDENSeenAtLine30 ()
	{
		// The DEN condition for bad lines is latched during raster line $30.
		// It is not the instantaneous value of DEN at the end of the line.
		if (_vicGraphicInfo._ROW == 0 && _cycleInRasterLine == 1)
			_DENSeenAtLine30 = false;

		if (_vicGraphicInfo._ROW == _FIRSTBADLINE &&
			!_VICIIRegisters -> blankEntireScreen ())
			_DENSeenAtLine30 = true;
	}

	// ---
	inline bool VICII::isBadLineCondition () const
	{
		return (
			_DENSeenAtLine30 && // Bad lines are possible only if DEN was seen active
								// during raster line $30 in the current frame...
			_vicGraphicInfo._ROW >= _FIRSTBADLINE &&
			_vicGraphicInfo._ROW <= _LASTBADLINE &&
			(unsigned char) (_vicGraphicInfo._ROW & 0x07) /** The three last bits only */ ==
				_VICIIRegisters -> verticalScrollPosition ()); // aligned with the scrollY
	}

	// ---
	inline void VICII::requestCPUStopForDMAIfNeeded (MCHEmul::CPU* cpu, unsigned int cC)
	{
		const bool aboutToReadSpriteInfo	= isAboutToReadSpriteInfo ();
		const bool aboutToReadCharacterInfo = isAboutToReadCharacterInfo ();
		if ((cpu -> lastState () != MCHEmul::CPU::_STOPPED && !cpu -> stopped ()) &&
			(aboutToReadSpriteInfo || aboutToReadCharacterInfo))
		{
			cpu -> setStop (
				true,
				MCHEmul::InstructionDefined::_CYCLEREAD,
				cC,
				3);

			// Specifically when reading the chars...
			if (aboutToReadCharacterInfo)
			{
				_badLineBAAlreadyRequested = true;
				_badLineBARequestCycle = _cycleInRasterLine;
			}
		}
	}

	// ---
	inline bool VICII::isAboutToReadSpriteInfo () const
	{
		return (
			(_cycleInRasterLine == 2								&& _vicSpriteInfo [5]._active) ||
			(_cycleInRasterLine == 4								&& _vicSpriteInfo [6]._active) ||
			(_cycleInRasterLine == 6								&& _vicSpriteInfo [7]._active) ||
			(_cycleInRasterLine == (55 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [0]._active) ||
			(_cycleInRasterLine == (57 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [1]._active) ||
			(_cycleInRasterLine == (59 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [2]._active) ||
			(_cycleInRasterLine == (61 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [3]._active) ||
			(_cycleInRasterLine == (63 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [4]._active));
	}

	// ---
	inline bool VICII::isAboutToReadCharacterInfo () const
	{ 
		return (
			_badLineConditionActive &&
			_cycleInRasterLine >= _BADLINE_START_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_START_LAST_CYCLE &&
			!_badLineBAAlreadyRequested);
	}

	// ---
	inline void VICII::treatRasterCycleAndRequestCPUStopIfNeeded (MCHEmul::CPU* cpu, unsigned int cC)
	{
		unsigned int cS = treatRasterCycle ();
		if (cS > 0)
			cpu -> setStop (
				true,
				MCHEmul::InstructionDefined::_CYCLEALL,
				cC,
				(int) cS);
	}

	// ---
	inline void VICII::treatGraphicFetchStartCycle ()
	{
		// At this cycle, the VIC-II reloads VC from VCBASE and starts a new
		// 40-position graphics fetch sequence for the current raster line.
		_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE & _VCMASK;

		resetGraphicAccessCountersForCurrentLine ();

		// Cycle 14 is the decisive point for a normal bad-line matrix fetch.
		// If the Bad Line Condition is active here, the c-access sequence is allowed
		// for this line. A sequence first latched at cycle 14 is treated as FLI-like:
		// the first c-access attempts return $ff before normal matrix/color reads.
		// Bad lines that appear after cycle 14 are enabled later by
		// treatBadLineStateAtCurrentCycle() as DMA-delay/VSP-like sequences.
		_badLineCAccessAllowedThisLine = _badLineConditionActive;
		_badLineInvalidCAccessCycles = 0;

		if (_badLineConditionActive)
		{
			// A normal or FLI-like bad line is valid at cycle 14.
			// If the c-access sequence was first latched exactly at cycle 14,
			// emulate the FLI/DMA-delay effect: the first three c-access attempts
			// read $ff on D0-D7.
			if (_badLineCAccessActive &&
				_badLineCAccessStartCycle == 14)
				_badLineInvalidCAccessCycles = 3;

			_vicGraphicInfo._RC = 0;
		}
		else
		{
			// A bad-line c-access sequence could have been latched in cycles 12/13
			// and then aborted before cycle 14. The sequencer remains in display
			// state, but no normal matrix/color c-accesses continue.
			_badLineCAccessActive = false;
			_badLineCAccessAllowedThisLine = false;
			_badLineInvalidCAccessCycles = 0;
			_badLineCAccessStartCycle = 0;
		}
	}

	// ---
	inline void VICII::treatGraphicRowEndCycle ()
	{
		// At this cycle, if the last row of the current character/bitmap cell has
		// just been processed, VC becomes the new VCBASE for the next character row.
		if (_vicGraphicInfo._RC == 7)
		{
			_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC & _VCMASK;
	
			// If no current Bad Line Condition is active, and no late Bad Line
			// Condition in cycles 54..57 has prevented the transition, the VIC-II
			// returns to idle state after completing the current 8-row block.
			if (!_badLineConditionActive &&
				!_badLinePreventedIdleThisLine)
				enterIdleState ();
		}
	
		// RC advances only if the sequencer remains in display state after the
		// cycle-58 decision. If idle state was entered, RC is preserved.
		if (screenStateActive ())
		{
			if (++_vicGraphicInfo._RC == 8)
				_vicGraphicInfo._RC = 0;
		}
	}

	// ---
	inline unsigned int VICII::treatGraphicAccessCycle ()
	{
		if (!isGraphicAccessCycle ())
			return (0);

		// During a graphics access cycle, the VIC-II view is needed both for the
		// optional c-access and for the mandatory g-access. Keep the VIC-II memory
		// view active for the whole emulated graphics access cycle instead of
		// switching it around each individual read.
		memoryRef () -> setActiveView (_VICIIView);

		unsigned int result = treatBadLineCAccessCycle ();

		readGraphicalInfo ();

		memoryRef () -> setCPUView ();

		advanceGraphicAccessCounters ();

		_IFDEBUG debugReadingGraphics ();

		return (result);
	}

	// ---
	inline unsigned int VICII::treatBadLineCAccessCycle ()
	{
		if (!isBadLineCAccessCycle ())
			return (0);

		const unsigned short fCA = firstBadLineCAccessCycle ();

		const bool invalidCAccess =
			_badLineInvalidCAccessCycles > 0 &&
			_cycleInRasterLine >= fCA &&
			_cycleInRasterLine < (fCA + _badLineInvalidCAccessCycles);
		if (invalidCAccess)
		{
			const size_t gAI = graphicAccessIndex ();

			// During the first invalid DMA-delay/FLI c-accesses, the VIC reads
			// $ff on D0-D7 instead of valid Video Matrix data.
			_vicGraphicInfo._lastScreenCodeDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._screenCodeData [gAI] = MCHEmul::UByte::_FF;

			// Accurate Color RAM data during these invalid cycles depends on CPU
			// bus data. Until that bus latch is modelled, use $0f as a deterministic
			// approximation.
			_vicGraphicInfo._lastColorDataRead =
				_vicGraphicInfo._colorData [gAI] = MCHEmul::UByte::_0F;

			_IFDEBUG debugReadingVideoMatrix ();

			return (1);
		}

		readVideoMatrixAndColorRAM ();

		_IFDEBUG debugReadingVideoMatrix ();

		return (1);
	}

	// ---
	inline unsigned short VICII::firstBadLineCAccessCycle () const
	{
		if (!_badLineCAccessActive ||
			_badLineCAccessStartCycle == 0)
			return (0);

		// In this grouped model, normal bad lines and FLI-like bad lines that
		// are active by cycle 14 start their attempted c-accesses at cycle 16.
		// Late DMA-delay/VSP sequences start after the BA/AEC delay.
		unsigned short result =
			(_badLineCAccessStartCycle <= 14)
				? _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE
				: (unsigned short) (_badLineCAccessStartCycle + 3);

		if (result < _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE)
			result = _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE;

		return (result);
	}

	// ---
	inline bool VICII::isBadLineCAccessCycle () const
	{
		const unsigned short fCA = firstBadLineCAccessCycle ();
		return (
			_badLineCAccessAllowedThisLine &&
			fCA != 0 &&
			_cycleInRasterLine >= _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_EFFECTIVE_CACCESS_LAST_CYCLE &&
			_cycleInRasterLine >= fCA);
	}

	// ---
	inline void VICII::resetGraphicAccessCountersForCurrentLine ()
	{
		_vicGraphicInfo._VLMI = 0;
		_vicGraphicInfo._GAccessIndex = 0;
	}

	// ---
	inline void VICII::resetGraphicCountersForNewFrame ()
	{
		_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC = 0;
		_vicGraphicInfo._RC = 0;

		resetGraphicAccessCountersForCurrentLine ();
	}

	// ---
	inline void VICII::advanceGraphicAccessCounters ()
	{
		if (screenStateActive ())
		{
			_vicGraphicInfo._VC = 
				(_vicGraphicInfo._VC + 1) & _VCMASK;
			_vicGraphicInfo._VLMI++;
		}

		_vicGraphicInfo._GAccessIndex++;
	}

	// ---
	inline void VICII::advanceRasterPosition ()
	{
		// First move to the next raster cycle.
		_cycleInRasterLine++;

		// Move the horizontal raster.
		const bool cLine = _raster.hData ().add (1 * _raster.step ());

		// If the horizontal retrace has just been overpassed,
		// the vertical raster has to advance one line.
		if (_raster.hData ().retraceJustOverPassed ())
			_raster.vData ().next ();

		// Has the raster reached the end of the current line?
		if (cLine)
		{
			_cycleInRasterLine = 1;

			resetBadLineStateForNewRasterLine ();

			// At the beginning of the new raster line, update the VIC-II ROW.
			// ...and if it is the first line of the frame, reset the video counters.
			if ((_vicGraphicInfo._ROW = _raster.currentLine ()) == 0)
			{ 
				resetGraphicCountersForNewFrame ();
			
				// ...and also allow a new light pen latch in the new frame.
				_lightPenFrameLatched = false;
			}
			else 
			{
				// At the beginning of every other line, VC starts from VCBASE.
				_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE & _VCMASK;
			}
		}
	}

	// ---
	inline void VICII::treatRasterIRQAtCurrentPosition ()
	{
		if (_vicGraphicInfo._ROW == _VICIIRegisters -> IRQRasterLineAt () &&
			_raster.currentColumn () == _IRQrasterPosition)
			_VICIIRegisters -> activateRasterIRQ ();
	}

	// ---
	inline void VICII::requestIRQIfNeeded (MCHEmul::CPU* cpu, unsigned int cC)
	{
		int cI = (int) _VICIIRegisters -> reasonIRQCode ();
		if (cI != 0)
			cpu -> requestInterrupt (
				_interruptId,
				cC,
				this,
				cI);
	}

	// ---
	inline void VICII::notifyGraphicsReadyIfNeeded ()
	{
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;
	
				notify (MCHEmul::Event (_GRAPHICSREADY));
			}
		}
		else
			_lastVBlankEntered = false;
	}

	// ---
	inline void VICII::treatLightPenAtCurrentRasterPosition ()
	{
		if (!_VICIIRegisters -> lightPenActive () ||
			!_lightPenButtonPressed ||
			_lightPenFrameLatched ||
			!_VICIIRegisters -> isMouseInVisibleZone () ||
			!_raster.isInVisibleZone ())
			return; // Nothing to do really...

		unsigned short cv = 0;
		unsigned short rv = 0;
		_raster.currentVisiblePosition (cv, rv);
		const int mx = _VICIIRegisters -> mousePositionX ();
		const int my = _VICIIRegisters -> mousePositionY ();

		// The raster advances horizontally in 8-pixel blocks in this emulation.
		// And it is needed to compare visible thing with visible things...
		if (my != (int) rv || 
			!((mx >= (int) cv) && (mx < (int) (cv + _raster.step ()))))
			return;

		/** At this point the raster beam is passing through the mouse/light-pen
			position. The light pen detects the beam and drives LP low. \n
			Convert from visible mouse coordinate to the corresponding
			VIC-II raster coordinate. \n
			_raster.hData().currentPosition() is the current internal
			raster X position. mx - cv gives the pixel offset inside the
			current 8-pixel group. */
		unsigned char lxo, lyo;
		_VICIIRegisters -> lightPenPositionLatched (&lxo, &lyo); // Before changing this one...
		_VICIIRegisters -> latchLightPenPositionFromRaster
			((unsigned char) (((_raster.hData ().currentPosition () + 
				((unsigned short) mx - cv)) >> 1) & 0xff), // LPX is in 2-pixel units...
			 (unsigned char) (_raster.vData ().currentPosition () & 0xff));
		unsigned char lx, ly;
		_VICIIRegisters -> lightPenPositionLatched (&lx, &ly); // The new one...

		_eventStatus._lightPenPositionLatched = true;
		_eventStatus._lightPenPositionChanged = (lx != lxo) || (ly != lyo);

		_lightPenFrameLatched = true;

		_VICIIRegisters -> activateLightPenOnScreenIRQ ();
	}

	// ---
	inline void VICII::actualizeMainBorderStatus (unsigned short cav, unsigned short rv)
	{
		_vicGraphicInfo._ffMBorderBegin = cav;
		_vicGraphicInfo._ffMBorderPixels = (cav + 8) > _raster.visibleColumns () 
			? (_raster.visibleColumns () - cav) : 8;

		// Only if it is in the screen position...
		// Looking at the right limit...
		if (cav == (_raster.hData ().lastScreenPosition () + 1))
			_vicGraphicInfo._ffMBorder = true;
		if (cav < _raster.hData ().lastScreenPosition () && 
				((cav + 8) >= _raster.hData ().lastScreenPosition ()))
		{
			_vicGraphicInfo._ffRBorder = true;
			// When out of the screen zone, the number of pixels to draw is complete...
			if (!_vicGraphicInfo._ffVBorder)
			{
				_vicGraphicInfo._ffMBorderBegin = _raster.hData ().lastScreenPosition () + 1;
				_vicGraphicInfo._ffMBorderPixels = (cav + 8) - _vicGraphicInfo._ffMBorderBegin;
			}
		}
		
		// Only if it is at the left limit...
		// ...atLeft variable will mark whether the raster getting the left side...
		// and if it is, _vicGraphicInfo._ffMBorderPixels will hold the number of pixels to draw... 
		// 8 by default (or the limit of the visible zone)!
		bool atLeft = false;
		if (cav < _raster.hData ().firstScreenPosition () &&
				((cav + 8) >= _raster.hData ().firstScreenPosition ()))
		{
			atLeft = true;
			if (_raster.vData ().currentPosition () >= _VICIIRegisters -> minRasterV () &&
				_raster.vData ().currentPosition () <= _VICIIRegisters -> maxRasterV ())
				_vicGraphicInfo._ffMBorderPixels = 
					_raster.hData ().firstScreenPosition () - cav;
		}
		
		// When the raster is getting the left position...
		// ...and it is also in the bottom visible limit...
		if (atLeft &&
				(_raster.vData ().currentPosition () == _VICIIRegisters -> maxRasterV ()))
			_vicGraphicInfo._ffVBorder = true; // ... the vertical border should appear...
		// ...but if it is in the top limit and the screen hasn't been declared as blank...
		if (atLeft &&
			(_raster.vData ().currentPosition () == _VICIIRegisters -> minRasterV () &&
			 !_VICIIRegisters -> blankEntireScreen ()))
			_vicGraphicInfo._ffVBorder = false; // ... the vertical border should disappear...
	
		// ...and if after all previous checks, the vertical flip flip is off...
		if (atLeft && !_vicGraphicInfo._ffVBorder)
			_vicGraphicInfo._ffLBorder = true; // ...this one is temporal, 
												// ...and when it is used, the main one will become false...

		// This is used later to draw events...if active!
		_eventStatus._ffMBorderChange.set (_vicGraphicInfo._ffMBorder);
	}

	// ---
	inline void VICII::actualizeVerticalBorderStatus ()
	{
		// The border appears when the first vertical raster border position is reached...
		if (_raster.vData ().currentPosition () == _VICIIRegisters -> maxRasterV ()) 
			_vicGraphicInfo._ffVBorder = true;
		// ...and disappears when the first vertical raster visible position is reached...
		if (_raster.vData ().currentPosition () == _VICIIRegisters -> minRasterV () &&
				!_VICIIRegisters -> blankEntireScreen ())
			_vicGraphicInfo._ffVBorder = false;

		// This is used later to draw events...if active!
		_eventStatus._ffVBorderChange.set (_vicGraphicInfo._ffVBorder);
	}

	// ---
	inline size_t VICII::graphicAccessIndex () const
	{ 
		assert (_vicGraphicInfo._GAccessIndex < _GRAPHMAXCHARCOLUMNS); 
		
		return ((size_t) _vicGraphicInfo._GAccessIndex);
	}

	// ---
	inline void VICII::readVideoMatrixAndColorRAM ()
	{
		// _VC determines the real VIC-II memory position to read from.
		// _GAccessIndex determines the emulator-side 40-byte buffer position
		// where the fetched data is stored.
		const size_t gAI = graphicAccessIndex ();
		const size_t vC = (size_t) (_vicGraphicInfo._VC & _VCMASK);
	
		_vicGraphicInfo._lastScreenCodeDataRead = 
			_lastVICDataRead =
			_vicGraphicInfo._screenCodeData [gAI] =
				memoryRef () -> value (_VICIIRegisters -> screenMemory () + vC);
		// In the invalid text mode, the bits 6 & 7 won't be used.
		// In invalid bitmap modes 1 & 2, this information won't be used later.
	
		// Color RAM is accessed directly by the VIC-II when fetching matrix data.
		_vicGraphicInfo._lastColorDataRead = 
			_vicGraphicInfo._colorData [gAI] = 
				_colorRAM -> valueDirect (_colorRAMAddress + vC) &
					(_VICIIRegisters -> invalidGraphicMode () ? 0x08 : 0x0f); 
		// In the invalid text mode, only the MG flag is relevant at this point.
	}

	// ---
	inline void VICII::readGraphicalInfo ()
	{
		// _GAccessIndex selects the emulator-side 40-byte graphics buffer slot
		// being filled in this graphics access cycle.
		const size_t gAI = graphicAccessIndex ();
		const unsigned short vC = _vicGraphicInfo._VC & _VCMASK;

		// In idle state, the VIC-II reads from a fixed address depending on the
		// active bank and graphic mode. The fetched byte is stored in the graphics
		// line buffer at the current graphics access index.
		if (idleStateActive ()) 
		{
			_vicGraphicInfo._lastGraphicDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._graphicData [gAI] = 
					_VICIIRegisters -> graphicExtendedColorTextModeActive () 
						? memoryRef () -> value (_MEMORYPOSIDLE1 + (bank () << 14))
						: memoryRef () -> value (_MEMORYPOSIDLE2 + (bank () << 14));
		}
		// In display state, graphic data is read from character memory or bitmap
		// memory. In text modes, the character code comes from the Video Matrix
		// buffer slot previously fetched for the same graphics access index.
		else 
		{
			_vicGraphicInfo._lastGraphicDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._graphicData [gAI] = _VICIIRegisters -> textMode () 
					? memoryRef () -> value (_VICIIRegisters -> charDataMemory () + 
						(((size_t) _vicGraphicInfo._screenCodeData [gAI].value () & 
							((_VICIIRegisters -> graphicExtendedColorTextModeActive () ||
							  _VICIIRegisters -> invalidGraphicMode ()) ? 0x3f : 0xff))
							/** In extended-background mode or invalid text mode,
								there are only 64 possible characters. */ << 3) + _vicGraphicInfo._RC)
					: memoryRef () -> value (_VICIIRegisters -> bitmapMemory () +
						(((size_t) (vC & 
							(_VICIIRegisters -> invalidGraphicMode () ? 0x033f : _VCMASK))) << 3) +
						(size_t) _vicGraphicInfo._RC);
		}

		// In invalid bitmap modes, bits 6 and 7 of the VC position are not taken
		// into account when forming the bitmap address.
	}

	// ---
	inline void VICII::readSpritesData ()
	{
		for (size_t i = 0; i < 8; i++) 
		{
			// Put it back to null, first...
			_vicSpriteInfo [i]._graphicsLineSprites = MCHEmul::UBytes::_E;
			// ...and update the info if the sprite is active...
			readSpriteData (i); 
		}
	}

	// ---
	inline bool VICII::readSpriteData (size_t nS)
	{
		bool result = false;

		memoryRef () -> setActiveView (_VICIIView);

		// The sprite pointer access is performed regardless of the simplified
		// sprite active flag. This approximates the real VIC-II p-accesses, which
		// are always done. The following three-byte sprite data read approximates
		// the real s-accesses and is currently gated by _active, which still acts
		// as a combined DMA/display flag.
		MCHEmul::UByte sprPtr =
			memoryRef () -> value (_VICIIRegisters -> spritePointersMemory () 
				/** Depends on where the screen is located. */ + nS);
		_lastVICDataRead = sprPtr;
		_vicSpriteInfo [nS]._spriteBaseAddress =
			_VICIIRegisters -> initAddressBank () + ((size_t) sprPtr.value () << 6);

		if (_vicSpriteInfo [nS]._active)
		{
			MCHEmul::UBytes sprData = 
				std::move (MCHEmul::UBytes (memoryRef () -> bytes (_vicSpriteInfo [nS]._spriteBaseAddress + 
					(_vicSpriteInfo [nS]._line * 3) /** bytes per line. */, 3)));
			if (sprData.size () > 0)
				_lastVICDataRead = sprData [sprData.size () - 1]; // Just the last one...
			_vicSpriteInfo [nS]._graphicsLineSprites = std::move (sprData);

			// When new info of the sprite is read, the "draw" condition starts back!
			_vicSpriteInfo [nS]._drawing = false;
			// The value of the _xS doesn't really cares!

			result = true;
		}
		else
			_vicSpriteInfo [nS]._graphicsLineSprites = MCHEmul::UBytes::_E;

		memoryRef () -> setCPUView ();

		return (result);
	}

	/** The version for PAL systems. 
		It couldn't be final. */
	class VICII_PAL : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static const unsigned short _CYCLESPERRASTERLINE = 63;

		VICII_PAL (int intId, MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV);

		private:
		virtual unsigned int treatRasterCycle () override;
	};

	/** The version for NTSC systems. \n
		It couldn't be final. */
	class VICII_NTSC : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICII_NTSC (int intId, MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV);

		private:
		virtual unsigned int treatRasterCycle () override;
	};
}

#endif
  
// End of the file
/*@}*/
