/** \ingroup C64 */
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

#ifndef __C64_VICIIREGISTERS__
#define __C64_VICIIREGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	/** In the VICII Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIIRegisters final : public MCHEmul::Memory
	{
		public:
		enum class GraphicMode
		{
			_CHARMODE = 0,
			_MULTICOLORCHARMODE,
			_BITMAPMODE,
			_MULTICOLORBITMAPMODE,
			_EXTENDEDBACKGROUNDMODE,
			_ILLEGALMODE
		};

		VICIIRegisters ();

		unsigned short borderColor () const
							{ return (_borderColor); }
		unsigned short backgroundColor (size_t p = 0) const
							{ return ( _backgroundColor [p]); }

		unsigned short spriteXCoord (size_t p) const
							{ return (_spriteXCoord [p]); }
		unsigned short spriteYCoord (size_t p) const
							{ return (_spriteYCoord [p]); }
		unsigned short spriteColor (size_t p) const
							{ return (_spriteColor [p]); }
		unsigned charspriteSharedColor (size_t p) const
							{ return (_spriteSharedColor [p]); }
		bool spriteEnable (size_t p) const
							{ return (_spriteEnabled [p]); }
		bool spriteDoubleWidth (size_t p) const
							{ return (_spriteDoubleWidth [p]); }
		bool spriteDoubleHeight (size_t p) const
							{ return (_spriteDoubleHeight [p]); }
		bool spriteToForegroundPriority (size_t p) const
							{ return (_spriteToForegroundPriority [p]); }
		
		unsigned short verticalScrollPosition () const
							{ return (_verticalScrollPosition); }
		unsigned short horizontalScrollPosition () const
							{ return (_horizontalScrollPosition); }

		bool textDisplay25RowsActive () const
							{ return (_textDisplay25RowsActive); }
		bool textDisplay40ColumnsActive () const
							{ return (_textDisplay40ColumnsActive); }
		bool screenSameColorBorderActive () const
							{ return (_screenSameColorBorderActive); }
		bool videoResetActive () const
							{ return (_videoResetActive); }

		bool graphicBitModeActive () const
							{ return (_graphicBitModeActive); }
		bool graphicExtendedColorTextModeActive () const
							{ return (_graphicExtendedColorTextModeActive); }
		bool graphicMulticolorTextModeActive () const
							{ return (_graphicMulticolorTextModeActive); }
		GraphicMode graphicModeActive () const
							{ return (_graphicModeActive); }

		bool rasterIRQActive () const
							{ return (_rasterIRQActive); }
		bool spriteCollisionWithDataIRQActive () const
							{ return (_spriteCollisionWithDataIRQActive); }
		bool spriteCollisionsIRQActive () const
							{ return (_spriteCollisionsIRQActive); }
		bool lightPenIRQActive () const
							{ return (_lightPenIRQActive); }

		unsigned short IRQRasterLineAt () const
							{ return (_IRQRasterLineAt);  }

		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemory); }
		const MCHEmul::Address& screenMemory () const 
							{ return (_screenMemory); }
		const MCHEmul::Address& bitmapMemory () const 
							{ return (_bitmapMemory); }

		// Managed from VICII Chip Emulator
		// The VICII chip also uses this object as a temporary storage
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

		bool rasterAtIRQLine () const
							{ return (_rasterAtIRQLine); }
		void setRasterAtLine (bool rL)
							{ _rasterAtIRQLine = rL; }
		bool spritesCollisionWithDataHappened () const
							{ return (_spritesCollisionWithDataHappened); }
		void setSpritesCollisionWithData (bool c)
							{ _spritesCollisionWithDataHappened = c; }
		bool spriteCollisionWithDataHappened (size_t p) const
							{ return (_spriteCollisionWithDataHappened [p]); }
		void setSpriteCollisionWithDataHappened (size_t p, bool c)
							{ _spriteCollisionWithDataHappened [p] = c; }
		bool spritesCollisionHappened () const
							{ return (_spritesCollisionHappened); }
		void setSpritesCollision (bool c)
							{ _spritesCollisionHappened = c; }
		bool spriteCollisionHappened (size_t p) const
							{ return (_spriteCollisionHappened [p]); }
		void setSpriteCollision (size_t p, bool c)
							{ _spriteCollisionHappened [p] = c; }
		bool lightPenOnScreenHappened () const
							{ return (_lightPenOnScreenHappened); }
		void setLightPenOnScreen (bool l)
							{ _lightPenOnScreenHappened = l; }
		bool vicIItoGenerateIRQ () const
							{ return (_vicIItoGenerateIRQ); }
		void setVicIItoGenerateIRQ (bool v)
							{ _vicIItoGenerateIRQ = v; }

		virtual bool initialize () override;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual MCHEmul::UByte readValue (size_t p) const override;

		private:
		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Depending on how bits ar set, a no valid mode could be set. */
		void setGraphicModeActive ();

		private:
		// The VICII registers
		/** Screen related variables. */
		unsigned short _borderColor;
		std::vector <unsigned short> _backgroundColor;
		/** Sprite related variables. */
		std::vector <unsigned short> _spriteXCoord; 
		std::vector <unsigned short> _spriteYCoord;
		std::vector <unsigned short> _spriteColor;
		std::vector <unsigned short> _spriteSharedColor;
		std::vector <bool> _spriteMulticolor;
		std::vector <bool> _spriteEnabled;
		std::vector <bool> _spriteDoubleWidth, _spriteDoubleHeight;
		std::vector <bool> _spriteToForegroundPriority;
		/** ScrollYRegister & ScrollXRegister. */
		unsigned short _verticalScrollPosition, _horizontalScrollPosition;
		/** Visible screen control. */
		bool _textDisplay25RowsActive, _textDisplay40ColumnsActive;
		bool _screenSameColorBorderActive;
		bool _videoResetActive;
		/** Graphical modes. */
		bool _graphicBitModeActive;
		bool _graphicExtendedColorTextModeActive;
		bool _graphicMulticolorTextModeActive;
		GraphicMode _graphicModeActive;
		/** IRQ Control. */
		bool _rasterIRQActive;
		bool _spriteCollisionWithDataIRQActive;
		bool _spriteCollisionsIRQActive;
		bool _lightPenIRQActive;
		/** Raster Control. */
		unsigned short _IRQRasterLineAt; // To define where to launch the IRQ. When reading therre is other variable...
		/** Location of the Graphical Memory. */
		MCHEmul::Address _charDataMemory; // Info about the characters
		MCHEmul::Address _screenMemory; // Where the characters to draw are
		MCHEmul::Address _bitmapMemory; // Where the bitmap to draw is

		// Some of this variables are set by the emulation of the VICII
		// The VICII chip also uses this object as a temporary storage
		unsigned short _currentRasterLine;
		unsigned short _currentLightPenHorizontalPosition, _currentLightPenVerticalPosition;
		bool _rasterAtIRQLine;
		bool _spritesCollisionWithDataHappened; /** In general. */
		mutable std::vector <bool> _spriteCollisionWithDataHappened; // Are modified when reading...
		bool _spritesCollisionHappened; /** In general. */
		mutable std::vector <bool> _spriteCollisionHappened;
		bool _lightPenOnScreenHappened;
		bool _vicIItoGenerateIRQ;
	};
}

#endif
  
// End of the file
/*@}*/
