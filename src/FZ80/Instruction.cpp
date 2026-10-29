#include <FZ80/Instruction.hpp>

// ---
FZ80::Instruction::Instruction (unsigned int c, unsigned int mp, unsigned int cc,
	const MCHEmul::InstructionDefined::CycleStructure& cS, const std::string& t)
	: MCHEmul::InstructionDefined (c, mp, cc, cS, t, false),
	  _mCyclesStructures (),
	  _mCyclesDatas (),
	  _executedMCyclesStructure (_NOMCYCLESSTRUCTURE),
	  _IOAccessClockCycle (0),
	  _IOContentionClockCycles (0)
{
	// Nothing else.
}

// ---
FZ80::Instruction::Instruction (unsigned int c, unsigned int mp, unsigned int cc,
	const std::string& t, const FZ80::MCyclesStructure& mCS)
	: MCHEmul::InstructionDefined (c, mp, cc, toCycleStructure (mCS), t, false),
	  _mCyclesStructures (1, mCS),
	  _mCyclesDatas (precalculateMCycles (_mCyclesStructures)),
	  _executedMCyclesStructure (_NOMCYCLESSTRUCTURE),
	  _IOAccessClockCycle (0),
	  _IOContentionClockCycles (0)
{
	// Nothing else.
}

// ---
FZ80::Instruction::Instruction (unsigned int c, unsigned int mp, unsigned int cc,
	const std::string& t, const FZ80::MCyclesStructures& mCSs)
	: MCHEmul::InstructionDefined (c, mp, cc, toCycleStructures (mCSs), t, false),
	  _mCyclesStructures (mCSs),
	  _mCyclesDatas (precalculateMCycles (_mCyclesStructures)),
	  _executedMCyclesStructure (_NOMCYCLESSTRUCTURE),
	  _IOAccessClockCycle (0),
	  _IOContentionClockCycles (0)
{
	// Nothing else.
}

// ---
void FZ80::Instruction::completePortRead
	(const MCHEmul::UByte& v, const FZ80::Z80Port::Access::ReadContext& rC)
{
	// Only input instruction families may own a pending port read.
	// Reaching this implementation indicates an invalid scheduling contract.
	assert (false);
}

// ---
bool FZ80::Instruction::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	// Alternative instructions must not inherit a preceding execution's selection.
	_executedMCyclesStructure =
		(numberMCyclesStructures () == 1) ? 0 : _NOMCYCLESSTRUCTURE;

	_lastExecutionData._INOUTAddress = MCHEmul::Address ();

	_IOAccessClockCycle = 0;
	_IOContentionClockCycles = 0;

	// Any fetch operation increment the register R (@see FZ80::CZ80::incrementRegisterR)
	static_cast <FZ80::CZ80*> (c) -> incrementRegisterR ();

	bool result = MCHEmul::InstructionDefined::execute (c, m , stk, pc);
	if (!result)
		_executedMCyclesStructure = _NOMCYCLESSTRUCTURE;
	else
		// A metadata-bearing instruction must identify the path it just executed.
		assert (!hasMCycles () || hasExecutedMCyclesStructure ());

	static_cast <FZ80::CZ80*> (c) -> setInstructionExecuted (); // Just for the EI...

	// At T4 of the M1 cycle (first machine cycle) 
	// the IR is put value into the address bus.
	// Later (other cycles), the bus can be used for other purposes...
	// But if after those other uses, the _lastINOUTAddress is still nothing...
	// it would mean that the info to be sent to the address bus is the IR!
	if (_lastExecutionData._INOUTAddress.value () == 0)
		cpu () -> setLastINOUTAddress (addressIR ()); // This instruction already move the value into the _lastINOUTAddress...

	return (result);
}

// ---
MCHEmul::CycleStructure FZ80::Instruction::toCycleStructure (const FZ80::MCyclesStructure& mCS)
{
	assert (!mCS.empty ());
	size_t numberCycles = 0;
	for (const auto& mC : mCS)
		numberCycles += mC.numberTCycles ();

	MCHEmul::CycleStructure result;
	result.reserve (numberCycles);
	// Concatenate without reinterpreting the generic characteristics. M-cycle
	// boundaries and sampling points remain in the Z80-specific descriptors.
	for (const auto& mC : mCS)
		result.insert (result.end (), mC.tCycles ().begin (), mC.tCycles ().end ());
	return (result);
}

// ---
MCHEmul::CycleStructures FZ80::Instruction::toCycleStructures (const FZ80::MCyclesStructures& mCSs)
{
	assert (!mCSs.empty ());
	MCHEmul::CycleStructures result;
	result.reserve (mCSs.size ());
	for (const auto& mCS : mCSs)
		result.emplace_back (toCycleStructure (mCS));
	return (result);
}

// ---
unsigned int FZ80::Instruction::instructionClockCycleAt (unsigned int c) const
{
	assert (c < clockCycles ());

	return ((cpu () -> ticksCounter () == nullptr)
		? cpu () -> clockCycles () + c
		: cpu () -> clockCycles () - (clockCycles () - c - 1));
}

// ---
FZ80::MCyclesDatas FZ80::Instruction::precalculateMCycles (const FZ80::MCyclesStructures& mCSs)
{
	FZ80::MCyclesDatas result;
	result.reserve (mCSs.size ());
	for (const auto& mCS : mCSs)
		result.emplace_back (mCS);
	return (result);
}

// ---
void FZ80::Instruction::prepareIOAccess
	(FZ80::Z80Port::Access& access)
{
	assert (hasMCycles () && hasExecutedMCyclesStructure ());

	const FZ80::MCyclesData& timing = mCyclesData (executedMCyclesStructure ());
	assert (timing.ioCycles ().size () == 1);

	const FZ80::MCycleData& io = timing.cycleData (timing.ioCycles ()[0]);
	unsigned int startClockCycle = instructionClockCycleAt (0);
	FZ80::CZ80::IOAccessTiming delays =
		static_cast <FZ80::CZ80*> (cpu ()) -> calculateIOAccessTiming
			(*this, access, startClockCycle);

	// Keep this field specific to contention generated by the port cycle.
	_IOContentionClockCycles = delays._port;

	// Following memory waits must not shift the port's sampling point.
	_IOAccessClockCycle = startClockCycle + (unsigned int) (io._dataCycle) +
		delays._memoryBefore + delays._port;
	access._clockCycle = _IOAccessClockCycle;

	// Include each wait once; repetition cycles are added separately by executeImpl.
	addAdditionalClockCycles
		(delays._memoryBefore + delays._port + delays._memoryAfter);
}

// ---
FZ80::InstructionUndefined::InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst)
	: MCHEmul::InstructionUndefined (c, inst),
	  _rawInstructions ()
{
	auto numberBytes = [](unsigned int c) -> size_t
		{
			unsigned int cC = c;
			size_t nB = 0;
			while (cC != 0) { nB++; cC >>= 8; }
			return (nB);
		};

	_rawInstructions = MCHEmul::ListOfInstructions (0x100 /** 256 */, nullptr);
	size_t nBSC = numberBytes (c);
	for (const auto& i : inst)
	{
		assert (numberBytes (i.first) == (nBSC + 1));						// They must by different in just one byte long...
		assert (_rawInstructions [(size_t) (i.first & 0xff)] == nullptr);	// ...and no duplications allowed...
		assert ((c << 8) == (i.first & (~0xff)));							// ...and the up part of the code has to be the same...
		
		_rawInstructions 
			[size_t (i.first /** instead i.second -> code () because it might be duplicated instructions. */ & 0xff)] = i.second;
	}
}

// ---
bool FZ80::Byte2InstructionCode::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	// Any fetch operation increment the register R (@see FZ80::CZ80::incrementRegisterR)
	static_cast <FZ80::CZ80*> (c) -> incrementRegisterR ();

	return (MCHEmul::InstructionUndefined::execute (c, m, stk, pc));
}
