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
	/** The chip that takes care of anything around the graphics in Commodore 64. @see GraphicalChip. */
	class VICII : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 104;

		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE	= 0x30;
		static const unsigned short _LASTBADLINE	= 0xf7;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25; // Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320; // Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** Static address. The color memory cann't be changed. */
		static const MCHEmul::Address _COLORMEMORY;

		// Some events.
		/** As the VICII only addresses 16k and some computers where it is connected to admitis 64,
			there is the possibility to change the bank. \n
			The unsigned ints associated to each must be consecutive for everything to work properly. */
		static const unsigned int _BANK0SET = 200;
		static const unsigned int _BANK1SET = 201;
		static const unsigned int _BANK2SET = 202;
		static const unsigned int _BANK3SET = 203;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICII constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (changes per version) and additional attributes. */
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

		/** To know ehether the light pen is ative or not. */
		bool lightPenActive () const
							{ return (_VICIIRegisters -> lightPenActive ()); }
		void setLightPenActive (bool lP)
							{ _VICIIRegisters -> setLigthPenActive (lP); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** Draw the border AFTER the graphic info is draw within the display zone. \n
			In this case sprite and graphical info is taken into account to determine collisions. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;
		/** Associated with the previous method. */
		void simulate_BEFOREVISIBLEZONE (MCHEmul::CPU* cpu, unsigned int& cS);
		void simulate_VISIBLEZONE (MCHEmul::CPU* cpu, unsigned int& cS);
		void simulate_AFTERVISIBLEZONE (MCHEmul::CPU* cpu, unsigned int &cS);
		void drawInVisibleZone (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * Registers	= InfoStructure: Info about the registers.
		  * Raster		= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** 
		  * @see DrawContext and @DraResult structure for a better understanding. 
		  *	The parameters are:
		  *	@param cv	The raster beam X position within the visible window. 0 is the first value.
		  *	@param cav	The same raster beam X position but adjusted to a multiple of 8. 
		  *	@param rv	The raster beam Y position with the visinle window. O is the first value.
		  */
		void drawGraphicsSpritesAndDetectCollisions 
			(unsigned short cv, unsigned short cav, unsigned short rv);

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Read screen data
		/** To read the graphics info. \n
			This method will use the ones below. */
		inline void readGraphicsInfoAt (unsigned short gl /** screen line, including the effect of the scroll y. */);
		/** To empty the list of info. */
		inline void emptyGraphicsInfo ();
		/** Read the chars present in the video matrix. \n
			The parameter received goes fom 0 to 24 (visible C64 line). */
		const MCHEmul::UBytes& readScreenCodeDataAt (unsigned short l) const
							{ return (_graphicsScreenCodeData = std::move (
								memoryRef () -> values (_VICIIRegisters -> screenMemory () +
									((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS))); }
		/** Read the info for the chars received as parameter. \n
			The method receives also a parameter to indicate whether the graphics mode is or not _EXTENDEDBACKGROUNDMODE,
			because in that case the info is read slightly different. */
		inline const MCHEmul::UBytes& readCharDataFor (const MCHEmul::UBytes& chrs, bool eM = false) const;
		/** Read the info of the bitmap. \n
			The info is read as they were char data. That is, the 8 x 8 block are sequential. 
			The parameter received goes from 0 to 199 (visible scan line ((0 - 25 lines) * 8) - 1, and
			takes into account the effect of the SCROLLY variable. */
		inline const MCHEmul::UBytes& readBitmapDataAt (unsigned short l) const;
		/** Reads the color of the chars. \n
			The _COLORMEMORY localtion is fixed and can not be changed using VICRegisters. 
			Th parameter received goes from 0 to 24. */
		const MCHEmul::UBytes& readColorDataAt (unsigned short l) const
							{ return (_graphicsColorData = std::move (memoryRef () -> values (_COLORMEMORY +
								((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS))); }

		// Read sprites data
		/** To know the line sprite data to read in a raster line. \n
			The method receives the raster line and the number of sprite. \n
			The result will be positive (indicating the line of data to read) when the sprite is present in the rasterline
			and -1 when the sprite is not present in the raster line because 
			either is not availble or it is not visible in that line. */
		inline int spriteLineDataAt (unsigned short l, size_t nS) const;
		/** Read the graphical info of the active sprites at one specific raster line. \n
			The parameter received goes from 0 to PAL/NTSC maximum raster line,
			and doesn't take into account the SCROLLY value. */
		inline const std::vector <MCHEmul::UBytes>& readSpritesDataAt (unsigned short l) const;
		/** Method used by the method previous to read the info of a sprite only. 
			It returns true when the sprite data was read, and false in other case. */
		inline bool readSpriteDataAt (unsigned short l, size_t nS) const;

		// Draw the graphics & Sprites in detail...
		/** To simplify the use of some of the routines dedicated to draw. */
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
			unsigned short _RR;		// Raster Y (Moves 1 by 1. No adjusted neadd)
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color ar e kept in 
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
		DrawResult drawMonoColorChar (int cb, int rc, 
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr);
		/** Draws a multicolor char. */
		DrawResult drawMultiColorChar (int cb, int rc,
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr);
		/** Draws an enhaced multicolor char. */
		DrawResult drawMultiColorExtendedChar (int cb, int rc,
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr);
		/** Draws a monocolor bitmap. */
		DrawResult drawMonoColorBitMap (int cb, int rc, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt);
		/** Draws a multicolor bitmap. */
		DrawResult drawMultiColorBitMap (int cb, int rc, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr);
		
		// Draw the sprites in detail...
		/** 
		  *	To draw the sprites: \n
		  *	This method uses the ones below. \n
		  *	All of them receive:
		  *	c = window column value where to start to draw 8 pixels. \n
		  *	r = window row value where to draw. \n
		  *	spr = the number of sprite to draw. \n
		  *	The reference to the array where to store the color is also passed. \n
		  *	This array comes from the outcome of drawing the text/bitmaps! \n
		  *	Info to detect collisions is also returned. 
		  */
		MCHEmul::UByte drawSpriteOver (size_t spr, unsigned int* d, const DrawContext& dC);
		/** Draws a monocolor sprite line. */
		MCHEmul::UByte drawMonoColorSpriteOver (int c, int r, size_t spr, unsigned int* d);
		/** Draws a multicolor sprite line. */
		MCHEmul::UByte drawMultiColorSpriteOver (int c, int r, size_t spr, unsigned int* d);

		// The last part...
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. \n
			That text/bitmap info included the sprite data as well ordered. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);
		/** Detect the collision between graphics and sprites info
			affecting the right registers in the VICII. */
		void detectCollisions (const MCHEmul::UByte& g, const std::vector <MCHEmul::UByte>& s);

		// Very internal methods
		// These methods are used in the simulation...
		/** To know whether the current raster line is a bad line. */
		inline bool isBadLine () const;
		/** To know whether the to VICII is about to initiate a reading activity for graphical or sprites info. */
		inline bool isAboutToReadGraphicalInfo () const;

		private:
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** When the CPU is not stopped (sometimes the VIC requires to stop it). \n 
			and a instruction is executed, the number of cycles that that instruction required, has to be taken into account
			to define qhat the VICII has to do. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** The bytes read describing a line of graphics. 
			They are all actualized at the methods readXXXX. */
		mutable MCHEmul::UBytes _graphicsScreenCodeData;
		mutable MCHEmul::UBytes _graphicsGraphicData;
		mutable MCHEmul::UBytes _graphicsColorData;
		mutable std::vector <MCHEmul::UBytes> _graphicsSprites; // Eight sprites...
		mutable std::vector <MCHEmul::UBytes> _graphicsLineSprites; // Eight sprites...
		/** Whenever a new raster line is reached, this variable becomes true. */
		bool _isNewRasterLine; 
		/** When a raster line is processed is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) cyclesPerRasterLine */
		unsigned short _cycleInRasterLine;
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the VICII register about whether the video ir or not active. \n
			In any other circunstance is kept its value. */
		bool _videoActive;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
	};

	// ---
	inline void VICII::emptyGraphicsInfo ()
	{
		_graphicsScreenCodeData = std::move (MCHEmul::UBytes ());
		_graphicsGraphicData = std::move (MCHEmul::UBytes ());
		_graphicsColorData = std::move (MCHEmul::UBytes ());
	}
	
	// ---
	inline void COMMODORE::VICII::readGraphicsInfoAt (unsigned short gl)
	{
		unsigned short chrLine = gl >> 3;

		// In real VIC II color is read at the same time than the graphics data
		// The color memory is always at the same location (only visible from VICII)
		readColorDataAt (chrLine);

		// Load _graphicsScreenCodeData first..
		readScreenCodeDataAt (chrLine); 
		// ...and if it is a text mode...
		if (_VICIIRegisters -> textMode ()) 
			readCharDataFor (_graphicsScreenCodeData, 
							 _VICIIRegisters -> graphicExtendedColorTextModeActive ()); // ...and then the detailed info...
		// ...and if it is not, load directly the graphics data...
		else 
			readBitmapDataAt (gl);
	}

	// ---
	const MCHEmul::UBytes& VICII::readCharDataFor (const MCHEmul::UBytes& chrs, bool eM) const
	{
		MCHEmul::Address cDM = _VICIIRegisters -> charDataMemory (); // The key....

		std::vector <MCHEmul::UByte> dt;
		for (const auto& i : chrs.bytes ())
		{
			std::vector <MCHEmul::UByte> chrDt = std::move (
				memoryRef () -> bytes (cDM + (((size_t) i.value () & (eM ? 0x3f : 0xff)) 
					/** In the extended graphics mode there is only 64 chars possible. */ << 3), 8));
			dt.insert (dt.end (), std::make_move_iterator (chrDt.begin ()), std::make_move_iterator (chrDt.end ()));
		}

		return (_graphicsGraphicData = std::move (MCHEmul::UBytes (dt)));
	}

	// ---
	inline const MCHEmul::UBytes& VICII::readBitmapDataAt (unsigned short l) const
	{
		MCHEmul::Address bDM = _VICIIRegisters -> bitmapMemory ();

		std::vector <MCHEmul::UByte> dt;
		unsigned short cL = l * _GRAPHMAXCHARCOLUMNS;
		for (unsigned short i = 0; i < _GRAPHMAXCHARCOLUMNS; i++)
		{
			std::vector <MCHEmul::UByte> btDt = std::move (
				memoryRef () -> bytes (bDM + (cL + ((size_t) i << 3)), 8));
			dt.insert (dt.end (), std::make_move_iterator (btDt.begin ()), std::make_move_iterator (btDt.end ()));
		}

		return (_graphicsGraphicData = std::move (MCHEmul::UBytes (dt)));
	}

	// ---
	inline int VICII::spriteLineDataAt (unsigned short l, size_t nS) const
	{
		int lY = l - _VICIIRegisters -> spriteYCoord (nS) - 1;
		return ((_VICIIRegisters -> spriteEnable (nS) &&
			(lY >= 0 && lY < (_VICIIRegisters -> spriteDoubleHeight (nS) ? 42 /** When double high. */ : 21))) ? lY : -1);
	}

	// ---
	inline const std::vector <MCHEmul::UBytes>& VICII::readSpritesDataAt (unsigned short l) const
	{
		_graphicsLineSprites = 
			std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes::_E);
		for (size_t i = 0; i < 8; 
			readSpriteDataAt (l, i++)); // _graphicsLineSprites is updated...
		return (_graphicsLineSprites);
	}

	// ---
	inline bool VICII::readSpriteDataAt (unsigned short l, size_t nS) const
	{
		bool result = false;

		int lY = spriteLineDataAt (l, nS);
		if (result = (lY != -1))
		{
			MCHEmul::Address sP = 
				_VICIIRegisters -> spritePointersMemory (); // Will depend on where the screen memory is located...
			MCHEmul::Address iAB = _VICIIRegisters -> initAddressBank ();
			_graphicsLineSprites [nS] = std::move (
				MCHEmul::UBytes (
					memoryRef () -> bytes (iAB + 
						((size_t) memoryRef () -> value (sP + nS).value () << 6) /** 64 bytes block size. */ + 
						/** If sprite is double-height, the data line read must be half. */
						(((((unsigned int) lY) >> (_VICIIRegisters -> spriteDoubleHeight (nS) ? 1 : 0))) * 3 /** bytes per line. */), 3)));
		}

		return (result);
	}

	// ---
	inline bool VICII::isBadLine () const
	{
		return (_videoActive && 
				_raster.currentLine () >= _FIRSTBADLINE &&
				_raster.currentLine () <= _LASTBADLINE && 
				(_raster.currentLine () & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ());
	}

	/** The version para NTSC systems. */
	class VICII_NTSC final : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICII_NTSC (int vV);
	};

	/** The version para PAL systems. */
	class VICII_PAL final : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 63;

		VICII_PAL (int vV);
	};
		
	// ---
	inline bool VICII::isAboutToReadGraphicalInfo () const
	{
		unsigned short nL = _raster.nextLine ();
		return (
			(_cycleInRasterLine == 2  && spriteLineDataAt (_raster.currentLine (), 5) != -1) ||
			(_cycleInRasterLine == 4  && spriteLineDataAt (_raster.currentLine (), 6) != -1) ||
			(_cycleInRasterLine == 6  && spriteLineDataAt (_raster.currentLine (), 7) != -1) ||
			(_cycleInRasterLine == 12 && isBadLine ()) ||
			(_cycleInRasterLine == (55 + // To addapt it to the type of Chip...
				(_cyclesPerRasterLine - VICII_PAL::_CYCLESPERRASTERLINE)) && spriteLineDataAt (nL, 0) != -1) ||
			(_cycleInRasterLine == (57 + 
				(_cyclesPerRasterLine - VICII_PAL::_CYCLESPERRASTERLINE)) && spriteLineDataAt (nL, 1) != -1) ||
			(_cycleInRasterLine == (59 + 
				(_cyclesPerRasterLine - VICII_PAL::_CYCLESPERRASTERLINE)) && spriteLineDataAt (nL, 2) != -1) ||
			(_cycleInRasterLine == (61 + 
				(_cyclesPerRasterLine - VICII_PAL::_CYCLESPERRASTERLINE)) && spriteLineDataAt (nL, 3) != -1) ||
			(_cycleInRasterLine == (63 + 
				(_cyclesPerRasterLine - VICII_PAL::_CYCLESPERRASTERLINE)) && spriteLineDataAt (nL, 4) != -1));
	}
}

#endif
  
// End of the file
/*@}*/
