#include <GENERALINSTRUMENTS/AY38910/AY38910LibWrapper.hpp>

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910LibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 3; i++)
		vDt.add (std::to_string (i), std::move (getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910SimpleLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result =
		std::move (GENERALINSTRUMENTS::AY38910LibWrapper::getInfoStructure ());

	std::vector <MCHEmul::UByte> cR
		(_registers.begin (), _registers.begin () + 0x10);
	MCHEmul::InfoStructure cDt;
	for (size_t i = 0; i < 3; i++)
	{
		MCHEmul::InfoStructure c;
		c.add ("ID",				(int) i);
		c.add ("USEENVELOPE",		_useEnvelope [i]);
		c.add ("TONEDISABLED",		_toneDisabled [i]);
		c.add ("NOISEDISABLED",		_noiseDisabled [i]);
		c.add ("VOLUME",			_volumen [i]);
		cDt.add (std::to_string (i), std::move (c));
	}

	result.add ("CHIPFREQUENCY",		_chipFrequency);
	result.add ("SAMPLINGFREQUENCY",	_samplingFrequency);
	result.add ("CONTROLREGISTERS",		std::move (cR));
	result.add ("CHANNELS",				std::move (cDt));
	result.add ("AY38910Envelope",		std::move (_envelope.getInfoStructure ()));

	return (result);
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::AY38910SimpleLibWrapper (unsigned int cF, unsigned int sF)
	: GENERALINSTRUMENTS::AY38910LibWrapper (
		{
			{ "Name", "SimpleAY38910" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2025" }
		}),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  // There are only 3 voices available in the AY38910,
	  // but the 4th is just used to be mixed with the rest...
	  _voices (
		{ new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (0, cF), 
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (1, cF), 
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (2, cF),
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (3, cF), }),
	  _envelope (cF),
	  _useEnvelope { false, false, false },
	  _toneDisabled { false, false, false },
	  _noiseDisabled { false, false, false },
	  _volumen { 0.0f, 0.0f, 0.0f },
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _cyclesPerSample ((double) cF / (double (sF))), // It doesn't change ever...
	  _counterCyclesPerSample (0.0f)
{ 
	// In each of the different voices the selected wave is the first one,
	// ...except the 4th voice that the noise is selected by default.
	// That is because the 4th voice is used to mix the rest of the voices,
	// The waves are inactive by default...
	_voices [0] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [1] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [2] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	// Voices are not active by default, but noise is always...
	auto* noiseVoice =
	    static_cast <GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice*> (_voices [3]);
	noiseVoice -> setWavesActive (1);
	noiseVoice -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (true);
	noiseVoice -> setActive (true);
	// The envelope is always active. It control depends on other internal variable (_useEnvelope)
	_envelope.setActive (true);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10; // Max 16...

	_registers [pp] = v;

	switch (pp)
	{
		// Fine & Coarse Tune Register Voice 0
		case 0x00:
		case 0x01:
			{
				_voices [0] -> setFrequency (toneFrequency (0x00, 0x01));
			}

			break;

		// Fine & Coarse Tune Register Voice 1
		case 0x02:
		case 0x03:
			{
				_voices [1] -> setFrequency (toneFrequency (0x02, 0x03));
			}

			break;

		// Fine & Coarse Tune Register Voice 2
		case 0x04:
		case 0x05:
			{
				_voices [2] -> setFrequency (toneFrequency (0x04, 0x05));
			}

			break;

		// Noise Generator Control Register
		case 0x06:
			{
				_voices [3] -> setFrequency (noiseFrequency ());
			}

			break;

		// Mixer Control I/O Enable Register
		case 0x07:
			{
				// Mixer register. Bits are inverted: 0 enables tone/noise output.
				_toneDisabled  [0] = v.bit (0);
				_toneDisabled  [1] = v.bit (1);
				_toneDisabled  [2] = v.bit (2);
				_noiseDisabled [0] = v.bit (3);
				_noiseDisabled [1] = v.bit (4);
				_noiseDisabled [2] = v.bit (5);
				// Bits 6 and 7 configure the generic I/O ports and have no audible effect.
			}

			break;

		// Amplitude Control Register Voice 0
		// In the Yamaha documentation this register is called R10 instead of R08
		case 0x08:
			{
				// When the bit 4 is false, the "manual" volumen will be taken into account...
				// ...otherwise the final volumen will rely in the envelope definition, 
				// so here is 1.0 (to do not affect!)...
				_volumen [0] = !v.bit (4) ? ((double) (v.value () & 0x0f) / 15.0f) : 1.0f;
				// Bit 4 do activate or desactivate the envelope really...
				_useEnvelope [0] = v.bit (4);
			}

			break;

		// Amplitude Control Register Voice 1
		// In the Yamaha documentation this register is called R11 instead of R09
		case 0x09:
		{
				_volumen [1] = !v.bit (4) ? ((double) (v.value () & 0x0f) / 15.0f) : 1.0f;
				_useEnvelope [1] = v.bit (4);
		}

		break;

		// Amplitude Control Register Voice 2
		// In the Yamaha documentation this register is called R12 instead of R10
		case 0x0a:
		{
				_volumen [2] = !v.bit (4) ? ((double) (v.value () & 0x0f) / 15.0f) : 1.0f;
				_useEnvelope [2] = v.bit (4);
		}

		break;

		// Envelope Period Control Register 1 & 2
		// In the Yamaha documentation this register is called R13/R14 instead of R11/R12
		case 0x0b:
		case 0x0c:
			{
				_envelope.setFrequency (envelopeFrequency ());
			}

			break;

		// Envelope Shape/Cycle Control Register
		// In the Yamaha documentation this register is called R15 instead of R13
		case 0x0d:
			{
				_envelope.setType (v);
			}

			break;

		// TODO The rest...

		default:
			break;
	}
}

// ---
const MCHEmul::UByte& GENERALINSTRUMENTS::AY38910SimpleLibWrapper::readValue (size_t p) const
{ 
	size_t pp = p % 0x10;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
			{
				result = _registers [pp];
			}

			break;

		// The rest of the registers are write only,
		// ...so reading it gets back 0!
		default:
			result = MCHEmul::UByte::_0;
			break;
	}

	return (_lastValueRead = result);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::initialize ()
{ 
	AY38910LibWrapper::initialize ();

	_counterCyclesPerSample = 0.0f;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// Activate the right wave per voice...
	_voices [0] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [1] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [2] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	auto* noiseVoice =
	    static_cast <GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice*> (_voices [3]);
	noiseVoice -> setWavesActive (1);
	noiseVoice -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (true);
	noiseVoice -> setActive (true);

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
	for (size_t i = 0; i < 3; i++) 
	{
		_useEnvelope [i] = false;
		_toneDisabled [i] = false;
		_noiseDisabled [i] = false;
		_volumen [i] = 0.0f;
	}

	_envelope.initialize ();
	_envelope.setFrequency (envelopeFrequency ());
}

// ---
bool GENERALINSTRUMENTS::AY38910SimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	for (auto i : _voices)
		i -> clock (); // just one...
	_envelope.clock (); // just one...

	_counterCyclesPerSample += 1.0;
	if ((result = 
			(_cyclesPerSample > 0.0f &&
			 _counterCyclesPerSample >= _cyclesPerSample)))
	{
		_counterCyclesPerSample = 
			std::fmod (_counterCyclesPerSample, _cyclesPerSample);

		// The AY mixer combines the tone and noise generators as digital gates.
		// Convert their bipolar audio outputs back into logical levels first.
		const bool noiseHigh = (_voices [3] -> data () >= 0.0f);

		double sample = 0.0f;
		size_t activeChannels = 0;
		for (size_t i = 0; i < 3; i++)
		{
			// With both generators disabled there is no varying audio signal.
			if (_toneDisabled [i] && _noiseDisabled [i])
				continue;

			const bool toneHigh =
				_toneDisabled [i] ||
				(_voices [i] -> wave (
					MCHEmul::SoundWave::Type::_SQUARE) -> data () >= 0.0f);
			const bool channelHigh =
				toneHigh &&
				(_noiseDisabled [i] || noiseHigh);
			const double channelSample = channelHigh ? 1.0f : -1.0f;
			const double gain =
				_volumen [i] *
				(_useEnvelope [i] ? _envelope.envelopeData () : 1.0f);

			sample += channelSample * gain;
			activeChannels++;
		}

		// Normalize the sample to the range [-1.0, 1.0] based on the number of active channels.
		if (activeChannels > 0)
			sample /= activeChannels;

		dt = MCHEmul::UBytes ({
			MCHEmul::normalizedSoundSampleToU8 (sample)	});
	}

	return (result);
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Envelope (unsigned int cF)
	: MCHEmul::SoundEnvelope (cF),
	  _type (GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type::_TYPE0),
	  _frequency (0.0f),
	  _continue (false),
	  _attack (false),
	  _alternate (false),
	  _hold (false),
	  _level (15),
	  _direction (-1),
	  _holding (false),
	  _cyclesPerEnvelopeStep (0),
	  _counterCyclesPerEnvelopeStep (0)
{
	setClassName ("AY38910Envelope"); 

	decodeType ();
	restart ();
	calculateSamplingData ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::setType (
	GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type t)
{
	_type = t;

	decodeType ();
	restart ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::setFrequency (double f)
{
	_frequency = (f > 0.0f) ? f : 0.0f;

	calculateSamplingData ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::setStart (bool s)
{
	if (s)
		restart ();
	else
		_holding = true;
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::initialize ()
{
	_type = GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type::_TYPE0;
	_frequency = 0.0f;

	decodeType ();
	calculateSamplingData ();
	restart ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::clock (unsigned int nC)
{
	if (nC == 0 || !_active || _holding || _cyclesPerEnvelopeStep == 0)
		return;

	_counterCyclesPerEnvelopeStep += nC;
	while (_counterCyclesPerEnvelopeStep >= _cyclesPerEnvelopeStep && !_holding)
	{
		_counterCyclesPerEnvelopeStep -= _cyclesPerEnvelopeStep;

		advanceLevel ();
	}
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundEnvelope::getInfoStructure ();

	result.add ("TYPE", (int) _type);
	result.add ("CONTINUE", _continue);
	result.add ("ATTACK", _attack);
	result.add ("ALTERNATE", _alternate);
	result.add ("HOLD", _hold);
	result.add ("LEVEL", (int) _level);
	result.add ("DIRECTION", _direction);
	result.add ("HOLDING", _holding);
	result.add ("FREQUENCY", _frequency);
	result.add ("CYCLESPERSTEP", _cyclesPerEnvelopeStep);

	return (result);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::calculateSamplingData ()
{
	_cyclesPerEnvelopeStep = (_frequency == 0.0f)
		? 0
		: (unsigned int) (((double) _chipFrequency / _frequency) + 0.5f);

	if (_cyclesPerEnvelopeStep == 0 ||
		_counterCyclesPerEnvelopeStep >= _cyclesPerEnvelopeStep)
		initializeInternalCounters ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::decodeType ()
{
	unsigned char t = (unsigned char) _type;

	_continue = ((t & 0x08) != 0);
	_attack = ((t & 0x04) != 0);
	_alternate = ((t & 0x02) != 0);
	_hold = ((t & 0x01) != 0);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::restart ()
{
	_direction = _attack ? 1 : -1;
	_level = _attack ? 0 : 15;
	_holding = false;

	initializeInternalCounters ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::advanceLevel ()
{
	if ((_direction < 0 && _level > 0) ||
		(_direction > 0 && _level < 15))
	{
		_level = (unsigned char) ((int) _level + _direction);

		return;
	}

	// If Continue is not set, the AY forces the envelope output to 0
	// after the first ramp, whatever Attack/Alternate/Hold were.
	if (!_continue)
	{
		_level = 0;
		_holding = true;

		return;
	}

	if (_hold)
	{
		if (_alternate)
			_level = (_direction < 0) ? 15 : 0;

		_holding = true;

		return;
	}

	if (_alternate)
		_direction = -_direction;
	else
		_level = _attack ? 0 : 15;
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::SquareSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF) // This is just like a fourth wave to be mixed with the rest...
		}, nullptr, { }), // The voices has no "individual" envelope neither filters, 
						  // but one that is common to all voices..no filter!
	  _wavesActive (0)
{ 
	setClassName ("AY38910Voice"); 
}

// ---
double GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::data () const
{ 
	if (!active ())
		return (0.0f);

	double result = 0.0f;

	switch (_wavesActive)
	{
		case 0:
			result = wave (MCHEmul::SoundWave::Type::_SQUARE) -> data ();
			break;

		case 1:
			result = wave (MCHEmul::SoundWave::Type::_NOISE) -> data ();
			break;

		default:
			break;
	}

	if (result < -1.0f)	result = -1.0f;
	else if (result > 1.0f)	result = 1.0f;
	return (result);
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::SoundVoice::getInfoStructure ());
		
	result.add ("WAVEACTIVE",		(int) _wavesActive);
	result.add ("WAVESCLOCKVALUE",	(int) wavesClockValue ());
	result.add ("OSCILLATORVALUE",	(int) oscillatorValue ());
		
	return (result);
}
