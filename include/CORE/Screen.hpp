/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: Emulating the screen connected to any computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SCREEN__
#define __MCHEMUL_SCREEN__

#include <CORE/IO.hpp>
#include <CORE/GraphicalChip.hpp>
#include <CORE/Clock.hpp>
#include <SDL.h>

namespace MCHEmul
{
	/** Represents the very typical output device of any computer: The Screen. ºn
		The screen can be drawn (or not) with a border. \n
		The way the border is drwan must be implemented in very simulatuion in the method "drawAdditional". \n
		At this generic class there is onky a method to activat eor desactivate that possibility what 
		allows a global command to exist. */
	class Screen : public IODevice
	{
		public:
		/** 
		  * Creates the instance of the window.
		  * @param n	: The title of the windo to show.
		  * @param id	: The id of the device.
		  * @param c	: The list of the chips related with this device.
		  * @param sc	: The number of columns of the screen.
		  * @param sr	: The number of rows of the screen.
		  * @param vFX	: The visibility factor. That is how many visible pixels represents a pixel in the computerm, in the X axis.
		  * @param vFY	: The visibility factor. That is how many visible pixels represents a pixel in the computerm, in the Y axis.
		  * @param nC	: The number of colors.
		  * @param hz	: The speed of the refresh in Hz.
		  */
		Screen (const std::string& n, int id,
			unsigned int sc, unsigned int sr, double vFX, double vFY, double hz,
			const Attributes& attrs = { });

		virtual ~Screen () override;

		/** Activate or desactivate the CRT mode. */
		void setCRTEffect (bool a);

		/** To activate or desactivate to draw a possible grid.
			That grid can be in many differenty ways which is something to be decided in the implementation. */
		void setDrawGrid (bool dB, unsigned int c = 0)
							{ _drawGrid = dB; _gridColor = c; }
		bool drawGrid () const
							{ return (_drawGrid); }
		unsigned int gridColor () const // Must be within the max number of colors managed by the computer emulation...
							{ return (_gridColor); }

		virtual void linkToChips (const Chips& c) override;

		/** The reference to the graphical chip. */
		const GraphicalChip* graphicalChip () const
							{ return (_graphicalChip); }
		GraphicalChip* graphicalChip ()
							{ return (_graphicalChip); }

		/** To get the value of all parameters. */
		const std::string screenName () const
							{ return (_screenName); }
		const unsigned int screenColumns () const
							{ return (_screenColumns); }
		const unsigned int screenRows () const
							{ return (_screenRows); }
		const double visibilityFactorX () const
							{ return (_visibilityFactorX); }
		const double visibilityFactorY () const
							{ return (_visibilityFactorY); }
		const double hertzs () const
							{ return (_hertzs); }
		unsigned int realHertzs () const
							{ return (_clock.realCyclesPerSecond ()); }

		/** Not possible to connect any peripheral. */
		virtual bool connectPeripheral (IOPeripheral* p) override
							{ return (false); }

		virtual bool initialize () override;

		/** Draws the screen using the info of the frame. */
		virtual bool simulate (CPU* cpu) override;

		/**
		  *	Adding to parent fields:
		  * NAME	= Name of the screen. \n
		  * COLUMNS	= The size in columns. \n
		  * ROWS	= The size in rows. \n
		  * HERTZS	= The refresh rate. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** To draw in the screen. \n
			Take care: _graphicalChip is not checked!, so the methods must not be used
			until the computer emulated is fully initialized. */
		void setGraphics (const std::vector <std::vector <UBytes>>& gr, 
			const std::vector <UBytes>& dgr)
							{ _graphicsDef = gr, _defaultGraphicDef = dgr; }
		void setGraphics (std::vector <std::vector <UBytes>>&& gr, std::vector <UBytes>&& dgr)
							{ _graphicsDef = std::move (gr), _defaultGraphicDef = std::move (dgr); }
		void drawPoint (size_t x, size_t y, unsigned int c)
							{ _graphicalChip -> screenMemory () -> setPixel (x, y, c); }
		void drawHorizontalLine (size_t x, size_t y, size_t l, unsigned int c)
							{ for (size_t i = 0; i < l; drawPoint (x + (i++), y, c)); }
		void drawHorizontalLineStep (size_t x, size_t y, size_t l, size_t s, unsigned int c)
							{ for (size_t i = 0; i < (l - s); drawPoint (x + (i += s), y, c)); }
		void drawVerticalLine (size_t x, size_t y, size_t l, unsigned int c)
							{ for (size_t i = 0; i < l; drawPoint (x, y + (i++), c)); }
		void drawVerticalLineStep (size_t x, size_t y, size_t l, size_t s, unsigned int c)
							{ for (size_t i = 0; i < (l - s); drawPoint (x, y + (i += s), c)); }
		inline void drawRectangle (size_t x0, size_t y0, size_t x1, size_t y1, unsigned int c);
		inline void drawLine (size_t x0, size_t y0, size_t x1, size_t y1, unsigned int c); // Very simple without antialiasing...
		/** The parameter is to indicate whether the graphics must be transparent or nor = false by default. \n
			The parameter ng indicates the number of graphics to draw. \n
			It it doesn't exist the default one will be drawn instead. \n
			x, y are the left up corner position of the graphic. \n
			cb is the color of the background (will depend omn the computer emulated). \n
			cf is the foreground color of the graphic. */
		inline void drawGraphic (size_t x, size_t y, size_t ng ,
			unsigned int cb, unsigned int cf, bool t = false);
		/** To draw a text. \n
			It is needed the translation between the ascii code and the graphics representing it. */
		inline void drawTextHorizontal (size_t x, size_t y, 
			const std::string& txt, unsigned int cb, unsigned int cf, bool t = false);
		inline void drawTextVertical (size_t x, size_t y, 
			const std::string& txt, unsigned int cb, unsigned int cf, bool t = false);

		protected:
		virtual void processEvent (const Event& evnt, Notifier* n) override;

		/** Before moving the graphs to the screen,
			something could be draw on top of everything, like a water maks.
			So this is an exit method that by default does nothing. \n
			Nothing to do by default. */
		virtual void drawAdditional ()
							{ }

		/** To get the graphic code equivalent to a ascii code.
			Must be redefined depending on the computer emulated. */
		virtual size_t charCodeFromASCII (unsigned char id) const = 0;

		/** To get the graphic data for a code or the default one if not possible. */
		const std::vector <UBytes>& graphicData (size_t id) const
							{ return ((id < _graphicsDef.size ()) ? _graphicsDef [id] : _defaultGraphicDef); }

		protected:
		bool _CRTActive; // When CRT effect is visible...
		const std::string _screenName;
		const unsigned int _screenColumns;
		const unsigned int _screenRows;
		const double _visibilityFactorX;
		const double _visibilityFactorY;
		const double _hertzs;
		/** The clock to control the frenquency to print out the screen. */
		Clock _clock;
	
		// To draw or not to draw a grid...
		// The way the grid is finbally draw will depend on the screen implemented....
		bool _drawGrid;
		unsigned int _gridColor;

		// Implementation
		/** Set when initialize after assigning the chips.
			The graphical chip managing the graphical memory. 
			That the screen displays. */
		GraphicalChip* _graphicalChip;
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		/** The texture where the draw happens. */
		SDL_Texture* _texture;
		/** To indicate that the graphics at chip level are ready
			and the screen can be actualized. */
		bool _graphicsReady;

		/** Related with the graphics. \n
			The context is null by default and it has to be defined depending on every computer emulator. */
		std::vector <std::vector <UBytes>> _graphicsDef;
		std::vector <UBytes> _defaultGraphicDef;
	};

	// ---
	inline void Screen::drawRectangle (size_t x0, size_t y0, size_t x1, size_t y1, unsigned int c)
	{
		drawHorizontalLine (x0, y0, x1 - x0 + 1, c);
		drawHorizontalLine (x0, y1, x1 - x0 + 1, c);
		drawVerticalLine   (x0, y0, y1 - y0 + 1, c);
		drawVerticalLine   (x1, y0, y1 - y0 + 1, c);
	}

	// ---
	inline void Screen::drawLine (size_t x0, size_t y0, size_t x1, size_t y1, unsigned int c)
	{
		auto plotLineLow = [&](int x0, int y0, int x1, int y1, unsigned int c) -> void
			{
				int dx = x1 - x0;
				int dy = y1 - y0;
				int yi = 1;
				if (dy < 0)
				{ 
					yi = -1;
					dy = -dy;
				}

				int D = (2 * dy) - dx;
				int y = y0;
				for (int x = x0; x <= x1; x++)
				{ 
					drawPoint ((size_t) x, (size_t) y, c);

					if (D > 0)
					{
						y = y + yi;
						D = D + (2 * (dy - dx));
					}
					else
						D = D + 2 * dy;
				}
			};

		auto plotLineHigh = [&](int x0, int y0, int x1, int y1, unsigned int c) -> void
			{
				int dx = x1 - x0;
				int dy = y1 - y0;
				int xi = 1;
				if (dx < 0)
				{ 
					xi = -1;
					dx = -dx;
				}

				int D = (2 * dx) - dy;
				int x = x0;
				for (int y = y0; y <= y1; y++)
				{ 
					drawPoint ((size_t) x, (size_t) y, c);

					if (D > 0)
					{
						x = x + xi;
						D = D + (2 * (dx - dy));
					}
					else
						D = D + 2 * dx;
				}
			};

		if (std::abs ((int) y1 - (int) y0) < std::abs ((int) x1 - (int) x0))
		{ 
			if (x0 > x1) plotLineLow ((int) x1, (int) y1, (int) x0, (int) y0, c);
			else plotLineLow ((int) x0, (int) y0, (int) x1, (int) y1, c);
		}
		else
		{ 
			if (y0 > y1) 
				plotLineHigh ((int) x1, (int) y1, (int) x0, (int) y0, c);
			else 
				plotLineHigh ((int) x0, (int) y0, (int) x1, (int) y1, c);
		}
	}

	//
	inline void Screen::drawGraphic (size_t x, size_t y, size_t ng, 
		unsigned int cb, unsigned int cf, bool t)
	{
		size_t pyi = y;
		const std::vector <MCHEmul::UBytes>& gr = graphicData (ng);
		for (const auto& i : gr)
		{
			int ji = (int) (i.sizeBits () - 1);
			for (int j = ji; j >= 0; j--)
			{
				if (i.bit ((size_t) j))
				{
					drawPoint (x + (ji - (size_t) j), pyi, cf);
				}
				else
				if (!t)
					drawPoint (x + (ji - (size_t) j), pyi, cb);
			}

			pyi++;
		}
	}

	// ---
	inline void Screen::drawTextHorizontal (size_t x, size_t y, 
		const std::string& txt, unsigned int cb, unsigned int cf, bool t)
	{
		auto maxWide = [](const std::vector <UBytes>& gr) -> size_t
			{
				size_t result = 0;
				for (const auto& i : gr)
					if (i.sizeBits () > result) 
						result = i.sizeBits ();
				return (result);
			};

		size_t xi = x;
		for (auto i : txt)
		{ 
			const std::vector <UBytes>& gr = 
				graphicData (charCodeFromASCII ((unsigned char) i));
			drawGraphic (xi, y, charCodeFromASCII ((unsigned char) i), cb, cf, t);
			xi += maxWide (gr);
		}
	}

	// ---
	inline void Screen::drawTextVertical (size_t x, size_t y, 
		const std::string& txt, unsigned int cb, unsigned int cf, bool t)
	{
		size_t yi = y;
		for (auto i : txt)
		{ 
			const std::vector <UBytes>& gr = 
				graphicData (charCodeFromASCII ((unsigned char) i));
			drawGraphic (x, yi, charCodeFromASCII ((unsigned char) i), cb, cf, t);
			yi += gr.size ();
		}
	}
}

#endif
  
// End of the file
/*@}*/
