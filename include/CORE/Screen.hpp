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
#include <CORE/Clock.hpp>
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

		virtual ~Screen () override;

		/** Activate or desactivate the CRT mode. */
		void setCRTEffect (bool a)
							{ _CRTActive = a; }

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

		protected:
		virtual void processEvent (const Event& evnt, Notifier* n) override;

		/** Before moving the graphs to the screen,
			something could be draw on top of everything, like a water maks.
			So this is an exit method that by default does nothing. \n
			Nothing to do by default. */
		virtual void drawAdditional ()
							{ }

		protected:
		bool _CRTActive; // When CRT effect is visible...
		const std::string _screenName;
		const unsigned int _screenColumns;
		const unsigned int _screenRows;
		const unsigned int _visibilityFactor;
		const double _hertzs;
		/** The clock to control the frenquency to print out the screen. */
		Clock _clock;

		// Implementation
		/** Set when initialize after assigning the chips.
			The graphical chip managing the graphical memory. 
			That the screen displays. */
		GraphicalChip* _graphicalChip;
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		/** The texture where the draw happens. */
		SDL_Texture* _texture;
		/** A texture created and initialized at construction time just to similñate the CRT effect. */
		SDL_Texture* _CRTTextureEffect;
		/** To indicate that the graphics at chip level are ready
			and the screen can be actualized. */
		bool _graphicsReady;
	};
}

#endif
  
// End of the file
/*@}*/
