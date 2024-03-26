/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICI.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: The VICI Chip.
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICI__
#define __COMMODORE_VICI__

#include <CORE/incs.hpp>
#include <COMMODORE/VICI/VICIRegisters.hpp>

namespace COMMODORE
{
	/** The chip that takes care of anything around the graphics in computers like VIC20. 
		@see GraphicalChip and @see SoundChip 
		The emulation has been built following: 
		http://tinyvga.com/6561 */
	class VICI : public MCHEmul::GraphicalChip
	{
		public:
		/** VICI is both a Graphical and a Sound Chip. \n
			VICI should then inherit from both MCHEmul::GraphicalCip & MCHEmul::SoundChip,
			but this could generate ambiguity when calling some parent methods, that will be the case. \n
			The main function of the VICI is, however, the graphical one. \n
			VICI will then inherit from MCHEmul::GraphicalChip only and it will use an object inside inheriting from MCHEmul::SoundChip. */
		class SoundFunction final : public MCHEmul::SoundChip
		{
			public:
			static const unsigned int _ID = 1060;

			SoundFunction (MCHEmul::SoundLibWrapper* sW);

			/** The main characteristics of the SoundChip. */
			virtual SDL_AudioFormat type () const override
								{ return (_SOUNDSAMPLINGFORMAT); }
			virtual int maxFrequency () const override
								{ return (_SOUNDSAMPLINGCLOCK >> 1); }
			virtual unsigned char numberChannels () const override
								{ return (_SOUNDCHANNELS); }

			virtual bool initialize () override;

			virtual bool simulate (MCHEmul::CPU* cpu) override;

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			// Implementation
			/** The number of cycles the CPU was executed once the simulated method finishes. */
			unsigned int _lastCPUCycles;
		};

		static const unsigned int _ID = 106;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICI constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICI version) 
			and additional attributes. */
		VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

		virtual ~VICI () override;

		const SoundFunction* soundFunction () const
							{ return (_soundFunction); }
		SoundFunction* soundFunction ()
							{ return (_soundFunction); }

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (short& x1, short& y1, short& x2, short& y2);
		/** The original ones. */
		inline void originalScreenPositions (short& x1, short& y1, short& x2, short& y2);

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		/** To set the position of the light - pen. \n
			The position received must be relative within the display zone. */
		void lightPenPosition (unsigned short& x, unsigned short& y) const
							{ _VICIRegisters -> currentLightPenPosition (x, y); }
		void setLightPenPosition (unsigned short x, unsigned short y)
							{ _VICIRegisters -> setCurrentLightPenPosition (x, y); }

		/** To know whether the light pen is active. */
		bool lightPenActive () const
							{ return (_VICIRegisters -> lightPenActive ()); }
		void setLightPenActive (bool lP)
							{ _VICIRegisters -> setLigthPenActive (lP); }

		virtual bool initialize () override;

		/** Simulates cycles in the VICI. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters	= InfoStructure: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  * Sound			= InfoStructire: Info about the sound.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics detail...
		/** To simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _LCD;	// Last Column of the Display 
			unsigned short _RC;		// Raster X (from the beginning of the visible zone)
			unsigned short _IRD;	// Initial Row Display (Not taken into account reductions of the size)
			unsigned short _LRD;	// Last Row of the Display 
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed)
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The color of the each pixel considered as background. */
			unsigned int _backgroundColorData [8];
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];

			DrawResult (unsigned int bC = ~0 /** Meaning transparent (jumped later when moving info to screen). */)
				: _backgroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _foregroundColorData { bC, bC, bC, bC, bC, bC, bC, bC }
							{ }
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. */
		void treatRasterCycle ();
		/** Read and draw the graphics. */
		void readGraphicsAndDrawVisibleZone ();

		// These all methods are invoked from the two ones above!
		// They are here just to structure better the code...
		// Read screen data
		// Methods linked to the treatRasterCycle method...
		/** To read the video matrix and the RAM color. \n
			Someting that happens during a badline. */
		inline void readVideoMatrixAndColorRAM ();
		/** To read the graphical info, considerig the info read in the previous method. \n
			This method is executed per raster cycles. */
		inline void readGraphicalInfo ();

		// Draw the screen
		// Methods linked to the drawVisibleZone method...
		/** Treat the visible zone.
			Draws the graphics and finally draw the border. */
		void drawVisibleZone ();
		/** To draw any type of graphic \n
			This method uses the ones below it \n
			The method receives: \n
			dC = other info about the raster used to finally draw. \n
			They all return a instance of DrawResult (@see above). */
		void drawGraphics (const DrawContext& dC);
		/** To draw the pixels in the high resolution mode. */
		DrawResult drawHighResolutionMode (int cb);
		/** To draw the pixels in the multicolor mode. */
		DrawResult drawMulticolorMode (int cb);
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);

		private:
		/** Just to calculate the screen positions. \n
			This method is invoked from treatRasterCycle, and always before actualizing the graphical info. */
		inline void calculateScreenPositions ();

		protected:
		/** A reference to the sound part of the chip. */
		SoundFunction* _soundFunction;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIRegisters* _VICIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIView;
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
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
		/** The positions of the text screen. */
		short _scrX1, _scrY1, _scrX2, _scrY2;

		/** 
		  *	Structure to control how the graphics are displayed in the screen. \n
		  * based on the article: http://tinyvga.com/6561 \n
		  * The VICI considers all lines in the screen as bad lines, 
		  * which means that in all lines graphical information is read from the memory 
		  * (char code, char data definition and colour info). \n
		  * The text windo is something that can be reloacted within the visible zone. \n
		  * The methods defined in this class flag when the initial position of this text window is detected. */
		struct VICGraphicInfo
		{
			VICGraphicInfo ()
				: _VBASE (0),
				  _HC (0), _COL (0), _COLMAX (0), _FH (false),
				  _VC (0), _ROW (0), _ROWMAX (0), _FV (false),
				  _RC (0), _DOUBLEHIGH (false), _RCF (false),
				  _DRAW (false), _SCREENORIGINX (false), _SCREENORIGINY (false),
				  _screenCodeData (MCHEmul::UByte::_0),
				  _graphicData (MCHEmul::UByte::_0),
				  _colorData (MCHEmul::UByte::_0)
							{ }

			/** Invoked onky from the VICI initialization method. */
			inline void initialize (VICIRegisters* r);
			inline void nextHorizontal (VICIRegisters* r);
			inline void nextVertical (VICIRegisters* r);
			bool draw () const
							{ return (_DRAW); }
			bool screenOriginDetected () const
							{ return (_SCREENORIGINX && _SCREENORIGINY); }

			/** The base of the screen matrix where to read the info
				about the colour and the pointer to the definition of the char. 
				This value will move into _COL, once a full row (with 8 pixels height) is drawn. */
			unsigned short _VBASE;

			// Horixontal...
			/** Delay to start to draw columns. 
				Until this value reaches 0, _COL won't start to count. */
			unsigned char _HC;
			/** Counter to add to _VCBASE where to find the info 
				about the char to draw and the color info to apply. */
			unsigned char _COL;
			/** The maximum value for the previous counter.
				It is determined by the info in the VICI registers. */
			unsigned char _COLMAX;
			/** Flip flop to indicate whether the counter has or not to be increment. \n 
				In the VIC every char is drawn using a minimun of 2 raster cycles,
				so the counter (_COL) moves every two raster cycles and this flip flop is used to gurantee that. */
			bool _FH;

			// Vertical...
			/** Delay to start to draw rows (and also _VBASE to be incremened). \n
				Until this value reaches 0, the counter of rows (_ROW) doesn't move. */
			unsigned char _VC;
			/** Counter with current number of row. \n
				It is not used to locate the char and colour info, just to control how many rows must be drawn. */
			unsigned char _ROW;
			/** Max number of rows, defined in the VICRegisters. */
			unsigned char _ROWMAX;
			/** The sensibility in starting to draw is 2, so this flip - flop control this. \n
				So this value is related with the change in the _VC. */
			bool _FV;

			// Control the byte in the character size...
			/** Byte within the char & colour matrix info read used to finally draw in the screen. */
			unsigned char _RC;
			/** When the characters can be double high this flag is set. */
			bool _DOUBLEHIGH;
			/** When that happens, then there is flip - flop tro control when exactly the _RC has to be incremented. */
			bool _RCF;

			/** Wether to draw or not. \n
				This flag is set when _HC & _VC reached 0 and the _COL and _ROW are below the limits. 
				That is, when the system is recognizing a text window. */
			bool _DRAW;
			/** Has the origin of the screen being detected in this cycle. \n
				First time the first column or the first row of the text window is detected, these flags get set. \n
				When both are true, the left up corner of the text window is detected. */
			bool _SCREENORIGINX, _SCREENORIGINY;

			// The graphics info read!
			// It is only 1 byte length. No more is needed...
			mutable MCHEmul::UByte _screenCodeData;
			mutable MCHEmul::UByte _graphicData; 
			mutable MCHEmul::UByte _colorData;

			private:
			void setDraw ()
						{ _DRAW = (_HC == 0) && (_VC == 0) && 
							(_COL < _COLMAX) && (_ROW < _ROWMAX); }
			
			inline void initializeHorizontal (VICIRegisters* r);
			inline void initializeVertical (VICIRegisters* r);
			inline void initializeHorizontalCounters ();
			inline void initializeVerticalCounters ();
		};

		VICGraphicInfo _vicGraphicInfo;

		private:
		static const MCHEmul::Address _MEMORYPOSOFF;
	};

	// ---
	inline void VICI::screenPositions (short& x1, short& y1, short& x2, short& y2)
	{ 
		// Extrract the info with the limits...
		x1 = _scrX1; 
		if (x1 < 0) x1 = 0;
		if (x1 >= _raster.hData ().visiblePositions ()) x1 = _raster.hData ().visiblePositions () - 1;
		y1 = _scrY1;
		if (y1 < 0) y1 = 0;
		if (y1 >= _raster.vData ().visiblePositions ()) y1 = _raster.vData ().visiblePositions () - 1;
		x2 = _scrX2; 
		if (x2 < 0) x2 = 0;
		if (x2 >= _raster.hData ().visiblePositions ()) x2 = _raster.hData ().visiblePositions () - 1;
		y2 = _scrY2;
		if (y2 < 0) y2 = 0;
		if (y2 >= _raster.vData ().visiblePositions ()) y2 = _raster.vData ().visiblePositions () - 1;
	}

	// ---
	inline void VICI::originalScreenPositions (short& x1, short& y1, short& x2, short& y2)
	{ 
		// The original limits, the could be negative...
		x1 = _scrX1; 
		y1 = _scrY1;
		x2 = _scrX2; 
		y2 = _scrY2;
	}

	// ---
	inline void VICI::readVideoMatrixAndColorRAM ()
	{
		// When the system is not yet within the text window,
		// the colour & char info is read from a fix position in memory!
		if (!_vicGraphicInfo.draw ())
		{
			_vicGraphicInfo._screenCodeData = 
			_vicGraphicInfo._colorData = memoryRef () -> value (_MEMORYPOSOFF);
		}
		else
		{
			_vicGraphicInfo._screenCodeData =
				memoryRef () -> value (_VICIRegisters -> screenMemory () + 
					(size_t) _vicGraphicInfo._VBASE + (size_t) _vicGraphicInfo._COL); 
			_vicGraphicInfo._colorData = 
				memoryRef () -> value (_VICIRegisters -> colourMemory () + 
					(size_t) _vicGraphicInfo._VBASE + (size_t) _vicGraphicInfo._COL);
		}
	}

	// ---
	inline void VICI::readGraphicalInfo ()
	{
		_vicGraphicInfo._graphicData = 
			memoryRef () -> value (_VICIRegisters -> charDataMemory () + 
				(size_t) ((_vicGraphicInfo._screenCodeData.value () << 3) + _vicGraphicInfo._RC));
		
		// The low nibble?
		_vicGraphicInfo._graphicData =
			_vicGraphicInfo._FH
				? _vicGraphicInfo._graphicData.value () & 0x0f // low...
				: ((_vicGraphicInfo._graphicData.value () & 0xf0) >> 4); // high...
		// Every bit in the nibble has to be draw twice...(@see the drawinf routines)
	}

	// ---
	inline void VICI::calculateScreenPositions ()
	{ 
		if (_vicGraphicInfo.screenOriginDetected ())
		{
			_scrX1 = // Can be negative...
				(short) _raster.hData ().currentVisiblePosition ();
			_scrX2 = // Can be longer that the width of the window...
				(short) (_raster.hData ().currentVisiblePosition ()) + 
				(short) (unsigned short (_VICIRegisters -> charsWidthScreen ()) << 4) - 1;

			_scrY1 = // Can be negative...
				(short) _raster.vData ().currentVisiblePosition ();
			_scrY2 = // Can be longer than the height of the window...
				(short) (_raster.vData ().currentVisiblePosition ()) + 
				(short) (unsigned short ((_VICIRegisters -> charsHeightScreen ()) << (_VICIRegisters -> charsExpanded () ? 4 : 3)) - 1);
		}
	}

	// ---
	inline void VICI::VICGraphicInfo::initialize (VICIRegisters* v)
	{
		initializeHorizontal (v);

		initializeVertical (v);

		_VBASE = 0;

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::nextHorizontal (VICIRegisters* v)
	{
		unsigned char HC_old = _HC;

		if (_HC != 0)
			_HC--;
		else
		if (!(_FH = !_FH) && 
			(_COL < _COLMAX))
			_COL++;

		_SCREENORIGINX = (_HC == 0 && HC_old != _HC);

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::nextVertical (VICIRegisters* v)
	{
		unsigned char VC_old = _VC;

		if (_VC != 0)
		{
			if (!(_FV = !_FV))
				_VC--;
		}
		else
		{
			if (!_DOUBLEHIGH ||
				(_DOUBLEHIGH && !(_RCF = !_RCF)))
			{
				if (++_RC == 8)
				{
					_RC = 0;

					_VBASE += (unsigned short) _COLMAX;

					if (_ROW < _ROWMAX)
						_ROW++;
				}
			}
		}

		_SCREENORIGINY = (_VC == 0 && VC_old != _VC);

		initializeHorizontal (v);
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeHorizontal (VICIRegisters* v)
	{
		_COLMAX			= v -> charsWidthScreen ();
		_HC				= v -> offsetXScreen ();

		initializeHorizontalCounters ();

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeVertical (VICIRegisters* v)
	{
		_ROWMAX			= v -> charsHeightScreen ();;
		_VC				= v -> offsetYScreen ();
		_DOUBLEHIGH		= v -> charsExpanded ();

		initializeVerticalCounters ();

		setDraw ();
	}

	inline void VICI::VICGraphicInfo::initializeHorizontalCounters ()
	{
		_COL			= 0;
		_FH				= false;

		_SCREENORIGINX	= (_HC == 0);
	}

	// --
	inline void VICI::VICGraphicInfo::initializeVerticalCounters ()
	{
		_ROW			= 0;
		_FV				= false;
		_RC				= 0;
		_RCF			= false;

		_SCREENORIGINY	= (_VC == 0);
	}

	/** The version para PAL systems. */
	class VICI_PAL final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 71;

		VICI_PAL (int vV, MCHEmul::SoundLibWrapper* wS);
	};

	/** The version para NTSC systems. */
	class VICI_NTSC final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS);
	};
}

#endif
  
// End of the file
/*@}*/
