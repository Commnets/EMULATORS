#include <CORE/Wire.hpp>
#include <CORE/MBElement.hpp>

// ---
void MCHEmul::Wire::connectElement (MCHEmul::MotherboardElement* mE)
{
	assert (mE != nullptr);

	mE -> observe (this);

	mE -> connectToWire (this);
}

// ---
void MCHEmul::Wire::disconnectElement (MCHEmul::MotherboardElement* mE)
{
	assert (mE != nullptr);

	mE -> unObserve (this);

	mE -> disconnectFromWire (this);
}
