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
		class RasterData
		{
			public:
			RasterData () = delete;

			RasterData (
						unsigned short fp,		// First position
						unsigned short fvp,		// First Visible position
						unsigned short fdp,		// First Display position
						unsigned short ldp,		// Last Display Sync osition
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

			bool isInBlankZone () const
							{ return ((_currentPosition_0 >= _firstPosition_0 && 
											_currentPosition_0 < _firstVisiblePosition_0) ||
									  (_currentPosition_0 > _lastVisiblePosition_0 && 
											_currentPosition_0 <= _lastPosition_0)); }
			bool isInLastBlankZone () const
							{ return (_currentPosition_0 > _lastVisiblePosition_0 && 
									  _currentPosition_0 <= _lastPosition_0); }

			bool isInVisibleZone () const 
							{ return ((_currentPosition_0 >= _firstVisiblePosition_0 && 
									   _currentPosition_0 <= _lastVisiblePosition_0)); }
			unsigned short currentVisiblePosition () const // The 
							{ return (_currentPosition_0 - _firstVisiblePosition_0); }
			unsigned short visiblePositions () const
							{ return (_lastVisiblePosition_0 - _firstVisiblePosition_0 + 1); }

			/** Takes into account potential reductions in the size. */
			bool isInDisplayZone () const
							{ return (_currentPosition_0 >= _firstDisplayPosition_0 && 
									  _currentPosition_0 <= _lastDisplayPosition_0); }
			unsigned short displayPositions () const
							{ return (_lastDisplayPosition_0 - _firstDisplayPosition_0 + 1); }

			unsigned short firstScreenPosition () const
							{ return (_firstDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short lastScreenPosition () const
							{ return (_lastDisplayPosition_0 - _firstVisiblePosition_0); }
			unsigned short currentScreenPosition () const
							{ return (_currentPosition_0 - _firstVisiblePosition_0); }

			/** Returns true when the limit of the raster is reached. 
				The parameter is the number of positions to increment the rasterData. */
			bool add (unsigned short i);
			bool next ()
							{ return (add (1)); }

			/** The display zone will reduced in both sides by half of the _positionsToReduce value. */
			void reduceDisplayZone (bool s);
			bool isDisplayZoneReduced () const
							{ return (_displayZoneReducted); }

			void initialize ()
							{ _currentPosition = _firstPosition; _currentPosition_0 = _firstPosition_0; }

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
			bool _displayZoneReducted;
		};

		/** The Raster simulates the set of sequential horizontal lines that, 
			in a CRT monitor, draws an image in the screen. */
		class Raster final
		{
			public:
			static const unsigned short _FIRSTBADLINE	= 0x30;
			static const unsigned short _LASTBADLINE	= 0xf7;

			Raster () = delete;

			Raster (const Raster&) = default;

			Raster& operator = (const Raster&) = default;

			Raster (const RasterData& vD, const RasterData& hD)
				: _vRasterData (vD), _hRasterData (hD)
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

			bool isInVBlank () const
							{ return (_vRasterData.isInBlankZone ()); }
			bool isInLastVBlank () const
							{ return (_vRasterData.isInLastBlankZone ()); }

			bool isInVisibleZone () const
							{ return (_vRasterData.isInVisibleZone () && _hRasterData.isInVisibleZone ()); }
			void currentVisiblePosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.currentVisiblePosition (); y = _vRasterData.currentVisiblePosition (); }

			bool isInDisplayZone () const
							{ return (_vRasterData.isInDisplayZone () && _hRasterData.isInDisplayZone ()); }

			void firstScreenPosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.firstScreenPosition (); y = _vRasterData.firstScreenPosition (); }
			void currentScreenPosition (unsigned short& x, unsigned short& y) const
							{ x = _hRasterData.currentScreenPosition (); y = _vRasterData.currentScreenPosition (); }
			void screenPositions (unsigned short& x1, unsigned short& y1, unsigned short& x2, unsigned short& y2)
							{ x1 = _hRasterData.firstScreenPosition (); y1 = _vRasterData.firstScreenPosition ();
							  x2 = _hRasterData.lastScreenPosition (); y2 = _vRasterData.lastScreenPosition (); }

			unsigned short visibleLines () const
							{ return (_vRasterData.visiblePositions ()); }
			unsigned short visibleColumns () const
							{ return (_hRasterData.visiblePositions ()); }

			void reduceDisplayZone (bool v, bool h)
							{ _vRasterData.reduceDisplayZone (v); _hRasterData.reduceDisplayZone (h); }
			
			/** Returns true when the raster goes to the next line. 
				The Parameter is the number of cycles to move the raster. */
			bool moveCycles (unsigned short nC)
							{ bool result = _hRasterData.add (nC * 8 /** columuns = piexels per cycle. */);
							  if (result) _vRasterData.next (); 
							  return (result); }

			void initialize ()
							{ _vRasterData.initialize (); _hRasterData.initialize (); }

			private:
			RasterData _vRasterData, _hRasterData;
		};

		static const unsigned int _ID = 1;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25; // Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320; // Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** CPU Cycles when the graphics are read. */
		static const unsigned int _CPUCYCLESWHENEADGRAPHS = 40;

		/** Static address. The color memory cann't be changed. */
		static const MCHEmul::Address _COLORMEMORY;

		/** Specific classes for PAL & NTSC have been created giving this data as default. */
		VICII (const RasterData& vd, const RasterData& hd, const MCHEmul::Attributes& attrs = { });

		~VICII ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		/** To read the graphics info. */
		void readGraphicsInfo ();

		/** To draw the graphics. */
		void drawGraphics (unsigned short r, unsigned short c);
		/** To draw the sprites. */
		void drawSprites (unsigned short r, unsigned short c);

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Read screen data
		/** Read the chars present in the video matrix. The vale received goes fom 0 to 24. */
		MCHEmul::UBytes readCharCodeDataAt (unsigned short l) const
							{ return (memoryRef () -> values (_VICIIRegisters -> screenMemory () + 
								(size_t) (l * _GRAPHMAXCHARCOLUMNS), _GRAPHMAXCHARCOLUMNS)); }
		/** Read the info for the chars received as parameter. */
		MCHEmul::UBytes readCharDataFor (const MCHEmul::UBytes& chrs) const;
		/** Read th info of the bitmap. 
			The info is read as they were char data. That is, the 8 x 8 block are sequential. 
			The value receive gos from 0 to 199. */
		MCHEmul::UBytes readBitmapDataAt (unsigned short l) const;
		/** Reads the color of the chars. The _COLORMEMORY is fixed and cann't be changed using VICRegisters. 
			Th value ecived goes from 0 to 24. */
		MCHEmul::UBytes readColorDataAt (unsigned short l) const
							{ return (memoryRef () -> values (_COLORMEMORY + 
								(size_t) (l * _GRAPHMAXCHARCOLUMNS), _GRAPHMAXCHARCOLUMNS)); }
		/** The value received is the number of sprite to be drawn. */
		MCHEmul::UBytes readSpriteDataAt (unsigned short l) const;

		// Draw the graphics in detail...
		/** Draw the char mode. */
		void drawGraphicsCharMode (unsigned short r, unsigned short c);
		/** Draw the bitmap mode. */
		void drawGraphicsBitMapMode (unsigned short r, unsigned short c);

		private:
		/** The memory is used also as the set of registers of the chip. */
		C64::VICIIRegisters* _VICIIRegisters;
		/** The raster. */
		Raster _raster;

		// Implementation
		/** The number of bytes drawn should be the same than the number of CPU cycles happened
			since the last invokation to the "simulate method". */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** The bytes read describing a line of graphics. */
		MCHEmul::UBytes _graphicsCharCodeData;
		MCHEmul::UBytes _graphicsCharData;
		MCHEmul::UBytes _graphicsBitmapData;
		MCHEmul::UBytes _graphicsColorData;
		/** Whenever a new rastr line is reached, this variable becomes true. */
		bool _isNewRasterLine; 
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
