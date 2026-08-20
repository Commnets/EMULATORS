#include <CORE/CPUTransaction.hpp>

const unsigned int MCHEmul::CPUCycle::_NOTDEFINED;
const unsigned int MCHEmul::CPUCycle::_INTERNAL;
const unsigned int MCHEmul::CPUCycle::_READ;
const unsigned int MCHEmul::CPUCycle::_WRITE;
const unsigned int MCHEmul::CPUCycle::_ALL;
const size_t MCHEmul::BusCycleData::_NOCYCLE;

// ---
MCHEmul::BusCycleData::BusCycleData ()
	: _numberCycles (0),
	  _numberReadCycles (0),
	  _numberWriteCycles (0),
	  _firstReadCycle (_NOCYCLE),
	  _lastReadCycle (_NOCYCLE),
	  _firstWriteCycle (_NOCYCLE),
	  _lastWriteCycle (_NOCYCLE),
	  _trailingWriteCycles (0),
	  _maximumConsecutiveWriteCycles (0),
	  _lastCycleType (MCHEmul::CPUCycle::_NOTDEFINED),
	  _nextReadCycles (1, _NOCYCLE),
	  _writeCycles ()
{
	// An empty descriptor is useful when no cycle structure was supplied.
}

// ---
MCHEmul::CycleStructure MCHEmul::BusCycleData::normalizedCycleStructure
	(unsigned int nC, const MCHEmul::CycleStructure& cS)
{
	assert (cS.empty () || cS.size () == nC);

	return (cS.empty ()
		? MCHEmul::CycleStructure (nC, MCHEmul::CPUCycle::_NOTDEFINED)
		: cS);
}

// ---
MCHEmul::CycleStructures MCHEmul::BusCycleData::normalizedCycleStructures
	(unsigned int nC, const MCHEmul::CycleStructures& cSs)
{
	if (cSs.empty ())
		return (MCHEmul::CycleStructures ({
			MCHEmul::CycleStructure (nC, MCHEmul::CPUCycle::_NOTDEFINED) }));

	assert (cSs [0].size () == nC);

	for (const auto& i : cSs)
		assert (!i.empty ());

	return (cSs);
}

// ---
MCHEmul::BusCycleData::BusCycleData (const MCHEmul::CycleStructure& cS)
	: _numberCycles (cS.size ()),
	  _numberReadCycles (0),
	  _numberWriteCycles (0),
	  _firstReadCycle (_NOCYCLE),
	  _lastReadCycle (_NOCYCLE),
	  _firstWriteCycle (_NOCYCLE),
	  _lastWriteCycle (_NOCYCLE),
	  _trailingWriteCycles (0),
	  _maximumConsecutiveWriteCycles (0),
	  _lastCycleType (cS.empty () ? MCHEmul::CPUCycle::_NOTDEFINED : cS.back ()),
	  _nextReadCycles (cS.size () + 1, _NOCYCLE),
	  _writeCycles ()
{
	_writeCycles.reserve (cS.size ());

	size_t consecutiveWriteCycles = 0;

	// Gather the scalar data once so bus arbitration does not need to
	// examine the complete cycle structure while the CPU is running.
	for (size_t i = 0; i < cS.size (); i++)
	{
		if ((cS [i] & MCHEmul::CPUCycle::_READ) != 0)
		{
			if (_firstReadCycle == _NOCYCLE)
				_firstReadCycle = i;

			_lastReadCycle = i;
			_numberReadCycles++;
		}

		if ((cS [i] & MCHEmul::CPUCycle::_WRITE) != 0)
		{
			if (_firstWriteCycle == _NOCYCLE)
				_firstWriteCycle = i;

			_lastWriteCycle = i;
			_numberWriteCycles++;
			_writeCycles.emplace_back (i);

			if (++consecutiveWriteCycles > _maximumConsecutiveWriteCycles)
				_maximumConsecutiveWriteCycles = consecutiveWriteCycles;
		}
		else
			consecutiveWriteCycles = 0;
	}

	// Trailing writes can complete after BA goes low and before AEC
	// removes the bus from the CPU.
	for (size_t i = cS.size ();
		i > 0 && (cS [i - 1] & MCHEmul::CPUCycle::_WRITE) != 0; i--)
		_trailingWriteCycles++;

	size_t nextReadCycle = _NOCYCLE;

	// Build the lookup backwards. Finding the next read afterwards
	// requires only one indexed access.
	for (size_t i = cS.size (); i > 0; i--)
	{
		const size_t cycle = i - 1;

		if ((cS [cycle] & MCHEmul::CPUCycle::_READ) != 0)
			nextReadCycle = cycle;

		_nextReadCycles [cycle] = nextReadCycle;
	}

	assert (_writeCycles.size () == _numberWriteCycles);
}
