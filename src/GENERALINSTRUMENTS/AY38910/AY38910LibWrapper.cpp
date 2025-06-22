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
	  _mixNoise { false, false, false }, // The noise is not mixed by default...
	  _volumen { 0.0f, 0.0f, 0.0f },
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) cF / (double (sF)))),
	  _counterClocksPerSample (0)
{ 
	// In each of the different voices the selected wave is the first one,
	// ...except the 4th voice that the noise is selected by default.
	// That is because the 4th voice is used to mix the rest of the voices,
	// The waves are inactive by default...
	_voices [0] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [1] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [2] -> wave (MCHEmul::SoundWave::Type::_SQUARE) -> setActive (true);
	_voices [3] -> wave (MCHEmul::SoundWave::Type::_NOISE)  -> setActive (true);
	// Voices are not active by default, but noise is always...
	_voices [3] -> setActive (true);
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
		// Fine Tune Register Voice 0
		case 0x00:
			{
				_voices [0] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((_registers [0x01].value () & 0x0f) << 8) + (unsigned short) v.value ()))));
			}

			break;

		// Coarse Tune Register Voice 0
		case 0x01:
			{
				_voices [0] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((v.value () & 0x0f) << 8) + (unsigned short) _registers [0x00].value ()))));
			}

			break;

		// Fine Tune Register Voice 1
		case 0x02:
			{
				_voices [1] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((_registers [0x03].value () & 0x0f) << 8) + (unsigned short) v.value ()))));
			}

			break;

		// Coarse Tune Register Voice 1
		case 0x03:
			{
				_voices [1] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((v.value () & 0x0f) << 8) + (unsigned short) _registers [0x02].value ()))));
			}

			break;

		// Fine Tune Register Voice 2
		case 0x04:
			{
				_voices [2] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((_registers [0x05].value () & 0x0f) << 8) + (unsigned short) v.value ()))));
			}

			break;

		// Coarse Tune Register Voice 2
		case 0x05:
			{
				_voices [2] -> setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((v.value () & 0x0f) << 8) + (unsigned short) _registers [0x04].value ()))));
			}

			break;	

		// Noise Generator Control Register
		case 0x06:
			{
				_voices [3] -> setFrequency ((unsigned short) (v.value () & 0x1f));
			}

		// Mixer Control I/O Enable Register
		case 0x07:
			{
				// The activation, activates also the envlope...
				_voices [0] -> setActive (v.bit (0));
				_voices [1] -> setActive (v.bit (1));
				_voices [2] -> setActive (v.bit (2));
				// The bits 3 to 5 determine whether the noise should be mixed 
				// with the tone comming from the 3 voices...
				_mixNoise [0] = v.bit (3);
				_mixNoise [1] = v.bit (4);
				_mixNoise [2] = v.bit (5);
				// The bits 6 & 7 are not implemented yet...
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
				_volumen [2] = !v.bit (4) ? ((double) (v.value () & 0x07) / 15.0f) : 1.0f;
				_useEnvelope [2] = v.bit (4);
		}

		break;

		// Envelope Period Control Register 1
		// In the Yamaha documentation this register is called R13 instead of R11
		case 0x0b:
			{
				_envelope.setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((_registers [0x0c].value () & 0x0f) << 8) + (unsigned short) v.value ()))));
			}

			break;

		// Envelope Period Control Register 2
		// In the Yamaha documentation this register is called R14 instead of R12
		case 0x0c:
			{
				_envelope.setFrequency 
					((unsigned short) ((double) _chipFrequency /
						(double) (16 * (unsigned short) (((v.value () & 0x0f) << 8) + (unsigned short) _registers [0x0b].value ()))));
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

	_counterClocksPerSample = 0;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
	for (size_t i = 0; i < 3; i++) 
		{ _useEnvelope [i] = _mixNoise [i] = false; _volumen [i] = 1.0f; }
}

// ---
bool GENERALINSTRUMENTS::AY38910SimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	for (auto i : _voices)
		i -> clock (); // just one...
	_envelope.clock (); // just one...

	if ((result = ((++_counterClocksPerSample) >= _clocksPerSample)))
	{
		if ((_counterClocksPerSample -= _clocksPerSample) >= _clocksPerSample)
			_counterClocksPerSample = 0; // Just in case _clocksPerSample == 0...

		double iR = 0;
		for (size_t i = 0; i < 3; i++) // The three voices are mixed with the noise if defined so...
			iR += 
				_voices [i] -> data () * 
				_volumen [i] * // 1.0 when Envelope is not active for this voice.
			    (_useEnvelope [i] ? _envelope.envelopeData () : 1.0f) * // Only if the envelope is active for that voice...
				(_mixNoise [i] ? _voices [3] -> data () : 1.0f); // Mixed with the noise voice is allowed...
		
		// This number could be greater than 1!
		if (iR > 1.0f) iR = 1.0f; // ...so it is needed to correct.

		dt = MCHEmul::UBytes ({ (unsigned char) (iR * 255.0f /** between 0 and 255. */) });
	}

	return (result);
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Envelope (unsigned short cF)
	: MCHEmul::SoundEnvelope (cF),
	  _type (GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type::_TYPE0),
	  _frequency (0),
	  _stateCounters (3, GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::StateCounters ())
{
	setClassName ("AY38910Envelope"); 

	calculateSamplingData ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::setType (const MCHEmul::UByte& v)
{
	if (v.bit (3))
		setType (v.bit (2) 
			? GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type::_TYPE1
			: GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type::_TYPE0);
	else
		setType 
			(GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::Type (0x08 | (v.value () & 0x07)));
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::setStart (bool s)
{
	// TODO
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::initialize ()
{
	// TODO

	_state = 
		GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::State::_ATTACK;

	calculateSamplingData ();
	// It also initializes the internal counters...
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::initializeInternalCounters ()
{
	for (size_t i = 0; i < 3; i++)
		_stateCounters [i].initialize ();
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::clock (unsigned int nC)
{
	if (_state == State::_SUSTAIN)
		return;

	bool end = false;
	while (!end)
	{
		GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::StateCounters& sCA = 
			_stateCounters [(int) _state];
		if (++sCA._counterCyclesPerState > sCA._cyclesPerState)
		{
			sCA._counterCyclesPerState = 0;

			switch (_state)
			{
				// TODO

				default:
					break;
			}
		}

		GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::StateCounters& sCN = 
			_stateCounters [(int) _state];
		// The limit of the previous state was reached...
		// unless the destination states is the same...
		sCA._limit = (&sCA != &sCN);
		// The loop finish when the new state is the same than previously...
		end = (&sCA == &sCN); 
	}
}

// ---
double GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::envelopeData () const
{
	// TODO

	return (1.0f);
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundEnvelope::getInfoStructure ();

	// TODO

	return (result);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::calculateSamplingData ()
{
	// TODO

	initializeInternalCounters ();

	_state = GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Envelope::State::_ATTACK;
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::SquareSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF) // This is just like a fourth wave to be mixed with the rest...
		}, nullptr), // The voices has no "individual" envelope, but one that is common to all voices..
	  _wavesActive (0)
{ 
	setClassName ("AY38910Voice"); 
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::initialize ()
{
	MCHEmul::SoundVoice::initialize ();
	
	// TODO
}

// ---
double GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::data () const
{ 
	double result = 0.0f;

	// The way the different waves is merged in the SID
	// is not the standard way of just adding the values...
	// ...because the way they are played are not the same...
	// ..and definetively is not adding data!
	switch (_wavesActive)
	{
		// TODO
		
		// This sitution is not possible but just in case!
		default:
			break;
	}

	return ((result > 1.0f) ? 1.0f : result);
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::SoundVoice::getInfoStructure ());
		
	// TODO
		
	return (result);
}
