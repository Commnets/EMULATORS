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
COMMODORE::TEDSoundSimpleLibWrapper::TEDSoundSimpleLibWrapper 
		(unsigned int tF, unsigned int dv, unsigned int sF)
	: TEDSoundLibWrapper (
		{
			{ "Name", "TEDSound" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2023" }
		}),
	  _tedFrequency (tF), 
	  _dividerValue (dv), // To get the CPU clock frequency...
	  _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _soundReloadActive (false),
	  _voices ( // Two voices...
		{ new COMMODORE::TEDSoundSimpleLibWrapper::Voice (0, tF / dv), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (1, tF / dv) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _cyclesPerSample ((double) tF / (double (sF) * double (dv))), // It doesn't change ever...
	  _counterCyclesPerSample (0.0f)
{
	// The voice 0...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v0 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [0]);
	static_cast <MCHEmul::PulseSoundWave*> (v0 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> 
		setActive (true); // The pulse is the one active always...
	static_cast <MCHEmul::PulseSoundWave*> (v0 -> wave (MCHEmul::SoundWave::Type::_PULSE)) ->
		setPulseUpPercentage (0.5); // 50% by default...
	v0 -> setWavesActive (0); // Always a pulse wave...

	// The voice 1...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v1 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]);
	static_cast <MCHEmul::PulseSoundWave*> (v1 -> wave (MCHEmul::SoundWave::Type::_PULSE)) ->
		setPulseUpPercentage (0.5); // 50% by default...
	v1 -> setWavesActive (0); // By defect a pulse wave, but it can be moved to noise!
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x20;

	_registers [pp] = v;

	switch (pp)
	{
		// Voice 1 LSB
		case 0x0e:
			{
				_voices [0] -> setFrequency (tedToneFrequency (voice0Register ()));
			}

			break;

		// Voice 2 LSB
		case 0x0f:
			{
				_voices [1] -> setFrequency (tedToneFrequency (voice1Register ()));
			}

			break;

		// Voice 2 MSB (only two bits)
		case 0x10:
			{
				// Just 2 bits only...
				// The information is rewritten...
				_registers [0x10] = MCHEmul::UByte (v.value () & 0x03);

				_voices [1] -> setFrequency (tedToneFrequency (voice1Register ()));
			}

			break;

		// Sound "Color" Register
		case 0x11:
			{
				const unsigned char programmedVolume = v.value () & 0x0f;
				const unsigned char effectiveVolume =
					(programmedVolume > 8) ? 8 : programmedVolume; // It can be bigger than 8, but it is not effective...
				setVolumen ((double) effectiveVolume / (double) 8);
				_voices [0] -> setActive (v.bit (4));
				auto* voice1 =
					static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]);
				const bool voice1ToneEnabled = v.bit (5);
				const bool noiseEnabled = ((v.value () & 0x60) == 0x40);
				voice1 -> setActive (voice1ToneEnabled || noiseEnabled);
				voice1 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> 
					setActive (voice1ToneEnabled && !noiseEnabled);
				voice1 -> wave (MCHEmul::SoundWave::Type::_NOISE) ->
					setActive (noiseEnabled);
				voice1 -> setWavesActive (noiseEnabled ? 1 : 0);

				// The reload bit restarts the tone phases and the noise sequence.
				const bool soundReloadActive = v.bit (7);
				if (soundReloadActive && !_soundReloadActive)
					reloadSoundGenerators ();
				_soundReloadActive = soundReloadActive;
			}

			break;

		// Voice 1 MSB (only two bits)
		case 0x12:
			{
				// Just 2 bits only...
				// The information is rewritten...
				_registers [0x12] = MCHEmul::UByte (v.value () & 0x03);

				_voices [0] -> setFrequency (tedToneFrequency (voice0Register ()));
			}

			break;

		// The rest are nor taken into account...
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
							  
	_volumen = 0.0f;
	_soundReloadActive = false;

	_counterCyclesPerSample = 0.0f;

	// Initialize all internal voice state.
	for (auto i : _voices)
		i -> initialize ();

	// Actualize the right wave per voice....
	auto* v0 = static_cast <Voice*> (_voices [0]);
	v0 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (true);
	static_cast <MCHEmul::PulseSoundWave*> 
		(v0 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> setPulseUpPercentage (0.5f);
	v0 -> setWavesActive (0); // Pulse...
	auto* v1 = static_cast <Voice*> (_voices [1]);
	v1 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (true);
	v1 -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (false);
	static_cast <MCHEmul::PulseSoundWave*>
		(v1 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> setPulseUpPercentage (0.5f);
	v1 -> setWavesActive (0); // Pulse...

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::TEDSoundSimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	for (auto i : _voices)
		i -> clock (); // just one...

	_counterCyclesPerSample += 1.0;
	if ((result = 
			(_cyclesPerSample > 0.0f &&
			 _counterCyclesPerSample >= _cyclesPerSample)))
	{
		_counterCyclesPerSample = 
			std::fmod (_counterCyclesPerSample, _cyclesPerSample);

		// Active voices are averaged to keep the mix normalized
		// before applying the master volume.
		double sample = 0.0f;
		size_t activeVoices = 0;
		for (auto i : _voices)
		{
			if (i -> active ())
			{
				sample += i -> data ();
				activeVoices++;
			}
		}

		if (activeVoices > 0)
			sample /= (double) activeVoices; // Average the voices...
		sample *= _volumen;

		dt = MCHEmul::UBytes ({
			MCHEmul::normalizedSoundSampleToU8 (sample)	});
	}

	return (result);
}

// ---
COMMODORE::TEDSoundSimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::PulseSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF, 0xffffu)
		}, nullptr, { }), // No envelope, no filters attached...
		_wavesActive (0)
{ 
	setClassName ("TEDVoice");
}

// ---
double COMMODORE::TEDSoundSimpleLibWrapper::Voice::data () const
{ 
	// When the voice is not active, nothing is returned...
	if (!active ())
		return (0.0f);

	double result = 0.0f;

	// TED voice output can be either pulse or noise depending on _wavesActive.
	switch (_wavesActive)
	{
		// pulse
		case 0x00:
			result = wave (MCHEmul::SoundWave::Type::_PULSE) -> data ();
			break;

		// noise
		case 0x01:
			result = wave (MCHEmul::SoundWave::Type::_NOISE) -> data ();
			break;

		// This sitution is not possible but just in case!
		default:
			break;
	}

	// There is no affection by the envelope, because there is no envelope in this chip!

	if (result < -1.0f)	result = -1.0f;
	else if (result > 1.0f)	result = 1.0f;
	return (result);
}
