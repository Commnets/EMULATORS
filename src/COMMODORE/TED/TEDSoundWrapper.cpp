#include <COMMODORE/TED/TEDSoundWrapper.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::TEDSoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 2; i++)
		vDt.add (std::to_string (i), std::move (getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}

// ---
COMMODORE::TEDPulseSoundWave::TEDPulseSoundWave (unsigned int cF)
	: MCHEmul::PulseSoundWave (cF),
	  _inputClockFrequency ((double) cF),
	  _frequencyRegister (0),
	  _reloadValue (1023),
	  _counter (1023),
	  _halfLength (1023),
	  _prescaler (0),
	  _output (false),
	  _previousOverflow (false),
	  _dividerEdge (false),
	  _dcActive (false)
{
	setClassName ("TEDPulseSoundWave");

	MCHEmul::PulseSoundWave::setPulseUpPercentage (0.5);
	setFrequencyRegister (0);
}

// ---
void COMMODORE::TEDPulseSoundWave::setClockReference
	(unsigned int masterFrequency, unsigned int divider)
{
	_chipFrequency = masterFrequency / divider;
	_inputClockFrequency = (double) masterFrequency / (double) divider;

	updateFrequencyInfo ();
}

// ---
void COMMODORE::TEDPulseSoundWave::setFrequencyRegister (unsigned short value)
{
	_frequencyRegister = value & 0x03ff;
	_reloadValue = (_frequencyRegister == 0x03ff) ? 1024 : 1023 - _frequencyRegister;

	updateFrequencyInfo ();
}

// ---
void COMMODORE::TEDPulseSoundWave::initialize ()
{
	MCHEmul::PulseSoundWave::initialize ();

	_dcActive = false;
	setFrequencyRegister (0);
	initializeInternalCounters ();
}

// ---
void COMMODORE::TEDPulseSoundWave::initializeInternalCounters ()
{
	// Deterministic emulator initialization, not a measured power-on state.
	_counter = _halfLength = _reloadValue;
	_prescaler = 0;
	_output = _dcActive;
	_previousOverflow = _dcActive && (_reloadValue == 1);
	_dividerEdge = false;
	_clockRestarted = false;
}

// ---
void COMMODORE::TEDPulseSoundWave::clock (unsigned int nC)
{
	_dividerEdge = false;

	for (unsigned int i = 0; i < nC; i++)
	{
		// One divider tick per four incoming single-clock cycles, even when output is muted.
		_prescaler = (_prescaler + 1) & 0x03;
		if (_prescaler != 0)
			continue;

		if (_dcActive)
		{
			_counter = _halfLength = _reloadValue;
			_output = true;
			_previousOverflow = (_reloadValue == 1);

			continue;
		}

		const bool overflow = (--_counter == 0);
		const bool edge = overflow && !_previousOverflow;

		if (overflow)
			_counter = _halfLength = _reloadValue;

		_previousOverflow = overflow;

		// A continuously asserted overflow is not a stream of edges (notably reload=1).
		// Analog decay of the retained output in that case is not modeled here.
		if (edge)
		{
			_output = !_output;
			_dividerEdge = true;

			if (_output)
				_clockRestarted = true;
		}
	}
}

// ---
double COMMODORE::TEDPulseSoundWave::clockValue () const
{
	if (_dcActive)
		return (0.0);

	// The active half-length is retained across frequency writes; no generic phase runs alongside it.
	return ((_output ? 0.0 : 0.5) +
		0.5 * ((double) (_halfLength - _counter) + (double) _prescaler / 4.0) / (double) _halfLength);
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDPulseSoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::PulseSoundWave::getInfoStructure ();

	result.add ("REGISTER", _frequencyRegister);
	result.add ("RELOAD", _reloadValue);
	result.add ("COUNTER", _counter);
	result.add ("HALFLENGTH", _halfLength);
	result.add ("PRESCALER", _prescaler);
	result.add ("LEVEL", _output);
	result.add ("DC", _dcActive);
	result.add ("OVERFLOW", _previousOverflow);

	return (result);
}

// ---
void COMMODORE::TEDPulseSoundWave::updateFrequencyInfo ()
{
	// Inspection metadata changes without touching the active divider or output phase.
	_frequency = (_frequencyRegister == 0x03fe)
		? 0.0 : _inputClockFrequency / (8.0 * (double) _reloadValue);
	_cyclesPerWave = (_frequency > 0.0) ? 8.0 * (double) _reloadValue : 0.0;
}

// ---
COMMODORE::TEDNoiseSoundWave::TEDNoiseSoundWave (unsigned int cF)
	: MCHEmul::SoundWave (MCHEmul::SoundWave::Type::_NOISE, cF),
	  _shiftRegister (0xff)
{
	setClassName ("TEDNoiseSoundWave");
}

// ---
void COMMODORE::TEDNoiseSoundWave::initialize ()
{
	MCHEmul::SoundWave::initialize ();

	initializeInternalCounters ();
}

// ---
void COMMODORE::TEDNoiseSoundWave::initializeInternalCounters ()
{
	_shiftRegister = 0xff;
	_clockRestarted = false;
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDNoiseSoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundWave::getInfoStructure ();

	result.add ("LFSR", _shiftRegister);
	result.add ("LEVEL", output ());
	result.add ("CLOCKSOURCE", std::string ("Channel2Divider"));

	return (result);
}

// ---
COMMODORE::TEDSoundSimpleLibWrapper::TEDSoundSimpleLibWrapper 
		(unsigned int tF, unsigned int dv, unsigned int sF)
	: TEDSoundLibWrapper (
		{
			{ "Name", "TEDSound" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2023" }
		}),
	  _tedFrequency (tF), 
	  _initialDivider (dv),
	  _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _voices ( // Two voices...
		{ new COMMODORE::TEDSoundSimpleLibWrapper::Voice (0, tF / dv), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (1, tF / dv) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _dividerValue (dv),
	  _samplePhase (0)
{
	setClockDivider (dv);
}

// ---
COMMODORE::TEDSoundSimpleLibWrapper::~TEDSoundSimpleLibWrapper ()
{
	// SoundVoice owns its waves; delete the concrete voice because its base destructor is not virtual.
	for (auto voice : _voices)
		delete (static_cast <Voice*> (voice));
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	const size_t pp = p % 0x20;

	switch (pp)
	{
		case 0x0e:
		case 0x12:
			{
				_registers [pp] = (pp == 0x12) ? MCHEmul::UByte (v.value () & 0x03) : v;
				static_cast <Voice*> (_voices [0]) ->
					setFrequencyRegister ((unsigned short) voice0Register ());
			}

			break;

		case 0x0f:
		case 0x10:
			{
				_registers [pp] = (pp == 0x10) ? MCHEmul::UByte (v.value () & 0x03) : v;
				static_cast <Voice*> (_voices [1]) ->
					setFrequencyRegister ((unsigned short) voice1Register ());
			}

			break;

		case 0x11:
			{
				_registers [pp] = v;
				updateControl ();
			}

			break;

		default:
			break;
	}
}

// ---
const MCHEmul::UByte& COMMODORE::TEDSoundSimpleLibWrapper::readValue (size_t p) const
{ 
	size_t pp = p % 0x20;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		case 0x10:
			{
				/** Bits 2 to 7 are not used and always read as 1. */
				result = MCHEmul::UByte ((_registers [0x10].value () & 0x03) | 0xfc);
			}

			break;

		case 0x12:
			{
				result = MCHEmul::UByte (_registers [0x12].value () & 0x03);
			}

			break;

		case 0x0e:
		case 0x0f:
		case 0x11:
			{
				result = _registers [pp];
			}

			break;

		// The rest are not taken into account...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::initialize ()
{
	TEDSoundLibWrapper::initialize ();

	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0);
	_volumen = 0.0;
	_samplePhase = 0;

	for (auto voice : _voices)
		voice -> initialize ();

	setClockDivider (_initialDivider);
	updateControl ();
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::setClockDivider
	(unsigned int divider)
{
	_dividerValue = divider;

	for (auto voice : _voices)
		static_cast <Voice*> (voice) -> setClockReference (_tedFrequency, divider);
}

// ---
bool COMMODORE::TEDSoundSimpleLibWrapper::getData
	(MCHEmul::CPU* cpu, MCHEmul::UBytes& dt)
{
	return (getDataForTEDCycle (cpu, _dividerValue, dt));
}

// ---
bool COMMODORE::TEDSoundSimpleLibWrapper::getDataForTEDCycle
	(MCHEmul::CPU* cpu, unsigned int masterCycles, MCHEmul::UBytes& dt)
{
	for (auto voice : _voices)
		voice -> clock ();

	// Preserve fractional sample time across divider changes and mixed-duration pulses.
	_samplePhase += (unsigned long long) _samplingFrequency * masterCycles;
	if (_samplePhase < _tedFrequency)
		return (false);

	const size_t samples = (size_t) (_samplePhase / _tedFrequency);
	_samplePhase %= _tedFrequency;

	// Fixed per-channel gain preserves channel 1 amplitude when channel 2 is silent.
	double sample = 0.0;
	for (auto voice : _voices)
		sample += voice -> data ();
	sample *= 0.5 * _volumen;

	dt = MCHEmul::UBytes (std::vector <MCHEmul::UByte>
		(samples, MCHEmul::normalizedSoundSampleToU8 (sample)));

	return (true);
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::updateControl ()
{
	const MCHEmul::UByte control = _registers [0x11];
	const unsigned char volume = control.value () & 0x0f;

	setVolumen ((double) ((volume > 8) ? 8 : volume) / 8.0);
	static_cast <Voice*> (_voices [0]) -> configure (control.bit (4), false, control.bit (7));
	static_cast <Voice*> (_voices [1]) -> configure (control.bit (5), control.bit (6), control.bit (7));
}

// ---
COMMODORE::TEDSoundSimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice
		(id, cF,
		 (id == 1)
			? MCHEmul::SoundWaves {
				new COMMODORE::TEDPulseSoundWave (cF),
				new COMMODORE::TEDNoiseSoundWave (cF) }
			: MCHEmul::SoundWaves {
				new COMMODORE::TEDPulseSoundWave (cF) },
		 nullptr, { }),
	  _pulse (static_cast <COMMODORE::TEDPulseSoundWave*> (wave (MCHEmul::SoundWave::Type::_PULSE))),
	  _noise ((id == 1)
		? static_cast <COMMODORE::TEDNoiseSoundWave*> (wave (MCHEmul::SoundWave::Type::_NOISE)) : nullptr),
	  _noiseSelected (false)
{
	setClassName ("TEDVoice");
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::Voice::setClockReference
	(unsigned int masterFrequency, unsigned int divider)
{
	_chipFrequency = masterFrequency / divider;
	_pulse -> setClockReference (masterFrequency, divider);

	if (_noise != nullptr)
		_noise -> setClockReference (masterFrequency, divider);
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::Voice::configure
	(bool toneEnabled, bool noiseEnabled, bool dcActive)
{
	// Tone 2 takes priority over noise; the output gate does not stop either divider.
	_noiseSelected = !toneEnabled && noiseEnabled && (_noise != nullptr);
	_active = toneEnabled || _noiseSelected;
	_pulse -> setActive (toneEnabled);
	_pulse -> setDCActive (dcActive);
	if (_noise != nullptr)
		_noise -> setActive (_noiseSelected);
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::Voice::initialize ()
{
	MCHEmul::SoundVoice::initialize ();

	_noiseSelected = false;
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::Voice::clock (unsigned int nC)
{
	// Process each input cycle so every divider edge clocks noise exactly once.
	// Do not also call SoundVoice::clock(), which would clock the same pulse a second time.
	for (unsigned int i = 0; i < nC; i++)
	{
		if (_noise != nullptr && _pulse -> dcActive ())
			_noise -> shiftOne ();

		_pulse -> clock (1);

		if (_noise != nullptr && _pulse -> dividerEdge ())
			_noise -> shift ();
	}
}

// ---
double COMMODORE::TEDSoundSimpleLibWrapper::Voice::data () const
{
	if (!_active)
		return (0.0);

	if (_noiseSelected)
		return (_pulse -> dcActive () ? 0.0 : (_noise -> output () ? 1.0 : 0.0));

	return (_pulse -> pulseUp () ? 1.0 : 0.0);
}
