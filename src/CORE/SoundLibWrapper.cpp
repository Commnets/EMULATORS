#include <CORE/SoundLibWrapper.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::SoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ATTRS", _attributes);

	return (result);
}
