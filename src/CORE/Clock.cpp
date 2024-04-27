#include <CORE/Clock.hpp>

// ---
std::string MCHEmul::Clock::asString () const
{
	std::string result;

	bool f = true;
	for (const auto& i : _ticks)
	{
		result += ((!f) ? "\n" : "") + i.asString ();

		f = false;
	}

	return (result);
}
