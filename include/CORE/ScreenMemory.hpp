/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ScreenMemory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/07/2022 \n
 *	Description: The screen buffer used by the Graphics Chips to draw (if needed).
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SCREENMEMORY__
#define __MCHEMUL_SCREENMEMORY__

namespace MCHEmul
{
	/** It represents a the memory where all things to be sent to the screen are store. \n
		It is actually a matrix of unsigned int, 
		representing each one the color of a pixel in the screen. */  
	class ScreenMemory
	{
		public:
		ScreenMemory () = delete;

		ScreenMemory (size_t c, size_t r, unsigned int* p);

		ScreenMemory (const ScreenMemory&) = delete;

		ScreenMemory& operator = (const ScreenMemory&) = delete;

		virtual ~ScreenMemory ()
							{ delete (_frameData); delete (_colorPalette);}

		ScreenMemory (ScreenMemory&&) = delete;

		ScreenMemory& operator = (ScreenMemory&&) = delete;

		// Size...
		size_t columns () const
							{ return (_columns); }
		size_t rows () const
							{ return (_rows); }
		const unsigned int* frameData () const
							{ return (_frameData); }

		// Managing basically the pixels of the frame...
		void setPixel (size_t x, size_t y, unsigned int color)
							{ _frameData [y * _columns + x] = _colorPalette [color]; }
		void setHorizontalLine (size_t x, size_t y, size_t nP, unsigned int color)
							{ for (unsigned int i = 0; i < nP; setPixel (x + i++, y, color)); }
		void setHorizontalLineStep (size_t x, size_t y, size_t nP, unsigned int color, unsigned int s)
							{ for (unsigned int i = 0; i < (nP - s); setPixel (x + (i += s), y, color)); }
		void setVerticalLine (size_t x, size_t y, size_t nP, unsigned int color)
							{ for (unsigned int i = 0; i < nP; setPixel (x, y + i++, color)); }
		void setVerticalLineStep (size_t x, size_t y, size_t nP, unsigned int color, unsigned int s)
							{ for (unsigned int i = 0; i < (nP - s); setPixel (x, y + (i += s), color)); }

		private:
		unsigned int* _frameData;
		size_t _columns, _rows;
		unsigned int* _colorPalette;
	};
}

#endif
  
// End of the file
/*@}*/
