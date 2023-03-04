/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: NotifyObserver.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/01/2023 \n
 *	Description: Define the interface to notify event between pieces in the emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_NOTIFYOBSERVER__
#define __MCHEMUL_NOTIFYOBSERVER__

#include <CORE/global.hpp>

namespace MCHEmul
{
	class Notifier;
	class Observer;

	/** The event is what is transmitted among pieces. */
	class Event final
	{
		public:
		struct Data
		{
			// Just to support data per event...
			virtual ~Data () 
							{ }
		};

		/** The Event is very simple it has only an identification number (id), 
			a potential additional value and also a pointer to data. \n
			This two last parameters are optional. \n
			The data doesn't belong to the class. So it is not destructed either. */
		Event (unsigned int id, unsigned int val = 0, std::shared_ptr <Data> dt = nullptr)
			: _id (id), _value (val), _data (dt)
							{ }

		unsigned int id () const
							{ return (_id); }
		unsigned int value () const
							{ return (_value); }
		const std::shared_ptr <Data> data () const
							{ return (_data); }
		std::shared_ptr <Data> data ()
							{ return (_data); }

		protected:
		unsigned int _id;
		unsigned int _value;
		std::shared_ptr <Data> _data;
	};

	/** The notifier is the element who notifies that something has happen. */
	class Notifier
	{
		public:
		friend Observer;

		/** To avoid inconsistency and recurrency invokin unObserve. */
		virtual ~Notifier ();

		/** To notify an event to the observers interested. */
		void notify (const Event& evnt);

		protected:
		// To manage entries in the list of notifiers...
		// Only Observers can access to this part
		// The public interface of the notification system is always through the Observers!
		/** Ony once, but it is not checked fo performance reasons. */
		void addObserver (Observer* o)
							{ _observers.push_back (o); }
		/** If it doesn't exist nothing happens. */
		void removeObserver (Observer* o)
							{ _observers.erase (
								std::remove_if (_observers.begin (), _observers.end (), 
									[=](Observer* i) -> bool { return (o == i); }), _observers.end ()); }

		protected:
		/** The list of observers. */
		mutable std::vector <Observer*> _observers;
	};

	/** The class Observer is the class who process the elements sent from the notifiers. */
	class Observer
	{
		public:
		friend Notifier;

		/** Take care: When adding this interface, 
			the 5 construction rule should be also acomplished. */
		virtual ~Observer ()
							{ for (Notifier* i : _notifiers) i -> removeObserver (this); }

		/** To observe a new element. */
		void observe (Notifier* n)
							{ if (std::find (_notifiers.begin (), _notifiers.end (), n) == _notifiers.end ()) // Only once...
								{ n -> addObserver (this); _notifiers.push_back (n); } }
		/** To stop observing any element. */
		void unObserve (Notifier* n)
							{ n -> removeObserver (this); justTakeOff (n); }

		protected:
		/** What to do when an event is received. 
			This method has to be overloded in any Observer. */
		virtual void processEvent (const Event& evnt, Notifier* n)
							{ /** Nothing by default. */ }

		// Implementation
		/** Just to take a notifier off the list.
			It is invoked from the notifier destructor. */
		void justTakeOff (Notifier* n)
							{ _notifiers.erase (
								std::remove_if (_notifiers.begin (), _notifiers.end (), 
									[=](Notifier* i) -> bool { return (n == i); }), _notifiers.end ()); }

		protected:
		/** The lst of notifiers. */
		mutable std::vector <Notifier*> _notifiers;
	};

	// ---
	inline Notifier::~Notifier ()
	{ 
		for (Observer* i : _observers) 
			i -> justTakeOff (this); 
	}
}

#endif
  
// End of the file
/*@}*/
