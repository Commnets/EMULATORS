#include <CORE/DebugFile.hpp>

// ---
bool MCHEmul::DebugFile::activate (const std::string& fN, bool a)
{
	if (_active)
		return (false); // It is already active...

	_file.open (_fileName = fN, (_addInfo = a)
		? (std::ios::out | std::ios::app) /** Add at the end */
		: (std::ios::out | std::ios::trunc) /** Remove everything */ );
	if (!_file)
		return (false);

	_active = true;

	return (true);
}

bool MCHEmul::DebugFile::desactivate ()
{
	if (!_active)
		return (false); // ...no desactivation will be possible, because it is alredy no active...

	// It can not be desactivated when it has been blocked for desactivation...
	if (_blockDesactivate)
	{
		_desactivateTryWhenBlockDesactivate = true;

		return (false);
	}

	_active = false;

	_file.close ();

	return (true);
}
