#include <CORE/ScreenMemory.hpp>
#include <assert.h>

// ---
MCHEmul::ScreenMemory::ScreenMemory (size_t c, size_t r, unsigned int* p)
	: _frameData (nullptr),
	  _columns (c), _rows (r), _colorPalette (p)
{ 
	assert (_columns > 0 && _rows > 0);
	assert (_colorPalette != nullptr);
							
	_frameData = new unsigned int [_rows * _columns]; 
}
