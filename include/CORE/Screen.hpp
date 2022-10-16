/** \ingroup CORE */
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
#include <CORE/GraphicalChip.hpp>
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
		  * @param c	: The list of the chips related with this device.
		  * @param sc	: The number of columns of the screen.
		  * @param sr	: The number of rows of the screen.
		  * @param vF	: The visibility factor. That is hw many visible pixels represents a pixel in the computer.
		  * @param nC	: The number of colors.
		  * @param hz	: The speed of the refresh in Hz.
		  */
		Screen (const std::string& n, int id,
			unsigned int sc, unsigned int sr, unsigned int vF, double hz,
			const Attributes& attrs = { });

		Screen (const Screen*) = delete;

		Screen& operator = (const Screen&) = delete;

		virtual ~Screen () override;

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
		const unsigned int visibilityFactor () const
							{ return (_visibilityFactor); }
		const double hertzs () const
							{ return (_hertzs); }

		virtual bool initialize () override
							{ return (true); }

		/** Draws the screen using the info of the frame. */
		virtual bool simulate () override;

		protected:
		const std::string _screenName;
		const unsigned int _screenColumns;
		const unsigned int _screenRows;
		const unsigned int _visibilityFactor;
		const double _hertzs;

		protected:
		/** Set when initialize after assigning the chips.
			The graphical chip managing the graphical memory. 
			That the screen displays. */
		GraphicalChip* _graphicalChip;

		// Implementation
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		SDL_Texture* _texture;

		/** At construction time, to speed up calculus. */
		double _refreshRate;
	};
}

#endif
  
// End of the file
/*@}*/
