#include <FZ80/Instruction.hpp>

// ---
bool FZ80::Instruction::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	_lastExecutionData._INOUTAddress = MCHEmul::Address ();

	_IOAccessClockCycle = 0;
	_IOContentionClockCycles = 0;
	_IOAccessed = false;
	_lastINOUTAccessWasIO = false;
	copyIOExecutionDataToCurrentInstruction (c);

	// Any fetch operation increment the register R (@see FZ80::CZ80::incrementRegisterR)
	static_cast <FZ80::CZ80*> (c) -> incrementRegisterR ();

	bool result = MCHEmul::InstructionDefined::execute (c, m , stk, pc);
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
unsigned int FZ80::Instruction::instructionClockCycleAt (unsigned int c) const
{
	assert (c < clockCycles ());

	return ((cpu () -> ticksCounter () == nullptr)
		? cpu () -> clockCycles () + c
		: cpu () -> clockCycles () - (clockCycles () - c - 1));
}

// ---
void FZ80::Instruction::copyIOExecutionDataToCurrentInstruction (MCHEmul::CPU* c)
{
	// During a prefixed instruction the CPU exposes the outer undefined
	// instruction while the selected instruction is being executed.
	MCHEmul::CPU* cR = (c == nullptr) ? cpu () : c;
	FZ80::InstructionUndefined* i = dynamic_cast <FZ80::InstructionUndefined*>
		(const_cast <MCHEmul::Instruction*> (cR -> currentInstruction ()));
	if (i != nullptr)
	{
		i -> _IOAccessClockCycle = _IOAccessClockCycle;
		i -> _IOAccessed = _IOAccessed;
		i -> _lastINOUTAccessWasIO = _lastINOUTAccessWasIO;
	}
}

// ---
void FZ80::Instruction::prepareIOAccess
	(unsigned short ab, unsigned char p, unsigned int c, bool lAIO)
{
	_IOContentionClockCycles = static_cast <FZ80::CZ80*> (cpu ()) ->
		additionalClockCyclesForIO (ab, p, instructionClockCycleAt (c));

	_IOAccessed = true;
	_lastINOUTAccessWasIO = lAIO;

	addAdditionalClockCycles (_IOContentionClockCycles);

	copyIOExecutionDataToCurrentInstruction ();
}

// ---
void FZ80::Instruction::setIOAccessClockCycle (unsigned int c)
{
	_IOAccessClockCycle =
		instructionClockCycleAt (c) + _IOContentionClockCycles;

	copyIOExecutionDataToCurrentInstruction ();
}

// ---
FZ80::InstructionUndefined::InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst)
	: MCHEmul::InstructionUndefined (c, inst),
	  _rawInstructions (),
	  _IOAccessClockCycle (0),
	  _IOAccessed (false),
	  _lastINOUTAccessWasIO (false)
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
