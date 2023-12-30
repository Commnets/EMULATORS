#include <CORE/Bus.hpp>
#include <CORE/MBElement.hpp>

// ---
void MCHEmul::Bus::connectElement (MCHEmul::MotherboardElement* mE)
{ 
	assert (mE != nullptr);

	mE -> observe (this);

	mE -> connectToBus (this);
}

// ---
void MCHEmul::Bus::disconnectElement (MCHEmul::MotherboardElement* mE)
{
	assert (mE != nullptr);

	mE -> unObserve (this);

	mE -> disconnectFromBus (this);
}
