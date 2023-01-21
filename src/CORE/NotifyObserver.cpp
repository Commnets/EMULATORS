#include <CORE/NotifyObserver.hpp>

// ---
void MCHEmul::Notifier::notify (const MCHEmul::Event& evnt)
{
	for (Observer* i : _observers)
		i -> processEvent (evnt, this);
}
