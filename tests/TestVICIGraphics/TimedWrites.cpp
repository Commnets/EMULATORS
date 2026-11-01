#include "TimedWrites.hpp"
#include <COMMODORE/VICI/VICI.hpp>
#include <F6500/C6502.hpp>
#include <F6500/IRQInterrupt.hpp>
#include <F6500/NMIInterrupt.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/VIA2Registers.hpp>
#include <iostream>

class VICITimingProbe final : public COMMODORE::VICI
{
	public:
	VICITimingProbe (bool pal, COMMODORE::VICISoundLibWrapper* sound)
		: COMMODORE::VICI
			(pal ? COMMODORE::VICI_PAL::_VRASTERDATA : COMMODORE::VICI_NTSC::_VRASTERDATA,
			 pal ? COMMODORE::VICI_PAL::_HRASTERDATA : COMMODORE::VICI_NTSC::_HRASTERDATA,
			 VIC20::Memory::_VICI_VIEW, pal ? 71 : 65, sound)
							{ }
	virtual ~VICITimingProbe () override
							{ delete (_soundFunction); }
	unsigned int cycle () const
							{ return (_lastCPUCycles); }
	unsigned int register15 () const
							{ return (_VICIRegisters -> readRegister (15).value ()); }
	void setup (size_t position, const MCHEmul::UByte& value)
							{ _VICIRegisters -> setRegister (position, value); }
};

/** Delegate actual sound generation while observing its parameter timeline. */
class VICITimingSound final : public COMMODORE::VICISoundLibWrapper
{
	public:
	struct Write final { unsigned int _cycle, _ticks, _position, _value; };
	VICITimingSound (bool pal)
		: _delegate (pal ? 1108405 : 1022727, 44100)
							{ }
	void link (const VICITimingProbe* vic)
							{ _vic = vic; }
	void begin ();
	void end ()
							{ _record = false; }
	unsigned int ticks () const
							{ return (_ticks); }
	const std::vector <Write>& writes () const
							{ return (_writes); }
	const std::vector <unsigned int>& states () const
							{ return (_states); }
	virtual void initialize () override;
	virtual void setValue (size_t position, const MCHEmul::UByte& value) override;
	virtual const MCHEmul::UByte& readValue (size_t position) const override
							{ return (_delegate.readValue (position)); }
	virtual bool getData (MCHEmul::CPU* cpu, MCHEmul::UBytes& data) override;
	virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char voice) const override
							{ return (_delegate.getVoiceInfoStructure (voice)); }

	private:
	COMMODORE::VICISoundSimpleLibWrapper _delegate;
	const VICITimingProbe* _vic = nullptr;
	unsigned int _ticks = 0;
	bool _record = false;
	std::vector <Write> _writes;
	std::vector <unsigned int> _states;
};

// ---
void VICITimingSound::begin ()
{
	_record = true;
	_writes.clear ();
	_states.clear ();
}

// ---
void VICITimingSound::initialize ()
{
	_delegate.initialize ();
	_ticks = 0;
	_writes.clear ();
	_states.clear ();
}

// ---
void VICITimingSound::setValue (size_t position, const MCHEmul::UByte& value)
{
	if (_record)
		_writes.push_back ({ _vic -> cycle (), _ticks, (unsigned int) (position), value.value () });
	_delegate.setValue (position, value);
}

// ---
bool VICITimingSound::getData (MCHEmul::CPU* cpu, MCHEmul::UBytes& data)
{
	if (_record)
		_states.push_back (_delegate.readValue (0x0a).value ());
	++_ticks;
	return (_delegate.getData (cpu, data));
}

/** Separate test-only overlays record CPU commits and pre-write video observations. */
class VICITimingRAM final : public MCHEmul::PhysicalStorageSubset
{
	public:
	struct Write final { unsigned int _cycle, _address, _value; };
	struct Sample final
	{
		unsigned int _cycle, _value, _register15, _color;
		unsigned char _stack [3];
	};
	VICITimingRAM (MCHEmul::PhysicalStorage* ram, const VICITimingProbe* vic)
		: MCHEmul::PhysicalStorageSubset (1, ram, 0, MCHEmul::Address (2, 0), 0x2000), _vic (vic)
	{
		_writes.reserve (8);
		_samples.reserve (32);
	}
	void begin (unsigned int watched);
	void end ()
							{ _record = false; }
	const std::vector <Write>& writes () const
							{ return (_writes); }
	const std::vector <Sample>& samples () const
							{ return (_samples); }
	virtual void initialize () override
							{ }

	private:
	virtual void setValue (size_t position, const MCHEmul::UByte& value) override;
	virtual const MCHEmul::UByte& readValue (size_t position) const override;
	const VICITimingProbe* _vic;
	unsigned int _watched = 0x1000;
	bool _record = false;
	std::vector <Write> _writes;
	mutable std::vector <Sample> _samples;
};

// ---
void VICITimingRAM::begin (unsigned int watched)
{
	_watched = watched;
	_writes.clear ();
	_samples.clear ();
	_record = true;
}

// ---
void VICITimingRAM::setValue (size_t position, const MCHEmul::UByte& value)
{
	if (_record)
		_writes.push_back ({ _vic -> cycle (), (unsigned int) (position), value.value () });
	MCHEmul::PhysicalStorageSubset::setValue (position, value);
}

// ---
const MCHEmul::UByte& VICITimingRAM::readValue (size_t position) const
{
	if (_record && (_samples.empty () || _samples.back ()._cycle != _vic -> cycle ()))
		_samples.push_back ({ _vic -> cycle (), _physicalStorage -> value (_watched).value (),
			_vic -> register15 (), _physicalStorage -> value (0x9400).value (),
			{ _physicalStorage -> value (0x01ff).value (), _physicalStorage -> value (0x01fe).value (),
			  _physicalStorage -> value (0x01fd).value () } });
	return (MCHEmul::PhysicalStorageSubset::readValue (position));
}

class VICITimingFixture final : public MCHEmul::Observer
{
	public:
	VICITimingFixture (bool pal)
		: _memory (VIC20::Memory::Configuration::_NOEXPANDED), _cpu (0),
		  _sound (new VICITimingSound (pal)), _vic (pal, _sound),
		  _trace (_memory.physicalStorage (VIC20::Memory::_RAM), &_vic),
		  _videoTrace (_memory.physicalStorage (VIC20::Memory::_RAM), &_vic)
							{ }
	virtual ~VICITimingFixture () override;
	bool initialize (bool zero);
	bool step (unsigned int opcode, unsigned int address = 0, unsigned int accumulator = 0,
		unsigned int watched = 0x1000);
	void seed (unsigned int address, unsigned int value)
							{ _memory.physicalStorage (VIC20::Memory::_RAM) -> set (address, MCHEmul::UByte ((unsigned char) (value))); }

	VIC20::Memory _memory;
	F6500::C6502 _cpu;
	VICITimingSound* _sound;
	VICITimingProbe _vic;
	VICITimingRAM _trace;
	VICITimingRAM _videoTrace;

	private:
	virtual void processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier) override;
};

// ---
VICITimingFixture::~VICITimingFixture ()
{
	_cpu.executeBufferedCommands (true);
	_memory.view (VIC20::Memory::_CPU_VIEW) -> removeSubSet (1);
	_memory.view (VIC20::Memory::_VICI_VIEW) -> removeSubSet (1);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
}

// ---
bool VICITimingFixture::initialize (bool zero)
{
	if (!_memory.initialize ()) return (false);
	_memory.view (VIC20::Memory::_CPU_VIEW) -> addSubset (&_trace);
	_memory.view (VIC20::Memory::_VICI_VIEW) -> addSubset (&_videoTrace);
	_cpu.setMemoryRef (&_memory);
	if (!_cpu.initialize () || (zero && !_cpu.MCHEmul::CPU::initialize ())) return (false);
	_cpu.statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, true);
	_vic.setMemoryRef (&_memory);
	if (!_vic.initialize () || !_vic.soundFunction () -> initialize ()) return (false);
	_sound -> link (&_vic);
	MCHEmul::PhysicalStorageSubsets selected = {
		{ 1, &_trace },
		{ COMMODORE::VICIRegisters::_VICREGS_SUBSET, _memory.subset (COMMODORE::VICIRegisters::_VICREGS_SUBSET) },
		{ VIC20::Memory::_STACK_SUBSET, _memory.subset (VIC20::Memory::_STACK_SUBSET) },
		{ VIC20::Memory::_SCREENC1RAM_SUBSET, _memory.subset (VIC20::Memory::_SCREENC1RAM_SUBSET) },
		{ VIC20::Memory::_SCREENC2RAM_SUBSET, _memory.subset (VIC20::Memory::_SCREENC2RAM_SUBSET) } };
	if (!_vic.enableCPUTimedWrites (&_cpu, selected)) return (false);
	observe (&_cpu);
	return (true);
}

// ---
bool VICITimingFixture::step
	(unsigned int opcode, unsigned int address, unsigned int accumulator, unsigned int watched)
{
	seed (0x0200, opcode); seed (0x0201, address & 0xff); seed (0x0202, address >> 8);
	_cpu.programCounter ().setAddress (MCHEmul::Address (2, 0x0200));
	_cpu.internalRegister (F6500::C6500::_ACCUMULATOR).set
		(std::vector <MCHEmul::UByte> { MCHEmul::UByte ((unsigned char) (accumulator)) });
	_sound -> begin ();
	// Capture video reads both in CPUAboutToExecute and in the later simulate().
	_trace.begin (watched);
	_videoTrace.begin (watched);
	const bool executed = _cpu.executeNextCycle ();
	const bool result = executed && _vic.simulate (&_cpu);
	_trace.end ();
	_videoTrace.end ();
	if (!result || !_vic.soundFunction () -> simulate (&_cpu)) return (false);
	const unsigned int ticks = _sound -> ticks ();
	if (!_vic.soundFunction () -> simulate (&_cpu) || _sound -> ticks () != ticks) return (false);
	_sound -> end ();
	return (true);
}

// ---
void VICITimingFixture::processEvent (const MCHEmul::Event& event, MCHEmul::Notifier* notifier)
{
	if (notifier != &_cpu) return;
	if (event.id () == MCHEmul::CPU::_CPUTOEXECUTEINSTRUCTION)
		_vic.CPUAboutToExecute (static_cast <const MCHEmul::InstructionContextEventData*> (event.data ().get ()));
	else if (event.id () == MCHEmul::CPU::_CPUTOEXECUTEINTERRUPT)
		_vic.CPUAboutToExecute (static_cast <const MCHEmul::InterruptContextEventData*> (event.data ().get ()));
}

// ---
bool VICITimedWritesRegression::run ()
{
	MCHEmul::PhysicalStorage storage (0, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	MCHEmul::PhysicalStorageSubset a (1, &storage, 0, MCHEmul::Address (2, 0), 4);
	MCHEmul::PhysicalStorageSubset b (2, &storage, 4, MCHEmul::Address (2, 4), 4);
	MCHEmul::PhysicalStorageSubset foreign (3, &storage, 8, MCHEmul::Address (2, 8), 4);
	MCHEmul::Memory::Configuration& configuration = MCHEmul::Memory::configuration ();
	configuration.addMemorySetCommand (MCHEmul::SetMemoryCommand (&a, 0, MCHEmul::UByte (1)));
	configuration.addMemorySetCommand (MCHEmul::SetMemoryCommand (&foreign, 0, MCHEmul::UByte (7)));
	configuration.addMemorySetCommand (MCHEmul::SetMemoryCommand (&b, 0, MCHEmul::UByte (2)));
	configuration.addMemorySetCommand (MCHEmul::SetMemoryCommand (&a, 0, MCHEmul::UByte (3)));
	configuration.addMemorySetCommand (MCHEmul::SetMemoryCommand (&foreign, 0, MCHEmul::UByte (8)));
	MCHEmul::PhysicalStorageSubsets selected = { { 1, &a }, { 2, &b } };
	MCHEmul::SetMemoryCommands extracted;
	extracted.reserve (8);
	const size_t capacity = extracted.capacity ();
	configuration.extractMemorySetCommandsBuffered (selected, extracted);
	if (extracted.size () != 3 || extracted.capacity () != capacity ||
		extracted [0].value ().value () != 1 || extracted [1].value ().value () != 2 ||
		extracted [2].value ().value () != 3) return (false);
	extracted = configuration.extractMemorySetCommandsBuffered (&foreign);
	if (extracted.size () != 2 || extracted [0].value ().value () != 7 ||
		extracted [1].value ().value () != 8) return (false);
	std::cout << "Multi-subset extraction preserves selected and excluded command order\n";

	for (bool pal : { true, false })
		for (bool zero : { true, false })
		{
			VICITimingFixture f (pal);
			if (!f.initialize (zero)) { std::cerr << "Timed fixture initialization failed\n"; return (false); }
			unsigned int start = f._cpu.clockCycles ();
			if (!f.step (0x8d, 0x900f, 0x25) || f._vic.register15 () != 0x25 || f._videoTrace.samples ().size () != 4)
				{ std::cerr << "First STA register scheduling failed\n"; return (false); }
			for (const auto& sample : f._videoTrace.samples ())
				if (sample._register15 != 0x1b) return (false);
			if (f._vic.cycle () != start + 4 || f._sound -> ticks () != 4) return (false);
			f._vic.setup (0x0a, MCHEmul::UByte (0x80));
			start = f._cpu.clockCycles ();
			const unsigned int oldTicks = f._sound -> ticks ();
			if (!f.step (0xee, 0x900a) || f._sound -> writes ().size () != 2 || f._sound -> states ().size () != 6)
				{ std::cerr << "RMW sound scheduling failed\n"; return (false); }
			for (unsigned int n = 0; n < 2; ++n)
				if (f._sound -> writes () [n]._cycle != start + 4 + n ||
					f._sound -> writes () [n]._ticks != oldTicks + 5 + n ||
					f._sound -> writes () [n]._value != 0x80 + n) return (false);
			for (unsigned int value : f._sound -> states ()) if (value != 0x80) return (false);
			if (!f.step (0xea) || f._sound -> states ().size () != 2 || f._sound -> states () [0] != 0x81) return (false);

			f.seed (0x1000, 0x35); start = f._cpu.clockCycles ();
			if (!f.step (0x8d, 0x1000, 0x5a) || f._trace.writes ().size () != 1 ||
				f._trace.writes () [0]._cycle != start + 3 || f._trace.writes () [0]._value != 0x5a) return (false);
			for (const auto& sample : f._videoTrace.samples ()) if (sample._value != 0x35) return (false);
			start = f._cpu.clockCycles ();
			if (!f.step (0xee, 0x1000) || f._trace.writes ().size () != 2) return (false);
			for (unsigned int n = 0; n < 2; ++n)
				if (f._trace.writes () [n]._cycle != start + 4 + n || f._trace.writes () [n]._value != 0x5a + n) return (false);
			f._cpu.internalRegister (F6500::C6500::_XREGISTER).set (std::vector <MCHEmul::UByte> { MCHEmul::UByte (1) });
			start = f._cpu.clockCycles ();
			if (!f.step (0x9d, 0x10ff, 0x67, 0x1100) || f._trace.writes ().size () != 1 ||
				f._trace.writes () [0]._cycle != start + 4 || f._trace.writes () [0]._address != 0x1100) return (false);
			// An indexed read with a page crossing must select the five-cycle context.
			start = f._cpu.clockCycles ();
			if (!f.step (0xbd, 0x10ff) || f._cpu.clockCycles () != start + 5) return (false);
			f.seed (0x9400, 0x03);
			if (!f.step (0x8d, 0x9400, 0xab)) return (false);
			for (const auto& sample : f._videoTrace.samples ()) if (sample._color != 3) return (false);
			if (f._memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400).value () != 11) return (false);
			if (!f.step (0x8d, 0x9123, 0x55)) return (false);
			MCHEmul::SetMemoryCommands foreign = MCHEmul::Memory::configuration ().extractMemorySetCommandsBuffered
				(f._memory.subset (VIC20::VIA2Registers::_VIA2_SUBSET));
			if (foreign.size () != 1 || foreign [0].value ().value () != 0x55) return (false);
			foreign [0].execute ();
			for (unsigned int interrupt : { F6500::NMIInterrupt::_ID, F6500::IRQInterrupt::_ID })
			{
				f._cpu.statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);
				if (!f.step (0xea)) return (false);
				f._memory.stack () -> reset ();
				for (unsigned int address : { 0x01ff, 0x01fe, 0x01fd }) f.seed (address, 0xcc);
				f._cpu.requestInterrupt (interrupt, f._cpu.clockCycles () - 2, nullptr, 0);
				start = f._cpu.clockCycles ();
				if (!f.step (0xea) || f._cpu.clockCycles () != start + 7 || f._videoTrace.samples ().size () != 7)
					{ std::cerr << "Interrupt transaction failed\n"; return (false); }
				for (unsigned int cycle = 0; cycle < 7; ++cycle)
					for (unsigned int cell = 0; cell < 3; ++cell)
						if ((cycle <= 2 + cell) != (f._videoTrace.samples () [cycle]._stack [cell] == 0xcc)) return (false);
			}
			std::cout << (pal ? "PAL" : "NTSC") << (zero ? " zero" : " six")
				<< " origin: timed STA/RMW/RAM/color/indexed/IRQ/NMI/audio passed\n" << std::flush;
		}
	return (true);
}
