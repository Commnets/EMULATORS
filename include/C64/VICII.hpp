/** \ingroup VICII */
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

#ifndef __C64_VICII__
#define __C64_VICII__

#include <CORE/incs.hpp>
#include <C64/VICIIRegisters.hpp>

namespace C64
{
	/** The chip that takes care of anything around the graphics in Commodore 64. @see GraphicalChip. */
	class VICII : public MCHEmul::GraphicalChip
	{
		public:
		/** The RasterData describes the infomation the raster need to move across the screen. */
		class RasterData final : public MCHEmul::InfoClass
		{
			public:
			RasterData () = delete;

			RasterData (
						unsigned short fp,		// First position
						unsigned short fvp,		// First Visible position
						unsigned short fdp,		// First Display position
						unsigned short ldp,		// Last Display position
						unsigned short lvp,		// Last Visible position
						unsigned short lp,		// Last position
						unsigned short mp,		// Maximum positions
						unsigned short pr1,		// Positions to reduce in the visible zone 1 & 2
						unsigned short pr2
					  );

			RasterData (const RasterData&) = default;

			RasterData& operator = (const RasterData&) = default;

			unsigned short currentPosition () const
							{ return (_currentPosition); }
			unsigned short currentPositionAtBase0 () const
							{ return (_currentPosition_0); }

			// Managing the blank zone...
			bool isInBlankZone () const
							{ return ((_currentPosition_0 >= _firstPosition_0 && 
											_currentPosition_0 < _firstVisiblePosition_0) ||
									  (_currentPosition_0 > _lastVisiblePosition_0 && 
											_currentPosition_0 <= _lastPosition_0)); }
			bool isInLastBlankZone () const
							{ return (_currentPosition_0 > _lastVisiblePosition_0 && 
									  _currentPosition_0 <= _lastPosition_0); }

			// Managin the visible zone...
			bool isInVisibleZone () const 
							{ return ((_currentPosition_0 >= _firstVisiblePosition_0 && 
									   _currentPosition_0 <= _lastVisiblePosition_0)); }
			unsigned short currentVisiblePosition () const // The 
							{ return (_currentPosition_0 - _firstVisiblePosition_0); }
			unsigned short visiblePositions () const
							{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0 + 1); }
			unsigned short currentInVisiblePosition () const
							{ return (_currentPosition_0 - _firstVisiblePosition_0); }

			// Managing the display
			/** The DISPLAY is the zone where the drawing can happen but not taking 
				into account potential reductions neither in the columns nor in the rows. */
			bool isInDisplayZone () const
							{ return (_currentPosition_0 >= _originalFirstDisplayPosition_0 && 
									  _currentPosition_0 <= _originalLastDisplayPosition_0); }
			unsigned short firstDisplayPosition () const
							{ return (_originalFirstDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short lastDisplayPosition () const
							{ return (_originalLastDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short displayPositions () const
							{ return (_originalLastDisplayPosition_0 - _originalFirstDisplayPosition_0 + 1); }

			// Managing the screen
			/** The SCREEN is the zone where the drawing can happen but taking
				into account potential reductions either in the columns on in the rows. */
			bool isInScreenZone () const
							{ return (_currentPosition_0 >= _firstDisplayPosition_0 &&
									  _currentPosition_0 <= _lastDisplayPosition_0); }
			unsigned short firstScreenPosition () const
							{ return (_firstDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short lastScreenPosition () const
							{ return (_lastDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short screenPositions () const
							{ return (_lastDisplayPosition_0 - _firstDisplayPosition_0 + 1); }

			/** Returns true when the limit of the raster is reached. 
				The parameter is the number of positions to increment the rasterData. */
			bool add (unsigned short i);
			bool next ()
							{ return (add (1)); }

			/** The display zone will reduced in both sides by half of the _positionsToReduce value. */
			void reduceDisplayZone (bool s);
			bool isDisplayZoneReduced () const
							{ return (_displayZoneReduced); }

			void initialize ()
							{ _currentPosition = _firstPosition; _currentPosition_0 = _firstPosition_0; }

			/**
			  *	The name of the fields are: \n
			  * POSITION	= Attribute: Position of the raster.
			  * POSITION_0	= Attribute: Position of the raster in base 0.
			  * FIRST		= Attribute: Initial position of the raster.
			  * LAST		= Attribute: Last position of the raster.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			friend std::ostream& operator << (std::ostream& o, const RasterData& r)
							{ return (o << *((MCHEmul::InfoClass*) &r)); }

			protected:
			/** Internal method used return a value considering the firrst position as 0. */
			unsigned short toBase0 (unsigned short m) const
							{ int t = (int) m - (int) _firstPosition; 
							  return ((t < 0) ? (unsigned short) t + _maxPositions : (unsigned short) t); }

			protected:
			const unsigned short _firstPosition = 0; // Adjusted at construction time.
			const unsigned short _firstVisiblePosition = 0;
			unsigned short _firstDisplayPosition; // Both can be changed by the method reduceDisplayZone...
			const unsigned short _originalFirstDisplayPosition; // Before reducing or extending the area...
			unsigned short _lastDisplayPosition;
			const unsigned short _originalLastDisplayPosition; // Before reduucing or extending the area...
			const unsigned short _lastVisiblePosition = 0;
			const unsigned short _lastPosition = 0;
			const unsigned short _maxPositions = 0;
			const unsigned short _positionsToReduce1 = 0;
			const unsigned short _positionsToReduce2 = 0;

			// Implementation
			// To speeed up calculus...
			unsigned short _firstPosition_0; 
			unsigned short _firstVisiblePosition_0;
			unsigned short _firstDisplayPosition_0; 
			unsigned short _originalFirstDisplayPosition_0; 
			unsigned short _lastDisplayPosition_0;
			unsigned short _originalLastDisplayPosition_0; 
			unsigned short _lastVisiblePosition_0;
			unsigned short _lastPosition_0;

			// Implementation
			unsigned short _currentPosition;
			unsigned short _currentPosition_0;
			bool _displayZoneReduced;
		};

		/** The Raster simulates the set of sequential horizontal lines that, 
			in a CRT monitor, draws an image in the screen. */
		class Raster final : public MCHEmul::InfoClass
		{
			public:
			static const unsigned short _FIRSTBADLINE	= 0x30;
			static const unsigned short _LASTBADLINE	= 0xf7;

			Raster () = delete;

			Raster (const Raster&) = default;

			Raster& operator = (const Raster&) = default;

			Raster (const RasterData& vD, const RasterData& hD)
				: MCHEmul::InfoClass ("Raster"),
				  _vRasterData (vD), _hRasterData (hD)
							{ }

			const RasterData& vData () const
							{ return (_vRasterData); }
			RasterData& vData ()
							{ return (_vRasterData); }
			const RasterData& hData () const
							{ return (_hRasterData); }
			RasterData& hData ()
							{ return (_hRasterData); }

			unsigned short currentLine () const
							{ return (_vRasterData.currentPosition ()); }
			unsigned short currentLineAtBase0 () const
							{ return (_vRasterData.currentPositionAtBase0 ()); }
			unsigned short currentColumn () const
							{ return (_hRasterData.currentPosition ()); }
			unsigned short currentColumnAtBase0 () const
							{ return (_hRasterData.currentPositionAtBase0 ()); }

			/** This method is not complete. It would have to consider 
				the position of the vertical scroll and also whether the display is or not disconnected. */
			bool isInPotentialBadLine () const
							{ return (_vRasterData.currentPosition () >= _FIRSTBADLINE && 
									  _vRasterData.currentPosition () <= _LASTBADLINE); }

			// Managing the blan zone
			bool isInVBlank () const
							{ return (_vRasterData.isInBlankZone ()); }
			bool isInLastVBlank () const
							{ return (_vRasterData.isInLastBlankZone ()); }

			// Managing the visible zone
			/** The visible zone is the complete c64 sreen. 
				The size will be different in PAL and in NTSC. */
			bool isInVisibleZone () const
							{ return (_vRasterData.isInVisibleZone () && _hRasterData.isInVisibleZone ()); }
			unsigned short visibleLines () const
							{ return (_vRasterData.visiblePositions ()); }
			unsigned short visibleColumns () const
							{ return (_hRasterData.visiblePositions ()); }
			void currentVisiblePosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.currentVisiblePosition (); y = _vRasterData.currentVisiblePosition (); }
			void currentInVisiblePosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.currentInVisiblePosition (); y = _vRasterData.currentInVisiblePosition (); }

			// Managing the display zone
			/** The display is where drawing is possible. The reduced zones if any are not considered. */
			bool isInDisplayZone () const
							{ return (_vRasterData.isInDisplayZone () && _hRasterData.isInDisplayZone ()); }
			void firstDisplayPosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.firstDisplayPosition (); y = _vRasterData.firstDisplayPosition (); }
			void displayPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2)
							{ x1 = _hRasterData.firstDisplayPosition (); y1 = _vRasterData.firstDisplayPosition ();
							  x2 = _hRasterData.lastDisplayPosition (); y2 = _vRasterData.lastDisplayPosition (); }

			/** To go from the display zone to the screen one. */
			void reduceDisplayZone (bool v, bool h)
							{ _vRasterData.reduceDisplayZone (v); _hRasterData.reduceDisplayZone (h); }

			// Managing the screen
			/** The screen is where the drawing is possible, considering the reduced zones if any.
				If there hadn't reduced zones the screen and the display would be equivalent. */
			bool isInScreenZone () const
							{ return (_vRasterData.isInScreenZone () && _hRasterData.isInScreenZone ()); }
			void firstScreenPosition (unsigned short& x, unsigned short& y, bool o = false) const
							{ x = _hRasterData.firstScreenPosition (); y = _vRasterData.firstScreenPosition (); }
			void screenPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2)
							{ x1 = _hRasterData.firstScreenPosition (); y1 = _vRasterData.firstScreenPosition ();
							  x2 = _hRasterData.lastScreenPosition (); y2 = _vRasterData.lastScreenPosition (); }
			
			/** Returns true when the raster goes to the next line. \n
				The Parameter is the number of cycles to move the raster. \n
				The raster moves 8 pixels per cycle. */
			bool moveCycles (unsigned short nC)
							{ bool result = _hRasterData.add (nC * 8 /** columuns = piexels per cycle. */);
							  if (result) _vRasterData.next (); 
							  return (result); }

			void initialize ()
							{ _vRasterData.initialize (); _hRasterData.initialize (); }

			/**
			  *	The name of the fields are: \n
			  * RasterX		= InfoStructure: Horizontal raster info.
			  * RasterY		= InfoStructure: Vertical raster info.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			friend std::ostream& operator << (std::ostream& o, const Raster& r)
							{ return (o << *((MCHEmul::InfoClass*) &r)); }

			private:
			RasterData _vRasterData, _hRasterData;
		};

		static const unsigned int _ID = 4;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25; // Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320; // Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** CPU Cycles when the graphics are read. */
		static const unsigned int _CPUCYCLESWHENREADGRAPHS	= 43;  // 40 for reading plus 3 blocking the end of 6510 access

		/** Static address. The color memory cann't be changed. */
		static const MCHEmul::Address _COLORMEMORY;

		/** Specific classes for PAL & NTSC have been created giving this data as default. */
		VICII (const RasterData& vd, const RasterData& hd, const MCHEmul::Attributes& attrs = { });

		~VICII ();

		/** To change and get the bank. */
		unsigned short bank () const
							{ return (_VICIIRegisters -> bank ()); }
		void setBank (unsigned char bk)
							{ _VICIIRegisters -> setBank (bk); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * Registers	= InfoStructure: Info about the registers.
		  * Raster		= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		void setDrawBorder (bool dB)
							{ _drawBorder = dB; }

		private:
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

		/** To read the graphics info. */
		void readGraphicsInfoAt (unsigned short gl /** screen line, including the effect of the scroll y. */);

		/** @see DrawContext structure. */
		void drawGraphicsAndDetectCollisions (const DrawContext& dC);

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Read screen data
		/** Read the chars present in the video matrix. The vale received goes fom 0 to 24. */
		const MCHEmul::UBytes& readScreenCodeDataAt (unsigned short l) const
							{ return (_graphicsScreenCodeData = 
								memoryRef () -> values (_VICIIRegisters -> screenMemory () +
									((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS)); }
		/** Read the info for the chars received as parameter. 
			The method receives also a parameter to indicate whether the graphics mode is or not _EXTENDEDBACKGROUNDMODE. */
		const MCHEmul::UBytes& readCharDataFor (const MCHEmul::UBytes& chrs, bool eM = false) const;
		/** Read th info of the bitmap. 
			The info is read as they were char data. That is, the 8 x 8 block are sequential. 
			The value receive gos from 0 to 199. */
		const MCHEmul::UBytes& readBitmapDataAt (unsigned short l) const;
		/** Reads the color of the chars. The _COLORMEMORY is fixed and cann't be changed using VICRegisters. 
			Th value ecived goes from 0 to 24. */
		const MCHEmul::UBytes& readColorDataAt (unsigned short l) const
							{ return (_graphicsColorData = memoryRef () -> values (_COLORMEMORY +
								((size_t) l * _GRAPHMAXCHARCOLUMNS), (size_t) _GRAPHMAXCHARCOLUMNS)); }
		/** Read the sprites data (only for the active ones. 
			The list of the sprites active (internal variable) is also actualized (not returned) = _spritesEnabled. */
		const std::vector <MCHEmul::UBytes>& readSpriteData () const;

		// Draw the graphics in detail...
		/** Draws a monocolor char. */
		MCHEmul::UByte drawMonoColorChar (int cb, int r, 
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC);
		/** Draws a multicolor char. */
		MCHEmul::UByte drawMultiColorChar (int cb, int r,
			const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC, bool blk = false);
		/** Draws an enhaced multicolor char. */
		MCHEmul::UByte drawMultiColorExtendedChar (int cb, int r,
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC);
		/** Draws a monocolor bitmap. */
		MCHEmul::UByte drawMonoColorBitMap (int cb, int r, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const DrawContext& dC, bool blk = false);
		/** Draws a multicolor bitmap. */
		MCHEmul::UByte drawMultiColorBitMap (int cb, int r, 
			const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const DrawContext& dC, bool blk = false);
		/** Draws a monocolor sprite line. */
		MCHEmul::UByte drawMonoColorSprite (int c, int r, size_t spr, const DrawContext& dC);
		/** Draws a multocolor sprite line. */
		MCHEmul::UByte drawMultiColorSprite (int c, int r, size_t spr, const DrawContext& dC);

		private:
		/** The memory is used also as the set of registers of the chip. */
		C64::VICIIRegisters* _VICIIRegisters;
		/** The raster. */
		Raster _raster;
		/** To draw or nor the border between the background and the foreground. */
		bool _drawBorder;

		// Implementation
		/** The number of bytes drawn should be the same than the number of CPU cycles happened
			since the last invokation to the "simulate method". */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** The bytes read describing a line of graphics. 
			They are all actualized at the methods readXXXX. */
		mutable MCHEmul::UBytes _graphicsScreenCodeData;
		mutable MCHEmul::UBytes _graphicsCharData;
		mutable MCHEmul::UBytes _graphicsBitmapData;
		mutable MCHEmul::UBytes _graphicsColorData;
		mutable std::vector <MCHEmul::UBytes> _graphicsSprites; // Eight sprites...
		mutable std::vector <size_t> _spritesEnabled; // With the list of sprites number actived
		/** Whenever a new raster line is reached, this variable becomes true. */
		bool _isNewRasterLine; 
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the VICII register about whether the video ir or not active. \n
			In any other circunstance is kept its value. */
		bool _videoActive;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
	};

	/** The version para NTSC systems. */
	class VICII_NTSC final : public VICII
	{
		public:
		static const RasterData _VRASTERDATA;
		static const RasterData _HRASTERDATA;

		VICII_NTSC ();
	};

	/** The version para PAL systems. */
	class VICII_PAL final : public VICII
	{
		public:
		static const RasterData _VRASTERDATA;
		static const RasterData _HRASTERDATA;

		VICII_PAL ();
	};
}

#endif
  
// End of the file
/*@}*/
