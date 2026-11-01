/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICIRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VICI Registers Emulation.
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICIREGISTERS__
#define __COMMODORE_VICIREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/VICI/VICISoundWrapper.hpp>

namespace COMMODORE
{
	class VICI;

	/** In the VICI Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIRegisters final : public MCHEmul::ChipRegisters
	{
		public:
		friend VICI;

		static const int _VICREGS_SUBSET = 1040;

		VICIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }
		
		/** To know whether the status of the interlaced mode. */
		bool interlacedActive () const
							{ return (_interlaced); }

		/** The screen. */
		unsigned char offsetXScreen () const
							{ return (_offsetXScreen); }
		unsigned char charsWidthScreen () const
							{ return (_charsWidthScreen); }
		unsigned char offsetYScreen () const
							{ return (_offsetYScreen); }
		unsigned char charsHeightScreen () const
							{ return (_charsHeightScreen); }

		/** Chars expanded. */
		bool charsExpanded () const
							{ return (_charsExpanded); }

		// Managing the raster
		// The raster info is in the VICI chip actually.
		// However its info is needed when checking some registers...
		void linkToRaster (MCHEmul::Raster* r)
							{ _raster = r; }
		unsigned short currentRasterLine () const
							{ return (_raster -> vData ().currentPosition ()); }
		unsigned short currentRasterPositionInLine () const
							{ return (_raster -> hData ().currentPosition ()); }

		// Knowing info about the lightpen...
		/** The lightpen is simulated using the mouse. */
		inline void latchLightPenPositionFromRaster (unsigned char x, unsigned char y)
							{ _latchLightPenHorizontalPosition = x; _latchLightPenVerticalPosition = y; }
		void lightPenPositionLatched (unsigned char* x, unsigned char* y)
							{ *x = _latchLightPenHorizontalPosition; *y = _latchLightPenVerticalPosition; }
		bool lightPenActive () const
							{ return (_lightPenActive); }
		void setLigthPenActive (bool lP)
							{ _lightPenActive = lP; }

		// Managing the info about the mouse, that is used to similate the ligthtPen
		/** To know where the mouse is within the visible zone, if is is.
			When it is not there the variables returned -1. */
		int mousePositionX () const
							{ return (_mousePositionX); }
		int mousePositionY () const
							{ return (_mousePositionY); }
		void setMousePosition (int x, int y)
							{ _mousePositionX = x; _mousePositionY = y; }
		bool isMouseInVisibleZone () const
							{ return (_mousePositionX != -1 && _mousePositionY != -1); }

		/** To manage the things related with the sound. */
		void setSoundLibWrapper (MCHEmul::SoundLibWrapper* w)
							{ _soundWrapper = dynamic_cast <VICISoundLibWrapper*> (w);
							  assert (_soundWrapper != nullptr); /** just on case... */ }

		/** Managing the colors... */
		unsigned char auxiliarColor () const
							{ return (_auxiliarColor); }
		unsigned char backgroundColor () const
							{ return (_backgroundColor); }
		unsigned char borderColor () const
							{ return (_borderColor); }
		bool inverseMode () const
							{ return (_inverseMode); }

		/** To knlow the different zones of the memory. */
		const MCHEmul::Address& screenMemory () const
							{ return (_screenMemory); }
		/** Add the offset in VIC address space before CPU-view translation. */
		MCHEmul::Address screenMemory (size_t offset) const
							{ return (cpuAddressFromVIC (_screenMemoryVIC + offset)); }
		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemory); }
		/** Add the offset in VIC address space before CPU-view translation. */
		MCHEmul::Address charDataMemory (size_t offset) const
							{ return (cpuAddressFromVIC (_charDataMemoryVIC + offset)); }
		const MCHEmul::Address& colourMemory () const
							{ return (_colourMemory); }
		/** Add an offset within the physical 1024-nibble color RAM. */
		inline MCHEmul::Address colourMemory (size_t offset) const;

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  *	AUXCOLOR		= Attribute; Auxiliar color used in multocolor graphic mode.
		  *	SCRCOLOR		= Attribute; Screen color used as a base of the writting zone.
		  *	BRDCOLOR		= Attribute; Color used in the border.
		  *	CHARADDRESS		= Attribute; The address of the characters definition. \n
		  *	SCREENADDRESS	= Attribute; The address of the video matrix. \n
		  *	COLOURADDRESS	= Attribute; The address of the colour map. \n
		  *	LIGHTPENX		= Attribute; Where the light pen X position is. \n
		  *	LIGHTPENY		= Attribute; Where the light pen Y position is.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** Used by construction and initialize(). \n
			Seeds every address selector before applying the register presets. */
		void initializeInternalValues ();
		/** Calculate the internal memory positions. */
		inline void calculateMemoryPositions ();
		/** Translate a wrapped 14-bit VIC address to its CPU-view equivalent. */
		inline MCHEmul::Address cpuAddressFromVIC (size_t address) const;

		private:
		/** Interlaced? */
		bool _interlaced;
		/** Offset X of the screen. \n
			Every inc in this value moves the screen 4 pixles to the right. \n
			The suitable (where the 1 column is already visible) values depend on the type of VIXCI chip. \n
			For PAL systems are between 5 and 19, and for NTSC between 1 and 8. */
		unsigned char _offsetXScreen;
		/** Offset Y of the screen. \n
			Every inc in this value moves the screen 2 pixels down. \n
			The suitable values (where the first row is already visible) depend on the type of VICI chip. \n
			For PAL systems are between 14 and 155 (312 raster rows/2 - 1), and for NTSC between (14 and 130 (261 raster rows-1)/2 */
		unsigned char _offsetYScreen;
		/** The width of the screen in chars. \n
			The suitable values depends on the video system. \n
			For PAL ones are between 0 and 29, and or NTSC between 0 and 26. */
		unsigned char _charsWidthScreen;
		/** The height of the screen in chars.
			The suitable values depends on the video system. \n
			For PAL ones are between 0 and 35, and for NSTC between 0 and 29. */
		unsigned char _charsHeightScreen;
		/** chars expanded?. */
		bool _charsExpanded;
		/** Memory numbers. */
		unsigned int _b9ScreenColorMemory, _b10to13ScreenMemory, _b10to13CharDatamemory;
		/** Where the lightpen has been detected. */
		unsigned char _latchLightPenHorizontalPosition;
		/** The position is consolidated once per frame. */
		unsigned char _latchLightPenVerticalPosition;
		/** To identify whether the light pen is or not active to take it into account in the simulation. */
		bool _lightPenActive;
		/** Non-owning sound wrapper linked by VICI. */
		VICISoundLibWrapper* _soundWrapper;
		/** Colors. */
		unsigned char _auxiliarColor, _backgroundColor, _borderColor;
		/** To indicate wheter the system is managing the inverse mode or not. */
		bool _inverseMode;

		// Some of this variables are set by the emulation of the VICI
		// The VICI chip also uses this object as a temporary storage
		/** Non-owning raster linked by VICI before raster-register reads. */
		MCHEmul::Raster* _raster;

		// Implementation...
		mutable MCHEmul::UByte _lastValueRead;
		/** Cached chip-space bases, recomputed only when address selectors change. */
		unsigned int _screenMemoryVIC, _charDataMemoryVIC;
		MCHEmul::Address _screenMemory;
		MCHEmul::Address _charDataMemory; 
		MCHEmul::Address _colourMemory;
		/** Where the mouse is. -1 when it is out of the visible zone. */
		int _mousePositionX, _mousePositionY;
	};

	// ---
	inline MCHEmul::Address VICIRegisters::colourMemory (size_t offset) const
	{
		return (MCHEmul::Address
			(2, 0x9400 + (unsigned int)
				(((_b9ScreenColorMemory << 9) + offset) & 0x03ff)));
	}

	// ---
	inline void VICIRegisters::calculateMemoryPositions ()
	{ 
		_screenMemoryVIC =
			((_b10to13ScreenMemory & 0x0f) << 10) |
			((_b9ScreenColorMemory & 0x01) << 9);
		_charDataMemoryVIC =
			(_b10to13CharDatamemory & 0x0f) << 10;

		_screenMemory = screenMemory (0);
		_charDataMemory = charDataMemory (0);

		_colourMemory = MCHEmul::Address
			(2, _b9ScreenColorMemory ? 0x9600 : 0x9400);
	}

	// ---
	inline MCHEmul::Address VICIRegisters::cpuAddressFromVIC (size_t address) const
	{
		address &= 0x3fff;

		// VIC A13 selects CPU A15 with inverted polarity.
		return (MCHEmul::Address
			(2, (unsigned int) ((address & 0x1fff) |
				((address & 0x2000) != 0 ? 0x0000 : 0x8000))));
	}

}

#endif
  
// End of the file
/*@}*/
