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

		/** CPU Cycles when the graphics are read. */
		static const unsigned int _CPUCYCLESWHENREADGRAPHS	= 43;  // 40 for reading plus 3 blocking the end of 6510 access
		/** Same when the sprites are read. */
		static const unsigned int _CPUCYCLESWHENREADSPRITES	= 3;	// 3 per sprite...

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
			The VICII constructor receives info over the raster data, the memory view to use and additional attributes. */
		VICII (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, const MCHEmul::Attributes& attrs = { });

		virtual ~VICII () override;

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }

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

		virtual bool simulate (MCHEmul::CPU* cpu) override;
		bool simulate_I (MCHEmul::CPU* cpu);
		bool simulate_II (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * Registers	= InfoStructure: Info about the registers.
		  * Raster		= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** To simplify the use of the routines dedicated to draw. */
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

		/** @see DrawContext structure. */
		void drawGraphicsSpritesAndDetectCollisions (unsigned short cv, unsigned short cav, unsigned short rv);

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Read screen data
		/** To read the graphics info. \n
			This method will use the ones below. */
		inline void readGraphicsInfoAt (unsigned short gl /** screen line, including the effect of the scroll y. */);
		/** The same but to empty the list of info. */
		inline void emptyGraphicsInfo ();
		/** Read the chars present in the video matrix. The vale received goes fom 0 to 24. */
		const MCHEmul::UBytes& readScreenCodeDataAt (unsigned short l) const
							{ return (_graphicsScreenCodeData = std::move (
								memoryRef () -> values (_VICIIRegisters -> screenMemory () +
									((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS))); }
		/** Read the info for the chars received as parameter. 
			The method receives also a parameter to indicate whether the graphics mode is or not _EXTENDEDBACKGROUNDMODE. */
		inline const MCHEmul::UBytes& readCharDataFor (const MCHEmul::UBytes& chrs, bool eM = false) const;
		/** Read th info of the bitmap. 
			The info is read as they were char data. That is, the 8 x 8 block are sequential. 
			The value receive gos from 0 to 199. */
		inline const MCHEmul::UBytes& readBitmapDataAt (unsigned short l) const;
		/** Reads the color of the chars. The _COLORMEMORY is fixed and cann't be changed using VICRegisters. 
			Th value ecived goes from 0 to 24. */
		const MCHEmul::UBytes& readColorDataAt (unsigned short l) const
							{ return (_graphicsColorData = std::move (memoryRef () -> values (_COLORMEMORY +
								((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS))); }

		// Read sprites data
		/** Read the sprites data (only for the active ones. \n
			The list of the sprites active (internal variable) is also actualized (not returned) = _spritesEnabled. \n
			nS = number of sprites read. */
		inline const std::vector <MCHEmul::UBytes>& readSpriteData (size_t& nS) const;
		/** Read the info of the active sprites at one specific raster line. \n
			nS = number of sprites read. */
		inline const std::vector <MCHEmul::UBytes>& readSpriteDataAt (unsigned short l, size_t& nS) const;

		// Draw the graphics in detail...
		/** To draw any type of graphic. \n
			This method uses the ones below. \n
			The method receives:
			dC = other info about the raster used to finally draw. */
		MCHEmul::UByte drawGraphics (const DrawContext& dC);
		/** Draws a monocolor char. \n
			All methods receive: \n
			cb = window column adjusted by the scrollX value where to start to draw 8 pixels. \n
			rc = window row adjusted by the scrollY value where to draw. \n
			dC = other info about the raster used to finally draw. */
		MCHEmul::UByte drawMonoColorChar (int cb, int rc, 
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC);
		/** Draws a multicolor char. */
		MCHEmul::UByte drawMultiColorChar (int cb, int rc,
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC, bool blk = false);
		/** Draws an enhaced multicolor char. */
		MCHEmul::UByte drawMultiColorExtendedChar (int cb, int rc,
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC);
		/** Draws a monocolor bitmap. */
		MCHEmul::UByte drawMonoColorBitMap (int cb, int rc, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const DrawContext& dC, bool blk = false);
		/** Draws a multicolor bitmap. */
		MCHEmul::UByte drawMultiColorBitMap (int cb, int rc, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC, bool blk = false);
		
		// Draw the sprites in detail...
		/** To draw the sprites: \n
			This method uses the ones below. \n
			The method receives:
			spr = the number of sprite to draw. \n
			dC = other info about the raster used to finally draw. */
		MCHEmul::UByte drawSprite (size_t spr, const DrawContext& dC);
		/** Draws a monocolor sprite line. \n
			all methos receives: \n 
			c = window column value where to start to draw 8 pixels. \n
			r = window row value where to draw. \n
			spr = the number of sprite to draw. \n
			dC = more info about the raster. */
		MCHEmul::UByte drawMonoColorSprite (int c, int r, size_t spr, const DrawContext& dC);
		/** Draws a multocolor sprite line. */
		MCHEmul::UByte drawMultiColorSprite (int c, int r, size_t spr, const DrawContext& dC);

		/** Detect the collision between graphics and sprites info
			affecting the right registers in the VICII. */
		void detectCollisions (const MCHEmul::UByte& g, const std::vector <MCHEmul::UByte>& s);

		private:
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIIView;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** The number of bytes drawn should be the same than the number of CPU cycles happened
			since the last invokation to the "simulate method". */
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
			readCharDataFor (_graphicsScreenCodeData, _VICIIRegisters -> graphicExtendedColorTextModeActive ()); // ...and then the detailed info...
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
	inline const std::vector <MCHEmul::UBytes>& VICII::readSpriteData (size_t& nS) const
	{
		_graphicsSprites = 
			std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes::_E);

		nS = 0;

		MCHEmul::Address sP = 
			_VICIIRegisters -> spritePointersMemory (); // Will depend on where the screen memory is located...
		MCHEmul::Address iAB = _VICIIRegisters -> initAddressBank ();

		for (size_t i = 0; i < 8; i++)
		{
			if (_VICIIRegisters -> spriteEnable ((size_t) i))
			{ 
				nS++;

				_graphicsSprites [i] = std::move (
					MCHEmul::UBytes (memoryRef () -> bytes (iAB + 
						((size_t) memoryRef () -> value (sP + i).value () << 6 /** 64 blocks. */), 63 /** size in bytes. */)));
			}
		}

		return (_graphicsSprites);
	}

	// ---
	inline const std::vector <MCHEmul::UBytes>& VICII::readSpriteDataAt (unsigned short l, size_t& nS) const
	{
		_graphicsLineSprites = 
			std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes::_E);

		nS = 0;

		MCHEmul::Address sP = 
			_VICIIRegisters -> spritePointersMemory (); // Will depend on where the screen memory is located...
		MCHEmul::Address iAB = _VICIIRegisters -> initAddressBank ();

		for (size_t i = 0; i < 8; i++)
		{ 
			if (_VICIIRegisters -> spriteEnable (i)) // Has to be active...
			{
				// Can be negative...(reason to use an int)
				// If some part of the sprite were visible in the line...
				int lY = l - _VICIIRegisters -> spriteYCoord (i) - 1;
				if (lY >= 0 && 
					lY < (_VICIIRegisters -> spriteDoubleHeight (i) ? 42 /** When double high. */ : 21))
				{ 
					// The information of that specific sprite line would be read...
					nS++;

					_graphicsLineSprites [i] = std::move (
						MCHEmul::UBytes (
							memoryRef () -> bytes (iAB + 
							((size_t) memoryRef () -> value (sP + i).value () << 6) /** 64 bytes block size. */ + 
							/** If sprite is double-height, the data line read must be half. */
							(((((unsigned int) lY) >> (_VICIIRegisters -> spriteDoubleHeight (i) ? 1 : 0))) * 3 /** bytes per line. */), 3)));
				}
			}
		}

		return (_graphicsLineSprites);
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

		VICII_PAL (int vV);
	};
}

#endif
  
// End of the file
/*@}*/
