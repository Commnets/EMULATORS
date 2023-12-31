/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICIRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VICI Registers Emulation.
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICIREGISTERS__
#define __COMMODORE_VICIREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/VICISoundWrapper.hpp>

namespace COMMODORE
{
	/** In the VICI Registers, 
		there are a couple of records that behave different
		when they are read that when they are written. */
	class VICIRegisters final : public MCHEmul::ChipRegisters
	{
		public:
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
		unsigned char charHeightScreen () const
							{ return (_charsHeightScreen); }

		/** Chars expanded. */
		bool charsExpanded () const
							{ return (_charsExpanded); }

		/** The raster line. */
		unsigned short currentRasterLine () const
							{ return (_currentRasterLine); }
		void setCurrentRasterLine (unsigned short rL)
							{ _currentRasterLine = rL; }

		/** To manage the light pen. \n
			This temporal variables are set from the VICI directly. */
		unsigned short currentLightPenHorizontalPosition () const
							{ return (_currentLightPenHorizontalPosition); }
		unsigned short currentLightPenVerticalPosition () const
							{ return (_currentLightPenVerticalPosition); }
		void currentLightPenPosition (unsigned short& x, unsigned short& y)
							{ x = _currentLightPenHorizontalPosition; y = _currentLightPenVerticalPosition; }
		void setCurrentLightPenPosition (unsigned short x, unsigned short y)
							{ _currentLightPenHorizontalPosition = x; _currentLightPenVerticalPosition = y; }
		bool lightPenActive () const
							{ return (_lightPenActive); }
		void setLigthPenActive (bool lP)
							{ _lightPenActive = lP; }

		/** To manage the things related with the sound. */
		void setSoundLibWrapper (MCHEmul::SoundLibWrapper* w)
							{ _soundWrapper = dynamic_cast <VICISoundLibWrapper*> (w);
							  assert (_soundWrapper != nullptr); /** just on case... */ }

		/** Managing the colors... */
		unsigned char auxiliarColor () const
							{ return (_auxiliarColor); }
		unsigned char screenColor () const
							{ return (_screenColor); }
		unsigned char borderColor () const
							{ return (_borderColor); }
		bool inverseMode () const
							{ return (_inverseMode); }

		/** To knlow the different zones of the memory. */
		const MCHEmul::Address& screenMemory () const
							{ return (_screenMemory); }
		const MCHEmul::Address& charDataMemory () const
							{ return (_charDataMemory); }
		const MCHEmul::Address& colourMemory () const
							{ return (_colourMemory); }

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
		/** Just to initialize the internal values. */
		void initializeInternalValues ();
		/** Calculate the internal memory positions. */
		inline void calculateMemoryPositions ();

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
		/** The raster line. */
		unsigned short _currentRasterLine;
		/** Memory numbers. */
		unsigned int _b9ScreenColorMemory, _b10to13ScreenMemory, _b10to13CharDatamemory;
		/** Related with the pigh pen. */
		unsigned short _currentLightPenHorizontalPosition, _currentLightPenVerticalPosition; // Where the light pen is...
		bool _lightPenActive;
		/** The sound wrapper. */
		VICISoundLibWrapper* _soundWrapper;
		/** Colors. */
		unsigned char _auxiliarColor, _screenColor, _borderColor;
		/** To indicate wheter the system is managing the inverse mode or not. */
		bool _inverseMode;

		// Implementation...
		mutable MCHEmul::UByte _lastValueRead;
		MCHEmul::Address _screenMemory;
		MCHEmul::Address _charDataMemory; 
		MCHEmul::Address _colourMemory;
	};

	// ---
	inline void VICIRegisters::calculateMemoryPositions ()
	{ 
		_screenMemory	= MCHEmul::Address ((_b9ScreenColorMemory << 9) + 
			((_b10to13ScreenMemory & 0x07 /** The bit 3 is not considered. */) << 10));
		_charDataMemory = MCHEmul::Address (((_b10to13CharDatamemory & 0x80) == 0x00 ? 0x8000 : 0x0000) + 
			((_b10to13CharDatamemory & 0x07 /** The bit 3 is used to determine the bank. */) << 10)); // This is how VIC20 sees the memory!...
		_colourMemory	= MCHEmul::Address (_b9ScreenColorMemory == 0x00 ? 0x9400 : 0x9600 );
	}
}

#endif
  
// End of the file
/*@}*/
