#include <CORE/Raster.hpp>

// ---
MCHEmul::RasterData::RasterData (
	unsigned short fp,
	unsigned short fvp,
	unsigned short fdp,
	unsigned short ldp,
	unsigned short lvp,
	unsigned short rp,
	unsigned short lp,
	unsigned short mp,
	unsigned short pr1,
	unsigned short pr2
					  )
				: MCHEmul::InfoClass ("RasterData"),
				  _firstPosition (fp), _firstVisiblePosition (fvp), _firstDisplayPosition (fdp), 
				  _lastDisplayPosition (ldp), _lastVisiblePosition (lvp), 
				  _retracePosition (rp), _lastPosition (lp),
				  _originalFirstDisplayPosition (fdp), _originalLastDisplayPosition (ldp),
				  _maxPositions (mp),
				  _positionsToReduce1 (pr1), _positionsToReduce2 (pr2),
				  _currentPosition (fp),
				  _displayZoneReduced (false),
				  _retraceJustOverPassed (false)
{
	_firstPosition_0					= toBase0 (_firstPosition);
	_firstVisiblePosition_0				= toBase0 (_firstVisiblePosition);
	_firstDisplayPosition_0				= toBase0 (_firstDisplayPosition);
	_originalFirstDisplayPosition_0		= toBase0 (_firstDisplayPosition);
	_lastDisplayPosition_0				= toBase0 (_lastDisplayPosition);
	_originalLastDisplayPosition_0		= toBase0 (_lastDisplayPosition);
	_lastVisiblePosition_0				= toBase0 (_lastVisiblePosition);
	_retracePosition_0					= toBase0 (_retracePosition);
	_lastPosition_0						= toBase0 (_lastPosition);

	// This data is requested very often...
	_firstDisplayPosition_inVisibleZone	= 
		_originalFirstDisplayPosition_0 - _firstVisiblePosition_0;
	_lastDisplayPosition_inVisibleZone	= 
		_originalLastDisplayPosition_0 - _firstVisiblePosition_0;
	_firstScreenPosition_inVisibleZone	= 
		_firstDisplayPosition_0 - _firstVisiblePosition_0;
	_lastScreenPosition_inVisibleZone	= 
		_lastDisplayPosition_0 - _firstVisiblePosition_0;

	_currentPosition_0				= toBase0 (_currentPosition_0);

	// Everything matches?
	assert (_lastPosition_0 == (_maxPositions - 1) &&
		    (_lastPosition_0			>= _retracePosition_0) &&  
			(_retracePosition_0			>= _lastVisiblePosition_0) &&
			(_lastVisiblePosition_0		>= _lastDisplayPosition_0) &&
			(_lastDisplayPosition_0		>= _firstDisplayPosition_0) && 
			(_firstDisplayPosition_0	>= _firstVisiblePosition_0) &&
			(_firstVisiblePosition_0	>= _firstPosition_0));
}

// ---
MCHEmul::InfoStructure MCHEmul::RasterData::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("POSITION",		_currentPosition);
	result.add ("POSITION0",	_currentPosition_0);
	result.add ("FIRST",		_firstPosition_0);
	result.add ("FIRSTVISIBLE",	_firstVisiblePosition_0);
	result.add ("FIRSTDISPLAY",	_firstDisplayPosition_0);
	result.add ("LASTDISPLAY",	_lastDisplayPosition_0);
	result.add ("RETRACE",		_retracePosition_0);
	result.add ("LASTVISIBLE",	_lastVisiblePosition_0);
	result.add ("LAST",			_lastPosition_0);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::Raster::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("RasterX", std::move (_hRasterData.getInfoStructure ()));
	result.add ("RasterY", std::move (_vRasterData.getInfoStructure ()));

	return (result);
}
