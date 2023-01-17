#include <CORE/NotifyObserver.hpp>

// ---
MCHEmul::Notifier::~Notifier ()
{ 
	for (MCHEmul::Observer* i : _observers) 
		i -> justTakeOff (this); 
}

// ---
void MCHEmul::Notifier::notify (MCHEmul::Event&& evnt)
{
	for (Observer* i : _observers)
		i -> processEvent (std::move (evnt), this);
}
