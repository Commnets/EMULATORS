/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: Emulating the screen connected to any computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SCREEN__
#define __MCHEMUL_SCREEN__

#include <CORE/IO.hpp>
#include <SDL.h>

namespace MCHEmul
{
	/** Represents the very typical output device of any computer: The Screen. */
	class Screen : public IODevice
	{
		public:
		/** 
		  * Creates the instance of the window.
		  * @param n	: The title of the windo to show.
		  * @param id	: The id of the device.
		  * @param c	: The list of the chipds related with this device.
		  * @param sc	: The number of columns of the screen.
		  * @param sr	: The number of rows of the screen.
		  * @param vF	: The visibility factor. That is hw many visible pixels represents a pixel in the computer.
		  * @param nC	: The number of colors.
		  * @param hz	: The speed of the refresh in Hz.
		  */
		Screen (const std::string& n, int id,
			unsigned int sc, unsigned int sr, unsigned int vF, unsigned int nC, double hz,
			const Attributes& attrs = { });

		Screen (const Screen*) = delete;

		Screen& operator = (const Screen&) = delete;

		virtual ~Screen () override;

		/** To get the value of all parameters. */
		const std::string screenName () const
							{ return (_screenName); }
		const unsigned int screenCols () const
							{ return (_screenCols); }
		const unsigned int screenRows () const
							{ return (_screenRows); }
		const unsigned int visibilityFactor () const
							{ return (_visibilityFactor); }
		const unsigned int numberColors () const
							{ return (_numberColors); }
		const double hertzs () const
							{ return (_hertzs); }

		/** It is usually invoked from video chips, to set the pixels. */
		void setPixel (unsigned int x, unsigned int y, unsigned int color)
							{ _frame [y * _screenCols + x] = _colorPalette [color]; }

		virtual bool initialize () override;

		/** Draws the screen using the info of the frame. */
		virtual bool refresh () override;

		protected:
		/** Invoked from initialize.
			The palette of colors will depend on the computer. */
		virtual void initializeColorPalette () = 0;

		protected:
		const std::string _screenName;
		const unsigned int _screenCols;
		const unsigned int _screenRows;
		const unsigned int _visibilityFactor;
		const unsigned int _numberColors;
		unsigned int* _colorPalette; // values set when initialize...
		const double _hertzs;

		protected:
		// Implementation
		// Screen
		SDL_Window *_window;
		SDL_Renderer *_renderer;
		SDL_Texture *_texture;
		SDL_PixelFormat *_format;
		uint32_t *_frame;

		double _refreshRate;
	};
}

#endif
  
// End of the file
/*@}*/
