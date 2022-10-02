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
#include <C64/ChipRegisters.hpp>

namespace C64
{
	/** In the VICII Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIIRegisters final : public ChipRegisters
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

		VICIIRegisters (int id, MCHEmul::PhisicalStorage* ps)
			: ChipRegisters (id, ps, 0xd000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400),
			  _lastValueRead (MCHEmul::PhisicalStorage::_DEFAULTVALUE),
			  _backgroundColor (4, 0x00),
			  _spriteXCoord (8, 0x0000), _spriteYCoord (8, 0x0000),
			  _spriteColor (8, 0x0000),
			  _spriteSharedColor (2, 0x0000), _spriteMulticolor (8, false),
			  _spriteEnabled (8, false), 
			  _spriteDoubleWidth (8, false), _spriteDoubleHeight (8, false),
			  _spriteToForegroundPriority (8, false)
			  // At this point the rest internal variables will have random values...
			  // The vector are initialized just to given them a default size!
							{ initializeInternalValues (); }

		virtual size_t numberRegisters () const override
							{ return (0x40); }

		unsigned char borderColor () const
							{ return (_borderColor); }
		unsigned char backgroundColor (size_t p = 0) const
							{ return ( _backgroundColor [p]); }

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
		bool spriteDoubleWidth (size_t p) const
							{ return (_spriteDoubleWidth [p]); }
		bool spriteDoubleHeight (size_t p) const
							{ return (_spriteDoubleHeight [p]); }
		bool spriteToForegroundPriority (size_t p) const
							{ return (_spriteToForegroundPriority [p]); }
		
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

		const MCHEmul::Address charDataMemory () const
							{ return (_charDataMemory + 
								(size_t) (0x4000 /* 16284 = 16k */ * _bank) /** VICII Only addresses 16k. */); }
		const MCHEmul::Address screenMemory () const 
							{ return (_screenMemory + (size_t) (0x4000 * _bank)); }
		const MCHEmul::Address bitmapMemory () const 
							{ return (_bitmapMemory + (size_t) (0x4000 * _bank)); }

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

		unsigned char bank () const
							{ return (_bank); }
		void setBank (unsigned char bk)
							{ if (bk == 0 || bk == 1 || bk == 2 || bk == 3) _bank = bk; }
		
		virtual void initialize () override;

		friend std::ostream& operator << (std::ostream& o, const VICIIRegisters& vr);

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
		unsigned char _borderColor;
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
		/** IRQ Control. */
		bool _rasterIRQActive;
		bool _spriteCollisionWithDataIRQActive;
		bool _spriteCollisionsIRQActive;
		bool _lightPenIRQActive;
		/** Raster Control. */
		unsigned short _IRQRasterLineAt; // To define where to launch the IRQ. When reading therre is other variable...
		/** Location of the Graphical Memory. */
		MCHEmul::Address _charDataMemory; // Info about the characters (the address with in the first 16k)
		MCHEmul::Address _screenMemory; // Where the characters to draw are (The address within the first 16k)
		MCHEmul::Address _bitmapMemory; // Where the bitmap to draw is (The address within the first 16k)

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
