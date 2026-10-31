#define SDL_MAIN_HANDLED
#include <COMMODORE/VIA/VIA.hpp>
#include <COMMODORE/VIA/VIAPort.hpp>
#include <F6500/C6502.hpp>
#include <VIC20/VIA1.hpp>
#include <VIC20/VIA2.hpp>
#include <iostream>
#include <stdexcept>
#include <new>

/** Exercise the shared signal modes without changing the production port wiring. */
class VIATimerProbe final : public COMMODORE::VIATimer
{
	public:
	VIATimerProbe (COMMODORE::VIAPortB* port)
		: COMMODORE::VIATimer (0)
							{ lookAtPort (port); }
};

/** Memory owns its storage, registers and view. */
class VIATestMemory final : public MCHEmul::Memory
{
	public:
	VIATestMemory (unsigned int machineVIA = 0)
		: MCHEmul::Memory (0, basicContent (machineVIA)),
		  _registersId (machineVIA == 1 ? VIC20::VIA1Registers::_VIA1_SUBSET :
			(machineVIA == 2 ? VIC20::VIA2Registers::_VIA2_SUBSET : 1)),
		  _baseAddress (machineVIA == 1 ? 0x9110 : 0x9120)
							{ }
	void writeRegister (unsigned int offset, unsigned int value)
							{ subset (_registersId) -> set (MCHEmul::Address (2, _baseAddress + offset), MCHEmul::UByte (value)); }
	unsigned int peekRegister (unsigned int offset)
							{ return (static_cast <COMMODORE::VIARegisters*> (subset (_registersId)) -> peekRegister (offset).value ()); }
	unsigned int readRegister (unsigned int offset)
							{ return (subset (_registersId) -> value (MCHEmul::Address (2, _baseAddress + offset)).value ()); }

	private:
	virtual MCHEmul::Stack* lookForStack () override
							{ return (nullptr); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }
	static MCHEmul::Memory::Content basicContent (unsigned int machineVIA);
	const int _registersId;
	const unsigned int _baseAddress;
};

// ---
MCHEmul::Memory::Content VIATestMemory::basicContent (unsigned int machineVIA)
{
	MCHEmul::PhysicalStorage* storage = new MCHEmul::PhysicalStorage
		(0, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	COMMODORE::VIARegisters* registers = nullptr;
	int registersId = 1;
	if (machineVIA == 1)
	{
		registersId = VIC20::VIA1Registers::_VIA1_SUBSET;
		registers = new VIC20::VIA1Registers (storage, 0, MCHEmul::Address (2, 0x9110), 16);
	}
	else if (machineVIA == 2)
	{
		registersId = VIC20::VIA2Registers::_VIA2_SUBSET;
		registers = new VIC20::VIA2Registers (storage, 0, MCHEmul::Address (2, 0x9120), 16);
	}
	else
		registers = new COMMODORE::VIARegisters (registersId, storage, 0, MCHEmul::Address (2, 0x9120), 16);
	registers -> setBufferMemorySetCommands (false);
	MCHEmul::Memory::Content result;
	result._physicalStorages = { { 0, storage } };
	result._subsets = { { registersId, registers } };
	result._views = { { 0, new MCHEmul::MemoryView (0, result._subsets) } };
	return (result);
}

/** Inspect construction links without changing the production API. */
class VIARegistersProbe final : public COMMODORE::VIARegisters
{
	public:
	VIARegistersProbe (MCHEmul::PhysicalStorage* storage)
		: COMMODORE::VIARegisters (1, storage, 0, MCHEmul::Address (2, 0x9120), 16)
							{ }
	bool disconnected () const;
};

// ---
bool VIARegistersProbe::disconnected () const
{
	return (_CA1 == nullptr && _CB1 == nullptr && _CA2 == nullptr && _CB2 == nullptr &&
		_T1 == nullptr && _T2 == nullptr && _SR == nullptr && _PA == nullptr && _PB == nullptr);
}

/** Observe the counter without acknowledging IFR5 by reading T2C-L. */
class VIATestChip final : public COMMODORE::VIA
{
	public:
	VIATestChip (unsigned int interruptId = 0, int registersId = 1)
		: COMMODORE::VIA (0, registersId, interruptId)
							{ }
	unsigned int latchedA () const
							{ return (_PA.valueLatched ().value ()); }
	unsigned int simulatedCycle () const
							{ return (_lastClockCycles); }
	unsigned short timer1Value () const
							{ return (_T1.currentValue ()); }
	unsigned short timer2Value () const
							{ return (_T2.currentValue ()); }
};

/** Observe committed register writes at the real VIA simulation boundary. */
class VIATimedRegisters final : public COMMODORE::VIARegisters
{
	public:
	struct Sample final
	{
		unsigned int _cycle, _position, _value, _beforeT1, _afterT1, _beforeT2, _afterT2;
	};
	VIATimedRegisters (int id, MCHEmul::PhysicalStorage* storage, unsigned int address)
		: COMMODORE::VIARegisters (id, storage, 0, MCHEmul::Address (2, address), 16)
							{ }
	void record (VIATestChip* chip)
							{ _chip = chip; _samples.clear (); }
	const std::vector <Sample>& samples () const
							{ return (_samples); }

	private:
	virtual void setValue (size_t position, const MCHEmul::UByte& value) override;
	VIATestChip* _chip = nullptr;
	std::vector <Sample> _samples;
};

// ---
void VIATimedRegisters::setValue (size_t position, const MCHEmul::UByte& value)
{
	Sample sample { 0, (unsigned int) (position), value.value (), 0, 0, 0, 0 };
	if (_chip != nullptr)
	{
		sample._cycle = _chip -> simulatedCycle ();
		sample._beforeT1 = _chip -> timer1Value ();
		sample._beforeT2 = _chip -> timer2Value ();
	}
	COMMODORE::VIARegisters::setValue (position, value);
	if (_chip != nullptr)
	{
		sample._afterT1 = _chip -> timer1Value ();
		sample._afterT2 = _chip -> timer2Value ();
		_samples.push_back (sample);
	}
}

/** A disjoint 64 KiB CPU map with RAM, stack and two independently owned VIAs. */
class VIATimedMemory final : public MCHEmul::Memory
{
	public:
	VIATimedMemory ()
		: MCHEmul::Memory (0, basicContent ())
							{ }
	VIATimedRegisters* registers (unsigned int index)
							{ return (static_cast <VIATimedRegisters*> (subset (2100 + (int) (index) * 10))); }

	private:
	virtual MCHEmul::Stack* lookForStack () override
							{ return (static_cast <MCHEmul::Stack*> (subset (1))); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }
	static MCHEmul::Memory::Content basicContent ();
};

// ---
MCHEmul::Memory::Content VIATimedMemory::basicContent ()
{
	MCHEmul::PhysicalStorage* ram = new MCHEmul::PhysicalStorage
		(0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);
	MCHEmul::PhysicalStorage* via1 = new MCHEmul::PhysicalStorage
		(1, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	MCHEmul::PhysicalStorage* via2 = new MCHEmul::PhysicalStorage
		(2, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	MCHEmul::Memory::Content result;
	result._physicalStorages = { { 0, ram }, { 1, via1 }, { 2, via2 } };
	result._subsets = {
		{ 0, new MCHEmul::PhysicalStorageSubset (0, ram, 0, MCHEmul::Address (2, 0), 0x100) },
		{ 1, new MCHEmul::Stack (1, ram, 0x100, MCHEmul::Address (2, 0x100), 0x100,
			MCHEmul::Stack::Configuration (true, true, false, 0x100)) },
		{ 2, new MCHEmul::PhysicalStorageSubset (2, ram, 0x200, MCHEmul::Address (2, 0x200), 0x8f10) },
		{ 3, new MCHEmul::PhysicalStorageSubset (3, ram, 0x9130, MCHEmul::Address (2, 0x9130), 0x6ed0) },
		{ 2100, new VIATimedRegisters (2100, via1, 0x9110) },
		{ 2110, new VIATimedRegisters (2110, via2, 0x9120) } };
	for (const auto& subset : result._subsets)
		subset.second -> setBufferMemorySetCommands (subset.first >= 2100);
	result._views = { { 0, new MCHEmul::MemoryView (0, result._subsets) } };
	return (result);
}

/** Use the same synchronous CPU context forwarding as CommodoreVIC20. */
class VIATimedFixture final : public MCHEmul::Observer
{
	public:
	VIATimedFixture (unsigned int interruptId = 0)
		: _cpu (0), _via1 (interruptId, 2100), _via2 (interruptId, 2110)
							{ }
	virtual ~VIATimedFixture () override
							{ _cpu.executeBufferedCommands (true); }
	bool initialize (bool zeroOrigin = true);
	bool execute (unsigned int opcode, unsigned int address, unsigned int accumulator = 0,
		unsigned int programAddress = 0x200);
	void setup (unsigned int offset, unsigned int value, unsigned int index = 1);
	unsigned int peek (unsigned int offset, unsigned int index = 1)
							{ return (_memory.registers (index) -> peekRegister (offset).value ()); }
	void clearSamples ();
	bool idle (unsigned int cycles);

	VIATimedMemory _memory;
	F6500::C6502 _cpu;
	VIATestChip _via1, _via2;

	private:
	virtual void processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier) override;
};

// ---
bool VIATimedFixture::initialize (bool zeroOrigin)
{
	if (!_memory.initialize ())
		return (false);
	_cpu.setMemoryRef (&_memory);
	if (!_cpu.initialize ())
		return (false);
	// C6500 reset accounts for six startup cycles. Also exercise an exact zero origin.
	if (zeroOrigin && !_cpu.MCHEmul::CPU::initialize ())
		return (false);
	_cpu.statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, true);
	_via1.setMemoryRef (&_memory);
	_via2.setMemoryRef (&_memory);
	if (!_via1.initialize () || !_via2.initialize () ||
		!_via1.enableCPUTimedWrites (&_cpu) || !_via2.enableCPUTimedWrites (&_cpu))
		return (false);
	observe (&_cpu);
	clearSamples ();
	return (true);
}

// ---
bool VIATimedFixture::execute
	(unsigned int opcode, unsigned int address, unsigned int accumulator, unsigned int programAddress)
{
	_memory.set (MCHEmul::Address (2, programAddress), MCHEmul::UByte (opcode));
	_memory.set (MCHEmul::Address (2, programAddress + 1), MCHEmul::UByte (address & 0xff));
	_memory.set (MCHEmul::Address (2, programAddress + 2), MCHEmul::UByte (address >> 8));
	_cpu.programCounter ().setAddress (MCHEmul::Address (2, programAddress));
	_cpu.internalRegister (F6500::C6500::_ACCUMULATOR).set (std::vector <MCHEmul::UByte> { MCHEmul::UByte (accumulator) });
	return (_cpu.executeNextCycle () && _via1.simulate (&_cpu) && _via2.simulate (&_cpu));
}

// ---
void VIATimedFixture::setup (unsigned int offset, unsigned int value, unsigned int index)
{
	_memory.registers (index) -> setRegister (offset, MCHEmul::UByte (value));
}

// ---
void VIATimedFixture::clearSamples ()
{
	_memory.registers (0) -> record (&_via1);
	_memory.registers (1) -> record (&_via2);
}

// ---
bool VIATimedFixture::idle (unsigned int cycles)
{
	_cpu.addClockCycles (cycles);
	return (_via1.simulate (&_cpu) && _via2.simulate (&_cpu));
}

// ---
void VIATimedFixture::processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier)
{
	if (notifier != &_cpu)
		return;
	if (event.id () == MCHEmul::CPU::_CPUTOEXECUTEINSTRUCTION)
	{
		const MCHEmul::InstructionContextEventData* context =
			static_cast <const MCHEmul::InstructionContextEventData*> (event.data ().get ());
		_via1.CPUAboutToExecute (context);
		_via2.CPUAboutToExecute (context);
	}
	else if (event.id () == MCHEmul::CPU::_CPUTOEXECUTEINTERRUPT)
	{
		const MCHEmul::InterruptContextEventData* context =
			static_cast <const MCHEmul::InterruptContextEventData*> (event.data ().get ());
		_via1.CPUAboutToExecute (context);
		_via2.CPUAboutToExecute (context);
	}
}

/** Record every wire notification, including glitches hidden by end-of-cycle reads. */
class VIASerialMonitor final : public MCHEmul::Observer
{
	public:
	struct Transition final
	{
		unsigned int _cycle;
		bool _clock, _level;
	};

	VIASerialMonitor (COMMODORE::VIA& via)
		: _clock (&via.CB1 ())
	{
		_events.reserve (128);
		observe (&via.CB1 ());
		observe (&via.CB2 ());
	}
	void beginCycle (unsigned int cycle)
							{ _cycle = cycle; _events.clear (); }
	const std::vector <Transition>& events () const
							{ return (_events); }

	private:
	virtual void processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier) override;
	MCHEmul::Notifier* _clock;
	unsigned int _cycle = 0;
	std::vector <Transition> _events;
};

// ---
void VIASerialMonitor::processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier)
{
	if (event.id () == MCHEmul::Wire::_WIREDCHANGED)
		_events.push_back ({ _cycle, notifier == _clock, event.value () != 0 });
}

class VIATimerTests final
{
	public:
	void run ();

	private:
	struct SerialSample final
	{
		bool _clock, _data;
		unsigned int _value, _flags;
		std::vector <unsigned int> _transitions;
	};

	void timedReads ();
	void timedWrites ();
	void registerConstruction ();
	void registerReinitialization ();
	void periods ();
	void phasesAndWrites ();
	void oneShots ();
	void pulseCounting ();
	void interruptAdmission ();
	void pcrReadback ();
	void serialOwnership ();
	void serialOutputTimelines ();
	void serialInputAndHandshake ();
	std::vector <SerialSample> serialACRTrace (unsigned int mode, unsigned int otherBits, unsigned int writeCycle);
	void serialACRPreservation ();
	void serialModeTransitions ();
	void via1InputCapture ();
	void portCaptureResolution ();
	void timerPB7Direction ();
	void timerPB7Keyboard ();
	void keyboardMatrixResolution ();
	void keyboardLatchAndPulses ();
	void setVIA1Input (MCHEmul::Notifier& source, unsigned int input, bool pressed);
	void tickVIA (COMMODORE::VIA& via, unsigned int cycles = 1);
	COMMODORE::VIATimer::CycleResult tick (COMMODORE::VIATimer& timer)
							{ return (timer.simulate (&_cpu)); }
	void require (bool condition, const std::string& message) const;

	F6500::C6502 _cpu { 0 };
};

// ---
void VIATimerTests::require (bool condition, const std::string& message) const
{
	if (!condition)
		throw std::runtime_error (message);
}

// ---
void VIATimerTests::timedReads ()
{
	for (unsigned int timer : { 1, 2 })
	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Read-point fixture initialization failed");
		fixture.setup (timer == 1 ? 4 : 8, 6);
		fixture.setup (timer == 1 ? 5 : 9, 0);
		require (fixture.execute (0xad, timer == 1 ? 0x9124 : 0x9128),
			"Real LDA timer-low read failed");
		require (fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 2 &&
			(timer == 1 ? fixture._via2.timer1Value () : fixture._via2.timer2Value ()) == 2 &&
			fixture._cpu.clockCycles () == 4,
			"LDA must receive the fourth-cycle counter value, without replaying advanced cycles");
		require (fixture.idle (1) &&
			(timer == 1 ? fixture._via2.timer1Value () : fixture._via2.timer2Value ()) == 1,
			"Cycle after an advanced read must execute exactly once");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Zero-flag read fixture failed");
		fixture.setup (0x0b, 0x40);
		fixture.setup (4, 4);
		fixture.setup (5, 0);
		require (fixture.execute (0xad, 0x9124) && fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 0 &&
			fixture._cpu.statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG),
			"LDA must set Z from the counter at the read point");
		require (fixture.execute (0xf0, 2) && fixture._cpu.clockCycles () == 7 &&
			fixture._cpu.programCounter ().asAddress ().value () == 0x204,
			"BEQ must use the flags obtained from the synchronized read");
	}

	for (unsigned int timer : { 1, 2 })
	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "High-byte read fixture failed");
		fixture.setup (timer == 1 ? 4 : 8, 2);
		fixture.setup (timer == 1 ? 5 : 9, 1);
		require (fixture.execute (0xad, timer == 1 ? 0x9125 : 0x9129) &&
			fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 0 &&
			(timer == 1 ? fixture._via2.timer1Value () : fixture._via2.timer2Value ()) == 0xfe,
			"High-byte LDA must observe the borrow occurring before its read cycle");
	}

	for (unsigned int crossing : { 0, 1 })
	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Indexed read fixture failed");
		fixture.setup (4, 9);
		fixture.setup (5, 0);
		fixture._cpu.internalRegister (F6500::C6500::_XREGISTER).set (std::vector <MCHEmul::UByte>
			{ MCHEmul::UByte (crossing ? 0x25 : 1) });
		require (fixture.execute (0xbd, crossing ? 0x90ff : 0x9123) &&
			fixture._cpu.clockCycles () == (crossing ? 5 : 4) &&
			fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == (crossing ? 4 : 5) &&
			fixture._via2.timer1Value () == (crossing ? 4 : 5),
			"Indexed LDA must use the selected four/five-cycle structure without duplicate ticks");
	}

	for (bool taken : { false, true })
	{
		for (bool crossing : { false, true })
		{
			VIATimedFixture fixture;
			require (fixture.initialize (), "Branch variant fixture failed");
			fixture.setup (4, 0);
			fixture.setup (5, 1);
			fixture._cpu.statusRegister ().setBitStatus (F6500::C6500::_ZEROFLAG, !taken);
			unsigned int cycles = taken ? (crossing ? 4 : 3) : 2;
			unsigned int programAddress = crossing ? 0x2fd : 0x200;
			require (fixture.execute (0xd0, 2, 0, programAddress) &&
				fixture._cpu.clockCycles () == cycles &&
				fixture._via2.timer1Value () == 0x100 - cycles &&
				fixture._cpu.programCounter ().asAddress ().value () == programAddress + 2 + (taken ? 2 : 0),
				"BNE alternatives must advance only their actual two/three/four cycles");
		}
	}

	for (unsigned int interruptId : { (unsigned int) F6500::IRQInterrupt::_ID,
		(unsigned int) F6500::NMIInterrupt::_ID })
	{
		{
			VIATimedFixture fixture (interruptId);
			require (fixture.initialize (), "Same-cycle acknowledge fixture failed");
			fixture.setup (4, 4);
			fixture.setup (5, 0);
			fixture.setup (0x0e, 0xc0);
			require (fixture.execute (0xad, 0x9124) && fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 0 &&
				(fixture.peek (0x0d) & 0xc0) == 0 && fixture._cpu.interruptsRequested ().empty (),
				"Final read-cycle timeout acknowledged by LDA must not publish a false IRQ/NMI request");
		}
		{
			VIATimedFixture fixture (interruptId);
			require (fixture.initialize (), "Earlier-cycle acknowledge fixture failed");
			fixture.setup (4, 3);
			fixture.setup (5, 0);
			fixture.setup (0x0e, 0xc0);
			require (fixture.execute (0xad, 0x9124) && (fixture.peek (0x0d) & 0xc0) == 0 &&
				fixture._cpu.interruptsRequested ().size () == 1 &&
				fixture._cpu.getNextInterruptRequest ().cycles () == 2,
				"A real earlier IRQ/NMI request must survive the final-cycle acknowledgement");
		}
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "IFR read fixture failed");
		fixture.setup (4, 2);
		fixture.setup (5, 0);
		fixture.setup (0x0e, 0xc0);
		require (fixture.execute (0xad, 0x912d) && fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 0xc0 &&
			(fixture.peek (0x0d) & 0xc0) == 0xc0,
			"IFR LDA must see a preceding timeout without acknowledging it");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Timed RMW read fixture failed");
		fixture.setup (4, 6);
		fixture.setup (5, 0);
		fixture.clearSamples ();
		require (fixture.execute (0xee, 0x9124), "INC counter-low failed");
		const auto& samples = fixture._memory.registers (1) -> samples ();
		require (samples.size () == 2 && samples [0]._value == 2 && samples [1]._value == 3 &&
			samples [0]._cycle == 4 && samples [1]._cycle == 5 &&
			fixture._via2.timer1Value () == 0 && fixture.peek (6) == 3 &&
			(fixture.peek (0x0d) & 0x40) != 0,
			"RMW must retain its synchronized original byte and execute each later write/tick once");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Read-pulse fixture failed");
		fixture.setup (0x0c, 0x0a);
		fixture._via2.CA1 ().setValue (false);
		require (fixture.execute (0xad, 0x9121) && !fixture._via2.CA2 ().value () &&
			(fixture.peek (0x0d) & 2) == 0,
			"Port read must acknowledge CA1 and begin a pulse at the advanced read point");
		require (fixture.idle (1) && fixture._via2.CA2 ().value (),
			"Read-generated CA2 pulse must finish on the next cycle");
		fixture._via2.CA1 ().setValue (true);
		require (fixture.idle (1), "CA1 release failed");
		fixture._via2.CA1 ().setValue (false);
		require (fixture.execute (0xad, 0x912f) && fixture._via2.CA2 ().value () &&
			(fixture.peek (0x0d) & 2) != 0,
			"Port A without handshake must preserve CA1 flag and avoid a pulse");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "SR read fixture failed");
		fixture.setup (0x0b, 0x18);
		fixture.setup (0x0a, 0xa5);
		// PHI2 output shifts on cycles 0 and 2: A5 -> 4B -> 96.
		require (fixture.execute (0xad, 0x912a) && fixture._cpu.internalRegister (F6500::C6500::_ACCUMULATOR) [0].value () == 0x96 &&
			fixture.peek (0x0a) == 0x96,
			"SR read must return its shifted value and not repeat SR work during completion");
		require (fixture.idle (1) && fixture.peek (0x0a) == 0x2d,
			"SR read restart must begin on the following cycle, exactly once");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Pre-write boundary fixture failed");
		require (fixture.execute (0x20, 0x400) && fixture._cpu.clockCycles () == 6 &&
			fixture._cpu.programCounter ().asAddress ().value () == 0x400,
			"JSR must not advance across its stack-write cycles before execution");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Overprediction fixture failed");
		MCHEmul::InstructionContextEventData context (&fixture._cpu);
		context._instruction = fixture._cpu.instruction (0xad);
		context._clockCycles = 4;
		fixture._via2.CPUAboutToExecute (&context);
		fixture._cpu.addClockCycles (2);
		require (!fixture._via2.simulate (&fixture._cpu) &&
			fixture._via2.error () == MCHEmul::_CHIP_ERROR,
			"Actual duration shorter than the advance must fail rather than underflow the remaining count");
	}

	std::cout << "Real 6502 advanced VIA reads, LDA flags/branches, byte borrow, variants, IRQ/NMI ordering, RMW, ports/SR and duration guards PASS\n";
}

// ---
void VIATimerTests::timedWrites ()
{
	for (unsigned int timer : { 1, 2 })
	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Timed VIA CPU fixture initialization failed");
		fixture.setup (timer == 1 ? 4 : 8, 0x20);
		fixture.setup (timer == 1 ? 5 : 9, 0);
		fixture.clearSamples ();
		require (fixture.execute (0x8d, timer == 1 ? 0x9125 : 0x9129, 1),
			"Real STA timer load failed");
		const auto& samples = fixture._memory.registers (1) -> samples ();
		require (samples.size () == 1 && samples [0]._cycle == 3,
			"First STA must commit at absolute cycle 3, including cycle-zero origin");
		require ((timer == 1 ? samples [0]._beforeT1 : samples [0]._beforeT2) == 0x1c &&
			(timer == 1 ? samples [0]._afterT1 : samples [0]._afterT2) == 0x120,
			"Old counter must run for four cycles and new counter must not decrement on load");
		require (fixture._memory.registers (0) -> samples ().empty (), "VIA1 must not consume VIA2 writes");
		require (fixture.idle (1), "Timer cycle after load failed");
		require ((timer == 1 ? fixture._via2.timer1Value () : fixture._via2.timer2Value ()) == 0x11f,
			"Loaded timer must decrement on the following cycle");
		require (fixture._memory.registers (1) -> samples ().size () == 1, "Buffered store must execute once");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "RMW fixture initialization failed");
		fixture.setup (2, 0xff);
		fixture.setup (0, 0x20);
		fixture.clearSamples ();
		require (fixture.execute (0xee, 0x9120), "Real INC ORB failed");
		const auto& samples = fixture._memory.registers (1) -> samples ();
		require (samples.size () == 2 && samples [0]._cycle == 4 && samples [1]._cycle == 5 &&
			samples [0]._value == 0x20 && samples [1]._value == 0x21,
			"INC must commit original and modified values at cycles 4 and 5");
		require (fixture.execute (0x8d, 0x9113, 0x55), "VIA1 store following RMW failed");
		require (fixture._memory.registers (0) -> samples ().size () == 1 &&
			fixture._memory.registers (0) -> samples () [0]._cycle == 9 &&
			fixture._memory.registers (1) -> samples ().size () == 2,
			"Consecutive transactions must preserve VIA subset isolation without replay");
		require (fixture.execute (0xea, 0), "No-write NOP transaction failed");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Latch fixture initialization failed");
		fixture.setup (4, 0x20);
		fixture.setup (5, 0);
		fixture.clearSamples ();
		require (fixture.execute (0x8d, 0x9124, 0x40), "Latch-only STA failed");
		require (fixture._via2.timer1Value () == 0x1c && fixture.peek (6) == 0x40,
			"Latch write must not reload or stop the active counter");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "PB6 fixture initialization failed");
		fixture.setup (2, 0xff);
		fixture.setup (0, 0xff);
		fixture.setup (0x0b, 0x20);
		fixture.setup (8, 5);
		fixture.setup (9, 0);
		require (fixture.idle (1), "Initial PB6 high publication failed");
		fixture.clearSamples ();
		require (fixture.execute (0x8d, 0x9120, 0xbf), "STA PB6 falling edge failed");
		require (fixture._via2.timer2Value () == 5, "Write-generated PB6 edge must await the next T2 sample");
		require (fixture.idle (1) && fixture._via2.timer2Value () == 4,
			"Write-generated PB6 edge must survive cleanup and count once");
		require (fixture.idle (2) && fixture._via2.timer2Value () == 4, "PB6 edge must not repeat");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Handshake fixture initialization failed");
		fixture.setup (0x0c, 0x0a);
		fixture.clearSamples ();
		require (fixture.execute (0x8d, 0x9121, 0x55) && !fixture._via2.CA2 ().value (),
			"CA2 pulse must begin at the CPU write and remain low after that cycle");
		require (fixture.idle (1) && fixture._via2.CA2 ().value (), "CA2 pulse must finish on the following cycle");
		require (fixture.execute (0x8d, 0x912f, 0xaa) && fixture._via2.CA2 ().value (),
			"ORA without handshake must not generate a pulse");
		require (fixture.execute (0x8d, 0x912c, 0xaa),
			"Timed PCR change must configure both peripheral pulse outputs");
		require (fixture._via2.CA2 ().value () && fixture._via2.CB2 ().value (),
			"PCR configuration alone must leave pulse outputs high");
		require (fixture.execute (0x8d, 0x9120, 0x55) && !fixture._via2.CB2 ().value (),
			"CB2 pulse must begin at the ORB write");
		require (fixture.idle (1) && fixture._via2.CB2 ().value (),
			"CB2 pulse must finish on the following cycle");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Timed ACR fixture initialization failed");
		fixture.setup (8, 0x20);
		fixture.setup (9, 0);
		fixture.clearSamples ();
		require (fixture.execute (0x8d, 0x912b, 0x20) && fixture._via2.timer2Value () == 0x1c,
			"ACR must change T2 source only after the four old-source cycles");
		require (fixture.idle (2) && fixture._via2.timer2Value () == 0x1c,
			"New PB6 source must take effect after the ACR write without invented edges");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "IFR fixture initialization failed");
		fixture.setup (4, 1);
		fixture.setup (5, 0);
		fixture.setup (0x0e, 0xc0);
		require (fixture.execute (0x8d, 0x912d, 0x40) && (fixture.peek (0x0d) & 0xc0) == 0,
			"IFR write must clear a timeout generated earlier in the instruction");
		fixture.setup (4, 1);
		fixture.setup (5, 0);
		require (fixture.execute (0x8d, 0x912e, 0x40) && (fixture.peek (0x0d) & 0xc0) == 0x40,
			"IER masking must retain the timeout flag while removing enabled status");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Reset fixture initialization failed");
		require (fixture.execute (0x8d, 0x9123, 0xaa), "Store before reinitialization failed");
		require (fixture._via1.initialize () && fixture._via2.initialize () &&
			fixture._via1.enableCPUTimedWrites (&fixture._cpu) &&
			fixture._via2.enableCPUTimedWrites (&fixture._cpu), "Reinitialization must clear timing context");
		fixture.clearSamples ();
		require (fixture.execute (0x8d, 0x9123, 0x55) &&
			fixture._memory.registers (1) -> samples ().size () == 1 &&
			fixture._memory.registers (1) -> samples () [0]._cycle == 7, "Reset must not retain an old write");
	}
	{
		VIATimedFixture fixture;
		require (fixture.initialize (false), "Normal 6502 startup fixture failed");
		require (fixture._cpu.clockCycles () == 6 && fixture.execute (0x8d, 0x9123, 0xaa) &&
			fixture._memory.registers (1) -> samples () [0]._cycle == 9,
			"Normal six-cycle startup must schedule the first STA at absolute cycle 9");
		fixture._memory.set (MCHEmul::Address (2, 0xfffa), MCHEmul::UByte (0x00));
		fixture._memory.set (MCHEmul::Address (2, 0xfffb), MCHEmul::UByte (0x03));
		// Present an edge at the previous instruction's polling point, not after it.
		fixture._cpu.requestInterrupt (F6500::NMIInterrupt::_ID,
			fixture._cpu.clockCycles () - 2, nullptr, 0);
		fixture.clearSamples ();
		unsigned int previousCycle = fixture._cpu.clockCycles ();
		require (fixture.execute (0xea, 0) && fixture._cpu.clockCycles () == previousCycle + 7 &&
			fixture._cpu.programCounter ().asAddress ().value () == 0x300 &&
			fixture._memory.registers (0) -> samples ().empty () &&
			fixture._memory.registers (1) -> samples ().empty (),
			"Interrupt entry must replace the CPU context without generating VIA writes");
		require (fixture.execute (0x8d, 0x9123, 0x55), "Store after interrupt entry failed");
	}

	{
		MCHEmul::TicksCounterDelayed ticks (1);
		VIATimedFixture fixture;
		require (fixture.initialize (), "Unsupported-mode fixture failed");
		require (fixture._via1.initialize (), "VIA reset before mode check failed");
		fixture._cpu.assignTicksCounter (ticks);
		require (!fixture._via1.enableCPUTimedWrites (&fixture._cpu) &&
			fixture._via1.error () == MCHEmul::_INIT_ERROR,
			"Per-cycle CPU mode must be rejected explicitly");
	}

	{
		VIATimedFixture fixture;
		require (fixture.initialize (), "Buffer policy fixture failed");
		require (fixture._via1.initialize (), "VIA reset before buffer check failed");
		fixture._memory.registers (0) -> setBufferMemorySetCommands (false);
		require (!fixture._via1.enableCPUTimedWrites (&fixture._cpu),
			"Timed writes require the target register subset to buffer commands");
	}

	std::cout << "Real 6502 timed VIA STA/RMW, timer loads/latches, PB6, handshake, IFR/IER, isolation and reset PASS\n";
}

// ---
void VIATimerTests::registerConstruction ()
{
	MCHEmul::PhysicalStorage storage (0, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	// Fill storage before object lifetime begins to expose dependence on accidental zero bytes.
	alignas (VIARegistersProbe) unsigned char genericBytes [sizeof (VIARegistersProbe)];
	alignas (VIC20::VIA1Registers) unsigned char via1Bytes [sizeof (VIC20::VIA1Registers)];
	alignas (VIC20::VIA2Registers) unsigned char via2Bytes [sizeof (VIC20::VIA2Registers)];
	for (size_t i = 0; i < sizeof (genericBytes); ++i)
		genericBytes [i] = 0xa5;
	for (size_t i = 0; i < sizeof (via1Bytes); ++i)
		via1Bytes [i] = 0xa5;
	for (size_t i = 0; i < sizeof (via2Bytes); ++i)
		via2Bytes [i] = 0xa5;
	VIARegistersProbe* generic = new (genericBytes) VIARegistersProbe (&storage);
	require (generic -> disconnected (), "Construction must initialize all nine VIA links");
	generic -> initialize ();
	require (generic -> disconnected (), "Unconnected initialization must preserve null links");
	generic -> ~VIARegistersProbe ();

	VIC20::VIA1Registers* via1 = new (via1Bytes) VIC20::VIA1Registers
		(&storage, 0, MCHEmul::Address (2, 0x9110), 16);
	require (via1 -> joystickStatus () == 0xff && !via1 -> tapeSensePressed (),
		"VIA1 construction must initialize joystick and tape before wiring");
	via1 -> initialize ();
	require (via1 -> joystickStatus () == 0xff && !via1 -> tapeSensePressed (),
		"VIA1 unconnected initialization must retain input defaults");
	via1 -> ~VIA1Registers ();

	VIC20::VIA2Registers* via2 = new (via2Bytes) VIC20::VIA2Registers
		(&storage, 0, MCHEmul::Address (2, 0x9120), 16);
	require (via2 -> joystickStatus () == 0xff, "VIA2 construction must initialize joystick before wiring");
	for (size_t i = 0; i < 8; ++i)
		require (via2 -> keyboardStatusMatrix (i).value () == 0xff &&
			via2 -> rev_keyboardStatusMatrix (i).value () == 0xff,
			"VIA2 construction must initialize both keyboard matrix views");
	via2 -> setJoystickStatus (0);
	via2 -> setKeyboardStatusMatrix (3, 5, false);
	via2 -> initialize ();
	require (via2 -> joystickStatus () == 0xff && via2 -> keyboardStatusMatrix (3, 5) &&
		via2 -> rev_keyboardStatusMatrix (5, 3), "Unconnected VIA2 reset must restore local state");
	via2 -> ~VIA2Registers ();
}

// ---
void VIATimerTests::registerReinitialization ()
{
	for (unsigned int machineVIA : { 1, 2 })
	{
		VIATestMemory memory (machineVIA);
		VIC20::VIA1 via1;
		VIC20::VIA2 via2;
		COMMODORE::VIA* via = machineVIA == 1 ?
			static_cast <COMMODORE::VIA*> (&via1) : static_cast <COMMODORE::VIA*> (&via2);
		via -> setMemoryRef (&memory);
		require (via -> initialize (), "Heap-backed VIA initialization failed");
		if (machineVIA == 1)
		{
			VIC20::VIA1Registers* registers = static_cast <VIC20::VIA1Registers*>
				(memory.subset (VIC20::VIA1Registers::_VIA1_SUBSET));
			registers -> setJoystickStatus (0);
			registers -> setTapeSensePressed (true);
		}
		else
		{
			VIC20::VIA2Registers* registers = static_cast <VIC20::VIA2Registers*>
				(memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET));
			registers -> setJoystickStatus (0);
			for (size_t column = 0; column < 8; ++column)
				for (size_t row = 0; row < 8; ++row)
					registers -> setKeyboardStatusMatrix (column, row, false);
		}
		memory.writeRegister (2, 0x55);
		memory.writeRegister (3, 0xaa);
		memory.writeRegister (0x0b, 0xff);
		memory.writeRegister (0x0c, 0xff);
		require (via -> initialize (), "Connected VIA reinitialization failed");
		require (memory.peekRegister (2) == (machineVIA == 1 ? 0 : 0xff) &&
			memory.peekRegister (3) == (machineVIA == 1 ? 0x80 : 0) &&
			memory.peekRegister (0x0b) == 0 && memory.peekRegister (0x0c) == 0,
			"Reinitialization must restore machine register defaults");
		if (machineVIA == 1)
		{
			VIC20::VIA1Registers* registers = static_cast <VIC20::VIA1Registers*>
				(memory.subset (VIC20::VIA1Registers::_VIA1_SUBSET));
			require (registers -> joystickStatus () == 0xff && !registers -> tapeSensePressed () &&
				(memory.peekRegister (1) & 0x7c) == 0x7c,
				"VIA1 reset must publish released joystick and tape inputs");
		}
		else
		{
			VIC20::VIA2Registers* registers = static_cast <VIC20::VIA2Registers*>
				(memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET));
			for (size_t i = 0; i < 8; ++i)
				require (registers -> keyboardStatusMatrix (i).value () == 0xff &&
					registers -> rev_keyboardStatusMatrix (i).value () == 0xff,
					"Connected VIA2 reset must release all keys in both matrix views");
			require (registers -> joystickStatus () == 0xff && memory.peekRegister (1) == 0xff,
				"VIA2 reset must publish released keyboard and joystick inputs");
		}
	}
}

// ---
void VIATimerTests::periods ()
{
	// MOS 6522 Figure 6: continuous timeout-to-timeout period is latch + 2.
	// Include PITFALL's PAL latch, whose period must equal 71 * 312.
	const unsigned short latches [] = { 0, 1, 2, 0x00ff, 0x0100, 0x5686, 0xffff };
	const unsigned int periods [] = { 2, 3, 4, 257, 258, 22152, 65537 };
	for (size_t index = 0; index < 7; ++index)
	{
		COMMODORE::VIATimer1 timer (0);
		timer.setCountAndRunMode (COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES,
			COMMODORE::VIATimer::RunMode::_CONTINUOUS);
		timer.setInitialValue (latches [index]);
		timer.loadCounter ();
		unsigned int previousTimeout = 0;
		unsigned int timeouts = 0;
		for (unsigned int cycle = 1; cycle <= periods [index] * 4; ++cycle)
		{
			if (tick (timer)._timeout)
			{
				require (timer.currentValue () == 0xffff, "Underflow must remain visible");
				require (timer.peekInterruptRequested (), "Missing timeout interrupt");
				if (timeouts != 0)
					require (cycle - previousTimeout == periods [index],
						"Wrong continuous period for latch " + std::to_string (latches [index]));
				previousTimeout = cycle;
				++timeouts;
				timer.clearInterrupt ();
			}
		}
		require (timeouts >= 3, "Insufficient repeated timeouts");
		std::cout << "Latch " << latches [index] << ": period " << periods [index] << " PASS\n";
	}
}

// ---
void VIATimerTests::phasesAndWrites ()
{
	using RunMode = COMMODORE::VIATimer::RunMode;
	using CountMode = COMMODORE::VIATimer::CountMode;
	COMMODORE::VIAPortB port (0);
	VIATimerProbe timer (&port);
	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_CONTINUOUSSIGNAL);
	timer.setInitialValue (2);
	timer.loadCounter ();
	port.setP7 (false);
	require (!tick (timer)._timeout && timer.currentValue () == 1, "Countdown 2 -> 1");
	auto result = tick (timer);
	require (!result._timeout && result._lowByteReached0 && timer.reaches0 () && !port.p7 (),
		"Zero must precede timeout and PB7 toggle");
	result = tick (timer);
	require (result._timeout && timer.currentValue () == 0xffff && port.p7 () &&
		!result._lowByteReached0 && !timer.reaches0 (), "Underflow/PB7 phase");
	timer.clearInterrupt ();
	timer.setInitialValue (0);
	result = tick (timer);
	require (timer.currentValue () == 0 && !result._timeout && !result._lowByteReached0 &&
		!timer.reaches0 () && !timer.peekInterruptRequested () && port.p7 (),
		"Reload must use latest latch without events or PB7 toggle");
	require (tick (timer)._timeout && !port.p7 (), "Zero latch must expire on next cycle");

	// A CPU counter load cancels a pending automatic reload.
	timer.setInitialValue (2);
	timer.loadCounter ();
	require (!tick (timer)._timeout && timer.currentValue () == 1,
		"Explicit load must cancel pending reload");
	tick (timer);
	timer.stop ();
	result = tick (timer);
	require (!result._timeout && !result._lowByteReached0 && !timer.reaches0 () &&
		!timer.reaches0LSB (), "Stopped timer must clear per-cycle flags");
	timer.start ();
	require (tick (timer)._timeout, "Restart must preserve countdown");

	// Switching the PB7 output mode retains an already pending reload.
	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_CONTINUOUS);
	result = tick (timer);
	require (!result._timeout && timer.currentValue () == 2, "Continuous mode change preserves reload");
	tick (timer);
	tick (timer);
	tick (timer);
	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_ONESHOOT);
	tick (timer);
	require (timer.currentValue () == 0xfffe, "One-shot mode discards continuous reload");

	// Latch-only changes must not replace an active countdown.
	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_CONTINUOUS);
	timer.setInitialValue (2);
	timer.loadCounter ();
	tick (timer);
	timer.setLatchLow (MCHEmul::UByte (5));
	tick (timer);
	require (timer.currentValue () == 0, "Latch write must preserve live counter");
	tick (timer);
	tick (timer);
	require (timer.currentValue () == 5, "New latch applies to automatic reload");

	// Initialization must cancel a pending automatic reload, even without a new counter load.
	timer.setInitialValue (0);
	timer.loadCounter ();
	tick (timer);
	timer.initialize ();
	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_CONTINUOUS);
	timer.start ();
	require (tick (timer)._timeout, "Initialize must cancel pending reload");
	timer.setInitialValue (2);
	timer.loadCounter ();
	require (timer.currentValue () == 2 && !timer.reaches0 () && !timer.reaches0LSB (),
		"Load clears zero flags");
	require (!tick (timer)._timeout && timer.currentValue () == 1, "Counter load cancels pending reload after initialize");

	timer.setCountAndRunMode (CountMode::_PROCESSORCYCLES, RunMode::_ONESHOOTSIGNAL);
	timer.loadCounter ();
	port.setP7 (false);
	require (!tick (timer)._timeout && !port.p7 (), "One-shot PB7 before timeout");
	require (tick (timer)._timeout && port.p7 (), "One-shot PB7 at timeout");
	std::cout << "Counter phases, PB7, flag clear, writes and mode changes PASS\n";
}

// ---
void VIATimerTests::oneShots ()
{
	COMMODORE::VIATimer1 timer1 (0);
	COMMODORE::VIATimer2 timer2 (1);
	COMMODORE::VIATimer* timers [] = { &timer1, &timer2 };
	for (COMMODORE::VIATimer* timer : timers)
	{
		timer -> setCountAndRunMode (COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES,
			COMMODORE::VIATimer::RunMode::_ONESHOOT);
		timer -> setInitialValue (2);
		timer -> loadCounter ();
		require (!tick (*timer)._timeout && timer -> currentValue () == 1, "One-shot first decrement");
		require (tick (*timer)._timeout && timer -> currentValue () == 0 &&
			timer -> peekInterruptRequested (), "Existing one-shot timeout must remain unchanged");
		timer -> clearInterrupt ();
		for (unsigned int cycle = 0; cycle < 65536; ++cycle)
			tick (*timer);
		require (timer -> currentValue () == 0 && !timer -> peekInterruptRequested (),
			"One-shot must not reload or rearm interrupt after flag clear");
		timer -> loadCounter ();
		tick (*timer);
		tick (*timer);
		require (timer -> peekInterruptRequested (), "Counter load must rearm one-shot");
	}
	std::cout << "T1/T2 PHI2 one-shot compatibility PASS\n";
}

// ---
void VIATimerTests::tickVIA (COMMODORE::VIA& via, unsigned int cycles)
{
	for (unsigned int cycle = 0; cycle < cycles; ++cycle)
	{
		_cpu.addClockCycles (1);
		require (via.simulate (&_cpu), "VIA simulation failed");
	}
}

// ---
void VIATimerTests::pulseCounting ()
{
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "VIA initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "VIA clock synchronization failed");
	memory.writeRegister (0x02, 0x00); // PB6 input, driven by external levels.
	memory.writeRegister (0x0b, 0x20); // T2 pulse count, SR disabled.
	memory.writeRegister (0x08, 0x02);
	memory.writeRegister (0x09, 0x00);
	via.setPortBExternalPins (MCHEmul::UByte (0xff));
	tickVIA (via, 5);
	require (via.timer2Value () == 2, "PB6 high must not count PHI2 cycles");

	via.setPortBExternalPins (MCHEmul::UByte (0xbf));
	tickVIA (via);
	require (via.timer2Value () == 1 && (memory.readRegister (0x0d) & 0x20) == 0,
		"First PB6 falling edge must decrement exactly once without early IFR5");
	tickVIA (via, 5);
	for (unsigned int read = 0; read < 5; ++read)
		memory.readRegister (0x00);
	tickVIA (via);
	require (via.timer2Value () == 1, "PB6 held low and repeated port reads must not count");

	via.setPortBExternalPins (MCHEmul::UByte (0xff));
	tickVIA (via);
	require (via.timer2Value () == 1, "PB6 rising edge must not count");
	via.setPortBExternalPins (MCHEmul::UByte (0xbf));
	tickVIA (via);
	require (via.timer2Value () == 0 && (memory.readRegister (0x0d) & 0x20) != 0,
		"Second PB6 falling edge must set IFR5 at the existing T2 timeout");
	require ((memory.readRegister (0x0d) & 0x20) != 0, "IFR read must not acknowledge T2");
	memory.readRegister (0x08); // Reading T2C-L acknowledges IFR5 without rearming.
	require ((memory.readRegister (0x0d) & 0x20) == 0, "T2C-L must acknowledge IFR5");

	// Complete another 16-bit wrap: flag acknowledgement must not rearm T2.
	for (unsigned int pulse = 0; pulse < 65536; ++pulse)
	{
		via.setPortBExternalPins (MCHEmul::UByte (0xff));
		tickVIA (via);
		via.setPortBExternalPins (MCHEmul::UByte (0xbf));
		tickVIA (via);
	}
	require (via.timer2Value () == 0 && (memory.readRegister (0x0d) & 0x20) == 0,
		"T2 must keep pulse counting without rearming after IFR5 acknowledgement");

	memory.writeRegister (0x08, 0x01);
	memory.writeRegister (0x09, 0x00);
	via.setPortBExternalPins (MCHEmul::UByte (0xff));
	tickVIA (via);
	via.setPortBExternalPins (MCHEmul::UByte (0xbf));
	tickVIA (via);
	require ((memory.readRegister (0x0d) & 0x20) != 0, "T2 counter load must rearm pulse timeout");
	memory.writeRegister (0x0d, 0x20);
	require ((memory.readRegister (0x0d) & 0x20) == 0, "IFR write must acknowledge T2");

	// Switch source without loading: fixed PB6 must no longer stop the counter.
	memory.writeRegister (0x0b, 0x00);
	tickVIA (via);
	require (via.timer2Value () == 0xffff, "Switching to PHI2 must resume cycle counting");
	std::cout << "VIA T2/PB6 wiring, edges, IFR5 acknowledgement, rearm and PHI2 switch PASS\n";
}

// ---
void VIATimerTests::interruptAdmission ()
{
	for (unsigned int interruptId : { F6500::NMIInterrupt::_ID, F6500::IRQInterrupt::_ID })
	{
		VIATestMemory memory;
		VIATestChip via (interruptId);
		via.setMemoryRef (&memory);
		require (via.initialize (), "Interrupt test VIA initialization failed");
		_cpu.interruptSystem () -> initialize ();
		_cpu.interrupt (interruptId) -> setNewInterruptRequestAdmitted (false);
		_cpu.addClockCycles (1);
		require (via.simulate (&_cpu), "Interrupt test clock synchronization failed");
		bool nmi = (interruptId == F6500::NMIInterrupt::_ID);

		// Both timers expire together; their enabled flags share one output.
		memory.writeRegister (0x04, 2);
		memory.writeRegister (0x05, 0);
		memory.writeRegister (0x08, 2);
		memory.writeRegister (0x09, 0);
		memory.writeRegister (0x0e, 0xe0);
		tickVIA (via, 2);
		require ((memory.readRegister (0x0d) & 0xe0) == 0xe0,
			"Both enabled timer flags must assert the VIA interrupt output");
		require (_cpu.interruptsRequested ().size () == 1 &&
			_cpu.getNextInterruptRequest ().type () == (int) interruptId,
			"First VIA assertion must admit a CPU interrupt request");
		MCHEmul::CPUInterruptRequest firstRequest = _cpu.getNextInterruptRequest ();
		_cpu.removeInterruptRequest (firstRequest);
		tickVIA (via, 4);
		require (nmi ? _cpu.interruptsRequested ().empty () : _cpu.interruptsRequested ().size () == 1,
			"Held NMI must not repeat; held IRQ must keep requesting");

		memory.writeRegister (0x0d, 0x40);
		tickVIA (via);
		require ((memory.readRegister (0x0d) & 0xe0) == 0xa0,
			"Acknowledging T1 must leave enabled T2 asserting the output");
		if (nmi)
			require (!_cpu.interrupt (interruptId) -> admitNewInterruptRequest (),
				"Partial acknowledgement must not rearm NMI");
		memory.writeRegister (0x0d, 0x20);
		tickVIA (via);
		require (_cpu.interrupt (interruptId) -> admitNewInterruptRequest (),
			"Releasing the final enabled source must rearm NMI");
		_cpu.interruptSystem () -> initialize ();

		// A second timer load produces a fresh assertion and a new admitted request.
		memory.writeRegister (0x08, 2);
		memory.writeRegister (0x09, 0);
		tickVIA (via, 2);
		require (_cpu.interruptsRequested ().size () == 1,
			"A new assertion after release must admit another interrupt");
		MCHEmul::CPUInterruptRequest pendingRequest = _cpu.getNextInterruptRequest ();

		// Disabling the last source releases the output without clearing its flag
		// or cancelling the already latched CPU request.
		memory.writeRegister (0x0e, 0x20);
		tickVIA (via);
		require ((memory.readRegister (0x0d) & 0xa0) == 0x20,
			"Disabling T2 must preserve IFR5 and release IFR7");
		require (_cpu.interruptsRequested ().size () == 1 &&
			_cpu.getNextInterruptRequest ().number () == pendingRequest.number (),
			"Output release must preserve the already admitted CPU request");
		require (_cpu.interrupt (interruptId) -> admitNewInterruptRequest (),
			"Disabling the final pending source must rearm NMI");
		_cpu.removeInterruptRequest (pendingRequest);
		memory.writeRegister (0x0e, 0xa0);
		tickVIA (via);
		require (_cpu.interruptsRequested ().size () == 1,
			"Enabling an already pending flag must admit a fresh interrupt");
		memory.writeRegister (0x0d, 0x20);
		tickVIA (via);
		_cpu.interruptSystem () -> initialize ();
		std::cout << (nmi ? "NMI" : "IRQ")
			<< " admission, held output, partial acknowledgement, IER and pending preservation PASS\n";
	}
}

// ---
void VIATimerTests::pcrReadback ()
{
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "PCR test VIA initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "PCR test clock synchronization failed");
	memory.writeRegister (0x0b, 0); // SR disabled: PCR owns the control-line configuration.

	// PCR contains only configuration fields; all eight written bits must read back.
	for (unsigned int value = 0; value < 256; ++value)
	{
		memory.writeRegister (0x0c, value);
		require (memory.readRegister (0x0c) == value,
			"PCR readback mismatch for value " + std::to_string (value));
		require (memory.peekRegister (0x0c) == value,
			"PCR peek mismatch for value " + std::to_string (value));
	}

	// Changing CA1 through a read/modify/write must preserve CB1 and CB2.
	memory.writeRegister (0x0c, 0x10);
	memory.writeRegister (0x0c, memory.readRegister (0x0c) | 0x01);
	require (memory.readRegister (0x0c) == 0x11 && memory.peekRegister (0x0c) == 0x11,
		"PCR read/modify/write must preserve unrelated control fields");

	// Real CA1/CB1 edges establish pending flags before PCR is inspected.
	memory.writeRegister (0x0c, 0);
	via.CA1 ().setValue (false);
	via.CB1 ().setValue (false);
	tickVIA (via);
	unsigned int flags = memory.readRegister (0x0d);
	require ((flags & 0x12) == 0x12, "CA1 and CB1 flags must be pending before PCR reads");
	for (unsigned int read = 0; read < 4; ++read)
	{
		require (memory.readRegister (0x0c) == 0 && memory.peekRegister (0x0c) == 0,
			"PCR inspection must preserve its configuration");
		require (memory.readRegister (0x0d) == flags, "PCR read/peek must not acknowledge pending flags");
	}
	std::cout << "PCR: 256 read/peek combinations, read/modify/write and flag preservation PASS\n";
}

// ---
void VIATimerTests::serialOwnership ()
{
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "Serial ownership VIA initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "Serial ownership clock synchronization failed");
	VIASerialMonitor monitor (via);
	memory.writeRegister (0x0c, 0xe0); // PCR manual high.
	memory.writeRegister (0x0b, 0x1c); // External-clock serial output.
	memory.writeRegister (0x0a, 0x00);
	monitor.beginCycle (1);
	tickVIA (via, 3);
	require (!via.CB2 ().value () && monitor.events ().empty (),
		"PCR must not overwrite serial zero while no shift clock arrives");

	// PCR writes retain configuration without driving CB2 during output.
	for (unsigned int mode = 0; mode < 8; ++mode)
	{
		monitor.beginCycle (2 + mode);
		memory.writeRegister (0x0c, mode << 5);
		memory.writeRegister (0x00, 0xff);
		memory.readRegister (0x00);
		tickVIA (via);
		require (!via.CB2 ().value () && monitor.events ().empty (),
			"PCR/ORB access must not glitch external-clock serial output");
		require (memory.readRegister (0x0c) == (mode << 5), "PCR configuration must remain readable during SR output");
	}

	memory.writeRegister (0x0a, 0xff);
	monitor.beginCycle (10);
	memory.writeRegister (0x0c, 0xc0); // Manual low conflicts with serial one.
	tickVIA (via, 3);
	require (via.CB2 ().value () && monitor.events ().empty (), "PCR must not overwrite serial one");
	memory.writeRegister (0x0b, 0x00);
	require (!via.CB2 ().value (), "Disabling SR must restore the latest PCR manual level");
	// A change of function must not erase an earlier peripheral CB2 interrupt.
	memory.writeRegister (0x0c, 0);
	via.CB2 ().setValue (true);
	tickVIA (via);
	via.CB2 ().setValue (false);
	tickVIA (via);
	require ((memory.readRegister (0x0d) & 8) != 0, "Peripheral CB2 flag must be pending before SR handover");
	memory.writeRegister (0x0b, 0x0c);
	require ((memory.readRegister (0x0d) & 8) != 0, "Serial handover must preserve a pending CB2 flag");
	memory.readRegister (0);
	require ((memory.readRegister (0x0d) & 8) == 0, "ORB must still acknowledge the prior dependent CB2 flag");
	via.CB2 ().setValue (true);
	tickVIA (via);
	via.CB2 ().setValue (false);
	tickVIA (via);
	require ((memory.readRegister (0x0d) & 8) == 0, "Serial input must not regenerate peripheral CB2 interrupts");
	std::cout << "Serial CB2 stability, PCR/ORB ownership and disable handover PASS\n";
}

// ---
void VIATimerTests::serialOutputTimelines ()
{
	for (unsigned int mode : { 4, 5, 6, 7 })
	{
		for (unsigned int pcrMode = 0; pcrMode < 8; ++pcrMode)
		{
			for (unsigned int polarity : { 0, 1 })
			{
				for (unsigned int pattern : { 0xa5, 0x5a })
				{
					VIATestMemory memory;
					VIATestChip via;
					via.setMemoryRef (&memory);
					require (via.initialize (), "Serial output VIA initialization failed");
					_cpu.addClockCycles (1);
					require (via.simulate (&_cpu), "Serial output clock synchronization failed");
					tickVIA (via);
					VIASerialMonitor monitor (via);
					memory.writeRegister (0x0c, (pcrMode << 5) | (polarity << 4));
					memory.writeRegister (0x08, 2); // T2-derived clock has four cycles per half-period.
					memory.writeRegister (0x0b, (mode << 2) | 2); // Also latch PB on the selected CB1 edge.
					memory.writeRegister (0x0a, pattern);
					bool finite = (mode == 5 || mode == 6);
					bool external = (mode == 7);
					unsigned int halfPeriod = (mode == 4 || mode == 5) ? 4 : 1;
					unsigned int shifts = 0;
					bool previousClock = true;
					bool previousData = (pattern & 0x80) != 0;
					require (via.CB2 ().value () == previousData, "SR write must publish the initial MSB");
					for (unsigned int cycle = 1; cycle <= (finite ? 16 * halfPeriod + 5 : 20 * halfPeriod); ++cycle)
					{
						monitor.beginCycle (cycle);
						memory.writeRegister (0x0d, 0x10);
						unsigned int pins = (cycle & 1) ? 0x3c : 0xc3;
						via.setPortBExternalPins (MCHEmul::UByte (pins));
						if (external)
							via.CB1 ().setValue ((cycle & 1) == 0);
						tickVIA (via);

						bool clock = (finite && cycle >= 16 * halfPeriod) || ((cycle / halfPeriod) & 1) == 0;
						bool edge = (clock != previousClock);
						bool shift = edge && (external ? clock : !clock);
						bool data = previousData;
						if (shift)
						{
							data = (pattern & (0x80 >> (shifts % 8))) != 0;
							++shifts;
						}
						if (finite && cycle >= 16 * halfPeriod)
							data = (pcrMode & 1) != 0;
						require (via.CB1 ().value () == clock && via.CB2 ().value () == data,
							"Serial output timeline mismatch: mode " + std::to_string (mode) + ", cycle " + std::to_string (cycle));
						require (monitor.events ().size () == (unsigned int) edge + (unsigned int) (data != previousData),
							"Serial output must not emit extra wire transitions between shifts");
						for (const auto& event : monitor.events ())
							require (event._cycle == cycle && event._level == (event._clock ? clock : data),
								"Serial output transition must match the expected clock/data level");
						unsigned int flags = memory.readRegister (0x0d);
						bool selectedEdge = edge && (clock == (polarity != 0));
						require (((flags & 0x10) != 0) == selectedEdge,
							"CB1 IFR4 must detect internal and external serial clock edges");
						if (selectedEdge)
							require (memory.peekRegister (0) == pins, "Selected serial CB1 edge must latch PB inputs");
						require (((flags & 4) != 0) == (mode != 4 && shifts >= 8), "Serial IFR2 completion/free-run mismatch");
						previousClock = clock;
						previousData = data;
					}
					if (finite)
					{
						memory.readRegister (0x0a);
						require (via.CB2 ().value () == ((pattern & 0x80) != 0), "SR read must restart output from its MSB");
						require ((memory.readRegister (0x0d) & 4) == 0, "SR read must acknowledge completion");
					}
				}
			}
		}
	}
	std::cout << "All serial output modes: clock/data transitions, final pulse, PCR idle, IFR2/IFR4 and PB latch PASS\n";
}

// ---
void VIATimerTests::serialInputAndHandshake ()
{
	for (unsigned int mode : { 1, 2, 3 })
	{
		for (unsigned int pcrMode = 0; pcrMode < 8; ++pcrMode)
		{
			VIATestMemory memory;
			VIATestChip via;
			via.setMemoryRef (&memory);
			require (via.initialize (), "Serial input VIA initialization failed");
			_cpu.addClockCycles (1);
			require (via.simulate (&_cpu), "Serial input clock synchronization failed");
			tickVIA (via);
			VIASerialMonitor monitor (via);
			memory.writeRegister (0x0c, pcrMode << 5);
			memory.writeRegister (0x08, 2);
			memory.writeRegister (0x0b, mode << 2);
			memory.writeRegister (0x0a, 0);
			unsigned int halfPeriod = (mode == 1) ? 4 : 1;
			unsigned int shifts = 0;
			bool previousClock = true;
			for (unsigned int cycle = 1; cycle <= 16 * halfPeriod; ++cycle)
			{
				monitor.beginCycle (cycle);
				bool data = (0xa5 & (0x80 >> (shifts % 8))) != 0;
				bool previousData = via.CB2 ().value ();
				via.CB2 ().setValue (data);
				memory.writeRegister (0x0c, pcrMode << 5);
				memory.writeRegister (0x00, 0xff);
				if (mode == 3)
					via.CB1 ().setValue ((cycle & 1) == 0);
				tickVIA (via);
				bool clock = ((cycle / halfPeriod) & 1) == 0;
				bool edge = clock != previousClock;
				if (edge && (mode == 3 ? clock : !clock))
					++shifts;
				require (via.CB2 ().value () == data && via.CB1 ().value () == clock,
					"PCR/ORB must not drive serial input or change its clock");
				require (monitor.events ().size () == (unsigned int) edge + (unsigned int) (data != previousData),
					"Serial input must not emit PCR/handshake glitches");
				require ((memory.readRegister (0x0d) & 8) == 0, "CB2 peripheral interrupts must not be generated by serial data");
				previousClock = clock;
			}
			require (shifts == 8 && memory.peekRegister (0x0a) == 0xa5 && (memory.readRegister (0x0d) & 4) != 0,
				"Serial input must capture eight external bits and set IFR2");
			// Finite input remains an input after its internal clock has stopped.
			if (mode != 3)
			{
				monitor.beginCycle (100);
				via.CB2 ().setValue (false);
				tickVIA (via, 3);
				require (!via.CB2 ().value () && via.CB1 ().value (), "Completed serial input must not become PCR output");
			}
		}
	}

	// Once SR is disabled, ordinary CB2 handshake and pulse outputs work again.
	for (unsigned int pcrMode : { 4, 5 })
	{
		VIATestMemory memory;
		VIATestChip via;
		via.setMemoryRef (&memory);
		require (via.initialize (), "Handshake VIA initialization failed");
		_cpu.addClockCycles (1);
		require (via.simulate (&_cpu), "Handshake clock synchronization failed");
		tickVIA (via);
		memory.writeRegister (0x0b, 0x1c);
		memory.writeRegister (0x0a, 0);
		memory.writeRegister (0x0c, pcrMode << 5);
		memory.writeRegister (0x0b, 0);
		require (via.CB2 ().value (), "Disabling SR must restore peripheral idle high");
		memory.writeRegister (0, 0);
		require (!via.CB2 ().value (), "Peripheral ORB write must start CB2 handshake/pulse");
		if (pcrMode == 4)
		{
			via.CB1 ().setValue (false);
			tickVIA (via);
		}
		else
		{
			tickVIA (via);
			require (!via.CB2 ().value (), "CB2 pulse must remain low for one cycle");
			tickVIA (via);
		}
		require (via.CB2 ().value (), "Peripheral handshake/pulse must return high");

		// CA2 stays peripheral while CB2 belongs to the SR.
		memory.writeRegister (0x0b, 0x1c);
		memory.writeRegister (0x0a, 0xff);
		memory.writeRegister (0x0c, 0x0a);
		memory.writeRegister (1, 0);
		require (!via.CA2 ().value (), "SR must not suppress CA2 peripheral pulses");
		tickVIA (via, 2);
		require (via.CA2 ().value (), "CA2 pulse must finish while SR owns CB2");
	}
	std::cout << "All serial input modes/PCR modes, restored CB2 handshake/pulse and independent CA2 PASS\n";
}

// ---
std::vector <VIATimerTests::SerialSample> VIATimerTests::serialACRTrace
	(unsigned int mode, unsigned int otherBits, unsigned int writeCycle)
{
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "ACR trace VIA initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "ACR trace clock synchronization failed");
	tickVIA (via);
	memory.writeRegister (0x0c, 0xe0);
	memory.writeRegister (0x08, 2);
	memory.writeRegister (0x0b, mode << 2);
	memory.writeRegister (0x0a, 0xa5);
	VIASerialMonitor monitor (via);
	unsigned int halfPeriod = (mode == 1 || mode == 4 || mode == 5) ? 4 : 1;
	unsigned int duration = 18 * halfPeriod + 4;
	std::vector <SerialSample> result;
	result.reserve (duration + 1);
	for (unsigned int cycle = 0; cycle <= duration; ++cycle)
	{
		monitor.beginCycle (cycle);
		if (cycle != 0 && mode <= 3)
		{
			// Feed the same external data schedule to both executions.
			unsigned int bit = (cycle - 1 + (mode == 3 ? 0 : halfPeriod)) / (2 * halfPeriod);
			via.CB2 ().setValue ((0xa5 & (0x80 >> (bit % 8))) != 0);
		}
		if (cycle != 0 && (mode == 3 || mode == 7))
			via.CB1 ().setValue ((cycle & 1) == 0);
		if (cycle == writeCycle)
		{
			unsigned int acr = (mode << 2) | otherBits;
			memory.writeRegister (0x0b, acr);
			require (memory.readRegister (0x0b) == acr, "Same-mode ACR write must still update its other fields");
		}
		if (cycle != 0)
			tickVIA (via);
		SerialSample sample;
		sample._clock = via.CB1 ().value ();
		sample._data = via.CB2 ().value ();
		sample._value = memory.peekRegister (0x0a);
		sample._flags = memory.readRegister (0x0d) & 4;
		for (const auto& transition : monitor.events ())
			sample._transitions.push_back ((transition._clock ? 2 : 0) | (transition._level ? 1 : 0));
		result.push_back (std::move (sample));
	}
	return (result);
}

// ---
void VIATimerTests::serialACRPreservation ()
{
	unsigned int scenarios = 0;
	for (unsigned int mode = 1; mode <= 7; ++mode)
	{
		// No injection: retain the established SR waveform as a reference execution.
		const std::vector <SerialSample> reference = serialACRTrace (mode, 0, 0xffffffff);
		for (unsigned int variant = 0; variant < 32; ++variant)
		{
			unsigned int otherBits = (variant & 3) | ((variant & 0x1c) << 3);
			for (unsigned int writeCycle = 0; writeCycle < reference.size (); ++writeCycle)
			{
				const std::vector <SerialSample> actual = serialACRTrace (mode, otherBits, writeCycle);
				require (actual.size () == reference.size (), "ACR trace length must remain unchanged");
				for (size_t cycle = 0; cycle < reference.size (); ++cycle)
				{
					const SerialSample& expected = reference [cycle];
					const SerialSample& observed = actual [cycle];
					require (observed._clock == expected._clock && observed._data == expected._data &&
						observed._value == expected._value && observed._flags == expected._flags &&
						observed._transitions == expected._transitions,
						"Same-mode ACR write altered SR: mode " + std::to_string (mode) +
						", bits " + std::to_string (otherBits) + ", write cycle " + std::to_string (writeCycle) +
						", first divergence " + std::to_string (cycle));
				}
				++scenarios;
			}
		}
	}
	std::cout << "Same-mode ACR: " << scenarios
		<< " differential cases, seven active modes, 32 unrelated-bit combinations, all cycle positions PASS\n";
}

// ---
void VIATimerTests::serialModeTransitions ()
{
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "Mode transition VIA initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "Mode transition clock synchronization failed");
	tickVIA (via);
	memory.writeRegister (0x0c, 0xe0);
	memory.writeRegister (0x0b, 0x18);
	memory.writeRegister (0x0a, 0xa5);
	tickVIA (via);
	unsigned int value = memory.peekRegister (0x0a);
	memory.writeRegister (0x0b, 0x1c); // A real change: PHI2 output to external-clock output.
	for (unsigned int pulse = 0; pulse < 8; ++pulse)
	{
		via.CB1 ().setValue (false);
		tickVIA (via);
		via.CB1 ().setValue (true);
		tickVIA (via);
		require (((memory.readRegister (0x0d) & 4) != 0) == (pulse == 7),
			"A real mode change must retain its existing eight-pulse counter reset");
	}
	require (memory.peekRegister (0x0a) == value, "Eight external output pulses must recirculate the byte");
	memory.writeRegister (0x0b, 0);
	require ((memory.readRegister (0x0d) & 4) == 0 && via.CB2 ().value (),
		"Disabling SR must still clear IFR2 and restore PCR");
	memory.writeRegister (0x0b, 0x18);
	tickVIA (via, 3);
	require (via.CB1 ().value () && (memory.readRegister (0x0d) & 4) == 0,
		"Selecting internal mode must not start a transfer without an SR access");
	memory.readRegister (0x0a);
	tickVIA (via, 16);
	require ((memory.readRegister (0x0d) & 4) != 0, "Reading SR must still initiate eight shifts");
	memory.writeRegister (0x0a, 0x5a);
	require ((memory.readRegister (0x0d) & 4) == 0 && !via.CB2 ().value (),
		"Writing SR must still acknowledge IFR2 and publish the new MSB");
	tickVIA (via, 16);
	require (memory.peekRegister (0x0a) == 0x5a && (memory.readRegister (0x0d) & 4) != 0,
		"Writing SR must still start another complete transfer");
	std::cout << "Real mode changes, disable, and SR read/write restart compatibility PASS\n";
}

// ---
void VIATimerTests::setVIA1Input (MCHEmul::Notifier& source, unsigned int input, bool pressed)
{
	if (input < 3)
	{
		int x = (pressed && input == 2) ? -1 : 0;
		int y = !pressed ? 0 : (input == 0 ? -1 : (input == 1 ? 1 : 0));
		source.notify (MCHEmul::Event (MCHEmul::InputOSSystem::_JOYSTICKMOVED, 0,
			std::make_shared <MCHEmul::InputOSSystem::JoystickMovementEvent> (0, std::vector <int> { x, y })));
	}
	else if (input == 3)
		source.notify (MCHEmul::Event (pressed ? MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED :
			MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED, 0,
			std::make_shared <MCHEmul::InputOSSystem::JoystickButtonEvent> (0, 0, pressed)));
	else
		source.notify (MCHEmul::Event (pressed ? MCHEmul::DatasetteIOPort::_KEYPRESSED :
			MCHEmul::DatasetteIOPort::_NOKEYPRESSED));
}

// ---
void VIATimerTests::via1InputCapture ()
{
	for (unsigned int polarity : { 0, 1 })
	{
		for (unsigned int input = 0; input < 5; ++input)
		{
			VIATestMemory memory (true);
			VIC20::VIA1 via;
			MCHEmul::Notifier source;
			via.setMemoryRef (&memory);
			require (via.initialize (), "Real VIA1 input fixture initialization failed");
			via.observe (&source);
			_cpu.addClockCycles (1);
			require (via.simulate (&_cpu), "Real VIA1 clock synchronization failed");
			memory.writeRegister (3, 0);
			via.setPortAExternalPins (MCHEmul::UByte (0x7e)); // Retain PA0/PA1/PA7 independently of joystick/tape.
			tickVIA (via);
			memory.writeRegister (0x0c, polarity);
			via.CA1 ().setValue (polarity == 0);
			tickVIA (via);
			memory.writeRegister (0x0b, 1);
			unsigned int mask = 1 << (input + 2);
			unsigned int pressedPins = 0x7e & ~mask;
			setVIA1Input (source, input, true);
			require (via.portA ().value () [0].value () == pressedPins,
				"VIA1 event must publish live joystick/tape pins before any register read");
			via.CA1 ().setValue (polarity != 0);
			tickVIA (via);
			require ((memory.readRegister (0x0d) & 2) != 0, "CA1 capture must set its interrupt flag");
			setVIA1Input (source, input, false);
			require (via.portA ().value () [0].value () == 0x7e, "Releasing input must update live pins while retaining capture");
			for (unsigned int read = 0; read < 3; ++read)
			{
				require (memory.readRegister (1) == pressedPins, "PA must return the input captured before release");
				require (via.portA ().value () [0].value () == 0x7e, "Reading captured PA must not drive live inputs");
			}
			require ((memory.readRegister (0x0d) & 2) == 0, "Ordinary PA read must acknowledge CA1");
			memory.writeRegister (0x0b, 0);
			require (memory.readRegister (1) == 0x7e, "Disabling input latching must expose the live input");
			memory.writeRegister (0x0b, 1);
			require (memory.readRegister (1) == pressedPins, "Enabling latching must not invent a new capture");
			memory.writeRegister (0x0c, polarity | 8); // CA2 handshake; $f must not activate it.
			via.CA1 ().setValue (polarity == 0);
			tickVIA (via);
			via.CA1 ().setValue (polarity != 0);
			tickVIA (via);
			require (memory.readRegister (0x0f) == 0x7e && via.CA2 ().value () &&
				(memory.readRegister (0x0d) & 2) != 0, "$911f must return the new capture without handshake or acknowledgement");
			require (memory.readRegister (1) == 0x7e && !via.CA2 ().value () &&
				(memory.readRegister (0x0d) & 2) == 0, "$9111 must retain ordinary handshake/acknowledgement");
		}
	}

	// PB reads must preserve user-port inputs, with or without latching (point 9 overlap).
	for (unsigned int polarity : { 0, 1 })
	{
		VIATestMemory memory (true);
		VIC20::VIA1 via;
		via.setMemoryRef (&memory);
		require (via.initialize (), "VIA1 PB fixture initialization failed");
		_cpu.addClockCycles (1);
		require (via.simulate (&_cpu), "VIA1 PB synchronization failed");
		memory.writeRegister (0x0c, polarity << 4);
		via.CB1 ().setValue (polarity == 0);
		tickVIA (via);
		via.setPortBExternalPins (MCHEmul::UByte (0xfe));
		require (memory.readRegister (0) == 0xfe && via.portB ().value () [0].value () == 0xfe,
			"Unlatched user-port read must preserve an external low input");
		memory.writeRegister (0x0b, 2);
		via.CB1 ().setValue (polarity != 0);
		tickVIA (via);
		via.setPortBExternalPins (MCHEmul::UByte (0xff));
		for (unsigned int read = 0; read < 3; ++read)
			require (memory.readRegister (0) == 0xfe && via.portB ().value () [0].value () == 0xff,
				"Captured PB must not be fed back into live user-port pins");
		via.CB1 ().setValue (polarity == 0);
		tickVIA (via);
		via.CB1 ().setValue (polarity != 0);
		tickVIA (via);
		require (memory.readRegister (0) == 0xff, "Next CB1 capture must see the updated live user-port input");
	}
	std::cout << "Real VIA1 joystick/tape events, live versus captured PA/PB, both polarities and $911f PASS\n";
}

// ---
void VIATimerTests::portCaptureResolution ()
{
	for (unsigned int polarity : { 0, 1 })
	{
		VIATestMemory memory;
		VIATestChip via;
		via.setMemoryRef (&memory);
		require (via.initialize (), "Port capture fixture initialization failed");
		_cpu.addClockCycles (1);
		require (via.simulate (&_cpu), "Port capture synchronization failed");
		tickVIA (via);
		memory.writeRegister (0x0c, polarity);
		via.CA1 ().setValue (polarity == 0);
		tickVIA (via);
		memory.writeRegister (0x0b, 1);
		memory.writeRegister (3, 0xf0);
		memory.writeRegister (1, 0xa0);
		via.CA1 ().setValue (polarity != 0);
		tickVIA (via);
		require (via.latchedA () == 0xaf, "CA1 capture must resolve newly written OR/DDR before copying pins");

		memory.writeRegister (0x0c, 0);
		memory.writeRegister (0x0b, 0x22); // PB latching plus T2 pulse count.
		memory.writeRegister (8, 2);
		memory.writeRegister (9, 0);
		via.setPortBExternalPins (MCHEmul::UByte (0xbf));
		via.CB1 ().setValue (false);
		tickVIA (via);
		require (via.timer2Value () == 1, "PB capture must not consume the PB6 edge twice");
		tickVIA (via, 3);
		require (via.timer2Value () == 1, "PB capture must not leave a stale T2 edge");
		via.setPortBExternalPins (MCHEmul::UByte (0xff));
		tickVIA (via);
		via.setPortBExternalPins (MCHEmul::UByte (0xbf));
		tickVIA (via);
		require (via.timer2Value () == 0, "A later PB6 edge must still count exactly once");
	}
	std::cout << "Fresh OR/DDR capture and PB6 pulse-count compatibility PASS\n";
}

// ---
void VIATimerTests::timerPB7Direction ()
{
	for (unsigned int ddr : { 0x00, 0x80, 0x55, 0xaa, 0xff })
	{
		for (unsigned int mode : { 0x80, 0xc0 })
		{
			for (unsigned int latch : { 0, 2 })
			{
				for (unsigned int orb : { 0x00, 0xff, 0x55, 0xaa })
				{
					VIATestMemory memory;
					VIATestChip via;
					via.setMemoryRef (&memory);
					require (via.initialize (), "PB7 direction fixture initialization failed");
					_cpu.addClockCycles (1);
					require (via.simulate (&_cpu), "PB7 direction clock synchronization failed");
					memory.writeRegister (2, ddr);
					memory.writeRegister (0, orb);
					memory.writeRegister (0x0b, mode | (latch ? 2 : 0));
					via.setPortBExternalPins (MCHEmul::UByte (0xa5));
					tickVIA (via);
					via.CB1 ().setValue (false);
					tickVIA (via);
					unsigned int captured = memory.readRegister (0);
					via.setPortBExternalPins (MCHEmul::UByte (0x5a));
					memory.writeRegister (4, 2);
					memory.writeRegister (5, 0);
					unsigned int currentORB = orb;
					for (unsigned int cycle = 1; cycle <= 14; ++cycle)
					{
						if (cycle == 4)
						{
							currentORB ^= 0x80;
							memory.writeRegister (0, currentORB);
						}
						tickVIA (via);
						bool timerHigh = mode == 0x80 ? cycle >= 2 : (((cycle + 1) / 4) & 1) != 0;
						unsigned int physical = ((currentORB & ddr) | (0x5a & ~ddr)) & 0x7f;
						unsigned int read = ((currentORB & ddr) | ((latch ? captured : 0x5a) & ~ddr)) & 0x7f;
						physical |= timerHigh ? 0x80 : 0;
						read |= timerHigh ? 0x80 : 0;
						require (via.portB ().value () [0].value () == physical,
							"T1 must drive physical PB7 independently of DDRB7");
						require (memory.readRegister (0) == read && memory.peekRegister (0) == read,
							"PB7 read/peek must reflect T1, even with input latching");
						require (memory.readRegister (2) == ddr, "Effective PB7 direction must not change stored DDRB");
					}
					memory.writeRegister (0x0b, (mode & 0x7f) | (latch ? 2 : 0));
					tickVIA (via);
					unsigned int physical = (currentORB & ddr) | (0x5a & (~ddr & 0xff));
					unsigned int read = (currentORB & ddr) | ((latch ? captured : 0x5a) & (~ddr & 0xff));
					require (via.portB ().value () [0].value () == physical && memory.readRegister (0) == read,
						"Disabling T1 output must restore ordinary DDR/ORB/input-latch selection");
				}
			}
		}
	}

	// While T1 drives high, a raw bus update on PB0 must not become a high PB7 input.
	VIATestMemory memory;
	VIATestChip via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "PB7 external preservation initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "PB7 external preservation synchronization failed");
	memory.writeRegister (0x0b, 0x80);
	via.setPortBExternalPins (MCHEmul::UByte (0x7f));
	memory.writeRegister (4, 2);
	memory.writeRegister (5, 0);
	tickVIA (via, 2);
	require (via.portB ().value () [0].bit (7), "T1 timeout must drive PB7 high");
	via.portB ().setValue (MCHEmul::UBytes ({ MCHEmul::UByte (0xfe) }));
	tickVIA (via);
	memory.writeRegister (0x0b, 0);
	tickVIA (via);
	require (via.portB ().value () [0].value () == 0x7e && memory.readRegister (0) == 0x7e,
		"Alternate PB7 output must not overwrite the preserved external input");

	// PB7 activity must not turn into PB6 count events.
	memory.writeRegister (0x0b, 0xe0);
	via.setPortBExternalPins (MCHEmul::UByte (0xff));
	memory.writeRegister (4, 2);
	memory.writeRegister (5, 0);
	memory.writeRegister (8, 2);
	memory.writeRegister (9, 0);
	tickVIA (via, 8);
	require (via.timer2Value () == 2, "PB7 toggles must not decrement PB6 pulse counter");
	via.setPortBExternalPins (MCHEmul::UByte (0xbf));
	tickVIA (via, 5);
	require (via.timer2Value () == 1, "PB6 falling edge must count once while PB7 toggles");
	via.setPortBExternalPins (MCHEmul::UByte (0xff));
	tickVIA (via);
	via.setPortBExternalPins (MCHEmul::UByte (0xbf));
	tickVIA (via);
	require (via.timer2Value () == 0, "Second PB6 edge must remain independent of T1 PB7 output");
	// Without ACR7, DDRB7 alone selects the ordinary external input or ORB output.
	for (unsigned int ddr : { 0, 0x80 })
	{
		for (unsigned int orb : { 0, 0x80 })
		{
			for (unsigned int external : { 0x7f, 0xff })
			{
				memory.writeRegister (0x0b, 0);
				memory.writeRegister (2, ddr);
				memory.writeRegister (0, orb);
				via.setPortBExternalPins (MCHEmul::UByte (external));
				tickVIA (via);
				bool high = ddr ? orb != 0 : (external & 0x80) != 0;
				require (via.portB ().value () [0].bit (7) == high && ((memory.readRegister (0) & 0x80) != 0) == high,
					"Without timer output, PB7 must follow stored DDRB and ordinary ORB/external levels");
			}
		}
	}
	std::cout << "PB7 effective direction: 80 mixed-DDR/ORB/latching cases, input preservation and PB6 independence PASS\n";
}

// ---
void VIATimerTests::timerPB7Keyboard ()
{
	for (unsigned int ddr : { 0x7f, 0xff })
	{
		for (unsigned int mode : { 0x80, 0xc0 })
		{
			VIATestMemory memory (2);
			VIC20::VIA2 via;
			via.setMemoryRef (&memory);
			require (via.initialize (), "Real VIA2 keyboard fixture initialization failed");
			_cpu.addClockCycles (1);
			require (via.simulate (&_cpu), "Real VIA2 synchronization failed");
			memory.writeRegister (2, ddr);
			memory.writeRegister (0, 0xff);
			static_cast <VIC20::VIA2Registers*> (memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET)) ->
				setKeyboardStatusMatrix (7, 0, false);
			memory.writeRegister (0x0b, mode);
			memory.writeRegister (4, 2);
			memory.writeRegister (5, 0);
			tickVIA (via);
			require ((memory.readRegister (1) & 1) == 0,
				"VIA2 keyboard must see column 7 low from T1 even when DDRB7 is zero");
			tickVIA (via, mode == 0x80 ? 1 : 2);
			require ((memory.readRegister (1) & 1) != 0 && memory.readRegister (2) == ddr,
				"VIA2 keyboard must release column 7 when T1 drives high without changing DDRB");
		}
	}
	std::cout << "Real VIA2 keyboard column 7 follows T1 with DDRB7 zero or one PASS\n";
}

// ---
void VIATimerTests::keyboardMatrixResolution ()
{
	VIATestMemory memory (2);
	VIC20::VIA2 via;
	MCHEmul::Notifier source;
	via.setMemoryRef (&memory);
	require (via.initialize (), "Keyboard matrix VIA2 initialization failed");
	via.observe (&source);
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "Keyboard matrix synchronization failed");
	tickVIA (via);
	VIC20::VIA2Registers* registers = static_cast <VIC20::VIA2Registers*>
		(memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET));
	memory.writeRegister (2, 0);
	memory.writeRegister (3, 0);
	registers -> setKeyboardStatusMatrix (7, 0, false);
	source.notify (MCHEmul::Event (MCHEmul::InputOSSystem::_JOYSTICKMOVED, 0,
		std::make_shared <MCHEmul::InputOSSystem::JoystickMovementEvent> (0, std::vector <int> { 1, 0 })));
	require (memory.readRegister (1) == 0xfe && memory.peekRegister (1) == 0xfe,
		"Joystick right must pull a keyboard row low through input column PB7");
	require (memory.readRegister (0) == 0x7f && memory.peekRegister (0) == 0x7f,
		"Both keyboard reads must agree on the joystick-driven column");
	source.notify (MCHEmul::Event (MCHEmul::InputOSSystem::_JOYSTICKMOVED, 0,
		std::make_shared <MCHEmul::InputOSSystem::JoystickMovementEvent> (0, std::vector <int> { 0, 0 })));
	require (memory.readRegister (1) == 0xff && memory.readRegister (0) == 0xff,
		"Releasing joystick must remove prior propagated lows despite the held key");
	registers -> setKeyboardStatusMatrix (7, 0, true);

	for (unsigned int column = 0; column < 8; ++column)
	{
		for (unsigned int row = 0; row < 8; ++row)
		{
			registers -> setKeyboardStatusMatrix (column, row, false);
			for (unsigned int reverse : { 0, 1 })
			{
				unsigned int selected = reverse ? row : column;
				unsigned int sensed = reverse ? column : row;
				memory.writeRegister (2, reverse ? 0 : 0xff);
				memory.writeRegister (3, reverse ? 0xff : 0);
				memory.writeRegister (reverse ? 1 : 0, 0xff ^ (1 << selected));
				unsigned int port = reverse ? 0 : 1;
				require (memory.readRegister (port) == (0xff ^ (1 << sensed)) &&
					memory.peekRegister (port) == (0xff ^ (1 << sensed)), "Every key must support forward/reverse scan and peek");
				memory.writeRegister (reverse ? 1 : 0, 0xff ^ (1 << ((selected + 1) % 8)));
				require (memory.readRegister (port) == 0xff, "An unselected line must not report the held key");
			}
			registers -> setKeyboardStatusMatrix (column, row, true);
			require (memory.readRegister (0) == 0xff, "Released key must not leave a reverse-scan input low");
		}
	}

	// Two driven columns combine their keys; an unselected third column stays inactive.
	memory.writeRegister (2, 0xff);
	memory.writeRegister (3, 0);
	memory.writeRegister (0, 0xfa);
	registers -> setKeyboardStatusMatrix (0, 1, false);
	registers -> setKeyboardStatusMatrix (2, 3, false);
	registers -> setKeyboardStatusMatrix (5, 6, false);
	require (memory.readRegister (1) == 0xf5, "Simultaneously selected columns must combine their pressed rows");
	registers -> setKeyboardStatusMatrix (2, 3, true);
	require (memory.readRegister (1) == 0xfd, "Releasing one key must preserve the other selected key");
	registers -> setKeyboardStatusMatrix (0, 1, true);
	registers -> setKeyboardStatusMatrix (5, 6, true);
	require (memory.readRegister (1) == 0xff, "Released multiple-key selection must return high");

	// A fifteen-switch chain spans all sixteen pins; one-pass propagation is insufficient.
	memory.writeRegister (2, 0);
	memory.writeRegister (3, 0);
	for (unsigned int row = 0; row < 7; ++row)
	{
		registers -> setKeyboardStatusMatrix (7 - row, row, false);
		registers -> setKeyboardStatusMatrix (6 - row, row, false);
	}
	registers -> setKeyboardStatusMatrix (0, 7, false);
	registers -> setJoystickStatus (0x7f);
	unsigned int clock = _cpu.clockCycles ();
	require (memory.peekRegister (1) == 0 && memory.peekRegister (0) == 0,
		"Matrix settling must propagate a low across the complete sixteen-pin chain");
	require (_cpu.clockCycles () == clock, "Matrix settling iterations must not add emulated CPU cycles");
	registers -> setKeyboardStatusMatrix (3, 3, true);
	require (memory.readRegister (1) == 0xf0 && memory.readRegister (0) == 0x0f,
		"Opening the middle switch must release the disconnected half of the matrix");
	registers -> setJoystickStatus (0xff);
	require (memory.readRegister (1) == 0xff && memory.readRegister (0) == 0xff,
		"Removing the last low source must release a connected matrix");
	for (unsigned int column = 0; column < 8; ++column)
		for (unsigned int row = 0; row < 8; ++row)
			registers -> setKeyboardStatusMatrix (column, row, true);

	// An enabled high timer output wins over the external joystick in this digital model.
	registers -> setKeyboardStatusMatrix (7, 0, false);
	registers -> setJoystickStatus (0x7f);
	memory.writeRegister (0x0b, 0x80);
	require (memory.readRegister (1) == 0xff && (memory.readRegister (0) & 0x80) != 0,
		"T1 high output must take priority over joystick-driven input on PB7");
	memory.writeRegister (4, 2);
	memory.writeRegister (5, 0);
	tickVIA (via);
	require (memory.readRegister (1) == 0xfe, "T1 low output must select column 7 in the common resolver");
	std::cout << "VIA2 joystick, 64 keys in both directions, long-chain settling/release and T1 priority PASS\n";
}

// ---
void VIATimerTests::keyboardLatchAndPulses ()
{
	VIATestMemory memory (2);
	VIC20::VIA2 via;
	via.setMemoryRef (&memory);
	require (via.initialize (), "Keyboard latch fixture initialization failed");
	_cpu.addClockCycles (1);
	require (via.simulate (&_cpu), "Keyboard latch synchronization failed");
	tickVIA (via);
	VIC20::VIA2Registers* registers = static_cast <VIC20::VIA2Registers*>
		(memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET));
	memory.writeRegister (2, 0x80);
	memory.writeRegister (0, 0);
	registers -> setKeyboardStatusMatrix (7, 0, false);
	memory.writeRegister (0x0b, 1);
	memory.peekRegister (1); // Publish before the synthetic capture edge.
	via.CA1 ().setValue (false);
	tickVIA (via);
	registers -> setKeyboardStatusMatrix (7, 0, true);
	require (memory.peekRegister (1) == 0xfe && via.portA ().value () [0].value () == 0xff &&
		(memory.readRegister (0x0d) & 2) != 0, "Keyboard peek must resolve live rows while retaining capture and CA1 flag");
	memory.writeRegister (0x0c, 8);
	require (memory.readRegister (0x0f) == 0xfe && via.CA2 ().value () && (memory.readRegister (0x0d) & 2) != 0,
		"Keyboard $912f must preserve no-handshake/no-acknowledgement semantics");
	require (memory.readRegister (1) == 0xfe && !via.CA2 ().value () && (memory.readRegister (0x0d) & 2) == 0,
		"Keyboard ordinary PA read must retain its handshake/acknowledgement");
	via.CA1 ().setValue (true);
	tickVIA (via);
	via.CA1 ().setValue (false);
	tickVIA (via);
	require (memory.readRegister (1) == 0xff, "A subsequent capture must use released rows");

	memory.writeRegister (0x0c, 0);
	memory.writeRegister (0x0b, 2);
	memory.writeRegister (2, 0);
	memory.writeRegister (3, 1);
	memory.writeRegister (1, 0);
	registers -> setKeyboardStatusMatrix (1, 0, false);
	memory.peekRegister (0);
	via.CB1 ().setValue (false);
	tickVIA (via);
	registers -> setKeyboardStatusMatrix (1, 0, true);
	require (memory.peekRegister (0) == 0xfd && via.portB ().value () [0].value () == 0xff &&
		(memory.readRegister (0x0d) & 0x10) != 0, "Keyboard PB peek must preserve capture and CB1 acknowledgement state");
	require (memory.readRegister (0) == 0xfd && (memory.readRegister (0x0d) & 0x10) == 0,
		"Keyboard PB read must acknowledge CB1 without changing live columns");

	// Matrix refresh publishes a level; repeated reads must not multiply its PB6 edge.
	memory.writeRegister (0x0b, 0x20);
	memory.writeRegister (8, 2);
	memory.writeRegister (9, 0);
	registers -> setKeyboardStatusMatrix (6, 0, false);
	for (unsigned int read = 0; read < 4; ++read)
		memory.peekRegister (0);
	tickVIA (via);
	require (memory.readRegister (8) == 1, "Keyboard-driven PB6 falling edge must count once");
	for (unsigned int read = 0; read < 4; ++read)
		memory.readRegister (0);
	tickVIA (via, 3);
	require (memory.readRegister (8) == 1, "Repeated matrix reads must not duplicate a PB6 edge");
	registers -> setKeyboardStatusMatrix (6, 0, true);
	memory.peekRegister (0);
	tickVIA (via);
	registers -> setKeyboardStatusMatrix (6, 0, false);
	memory.peekRegister (0);
	tickVIA (via);
	require (memory.readRegister (8) == 0, "Later keyboard PB6 edge must remain visible to T2");
	std::cout << "VIA2 live versus captured matrix, read/peek/handshake and PB6 pulse compatibility PASS\n";
}

// ---
void VIATimerTests::run ()
{
	timedReads ();
	timedWrites ();
	registerConstruction ();
	registerReinitialization ();
	keyboardMatrixResolution ();
	keyboardLatchAndPulses ();
	timerPB7Direction ();
	timerPB7Keyboard ();
	periods ();
	phasesAndWrites ();
	oneShots ();
	pulseCounting ();
	interruptAdmission ();
	pcrReadback ();
	serialOwnership ();
	serialOutputTimelines ();
	serialInputAndHandshake ();
	serialACRPreservation ();
	serialModeTransitions ();
	via1InputCapture ();
	portCaptureResolution ();
}

int main ()
{
	try
	{
		VIATimerTests tests;
		tests.run ();
		return (0);
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what () << '\n';
		return (1);
	}
}
