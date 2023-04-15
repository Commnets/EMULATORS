/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: GraphicalChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/06/2022 \n
 *	Description: A special type of chip mamanging a screen memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_GRAPHICALCHIP__
#define __MCHEMUL_GRAPHICALCHIP__

#include <CORE/Chip.hpp>
#include <CORE/ScreenMemory.hpp>

namespace MCHEmul
{
	/**
	  * GraphicalChip is very connected with a Screen. \n
	  * The graphical chip uses the ScreenMemory to write. \n
	  * The same object will be taken by th Screen to finaly put the graphics out to the display. \n
	  * The instance of the ScreenMemory object used is created in the method "createScreenMemory ()" that has to be overload. \n
	  * When a graphic is ready (complete) to be sent to the screen, a notification has to be sent. \n
	  * That notification will be received at the screen and converted into the graphics. */
	class GraphicalChip : public Chip
	{
		public:
		// Different events.
		/** When the graphical chip is ready to send out the graphic bufer filled up
			a notification with this event should happen. (@see Screen for more info). */
		static const unsigned int _GRAPHICSREADY				= 100;

		GraphicalChip (int id, const Attributes& attrs = { })
			: Chip (id, attrs), 
			  _screenMemory (nullptr)
							{ }

		virtual ~GraphicalChip () override
							{ delete (_screenMemory); }

		/** To know the size of the window. */
		virtual unsigned short numberColumns () const = 0;
		virtual unsigned short numberRows () const = 0;
		int screenBufferSize () const
							{ return (numberColumns () * numberRows ()); }

		// They will be nullptr if the initialize method hasn't been invoked...
		const ScreenMemory* screenMemory () const
							{ return (_screenMemory); }
		ScreenMemory* screenMemory ()
							{ return (_screenMemory); }

		virtual bool initialize () override;

		protected:
		/** Invoked from initialize to create the right screen memory. */
		virtual ScreenMemory* createScreenMemory () = 0;

		protected:
		ScreenMemory* _screenMemory;
	};
}

#endif
  
// End of the file
/*@}*/

