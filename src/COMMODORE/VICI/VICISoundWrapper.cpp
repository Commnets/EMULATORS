#include <COMMODORE/VICI/VICISoundWrapper.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::VICISoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 4; i++)
		vDt.add (std::to_string (i), std::move (getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}

// ---
COMMODORE::VICISoundSimpleLibWrapper::VICISoundSimpleLibWrapper (unsigned int cF, unsigned int sF)
	: VICISoundLibWrapper (
		{
			{ "Name", "VICSound" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2023" }
		}),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _voices (
		{ new COMMODORE::VICISoundSimpleLibWrapper::Voice (0, cF), 
		  new COMMODORE::VICISoundSimpleLibWrapper::Voice (1, cF), 
		  new COMMODORE::VICISoundSimpleLibWrapper::Voice (2, cF), 
		  new COMMODORE::VICISoundSimpleLibWrapper::Voice (3, cF) }),
	  _registers (std::vector <MCHEmul::UByte> (0x10, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) cF / (double (sF)))),
	  _counterClocksPerSample (0)
{
	// The voice 0...
	COMMODORE::VICISoundSimpleLibWrapper::Voice* v0 = 
		static_cast <COMMODORE::VICISoundSimpleLibWrapper::Voice*> (_voices [0]);
	v0 -> setWavesActive (0x40); // Always a pulse wave...
	v0 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (true);
	static_cast <MCHEmul::PulseSoundWave*> (v0 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> 
		setPulseUpPercentage (0.5f);

	// The voice 1...
	COMMODORE::VICISoundSimpleLibWrapper::Voice* v1 = 
		static_cast <COMMODORE::VICISoundSimpleLibWrapper::Voice*> (_voices [1]);
	v1 -> setWavesActive (0x40); // Always a pulse wave...
	v1 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (true);
	static_cast <MCHEmul::PulseSoundWave*> (v1 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> 
		setPulseUpPercentage (0.5f);

	// The voice 2...
	COMMODORE::VICISoundSimpleLibWrapper::Voice* v2 = 
		static_cast <COMMODORE::VICISoundSimpleLibWrapper::Voice*> (_voices [2]);
	v2 -> setWavesActive (0x40); // Always a pulse wave...
	v2 -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (true);
	static_cast <MCHEmul::PulseSoundWave*> (v2 -> wave (MCHEmul::SoundWave::Type::_PULSE)) -> 
		setPulseUpPercentage (0.5f);

	// The voice 3...
	COMMODORE::VICISoundSimpleLibWrapper::Voice* v3 = 
		static_cast <COMMODORE::VICISoundSimpleLibWrapper::Voice*> (_voices [3]);
	v3 -> setWavesActive (0x80); // Always a noise wave...
	v3 -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (true);
}

// ---
void COMMODORE::VICISoundSimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	_registers [pp] = v;

	switch (pp)
	{
		// Voice 0
		case 0x0a:
			{
				_voices [0] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [0] -> setFrequency ((unsigned short) (((double) _chipFrequency / 256.0f) / 
					(double) (0x80 - (v.value () & 0x7f)))); // Bass
			}

			break;

		// Voice 1
		case 0x0b:
			{
				_voices [1] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [1] -> setFrequency ((unsigned short) (((double) _chipFrequency / 256.0f) / 
					(double) (0x80 - (v.value () & 0x7f))) << 1); // Alto

			}

			break;

		// Voice 2
		case 0x0c:
			{
				_voices [2] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [2] -> setFrequency ((unsigned short) (((double) _chipFrequency / 256.0f) / 
					(double) (0x80 - (v.value () & 0x7f))) << 2); // Soprano
			}

			break;

		// Voice 3 (noise)
		case 0x0d:
			{
				_voices [3] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [3] -> setFrequency ((unsigned short) (((double) _chipFrequency / 256.0f) / 
					(double) (0x80 - (v.value () & 0x7f))) << 3); // Pure noise

			}

			break;

		// The volumen
		case 0x0e:
			{
				_volumen = v.value () & 0x0f;
			}

			break;

		// The rest are nor taken into account...
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& COMMODORE::VICISoundSimpleLibWrapper::readValue (size_t p) const
{ 
	size_t pp = p % 0x20;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		case 0x0e:
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
void COMMODORE::VICISoundSimpleLibWrapper::initialize ()
{ 
	VICISoundLibWrapper::initialize ();
							  
	_volumen = 0.0f;

	_counterClocksPerSample = 0;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x10, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::VICISoundSimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
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
COMMODORE::VICISoundSimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::PulseSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF)
		}, nullptr), // There is no envelope around this sound!
		_wavesActive (0)
{ 
	setClassName ("VICIVoice");
}

// ---
double COMMODORE::VICISoundSimpleLibWrapper::Voice::data () const
{ 
	// When the wave is active or is in test active and the selected wave is a pulse...
	// ...the sound has to be produced
	if (!(active () || 
		 (!active () && _wavesActive == 0x40 /** pulse. */)))
		return (0.0f);

	double result = 0.0f;

	// The way the different waves is merged in the SID
	// is not the standard way of just adding the values...
	// ...because the way they are played are not the same...
	// ..and definetively is not adding data!
	switch (_wavesActive)
	{
		// pulse
		case 0x40:
			{
				result = wave (MCHEmul::SoundWave::Type::_PULSE) -> data ();
			}

			break;

		// noise
		case 0x80:
			{
				result = wave (MCHEmul::SoundWave::Type::_NOISE) -> data ();
			}

			break;

		default:
			{ 
				// It shouldn't be here...
				_LOG ("VIC20 wave not supported:" + std::to_string (_wavesActive));
				assert (false); // Just to be used ben running under debug compilation...
			}

			break;
	}

	// There is no need to pass the result through the envelope, as there is no any!

	return ((result > 1.0f) ? 1.0f : result);
}
