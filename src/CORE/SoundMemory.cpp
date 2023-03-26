#include <CORE/SoundMemory.hpp>
#include <assert.h>

// ---
MCHEmul::SoundMemory::SoundMemory (size_t ns, size_t ss)
	: _samplingData (nullptr),
	  _numberSamples (ns), 
	  _sizeSample (ss)
{ 
	assert (_numberSamples > 0 && _sizeSample > 0);
							
	_samplingData = new unsigned char [_numberSamples * _sizeSample]; 
}
