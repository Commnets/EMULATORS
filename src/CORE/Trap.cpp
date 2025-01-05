#include <CORE/Trap.hpp>
#include <CORE/Memory.hpp>

// ---
bool MCHEmul::Trap::verifyFingerTipAgainst (Memory* mem) const
{
	bool result = true;

	bool match = true;
	std::vector <MCHEmul::UByte> mDt = 
		mem -> values (_addressIn, _fingerTip.size ()).bytes ();
	for (size_t i = 0; i < mDt.size () && match; i++)
		match = (mDt [i] == _fingerTip [i]);

	return (result);
}
