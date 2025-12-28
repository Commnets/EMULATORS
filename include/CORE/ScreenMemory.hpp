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

#include <vector>
#include <CORE\UByte.hpp>

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
		// Take into account that no bounds checking is done...
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
		inline void setBox (size_t x1, size_t y1, size_t nPX, size_t nPY, unsigned int color);
		void setByte (size_t x, size_t y, UByte c, unsigned int color)
							{ for (unsigned int i = 0; i < 8; i++) if (c.bit (7 - i)) setPixel (x + i, y, color); }
		inline void setChar (size_t x, size_t y, char c, unsigned int color);
		void setString (size_t x, size_t y, const std::string& s, unsigned int color)
							{ for (size_t i = 0; i < s.size (); i++) setChar (x + (i * 6), y, s [i], color); }

		inline std::vector <unsigned int> getValues () const;

		protected:
		unsigned int* _frameData;
		size_t _columns, _rows;
		unsigned int* _colorPalette;

		// The definition of the ascii codes...
		static const std::vector <std::vector <UByte>> _CHARS;
	};

	// ---
	inline void ScreenMemory::setBox (size_t x1, size_t y1, size_t nPX, size_t nPY, unsigned int color)
	{
		setHorizontalLine (x1, y1, nPX, color);
		setHorizontalLine (x1, y1 + nPY - 1, nPX, color);
		setVerticalLine (x1, y1, nPY, color);
		setVerticalLine (x1 + nPX - 1, y1, nPY, color);
	}

	// ---
	inline void ScreenMemory::setChar (size_t x, size_t y, char c, unsigned int color)
	{
		size_t p = (std::isalpha (c) 
			? (std::toupper (c) - 'A' + 11) // A=11,B=12,...,Z=36
			: (std::isdigit (c)
				? (c - '0' + 1) // 0=0, 1=1, ..., 9=9
				: 0 /** Space for any other char. */));

		for (size_t i = 0; i < 5; i++)
			setByte (x, y + i, _CHARS [p][i], color);
	}

	// ---
	inline std::vector <unsigned int> ScreenMemory::getValues () const
	{
		std::vector <unsigned int> result;
		for (size_t i = 0; i < (_columns * _rows); i++)
			result.emplace_back (_frameData [i]);
		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
