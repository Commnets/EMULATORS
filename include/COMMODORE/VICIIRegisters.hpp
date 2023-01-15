/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICIIRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
		static const int _VICREGS_SUBSET = 1000;
			
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
							{ return (_spriteXCoord [p]); }
		unsigned char spriteYCoord (size_t p) const
							{ return (_spriteYCoord [p]); }
		unsigned char spriteColor (size_t p) const
							{ return (_spriteColor [p]); }
		unsigned char spriteSharedColor (size_t p) const
							{ return (_spriteSharedColor [p]); }
		bool spriteEnable (size_t p) const
							{ return (_spriteEnabled [p]); }
		bool spriteMulticolorMode (size_t p) const
							{ return (_spriteMulticolor [p]); }
		bool spriteDoubleWidth (size_t p) const
							{ return (_spriteDoubleWidth [p]); }
		bool spriteDoubleHeight (size_t p) const
							{ return (_spriteDoubleHeight [p]); }
		bool spriteToForegroundPriority (size_t p) const
							{ return (_spriteToForegroundPriority [p]); }
		
		// ScrollX & ScrollY.
		unsigned char verticalScrollPosition () const
							{ return (_verticalScrollPosition); }
		unsigned char horizontalScrollPosition () const
							{ return (_horizontalScrollPosition); }

		bool textDisplay25RowsActive () const
							{ return (_textDisplay25RowsActive); }
		bool textDisplay40ColumnsActive () const
							{ return (_textDisplay40ColumnsActive); }
		bool screenSameColorBorderActive () const
							{ return (_screenSameColorBorderActive); }
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
		void activateRasterAtLineIRQ ()
							{ _rasterAtLineIRQHappened = true; }
		void activateSpritesCollisionWithDataIRQ ()
							{ _spritesCollisionWithDataIRQHappened = true; }
		void setSpriteCollisionWithDataHappened (size_t p)
						{ _spriteCollisionWithDataHappened [p] = true; }
		void activateSpritesCollisionIRQ ()
							{ _spritesCollisionIRQHappened = true; }
		void setSpriteCollision (size_t p)
							{ _spriteCollisionHappened [p] = true; }
		void activateLightPenOnScreenIRQ ()
							{ _lightPenOnScreenIRQHappened = true; }
		/** To know whether the VICII might launch a IRQ (from its internal perspective only).
			The IRQ will be or not actually launched depending on other elements like whether the IRQ flag is or not active. */
		bool hasVICIIToGenerateIRQ () const
							{ return ((_rasterAtLineIRQHappened && _rasterIRQActive) || 
									  (_spritesCollisionWithDataIRQHappened && _spriteCollisionWithDataIRQActive) || 
									  (_spritesCollisionIRQHappened && _spriteCollisionsIRQActive) || 
									  (_lightPenOnScreenIRQHappened && _lightPenIRQActive)); }

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
		void setCurrentLightPenPosition (unsigned char x, unsigned char y)
							{ _currentLightPenHorizontalPosition = x; _currentLightPenVerticalPosition = y; }

		// Memory address for the different elements managed from VICII
		const MCHEmul::Address initAddressBank () const
							{ return (MCHEmul::Address ({ 0x00, 0x00 }, false) + 
								((size_t) 0x4000 /* 16284 = 16k */ * _bank) /** VICII Only addresses 16k. */); }
		const MCHEmul::Address charDataMemory () const
							{ return (_charDataMemory + ((size_t) 0x4000 * _bank)); }
		const MCHEmul::Address screenMemory () const 
							{ return (_screenMemory + ((size_t) 0x4000 * _bank)); }
		const MCHEmul::Address bitmapMemory () const 
							{ return (_bitmapMemory + ((size_t) 0x4000 * _bank)); }
		const MCHEmul::Address spritePointersMemory () const
							{ return (screenMemory () + (size_t) 0x03f8); /** The last 8 bytes of the screen memory. */ }

		// Active VIVII bank. VICII only seed 16k out of the 64k, 
		// so 4 different banks (from 0 to 3) can be managed...
		unsigned char bank () const
							{ return (_bank); }
		void setBank (unsigned char bk)
							{ if (bk == 0 || bk == 1 || bk == 2 || bk == 3) _bank = bk; }
		
		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chipregisters plus:
		  * MODE  = Attribute: The mode working in the VICII.
		  *	40C   = Attribute: Whether there is 40 columns or not.
		  * 25R   = Attribute: Whether there is 25 rows or not.
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

		private:
		// The VICII registers
		/** Screen related variables. */
		unsigned char _foregroundColor;
		std::vector <unsigned char> _backgroundColor;
		/** Sprite related variables. */
		std::vector <unsigned short> _spriteXCoord; 
		std::vector <unsigned char> _spriteYCoord;
		std::vector <unsigned char> _spriteColor;
		std::vector <unsigned char> _spriteSharedColor;
		std::vector <bool> _spriteMulticolor;
		std::vector <bool> _spriteEnabled;
		std::vector <bool> _spriteDoubleWidth, _spriteDoubleHeight;
		std::vector <bool> _spriteToForegroundPriority;
		/** ScrollYRegister & ScrollXRegister. */
		unsigned char _verticalScrollPosition, _horizontalScrollPosition;
		/** Visible screen control. */
		bool _textDisplay25RowsActive, _textDisplay40ColumnsActive;
		bool _screenSameColorBorderActive;
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
		bool _rasterAtLineIRQHappened; // Whether the raster line has reached the one defined to generate an IRQ.
		bool _spritesCollisionWithDataIRQHappened; // Whether a collision among sprites and data has happened. The detail is next.
		mutable std::vector <bool> _spriteCollisionWithDataHappened;
		bool _spritesCollisionIRQHappened; // Whether a collision among sprites has happened. The detail is next.
		mutable std::vector <bool> _spriteCollisionHappened;
		bool _lightPenOnScreenIRQHappened; // Whether the lightpen is on the screen.

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
	};
}

#endif
  
// End of the file
/*@}*/
