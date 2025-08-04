/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: OSIOPeripheral.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 31/07/2025 \n
 *	Description: To emulate the behaviour of the elements connected 
 *				 to an InputOSSystem (unique per simulation). \n
 *				 Those elements can be useful to e.g. inject events into the system from an external source.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_OSIOPERIPHERALS__
#define __MCHEMUL_OSIOPERIPHERALS__

#include <CORE/IOPeripheral.hpp>
#include <CORE/OBool.hpp>
#include <SDL_events.h>

namespace MCHEmul
{
	class Computer;
	class InputOSSystem;
	
	/** Any InputOSSystem Peripheral must inherit from this class. */
	class InputOSSystemPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		/** The commands accepted by this peripheral. */
		static const int _FOWARD	= 1;
		static const int _REWIND	= 2;
		static const int _STOP		= 4;
		static const int _PLAY		= 8;
		static const int _BEGINNING = 16; // Moves the situation at the beginning and stops!
		static const int _EJECT		= 32; // To clean up the data loaded, or to simulate a new casette is inserted...

		/** To set up an instance of this class.
			A computer reference must be given to determine the InputOSSystem from it. */
		InputOSSystemPeripheral (int id, unsigned int wt, Computer* c, const Attributes& attrs);
	
		/** Is there any event ready to be process. 
			Once checked, if there was any, the signal is disactivated. */
		bool eventReady () const
							{ return (_eventReady); }
		bool peekEventReady () const
							{ return (_eventReady.peekValue ()); }
		/** To get the last event ready to be injected.
			Obviously it will only make sense when _eventReady is true. */
		const SDL_Event& eventToInject () const
							{ return (_eventToInject); }
	
		virtual bool initialize () override;
		/** Nothing to be finished. */
		virtual bool finalize () override
							{ return (true); }

		// Notice that the methods related with the mabnagement of the FileData are not defined here...
		// ...because the way the FileData info data is managed will depend on the specific peripheral...

		virtual bool executeCommand (int id, const Strings& prms) override;
		virtual Strings commandDescriptions () const override
							{ return (Strings (
								{ "1:FORWARD", "2:REWIND", "4:STOP", "8:PLAY", 
								  "16:BEGINING", "32:EJECT(and clear data)" })); }

		/** The simulation will consists on injecting an event every 1/25 seconds. \n
			That number ois defined in the header. */
		virtual bool simulate (CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class plus:
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** This method has to be invoked from connectData once the FileData received has been "received and "translated"
			into SDL_Events in the method connectData that has to be overloaded. */
		void loadEvents (const std::vector <SDL_Event>& evnts);

		protected:
		/** This list has to be loaded with the method load events. 
			The way the list is loaded will depend on the format of the information 
			received usually through out the method connectData \n
			That methid has to be overloaded in the specific peripheral. */
		std::vector <SDL_Event> _events;
		/** Number of 1/25 seconds to wait before injecting the next event. */
		unsigned int _waitTime;
		/** Extracted from the computer. */
		InputOSSystem* _OSIO;
		
		// Implementation
		/** When it is running. 
			By default it will be always stooped. \n
			The command PLAY must be sent. */
		bool _running;
		/** When there is an event ready to be processed. */
		OBool _eventReady;
		/** The event to be tretaed. */
		SDL_Event _eventToInject;
		/** Which is the number of event to ve injected. */
		size_t _eventNumber;
		/** The counter of witing times blocks. */
		unsigned int _waitTimeCounter;
	};
}

#endif
  
// End of the file
/*@}*/
