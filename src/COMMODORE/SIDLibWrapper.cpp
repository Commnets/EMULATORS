#include <COMMODORE/SIDLibWrapper.hpp>

unsigned short COMMODORE::SoundSimpleWrapper::_ATTACKTIMES [0x10] =
	{ 2, 8, 16, 24, 38, 56, 68, 80, 100, 250, 500, 800, 1000, 3000, 5000, 8000 };
unsigned short COMMODORE::SoundSimpleWrapper::_DECAYTIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };
unsigned short COMMODORE::SoundSimpleWrapper::_RELEASETIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };

// ---
const MCHEmul::UByte& COMMODORE::SoundRESIDWrapper::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x20;

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
		case 0x0e:
		case 0x0f:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
			result = std::move ((*(const_cast <RESID::SID*> (&_resid_sid))).read_state ()).sid_register [pp];
			break;


		// Direct information from the structure kept into the REDSID::SID
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
			MCHEmul::UByte ((unsigned char) (*(const_cast <RESID::SID*> (&_resid_sid))).read ((RESID::reg8) (p % 0x20)));
			break;

		// Not connected...
		case 0x1d:
		case 0x1e:
		case 0x1f:
			result = MCHEmul::UByte::_FF;
			break;

		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
bool COMMODORE::SoundRESIDWrapper::getData (MCHEmul::CPU* cpu, MCHEmul::UBytes& dt)
{
	_resid_sid.clock ();

	// Always a positive number between 0 and 255...
	dt = MCHEmul::UBytes ({ (unsigned char) 
		((char) ((double) _resid_sid.output () / 256.0f) - std::numeric_limits <char>::min ()) });

	return (true);
}

// ---
void COMMODORE::SoundSimpleWrapper::initialize ()
{ 
	SIDLibWrapper::initialize ();
							  
	_lastClockCycles = 0;

	_counterClocksPerSample = 0;

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::SoundSimpleWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	unsigned int nC = cpu -> clockCycles () - _lastClockCycles;

	for (auto i : _voices)
		i -> clock (nC);

	if ((result = ((_counterClocksPerSample += nC) >= _clocksPerSample)))
	{
		if ((_counterClocksPerSample -= _clocksPerSample) >= _clocksPerSample)
			_counterClocksPerSample = 0;

		double iR = 0;
		for (auto i : _voices)
			iR += i -> data (); // This number could be greater than 1!
		if (iR > 1.0f) iR = 1.0f; // ...so it is needed to correct.

		// TODO: To apply the filters...

		dt = MCHEmul::UBytes ({ (unsigned char) (iR * 256.0f) });
	}

	_lastClockCycles += nC;

	return (result);
}

// ---
void COMMODORE::SoundSimpleWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x20;

	_registers [pp] = v;

	switch (pp)
	{
		// Frequency for voice 1: FRELO1, FREHI1
		case 0x00:
		case 0x01:
			{
				_voices [0] -> setFrequency
					((((unsigned short) _registers [0x01].value ()) << 8) + ((unsigned short) _registers [0x00].value ()));
			}

			break;

		// Pulse wave duty cycle width for voice 1: PWLO1, PWHI1
		case 0x02:
		case 0x03:
			{
				_voices [0] -> setDutyCycle
					((((unsigned short) _registers [0x03].value ()) << 8) + ((unsigned short) _registers [0x00].value ()));
			}

			break;

		// Voice 1 control regiter: VCREG1
		case 0x04:
			{
				_voices [0] -> setStart (v.bit (0));
				// Bits 1 and 2 not implemented...
				_voices [0] -> setActive (v.bit (3));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_PULSE)	-> setActive (v.bit (6));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_NOISE)	-> setActive (v.bit (7));
			}

			break;

		// Voice 1 Attack/Decay register: ATDCY1
		case 0x05:
			{
				_voices [0] -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				_voices [0] -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 1 Sustain/Release register: SUREL1
		case 0x06:
			{
				_voices [0] -> setSustainVolumen ((v.value () & 0xf0) >> 4);
				_voices [0] -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;

		// Frequency for voice 2: FRELO2, FREHI2
		case 0x07:
		case 0x08:
			{
				_voices [1] -> setFrequency
					((((unsigned short) _registers [0x08].value ()) << 8) + ((unsigned short) _registers [0x07].value ()));
			}

			break;

		// Pulse wave duty cycle width for voice 2: PWLO2, PWHI2
		case 0x09:
		case 0x0a:
			{
				_voices [1] -> setDutyCycle
					((((unsigned short) _registers [0x0a].value ()) << 8) + ((unsigned short) _registers [0x09].value ()));
			}

			break;

		// Voice 2 control regiter: VCREG2
		case 0x0b:
			{
				_voices [1] -> setStart (v.bit (0));
				// Bits 1 and 2 not implemented...
				_voices [1] -> setActive (v.bit (3));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_PULSE)	-> setActive (v.bit (6));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_NOISE)	-> setActive (v.bit (7));
			}

			break;

		// Voice 2 Attack/Decay register: ATDCY2
		case 0x0c:
			{
				_voices [1] -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				_voices [1] -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 2 Sustain/Release register: SUREL2
		case 0x0d:
			{
				_voices [1] -> setSustainVolumen ((v.value () & 0xf0) >> 4);
				_voices [1] -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;
		
		// Frequency for voice 2: FRELO3, FREHI3
		case 0x0e:
		case 0x0f:
			{
				_voices [2] -> setFrequency
					((((unsigned short) _registers [0x0f].value ()) << 8) + ((unsigned short) _registers [0x0e].value ()));
			}

			break;

		// Pulse wave duty cycle width for voice 3: PWLO3, PWHI3
		case 0x10:
		case 0x11:
			{
				_voices [2] -> setDutyCycle
					((((unsigned short) _registers [0x11].value ()) << 8) + ((unsigned short) _registers [0x10].value ()));
			}

			break;

		// Voice 3 control regiter: VCREG3
		case 0x12:
			{
				_voices [2] -> setStart (v.bit (0));
				// Bits 1 and 2 not implemented...
				_voices [2] -> setActive (v.bit (3));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_PULSE)	-> setActive (v.bit (6));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_NOISE)	-> setActive (v.bit (7));
			}

			break;

		// Voice 3 Attack/Decay register: ATDCY3
		case 0x13:
			{
				_voices [2] -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				_voices [2] -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 3 Sustain/Release register: SUREL3
		case 0x14:
			{
				_voices [2] -> setSustainVolumen ((v.value () & 0xf0) >> 4);
				_voices [2] -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;

		// Filter functions not implemented still
		// From registers 0x15 to 0x17
		case 0x15:
		case 0x16:
		case 0x17:
			break;

		// Volumen: SIGVOL
		case 0x18:
			{
				for (auto i : _voices) 
					i -> setVolumen (v.value () & 0x0f);
			}

			break;

		// Game paddle are not still implemented
		// They have some king of connection with CIA1
		// From registers 0x19 to 0x1a
		case 0x19:
		case 0x1a:
			break;

		// Random number generator not still implemented
		case 0x1b:
		case 0x1c:
			break;

		// Not connected
		case 0x1d:
		case 0x1e:
		case 0x1f:
			break;

		default:
			break;
	}
}
