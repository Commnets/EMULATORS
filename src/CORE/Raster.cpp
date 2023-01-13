#include <CORE/Raster.hpp>

// ---
MCHEmul::RasterData::RasterData (
	unsigned short fp,
	unsigned short fvp,
	unsigned short fdp,
	unsigned short ldp,
	unsigned short lvp,
	unsigned short lp,
	unsigned short mp,
	unsigned short pr1,
	unsigned short pr2
					  )
				: MCHEmul::InfoClass ("RasterData"),
				  _firstPosition (fp), _firstVisiblePosition (fvp), _firstDisplayPosition (fdp), 
				  _lastDisplayPosition (ldp), _lastVisiblePosition (lvp), _lastPosition (lp),
				  _originalFirstDisplayPosition (fdp), _originalLastDisplayPosition (ldp),
				  _maxPositions (mp),
				  _positionsToReduce1 (pr1), _positionsToReduce2 (pr2),
				  _currentPosition (fp),
				  _displayZoneReduced (false)
{
	_firstPosition_0				= toBase0 (_firstPosition);
	_firstVisiblePosition_0			= toBase0 (_firstVisiblePosition);
	_firstDisplayPosition_0			= toBase0 (_firstDisplayPosition);
	_originalFirstDisplayPosition_0 = toBase0 (_firstDisplayPosition);
	_lastDisplayPosition_0			= toBase0 (_lastDisplayPosition);
	_originalLastDisplayPosition_0	= toBase0 (_lastDisplayPosition);
	_lastVisiblePosition_0			= toBase0 (_lastVisiblePosition);
	_lastPosition_0					= toBase0 (_lastPosition);

	_currentPosition_0				= toBase0 (_currentPosition_0);

	assert (_lastPosition_0 == (_maxPositions - 1));
}

// ---
bool MCHEmul::RasterData::add (unsigned short i)
{
	bool result = false;

	int cP = (int)_currentPosition_0;
	cP += i; // Can move to the next (o nexts) lines...
	if (result = (cP >= (int) _maxPositions))
		while (cP >= (int) _maxPositions)
			cP -= (int) _maxPositions;

	cP += (int) _firstPosition;
	if (cP >= (int) _maxPositions)
		cP -= (int) _maxPositions;

	_currentPosition = (unsigned short) cP;
	_currentPosition_0 = toBase0 (_currentPosition);

	return (result);
}

// ---
void MCHEmul::RasterData::reduceDisplayZone (bool s)
{
	if (_displayZoneReduced == s)
		return; // If nothing changes, nothing to do...

	if (_displayZoneReduced = s)
	{
		_firstDisplayPosition	+= _positionsToReduce1;
		_firstDisplayPosition_0	+= _positionsToReduce1;
		_lastDisplayPosition	-= _positionsToReduce2;
		_lastDisplayPosition_0	-= _positionsToReduce2;
	}
	else
	{
		_firstDisplayPosition	-= _positionsToReduce1;
		_firstDisplayPosition_0	-= _positionsToReduce1;
		_lastDisplayPosition	+= _positionsToReduce2;
		_lastDisplayPosition_0	+= _positionsToReduce2;
	}
}

// ---
MCHEmul::InfoStructure MCHEmul::RasterData::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("POSITION",		_currentPosition);
	result.add ("POSITION0",	_currentPosition_0);
	result.add ("FIRST",		_firstPosition_0);
	result.add ("LAST",			_lastPosition_0);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::Raster::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("RasterX", _hRasterData.getInfoStructure ());
	result.add ("RasterY", _vRasterData.getInfoStructure ());

	return (result);
}
