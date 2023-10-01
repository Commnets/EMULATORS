/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICII.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: The VICII Chip.
 *	Versions: 1.0 Initial
 *	Based on https://www.cebix.net/VIC-Article.txt.
 */

#ifndef __COMMODORE_VICII__
#define __COMMODORE_VICII__

#include <CORE/incs.hpp>
#include <COMMODORE/VICIIRegisters.hpp>

namespace COMMODORE
{
	/** The chip that takes care of anything around the graphics in Commodore 64. 
		@see GraphicalChip. */
	class VICII : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 104;

		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE	= 0x30;
		static const unsigned short _LASTBADLINE	= 0xf7;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25;	// Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320;	// Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** Static address. The color memory can not be changed. */
		static const MCHEmul::Address _COLORMEMORY;

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
			The VICII constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		VICII (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~VICII () override;

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

		/** To set the position of the light - pen. \n
			The position received must be relative within the display zone. */
		void lightPenPosition (unsigned short& x, unsigned short& y) const
							{ _VICIIRegisters -> currentLightPenPosition (x, y); }
		void setLightPenPosition (unsigned short x, unsigned short y)
							{ _VICIIRegisters -> setCurrentLightPenPosition (x, y); }

		/** To know whether the light pen is active. */
		bool lightPenActive () const
							{ return (_VICIIRegisters -> lightPenActive ()); }
		void setLightPenActive (bool lP)
							{ _VICIIRegisters -> setLigthPenActive (lP); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** Simulates cycles in the VICII. \n
			It draws the border AFTER once graphics info has been drawn within the display zone. \n
			So sprites can be drawn behing the border and collisions could take place out of the visible zone. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters	= InfoStructure: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics & Sprites in detail...
		/** To simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _ICS;	// Initial Column Screen (Screen = Display with reductions in the size considered).
			unsigned short _LCD;	// Last Column of the Display 
			unsigned short _LCS;	// Last Column of the Screen
			unsigned short _SC;		// Scroll X
			unsigned short _RC;		// Raster X (from the beginning of the visible zone)
			unsigned short _RCA;	// Raster X adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _IRD;	// Initial Row Display (Not taken into account reductions of the size)
			unsigned short _IRS;	// Initial Row Screen (Screen = Display with reductions of size taken into account).
			unsigned short _LRD;	// Last Row of the Display 
			unsigned short _LRS;	// Last Row of the Screen
			unsigned short _SR;		// Scroll Y
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed)
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The data used to detect the collisions. */
			MCHEmul::UByte _collisionData;
			/** The color of the each pixel considered as background. */
			unsigned int _backgroundColorData [8];
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];
			/** To indicate whether it is or not consecuencia of a "bad mode",
				and the has to be everything draw in black, but taking into account the real graphics behaind. */
			bool _invalid;

			DrawResult (unsigned int bC = ~0 /** Meaning transparent (jumped later when moving info to screen). */)
				: _collisionData (MCHEmul::UByte::_0),
				  _backgroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _foregroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _invalid (false)
							{ }
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. \n
			Returns the number of cycles that, as a consequence of dealing with a raster line, 
			the CPU should be stopped. \n
			The way the raster cycles are treated will depend (somehow) on the type of VICII chip. */
		virtual unsigned int treatRasterCycle ();
		/** Treat the viciel zone.
			Draws the graphics, detect collions, and finally draw the border. */
		void drawVisibleZone (MCHEmul::CPU* cpu);
		/** Invoked from the previous one, just to draw and detect collisions. \n
		  *	The parameters are the context of the draw. \n
		  *	@see also DrawContext and DrawResult structures. */
		void drawGraphicsSpritesAndDetectCollisions (const DrawContext& dC);

		// These all methods are invoked from the three ones above!
		// They are here just to structure better the code...
		// Read screen data
		// Methods linked to the raster line to the read the graphics...
		/** To read the video matrix and the RAM color. \n
			Someting that happens during a badline. */
		inline void readVideoMatrixAndColorRAM ();
		/** To read the graphical info, considerig the info read in the previous method. \n
			This method is executed per raster cycles. */
		inline void readGraphicalInfo ();

		// Read sprites data
		// The sprite data is read a long as the raster cycle progresses.
		// The info is read attending to the contecytt of the interval variable _vicSpriteInfo (@see below)
		/** Read the graphical info of the active sprites. */
		inline void readSpritesData ();
		/** Method used from the previous method to read the info of one sprite only. */
		inline bool readSpriteData (size_t nS);

		/** To draw any type of graphic \n
			This method uses the ones below it \n
			The method receives: \n
			dC = other info about the raster used to finally draw. \n
			They all return a instance of DrawResult (@see above). */
		DrawResult drawGraphics (const DrawContext& dC);
		/**
		  *	Draws a monocolor char. \n
		  *	All methods receive: \n
		  *	cb	= window column adjusted by the scrollX value where to start to draw 8 pixels. \n
		  *	rc	= window row adjusted by the scrollY value where to draw. \n
		  *	also some of those:
		  *	bt	= The bits to draw, from the graphical memory. \n
		  *	clr = The color of those bits, from the ram color memory. \n
		  *	sc  = The screen codes, from the matrix memory. \n
		  *	and, some of them:
		  *	blk = When the graphical mode is inavlid and nothing has to be drawn.
		  */
		DrawResult drawMonoColorChar (int cb);
		/** Draws a multicolor char. */
		DrawResult drawMultiColorChar (int cb);
		/** Draws an enhaced multicolor char. */
		DrawResult drawMultiColorExtendedChar (int cb);
		/** Draws a monocolor bitmap. */
		DrawResult drawMonoColorBitMap (int cb);
		/** Draws a multicolor bitmap. */
		DrawResult drawMultiColorBitMap (int cb);
		
		// Draw the sprites in detail...
		/** 
		  *	To draw the sprites: \n
		  *	This method uses the ones below. \n
		  *	All of them receive:
		  *	c = raster column where to start to draw 8 pixels. \n
		  *	r = raster line where to draw. \n
		  *	spr = the number of sprite to draw. \n
		  *	The reference to the array where to store the color info is also passed. \n
		  *	This array comes from the outcome of drawing the text/bitmaps! \n
		  *	Info to detect collisions later is also returned.
		  */
		MCHEmul::UByte drawSpriteOver (size_t spr, unsigned int* d);
		/** Draws a monocolor sprite line. */
		MCHEmul::UByte drawMonoColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d);
		/** Draws a multicolor sprite line. */
		MCHEmul::UByte drawMultiColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d);

		// The last part...
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. \n
			That text/bitmap info included the sprite data as well ordered. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);
		/** Detect the collision between graphics and sprites info
			affecting the right registers in the VICII. */
		void detectCollisions (const MCHEmul::UByte& g, const std::vector <MCHEmul::UByte>& s);

		protected:
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The difference with the PAL System. */
		unsigned short _incCyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

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
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the VICII register about whether the video ir or not active. \n
			In any other circunstance it kepts its value. */
		bool _videoActive;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
		/** The last value of the SCROLLY variable when a bad line was detected.
			This value is gathered anytime the bad line condition is checked (usually between raster cycles 12 and 52). 
			-1 will mean no previous value to be taken into account. \n
			This variable is set to -1 at the beginning of every raster line. */
		mutable int _lastBadLineScrollY;
		/** When the situation of a new bad line araises, is latched in this variable. \n
			This variabe is desactivated at the end of the line, and when the graphical info is finally read. */
		mutable bool _newBadLineCondition;
		/** This very simple variable manages only when the additional stop bad line related cycles applies. */
		mutable bool _badLineStopCyclesAdded;

		/** 
		  *	Structure to control how the graphics are displayed in the screen. \n
		  *	https://www.cebix.net/VIC-Article.txt. (points 3.7.1 & 3.7.2): \n
		  *	The VICII can be in either "idle" state or "screen" state... \n
		  *	In the idle state c and g accesses (code and color matrix and graphics data) takes place one, 
		  *	Whilest in the second one only access to $3fff memory takes place
		  *	(or $39ff when ECM = multicolor bit in register $d016 is set). \n
		  *	The screen state is set as soon as a bad condition comes. \n
		  *	The idle state is set at cycle 58 if RC == 7 and ther eis no bad condition. \n
		  * There are a couple of important things that happen as the raster line moves accross the line. \n
		  *	The rules to manipulate every value are decribed below attending to the cycle in the raster. \n
		  * There are 3 important registers within the VICII related with the graphics: \n
		  * VCBASE moves from 0 to 1000 (40 * 25) in a 40 step length. \n
		  * VC moves from 0 to 1000 1 by 1. \n
		  * Both will help to determine the position within the video matrix/color RAM to read. \n
		  * RC counts from 0 to 7 one by one and determines the line the graphics data RAM where to find the final info. \n
		  * RASTER LINE 0:			VCBASE is set to 0. \n
		  * RASTER LINE $30 - $f7: \n
		  *		CYCLE 14:			VC = VCBASE. If Bad Line => RC = 0 \n
		  *		CYCLES 15 - 54:		VC = VC + 1 \n
		  *		CYCLE 58:			if RC == 7 => VCBASE = VC; RC = RC + 1 \n
		  * So VC goes from 0 to 40 7 times before VBASE incrementes in 40,
		  * then 7 times more from 40 to 80 before VCBASE moves to 80 and so on and so forth.
		  */
		struct VICGraphicInfo
		{
			VICGraphicInfo ()
				: _VCBASE (0), _VC (0), _VLMI (0),
				  _RC (0),
				  _idleState (true),
				  _screenCodeData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _graphicData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0))
							{ }

			void emptyVideoMatrixAndColorRAMData ()
							{ _screenCodeData	= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _colorData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }
			
			void emptyGraphicData ()
							{ _graphicData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }

			unsigned short _VCBASE, _VC, _VLMI;
			unsigned char _RC;
			bool _idleState; 
			mutable MCHEmul::UBytes _screenCodeData;
			mutable MCHEmul::UBytes _graphicData; 
			mutable MCHEmul::UBytes _colorData;
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
				  _ff (false)
							{ }

			VICSpriteInfo (bool a, unsigned char l, bool e)
				: _active (a), _line (l), _expansionY (e),
				  _graphicsLineSprites (MCHEmul::UBytes::_E),
				  _ff (false)
							{ }

			bool _active; // True when the sprite is active in the current raster line
			unsigned char _line; // Line of the sprite to be drawn (from 0 to 21)
			bool _expansionY; // True when the sprite is expanded in the Y axis
			mutable MCHEmul::UBytes _graphicsLineSprites; // 3 bytes line info each

			// Implementation
			bool _ff; // Used in controlling how _line is incremented (per line)
		};

		VICSpriteInfo _vicSpriteInfo [8];

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	inline void VICII::readVideoMatrixAndColorRAM ()
	{
		memoryRef () -> setActiveView (_VICIIView);
		_vicGraphicInfo._screenCodeData [_vicGraphicInfo._VLMI] =
			memoryRef () -> value (_VICIIRegisters -> screenMemory () + (size_t) _vicGraphicInfo._VC); 
		_vicGraphicInfo._colorData [_vicGraphicInfo._VLMI] = 
			memoryRef () -> value (_COLORMEMORY + (size_t) _vicGraphicInfo._VC); 
		memoryRef () -> setCPUView ();
	}

	// ---
	inline void VICII::readGraphicalInfo ()
	{
		memoryRef () -> setActiveView (_VICIIView);
		if (_vicGraphicInfo._idleState) // In this state the info is read from a specific place of the memory...
			_vicGraphicInfo._graphicData [_vicGraphicInfo._VLMI] = 
			_VICIIRegisters -> graphicExtendedColorTextModeActive () 
				? memoryRef () -> value (_MEMORYPOSIDLE1) : memoryRef () -> value (_MEMORYPOSIDLE2);
		else // ..in the other one the info will be read attending to the situation of the memory...
			_vicGraphicInfo._graphicData [_vicGraphicInfo._VLMI] = _VICIIRegisters -> textMode () 
				? memoryRef () -> value (_VICIIRegisters -> charDataMemory () + 
					(((size_t) _vicGraphicInfo._screenCodeData [_vicGraphicInfo._VLMI].value () & 
						(_VICIIRegisters -> graphicExtendedColorTextModeActive () ? 0x3f : 0xff)) 
						/** In the extended graphics mode there is only 64 chars possible. */ << 3) + _vicGraphicInfo._RC)
				: memoryRef () -> value (_VICIIRegisters -> bitmapMemory () + 
					(_vicGraphicInfo._VC << 3) + _vicGraphicInfo._RC);
		memoryRef () -> setCPUView ();
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
		if (!_vicSpriteInfo [nS]._active)
			return (false);

		memoryRef () -> setActiveView (_VICIIView);
		_vicSpriteInfo [nS]._graphicsLineSprites = std::move (
			MCHEmul::UBytes (
				memoryRef () -> bytes (_VICIIRegisters -> initAddressBank () + 
					((size_t) memoryRef () -> value 
						(_VICIIRegisters -> spritePointersMemory () /** Depnds on where the escreen is located. */ + nS).value () << 6) /** 64 bytes block. */ +
					/** If sprite is double-height, the data line read must be half. */
					(_vicSpriteInfo [nS]._line * 3) /** bytes per line. */, 3)));
		memoryRef () -> setCPUView ();

		return (true);
	}

	/** The version para NTSC systems. */
	class VICII_NTSC final : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICII_NTSC (int vV);

		private:
		virtual unsigned int treatRasterCycle () override;
	};

	/** The version para PAL systems. */
	class VICII_PAL final : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 63;

		VICII_PAL (int vV);

		private:
		virtual unsigned int treatRasterCycle () override;
	};
}

#endif
  
// End of the file
/*@}*/
