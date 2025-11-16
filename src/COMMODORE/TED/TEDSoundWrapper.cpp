#include <COMMODORE/TED/TEDSoundWrapper.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::TEDSoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 3; i++)
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
	  _voices ( // Two voices...
		{ new COMMODORE::TEDSoundSimpleLibWrapper::Voice (0, tF / dv), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (1, tF / dv) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) tF / (double (sF) * double (dv)))),
	  _counterClocksPerSample (0)
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
		// Voice 0 LSB
		case 0x0e:
			{
				_voices [0] -> setFrequency ((unsigned short) 
					(((float) _tedFrequency / (float) (_dividerValue << 3)) /
					 (float) (1024 - ((unsigned short)
						 ((_registers [0x10].value () & 0x03) << 8) | (unsigned short) v.value ()))));
			}

			break;

		// Voice 1 LSB
		case 0x0f:
			{
				_voices [1] -> setFrequency ((unsigned short) 
					(((float) _tedFrequency / (float) (_dividerValue << 3)) /
					 (float) (1024 - ((unsigned short)
						 ((_registers [0x12].value () & 0x03) << 8) | (unsigned short) v.value ()))));
			}

			break;

		// Voice 1 MSB (only two bits)
		case 0x10:
			{
				_voices [1] -> setFrequency ((unsigned short) 
					(((float) _tedFrequency / (float) (_dividerValue << 3)) /
					 (float) (1024 - ((unsigned short)
						 ((v.value () & 0x03) << 8) | (unsigned short) _registers [0x0f].value ()))));
			}

			break;

		// Sound Color Register
		case 0x11:
			{
				setVolumen ((float) (v.value () & 0x0f) / 15.0f);
				_voices [0] -> setActive (v.bit (4)); // Active/Desactive the voice 0
				_voices [1] -> setActive (v.bit (5)); // Active/Desactive the voice 1
				static_cast <MCHEmul::PulseSoundWave*> (_voices [1] -> wave 
					(v.bit (6) ? MCHEmul::SoundWave::Type::_PULSE : MCHEmul::SoundWave::Type::_NOISE)) ->
						setActive (true); // The one active will be either the pulse or the noise...
				static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]) -> 
					setWavesActive (v.bit (6) ? 1 : 0); // Either the pulse or the noise wave is selected...
			}

			break;

		// Voice 1 MSB (only two bits)
		case 0x12:
			{
				_voices [0] -> setFrequency ((unsigned short) 
					(((float) _tedFrequency / (float) (_dividerValue << 3)) /
					 (float) (1024 - ((unsigned short)
						 ((v.value () & 0x03) << 8) | (unsigned short) _registers [0x0e].value ()))));
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
		case 0x0e:
		case 0x0f:
		case 0x10:
		case 0x11:
		case 0x12:
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

	_counterClocksPerSample = 0;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x10, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::TEDSoundSimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	for (auto i : _voices)
		i -> clock (); // just one...

	if ((result = ((++_counterClocksPerSample) >= _clocksPerSample)))
	{
		if ((_counterClocksPerSample -= _clocksPerSample) >= _clocksPerSample)
			_counterClocksPerSample = 0; // Just in case _clocksPerSample == 0...

		double iR = 0;
		for (auto i : _voices)
			iR += i -> data (); // but the values are added...
		iR *= _volumen; // Adjust the volumen...
		
		// This number could be greater than 1!
		if (iR > 1.0f) iR = 1.0f; // ...so it is needed to correct.

		dt = MCHEmul::UBytes ({ (unsigned char) (iR * 255.0f /** between 0 and 255. */) });
	}

	return (result);
}

// ---
COMMODORE::TEDSoundSimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::PulseSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF)
		}, nullptr), // No envelope attached...
		_wavesActive (0)
{ 
	setClassName ("TEDVoice");
}

// ---
double COMMODORE::TEDSoundSimpleLibWrapper::Voice::data () const
{ 
	// When the wave is active or is in test active and the selected wave is a pulse...
	// ...the sound has to be produced
	if (!(active () || (!active () && _wavesActive == 0x40 /** pulse. */)))
		return (0.0f);

	double result = 0.0f;

	// The way the different waves is merged in the SID
	// is not the standard way of just adding the values...
	// ...because the way they are played are not the same...
	// ..and definetively is not adding data!
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

	return ((result > 1.0f) ? 1.0f : result);
}
