/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICIIRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: VICII Registers Emulation
 *	Versions: 1.0 Initial
 *	Based on https://www.cebix.net/VIC-Article.txt.
 */

#ifndef __COMMODORE_VICIIREGISTERS__
#define __COMMODORE_VICIIREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** In the VICII Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIIRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		static const int _VICREGS_SUBSET = 1060;
			
		enum class GraphicMode
		{
			_CHARMODE = 0,
			_MULTICOLORCHARMODE,
			_BITMAPMODE,
			_MULTICOLORBITMAPMODE,
			_EXTENDEDBACKGROUNDMODE,
			_INVALIDTEXMODE,
			_INVALIDBITMAPMODE1,
			_INVALIDBITMAPMODE2,
		};

		VICIIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x40); }

		// Managing the different attributes of the elements managed by VICII
		// Foreground (border) & background.
		unsigned char foregroundColor () const
							{ return (_foregroundColor); }
		unsigned char backgroundColor (size_t p = 0) const
							{ return ( _backgroundColor [p]); }

		// Sprite attributes
		unsigned short spriteXCoord (size_t p) const
							{ return (_spriteInfo [p]._spriteXCoord); }
		unsigned char spriteYCoord (size_t p) const
							{ return (_spriteInfo [p]._spriteYCoord); }
		unsigned char spriteColor (size_t p) const
							{ return (_spriteInfo [p]._spriteColor); }
		bool spriteEnable (size_t p) const
							{ return (_spriteInfo [p]._spriteEnabled); }
		bool spriteMulticolorMode (size_t p) const
							{ return (_spriteInfo [p]._spriteMulticolor); }
		bool spriteDoubleWidth (size_t p) const
							{ return (_spriteInfo [p]._spriteDoubleWidth); }
		bool spriteDoubleHeight (size_t p) const
							{ return (_spriteInfo [p]._spriteDoubleHeight); }
		bool spriteToForegroundPriority (size_t p) const
							{ return (_spriteInfo [p]._spriteToForegroundPriority); }
		unsigned char spriteSharedColor (size_t p) const
							{ return (_spriteSharedColor [p]); }
		
		// ScrollX & ScrollY.
		unsigned char verticalScrollPosition () const
							{ return (_verticalScrollPosition); }
		unsigned char horizontalScrollPosition () const
							{ return (_horizontalScrollPosition); }

		bool textDisplay25RowsActive () const
							{ return (_textDisplay25RowsActive); }
		bool textDisplay40ColumnsActive () const
							{ return (_textDisplay40ColumnsActive); }
		bool blankEntireScreen () const
							{ return (_blankEntireScreen); }
		bool videoResetActive () const
							{ return (_videoResetActive); }

		// Graphical modes.
		bool graphicBitModeActive () const
							{ return (_graphicBitModeActive); }
		bool graphicExtendedColorTextModeActive () const
							{ return (_graphicExtendedColorTextModeActive); }
		bool graphicMulticolorTextModeActive () const
							{ return (_graphicMulticolorTextModeActive); }
		GraphicMode graphicModeActive () const
							{ return (_graphicModeActive); }
		bool textMode () const
							{ return (_textMode); }

		// IRQs...
		// To know whether a IRQ could be generated in different scenarios...
		// To generate the IRQ the codition has to be reached
		// To activate or desactivate this sources of IRQ the register $d01A has to be accessed...
		/** IRQ launched when raster was at line...? */
		bool rasterIRQActive () const
							{ return (_rasterIRQActive); }
		/** IRQ launched when a sprite collisions with data? */
		bool spriteCollisionWithDataIRQActive () const
							{ return (_spriteCollisionWithDataIRQActive); }
		/** IRQ launched when a sprite collisions with other? */
		bool spriteCollisionsIRQActive () const
							{ return (_spriteCollisionsIRQActive); }
		/** IRQ launched when lightpen touche the screen? */
		bool lightPenIRQActive () const
							{ return (_lightPenIRQActive); }
		
		/** Raster line number when the VICII (if active) will generate an IRQ. */
		unsigned short IRQRasterLineAt () const
							{ return (_IRQRasterLineAt);  }

		// Activate the IRQ. 
		// All these methods are called from VICII simulation...
		void activateRasterIRQ ()
							{ _rasterIRQHappened = true; }
		void activateSpriteCollisionWithDataIRQ ()
							{ _spriteCollisionWithDataIRQHappened = true; }
		void setSpriteCollisionWithDataHappened (size_t p)
							{ _spriteCollisionWithDataHappened [p] = true; }
		void activateSpriteCollisionIRQ ()
							{ _spriteCollisionsIRQHappened = true; }
		void setSpriteCollision (size_t p)
							{ _spriteCollisionHappened [p] = true; }
		void activateLightPenOnScreenIRQ ()
							{ _lightPenIRQHappened = true; }
		/** To know whether the VICII might launch a IRQ (from its internal perspective only).
			The IRQ will be or not actually launched depending on other elements like whether the IRQ flag is or not active. */
		inline bool launchIRQ () const;
		/** To have a code for the reason of the interruption. 
			There might be several reasons defined within the code. */
		inline unsigned int reasonIRQCode () const;

		// Temporal variables to know, when an raster or lightpen IRQ happened, where was the element that generated that.
		// This temporal variables are set from the VICII directly...
		unsigned short currentRasterLine () const
							{ return (_currentRasterLine); }
		void setCurrentRasterLine (unsigned short rL)
							{ _currentRasterLine = rL; }
		unsigned short currentLightPenHorizontalPosition () const
							{ return (_currentLightPenHorizontalPosition); }
		unsigned short currentLightPenVerticalPosition () const
							{ return (_currentLightPenVerticalPosition); }
		void currentLightPenPosition (unsigned short& x, unsigned short& y) const
							{ x = _currentLightPenHorizontalPosition; y = _currentLightPenVerticalPosition; }
		void setCurrentLightPenPosition (unsigned short x, unsigned short y)
							{ _currentLightPenHorizontalPosition = x; _currentLightPenVerticalPosition = y; }
		bool lightPenActive () const
							{ return (_lightPenActive); }
		void setLigthPenActive (bool lP)
							{ _lightPenActive = lP; }

		// Memory address for the different elements managed from VICII...
		// These methods are invoked from the VIC mainly when reading elements...
		const MCHEmul::Address& initAddressBank () const
							{ return (_initAddresbankPos); } 
		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemoryPos); }
		const MCHEmul::Address& screenMemory () const 
							{ return (_screenMemoryPos); }
		const MCHEmul::Address& bitmapMemory () const 
							{ return (_bitmapMemoryPos); }
		const MCHEmul::Address& spritePointersMemory () const
							{ return (_spritePointerMemoryPos); }

		// Active VIVII bank. VICII only seed 16k out of the 64k, 
		// so 4 different banks (from 0 to 3) can be managed...
		unsigned char bank () const
							{ return (_bank); }
		void setBank (unsigned char bk)
							{ if (bk == 0 || bk == 1 || bk == 2 || bk == 3) 
								{ _bank = bk; calculateMemoryPositions (); } }
		
		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chipregisters plus:
		  * MODE			= Attribute: The mode working in the VICII. \n
		  *	40C				= Attribute: Whether there is 40 columns or not. \n
		  * 25R				= Attribute: Whether there is 25 rows or not. \n
		  * SCROLLX			= Attribute; Value of the horizontal scroll. \n
		  * SCROLLY			= Attribute; Value of the vertical scroll. \n
		  * FORECOLOR		= Attribute; Main color. \n
		  * BKCOLOR1		= Attribute; Background 1 color value. \n
		  * BKCOLOR2		= Attribute; Background 2 color value. \n
		  * BKCOLOR3		= Attribute; Background 3 color value. \n
		  * BKCOLOR4		= Attribute; Background 4 color value. \n
		  * IRQ				= Attribute; Whether the IRQ is or not requested. \n
		  * IRQLINE			= Attribute; The number of scan line where the IRQ should be requested if on. \n
		  * CHARADDRESS		= Attribute; The address of the characters definition. \n
		  * SCREENADDRESS	= Attribute; The address of the video matrix. \n
		  * BITMAPADDRESS	= Attribute; The address of the bit maps. \n
		  * SPRITEADDRESS	= Attribute; The address of the pointers to sprites definition. \n
		  * LIGHTPENX		= Attribute; Where the lightpen is in the horizontal space. \n
		  * LIGHTPENY		= Attribute; Where the lightpen is in the vertical space. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Depending on how bits ar set, a no valid mode could be set. */
		void setGraphicModeActive ();
		/** Calculate the internal memory positions. */
		inline void calculateMemoryPositions ();

		private:
		struct SpriteInfo : public MCHEmul::InfoClass
		{
			SpriteInfo ()
				: MCHEmul::InfoClass ("SpriteInfo"),
				  _spriteXCoord (0x0000), 
				  _spriteYCoord (0x00),
				  _spriteColor (0x00),
				  _spriteMulticolor (false),
				  _spriteEnabled (false),
				  _spriteDoubleWidth (false), 
				  _spriteDoubleHeight (false),
				  _spriteToForegroundPriority (false)
							{ }

			/**
			 *	The name of the fields are: \n
			 *	X				= X Position of the sprite.
			 *	Y				= Y Position of the sprite.
			 *	COLOR			= Attribute with the color number if the sprite.
			 *	MULTICOLOR		= Attribute with YES when Multicolor is active and NO in other case.
			 *	ENABLED			= Attribute with YES when the sprite is enabled and NO in other case.
			 *	DOUBLEW			= Attribute with YES when the sprite is x expanded and NO in other case.
			 *	DOUBLEH			= Attribute with YES when the sprite is y expanded and NO in other case.
			 *	PRIORITY		= Attribute with YES when the sprite has priority over the foreground characters.
			 */
			MCHEmul::InfoStructure getInfoStructure () const override;

			unsigned short _spriteXCoord; 
			unsigned char _spriteYCoord;
			unsigned char _spriteColor;
			bool _spriteMulticolor;
			bool _spriteEnabled;
			bool _spriteDoubleWidth, _spriteDoubleHeight;
			bool _spriteToForegroundPriority;
		};

		// The VICII registers
		/** Screen related variables. */
		unsigned char _foregroundColor;
		std::vector <unsigned char> _backgroundColor;
		/** Sprite related variables. */
		std::vector <SpriteInfo> _spriteInfo;
		std::vector <unsigned char> _spriteSharedColor;
		/** ScrollYRegister & ScrollXRegister. */
		unsigned char _verticalScrollPosition, _horizontalScrollPosition;
		/** Visible screen control. */
		bool _textDisplay25RowsActive, _textDisplay40ColumnsActive;
		bool _blankEntireScreen;
		bool _videoResetActive;
		/** Graphical modes. */
		bool _graphicBitModeActive;
		bool _graphicExtendedColorTextModeActive;
		bool _graphicMulticolorTextModeActive;
		GraphicMode _graphicModeActive;
		bool _textMode; // True when a text mode is active
		/** IRQ Control. */
		bool _rasterIRQActive;
		bool _spriteCollisionWithDataIRQActive;
		bool _spriteCollisionsIRQActive;
		bool _lightPenIRQActive;
		/** Raster Control. */
		unsigned short _IRQRasterLineAt; // To define where to launch the IRQ. When reading therre is other variable...

		// Some of this variables are set by the emulation of the VICII
		// The VICII chip also uses this object as a temporary storage
		unsigned short _currentRasterLine; // Where the raster is now...
		unsigned short _currentLightPenHorizontalPosition, _currentLightPenVerticalPosition; // Where the light pen is...
		bool _lightPenActive;
		/** Whether the raster line has reached the one defined to generate an IRQ. */
		bool _rasterIRQHappened;
		/** Whether a collision among sprites and data has happened. The detail is next. */
		bool _spriteCollisionWithDataIRQHappened;
		mutable std::vector <bool> _spriteCollisionWithDataHappened;
		/** Whether a collision among sprites has happened. The detail is next. */
		bool _spriteCollisionsIRQHappened;
		mutable std::vector <bool> _spriteCollisionHappened;
		/** Whether the lightpen is on the screen. */
		bool _lightPenIRQHappened;

		/** Location of the Graphical Memory. */
		MCHEmul::Address _charDataMemory; // Info about the characters (the address with in the first 16k)
		MCHEmul::Address _screenMemory; // Where the characters to draw are (The address within the first 16k)
		MCHEmul::Address _bitmapMemory; // Where the bitmap to draw is (The address within the first 16k)

		/** A very important variable comming from other chip. \n
			Can be 0, 1, 2 or 3 and it affects to the determination of any memory address. 
			By default the bank is 0. */
		unsigned char _bank; 

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
		MCHEmul::Address _initAddresbankPos;
		MCHEmul::Address _charDataMemoryPos; 
		MCHEmul::Address _screenMemoryPos;
		MCHEmul::Address _bitmapMemoryPos;
		MCHEmul::Address _spritePointerMemoryPos;
	};

	// ---
	inline bool VICIIRegisters::launchIRQ () const
	{ 
		return ((_rasterIRQHappened && _rasterIRQActive) ||
				(_spriteCollisionWithDataIRQHappened && _spriteCollisionWithDataIRQActive) ||
				(_spriteCollisionsIRQHappened && _spriteCollisionsIRQActive) ||
				(_lightPenIRQHappened && _lightPenIRQActive)); 
	}

	// ---
	inline unsigned int VICIIRegisters::reasonIRQCode () const
	{
		return (((_rasterIRQHappened && _rasterIRQActive) ? 1 : 0) +
				((_spriteCollisionWithDataIRQHappened && _spriteCollisionWithDataIRQActive) ? 2 : 0) +
				((_spriteCollisionsIRQHappened && _spriteCollisionsIRQActive) ? 4 : 0) +
				((_lightPenIRQHappened && _lightPenIRQActive) ? 8 : 0)); 
	}

	// ---
	inline void VICIIRegisters::calculateMemoryPositions ()
	{ 
		// All will depend on the bank...
		_initAddresbankPos			= MCHEmul::Address ({ 0x00, 0x00 }, false) + 
			((size_t) 0x4000 /* 16284 = 16k */ * _bank); /** VICII Only addresses 16k. */
		_screenMemoryPos			= _screenMemory + ((size_t) 0x4000 * _bank);
		_charDataMemoryPos			= _charDataMemory + ((size_t) 0x4000 * _bank); 
		_bitmapMemoryPos			= _bitmapMemory + ((size_t) 0x4000 * _bank);
		_spritePointerMemoryPos		= _screenMemoryPos + (size_t) 0x03f8; /** The last 8 bytes of the screen memory. */
	}
}

#endif
  
// End of the file
/*@}*/
