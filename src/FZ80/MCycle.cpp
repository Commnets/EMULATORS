#include <FZ80/MCycle.hpp>
#include <cassert>

const size_t FZ80::MCycle::_NOCYCLE;

// ---
FZ80::MCycle::MCycle (Type t, std::initializer_list <unsigned int> cS,
	size_t dC, size_t cC, size_t rC)
	: _type (t),
	  _tCycles (cS),
	  _dataCycle (dC),
	  _contentionCycle (cC),
	  _refreshCycle (rC)
{
	// Validate declaration-time inputs once, outside execution-time lookups.
	assert (!_tCycles.empty ());
	assert (_dataCycle == _NOCYCLE || _dataCycle < _tCycles.size ());
	assert (_contentionCycle == _NOCYCLE || _contentionCycle < _tCycles.size ());
	assert (_refreshCycle == _NOCYCLE || _refreshCycle < _tCycles.size ());

	// Internal cycles transfer no byte, but their bus address may matter later.
	assert ((_type == Type::_INTERNAL)
		? (_dataCycle == _NOCYCLE) : (_dataCycle != _NOCYCLE));

	// Refresh belongs to M1 rather than representing another memory read.
	assert (_refreshCycle == _NOCYCLE ||
		_type == Type::_OPCODEFETCH || _type == Type::_INTERRUPTACKNOWLEDGE);
}

// ---
FZ80::MCycleData::MCycleData (const MCycle& mC, size_t fC)
	: _type (mC.type ()),
	  _firstCycle (fC),
	  _endCycle (fC + mC.numberTCycles ()),
	  _dataCycle (mC.dataCycle () == MCycle::_NOCYCLE
		? MCycle::_NOCYCLE : fC + mC.dataCycle ()),
	  _contentionCycle (mC.contentionCycle () == MCycle::_NOCYCLE
		? MCycle::_NOCYCLE : fC + mC.contentionCycle ()),
	  _refreshCycle (mC.refreshCycle () == MCycle::_NOCYCLE
		? MCycle::_NOCYCLE : fC + mC.refreshCycle ())
{
	// Nothing else to do...
}

// ---
FZ80::MCyclesData::MCyclesData ()
	: _cycles (),
	  _positions (),
	  _memoryReads (),
	  _memoryWrites (),
	  _ioCycles (),
	  _internalCycles (),
	  _interruptAcknowledges ()
{
	// Nothing else to do...
}

// ---
FZ80::MCyclesData::MCyclesData (const MCyclesStructure& mCS)
	: MCyclesData ()
{
	size_t nTC = 0, nMR = 0, nMW = 0, nIO = 0, nI = 0, nIA = 0;

	// Count logical accesses once to reserve the exact final capacities.
	for (const auto& mC : mCS)
	{
		nTC += mC.numberTCycles ();
		switch (mC.type ())
		{
			case MCycle::Type::_OPCODEFETCH:
			case MCycle::Type::_MEMORYREAD: nMR++; break;
			case MCycle::Type::_MEMORYWRITE: nMW++; break;
			case MCycle::Type::_IOREAD:
			case MCycle::Type::_IOWRITE: nIO++; break;
			case MCycle::Type::_INTERNAL: nI++; break;
			case MCycle::Type::_INTERRUPTACKNOWLEDGE: nIA++; break;
		}
	}

	_cycles.reserve (mCS.size ());
	_positions.reserve (nTC);
	_memoryReads.reserve (nMR);
	_memoryWrites.reserve (nMW);
	_ioCycles.reserve (nIO);
	_internalCycles.reserve (nI);
	_interruptAcknowledges.reserve (nIA);

	size_t firstCycle = 0, mCycleIndex = 0;
	for (const auto& mC : mCS)
	{
		// Precalculate sequence-relative points and the direct T-state mapping.
		// No scans or subtraction are needed to identify a cycle at runtime.
		_cycles.emplace_back (mC, firstCycle);
		for (size_t localCycle = 0; localCycle < mC.numberTCycles (); localCycle++)
			_positions.push_back (MCyclePosition { mCycleIndex, localCycle });

		switch (mC.type ())
		{
			case MCycle::Type::_OPCODEFETCH:
			case MCycle::Type::_MEMORYREAD: _memoryReads.emplace_back (mCycleIndex); break;
			case MCycle::Type::_MEMORYWRITE: _memoryWrites.emplace_back (mCycleIndex); break;
			case MCycle::Type::_IOREAD:
			case MCycle::Type::_IOWRITE: _ioCycles.emplace_back (mCycleIndex); break;
			case MCycle::Type::_INTERNAL: _internalCycles.emplace_back (mCycleIndex); break;
			case MCycle::Type::_INTERRUPTACKNOWLEDGE:
				_interruptAcknowledges.emplace_back (mCycleIndex); break;
		}

		firstCycle += mC.numberTCycles ();
		mCycleIndex++;
	}
}
