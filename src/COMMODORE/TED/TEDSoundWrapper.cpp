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
	  _voices ( // Two voices...
		{ new COMMODORE::TEDSoundSimpleLibWrapper::Voice (0, cF), 
		  new COMMODORE::TEDSoundSimpleLibWrapper::Voice (1, cF) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) cF / (double (sF)))),
	  _counterClocksPerSample (0)
{
	// The voice 0...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v0 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [0]);
	v0 -> setWavesActive (0); // Always a pulse wave...
	v0 -> setADR (0, 0, 0); // No ADR effect....

	// The voice 1...
	COMMODORE::TEDSoundSimpleLibWrapper::Voice* v1 = 
		static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]);
	v1 -> setWavesActive (1); // By defect a pulse wave, but it can be moved to noise!
	v1 -> setADR (0, 0, 0); // No ADR effect....
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
				_voices [0] -> setFrequency 
					(1023 - (unsigned short) (111860.78125f / (float) ((_voices [0] -> frequency () & 0xff00) | v.value ())));
			}

			break;

		// Voice 1 LSB
		case 0x0f:
			{
				_voices [1] -> setFrequency 
					(1023 - (unsigned short) (111860.78125f / (float) ((_voices [1] -> frequency () & 0xff00) | v.value ())));
			}

			break;

		// Voice 0 MSB (only two bits)
		case 0x10:
			{
				_voices [0] -> setFrequency 
					(1023 - (unsigned short) (111860.78125f / 
						(float) ((_voices [0] -> frequency () & 0x00ff) | (((unsigned short) (v.value () & 0x03)) << 8))));
			}

			break;

		// Sound Color Register
		case 0x11:
			{
				setVolumen ((float) (v.value () & 0x0f) / 15.0f);
				_voices [0] -> setActive (v.bit (4));
				switch ((v.value () & 0x60 /** bits 5 y 6. */) >> 5) // 0, 1, 2, 3
				{
					case 0:
						{
							_voices [0] -> setActive (false);
							_voices [1] -> setActive (false);
						}
						
						break;

					case 1:
					case 3:
						{
							_voices [0] -> setActive (true);
							static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]) -> 
								setWavesActive (0); // The noise is selected...
							_voices [1] -> setActive (false); // But it is not active...
						}

					case 2:
						{
							_voices [0] -> setActive (false);
							static_cast <COMMODORE::TEDSoundSimpleLibWrapper::Voice*> (_voices [1]) -> 
								setWavesActive (1);
							_voices [1] -> setActive (true);
						}

						break;

					// This sutation never happens...
					default:
						break;
				}
			}

			break;

		// Voice 1 MSB (only two bits)
		case 0x12:
			{
				_voices [1] -> setFrequency 
					(1023 - (unsigned short) (111860.78125f / 
						(float) ((_voices [1] -> frequency () & 0x00ff) | (((unsigned short) (v.value () & 0x03)) << 8))));
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
