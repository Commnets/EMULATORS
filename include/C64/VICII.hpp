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
		static const unsigned int _ID = 1;

		// Public data about the behaviour of the VICII chip
		/** Data about the raster lines of the screen. */
		static const unsigned short _RASTERLINES = 312;
		static const unsigned short _RASTERVISIBLELINES = 284;
		static const unsigned short _RASTERFIRSTVISIBLELINE = 14;
		static const unsigned short _RASTERLASTVISIBLELINE = 298;
		static const unsigned short _RASTERFIRSTGRAPHLINE = 56;
		static const unsigned short _RASTERLASTGRAPHLINE = 256;
		/** Regarding the horizontal space. */
		static const unsigned short _SCREENCOLUMNS = 504;
		static const unsigned short _SCREENVISIBLECOLUMNS = 403;
		static const unsigned short _SCREENFIRSTGRAPHCOLUMN = 42;
		/** Data about the size of the screen */
		static const unsigned short _GRAPHCHARLINES = 25;
		static const unsigned short _GRAPHCHARCOLUMNS = 40;
		static const unsigned short _GRAPHBITMAPCOLUMNS = 320;
		static const unsigned short _GRAPHBITMAPROWS = 200;

		VICII (const MCHEmul::Attributes& attrs);

		~VICII ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		void drawRasterCharMode ();
		void drawRasterBitmapMode ();
		void drawRasterSprites ();
		
		void drawChar (unsigned short x, unsigned short y, MCHEmul::UByte dt, unsigned int c);
		void drawMultiColorChar (unsigned short x, unsigned short y, MCHEmul::UByte dt, unsigned int c);
		void drawBitMap (unsigned short x, unsigned short y, MCHEmul::UByte dt, unsigned int c);
		void drawMultiColorBitMap (unsigned short x, unsigned short y, MCHEmul::UByte dt, unsigned int c);
		void drawSprite (unsigned short x, unsigned short y, size_t s, unsigned short row);
		void drawMultiColorSprite (unsigned short x, unsigned short y, size_t s, unsigned short row);

		// To get definition bytes from the memory...
		/** To get the character written in a position of the screen 
			(the screen can be placed in different locations managing the registers of the VIC) */
		MCHEmul::UByte getScreenCharCode (unsigned short c, unsigned short r)
							{ return (memoryRef () -> value (_VICIIRegisters -> screenMemory () + (r * _GRAPHCHARCOLUMNS + c))); }
		/** To get the color of the character written in a position of the screen 
			(the memory color is always at the same location). */
		MCHEmul::UByte getScreenCharColor (unsigned short c, unsigned short r)
							{ return (memoryRef () -> value (_COLORMEMORY + (r * _GRAPHCHARCOLUMNS + c))); }
		/** To get the bytes defining a specific character. 
			Where that information is in the memory can also be changed using the VICII registers. */
		MCHEmul::UByte getCharData (unsigned char chr, unsigned short r)
							{ return (memoryRef () -> value (_VICIIRegisters -> charMemory () + ((((unsigned int) chr) << 3) + r))); }
		/** To get the bytes defining a 8 pixels in a bitmap. 
			Again where the bitmap is stored can be changed using the VICII registers. */
		MCHEmul::UByte getBitMapData (unsigned short c, unsigned short r, unsigned short l)
							{ return (memoryRef () -> value (_VICIIRegisters -> bitmapMemory () + ((r * _GRAPHCHARCOLUMNS + c) << 3) + l)); }
		/** To get the address where to find the definition of an sprite. */
		MCHEmul::Address getSpriteAddress (size_t s)
							{ return (MCHEmul::Address ()); }

		private:
		/** The memory is used also as the set of registers of the chip. */
		C64::VICIIRegisters* _VICIIRegisters;

		// Implementation
		/** Keeps the raster line that is being processed. 
			This is important to determine what is the border and what is the content of the screen. */
		unsigned short _nextRasterCycle;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;

		// Private data regarding the behaviour of the VICII chip
		/** Regarding the clock cycles. */
		static const unsigned short _BADLINERASTERCYCLES = 23;
		static const unsigned short _USUALRASTERCYCLES = 63;

		/** Static address. The color memory cann't be changed. */
		static const MCHEmul::Address _COLORMEMORY;
	};

	/** The version para NTSC systems. */
	class VICII_NTSC final : public VICII
	{
		public:
		VICII_NTSC ();
	};

	/** The version para PAL systems. */
	class VICII_PAL final : public VICII
	{
		public:
		VICII_PAL ();
	};
}

#endif
  
// End of the file
/*@}*/
