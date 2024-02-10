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
COMMODORE::TEDSoundSimpleLibWrapper::TEDSoundSimpleLibWrapper (unsigned int cF, unsigned int sF)
	: TEDSoundLibWrapper (
		{
			{ "Name", "TEDSound" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2023" }
		}),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _voices (
		{ new COMMODORE::TEDSoundSimpleLibWrapper::Voice (0, cF), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (1, cF), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (2, cF), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (3, cF) }),
	  _registers (std::vector <MCHEmul::UByte> (0x10, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) cF / (double (sF)))),
	  _counterClocksPerSample (0)
{
	// The voice 0...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v0 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [0]);
	v0 -> setWavesActive (0); // Always a sawsmooth...
	v0 -> setADR (0, 0, 0); // No ADR effect....

	// The voice 1...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v1 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]);
	v1 -> setWavesActive (1); // Always a pulse wave...
	v1 -> setADR (0, 0, 0); // No ADR effect....

	// The voice 2...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v2 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [2]);
	v2 -> setWavesActive (1); // Always a pulse wave...
	v2 -> setADR (0, 0, 0); // No ADR effect....

	// The voice 3...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v3 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [3]);
	v3 -> setWavesActive (2); // Always a noise wave...
	v3 -> setADR (0, 0, 0); // No ADR effect....
}

// ---
void COMMODORE::TEDSoundSimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	_registers [pp] = v;

	switch (pp)
	{
		// Voice 0
		case 0x0a:
			{
				_voices [0] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [0] -> setFrequency ((unsigned short) (((double) _chipFrequency / 255.0f) / 
					(double) (0x80 - (v.value () & 0x7f)))); // Bass
			}

			break;

		// Voice 1
		case 0x0b:
			{
				_voices [1] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [1] -> setFrequency ((unsigned short) (((double) _chipFrequency / 255.0f) / 
					(double) (0x80 - (v.value () & 0x7f))) << 1); // Alto

			}

			break;

		// Voice 2
		case 0x0c:
			{
				_voices [2] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [2] -> setFrequency ((unsigned short) (((double) _chipFrequency / 255.0f) / 
					(double) (0x80 - (v.value () & 0x7f))) << 2); // Soprano
			}

			break;

		// Voice 3 (noise)
		case 0x0d:
			{
				_voices [3] -> setActive ((v.value () & 0x80) != 0x00);
				_voices [3] -> setFrequency ((unsigned short) (((double) _chipFrequency / 255.0f) / 
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
			assert (false); //.. but the code should not pass over this point...
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
			assert (false); // ..but the code should not be here...
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
		// sawtooth
		case 0x20:
			result = waves ()[(size_t) MCHEmul::SoundWave::Type::_SAWTOOTH] -> data ();
			break;

		// pulse
		case 0x40:
			result = waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE] -> data ();
			break;

		// noise
		case 0x80:
			result = waves ()[(size_t) MCHEmul::SoundWave::Type::_NOISE] -> data ();
			break;

		// This sitution is not possible but just in case!
		default:
			break;
	}

	if (result != 0.0f)
		result *= ADSRData ();

	return ((result > 1.0f) ? 1.0f : result);
}
