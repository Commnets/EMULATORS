#include <COMMODORE/SIDLibWrapper.hpp>

unsigned short COMMODORE::SoundSimpleWrapper::_ATTACKTIMES [0x10] =
	{ 2, 8, 16, 24, 38, 56, 68, 80, 100, 250, 500, 800, 1000, 3000, 5000, 8000 };
unsigned short COMMODORE::SoundSimpleWrapper::_DECAYTIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };
unsigned short COMMODORE::SoundSimpleWrapper::_RELEASETIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };

// Sampled from a 6581R4
// When the triangle and sawtooth waves are mixed...
const unsigned char COMMODORE::SoundSimpleWrapper::Voice::_SAWTRIWAVE_6581 [0x100] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x08,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10, 0x3c,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x08,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10, 0x3c
};

// When the triangle and pulse waves are mixed...
const unsigned char COMMODORE::SoundSimpleWrapper::Voice::_PULSETRIWAVE_6581 [0x100] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80,
	0, 0, 0, 0, 0, 0, 0x80, 0xc0, 0, 0x80, 0x80, 0xe0, 0x80, 0xe0, 0xf0, 0xfc,
	0xff, 0xfc, 0xfa, 0xf0, 0xf6, 0xe0, 0xe0, 0x80, 0xee, 0xe0, 0xe0, 0x80, 0xc0, 0, 0, 0,
	0xde, 0xc0, 0xc0, 0, 0x80, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 0, 0,
	0xbe, 0x80, 0x80, 0, 0x80, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x7e, 0x40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const unsigned char COMMODORE::SoundSimpleWrapper::Voice::_PULSESAWWAVE_6581 [0x100] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x78,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x78
};

const unsigned char COMMODORE::SoundSimpleWrapper::Voice::_PULSESAWTRIWAVE_6581 [0x100] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x78,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x78
};

// ---
bool COMMODORE::SoundRESIDWrapper::getData (MCHEmul::CPU* cpu, MCHEmul::UBytes& dt)
{
	// Maximum 1 element...
	RESID::cycle_count nC = 1;
	// A minumum buffer (it could be even shorter!)
	short buffer [4];
	if (_resid_sid.clock (nC, buffer, 1) != 0) // When element ready...
		dt = MCHEmul::UBytes ({ (unsigned char) ((int (*buffer) / 256) + 128) }); // ...they are transformed into a unsigned char...

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::SoundRESIDWrapper::getVoiceInfoStructure (unsigned char nV) const
{
	MCHEmul::InfoStructure result;

	RESID::SID::State st = (*(const_cast <RESID::SID*> (&_resid_sid))).read_state ();

	// All voices have the same structured info in the register...
	size_t iP = nV * 7;
	result.add ("CLASSNAME", std::string ("SIDVoice"));
	result.add ("ID", nV);
	result.add ("ACTIVE", (st.sid_register [iP + 4] & 0x01) != 0);
	result.add ("ATTACK", 
		COMMODORE::SoundSimpleWrapper::_ATTACKTIMES [(st.sid_register [iP + 5] & 0xf0) >> 4]);
	result.add ("DECAY", 
		COMMODORE::SoundSimpleWrapper::_DECAYTIMES [st.sid_register [iP + 5] & 0x0f]);
	result.add ("SUSTAIN", (st.sid_register [iP + 6] & 0xf0) >> 4);
	result.add ("RELEASE", 
		COMMODORE::SoundSimpleWrapper::_RELEASETIMES [st.sid_register [iP + 6] & 0x0f]);
	result.add ("RINGMODULATION", (st.sid_register [iP + 4] & 0x04) != 0);
	result.add ("VOICERELATED", (nV == 0) ? 2 : ((nV == 1) ? 0 : 1));
	result.add ("SYNC", (st.sid_register [iP + 4] & 0x02) != 0);

	MCHEmul::InfoStructure wDt, wDt1;
	wDt1.add ("CLASSNAME", std::string ("SoundWave"));
	wDt1.add ("ACTIVE", true);
	wDt1.add ("TYPE", (int) ((st.sid_register [iP + 4] & 0xf0) >> 4));
	wDt1.add ("FREQUENCY", 
		int (double (st.sid_register [1] * 256 + st.sid_register [0]) * (double) _chipFrequency / 16777216.0f));
	wDt.add ("0", std::move (wDt1));
	result.add ("WAVES", wDt);

	return (result);
}

// ---
COMMODORE::SoundSimpleWrapper::SoundSimpleWrapper (unsigned int cF, unsigned int sF)
	: SIDLibWrapper (),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _voices (
		{ new COMMODORE::SoundSimpleWrapper::Voice (0, cF), 
		  new COMMODORE::SoundSimpleWrapper::Voice (1, cF), 
		  new COMMODORE::SoundSimpleWrapper::Voice (2, cF) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _clocksPerSample ((unsigned int) ((double) cF / (double (sF)))),
	  _counterClocksPerSample (0)
{ 
	// Link the different voices to make complex effects when requested...
	static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [0]) -> 
		setRelation (static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [2]));
	static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [1]) -> 
		setRelation (static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [0]));
	static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [2]) -> 
		setRelation (static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [1]));
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
					((unsigned short) ((double) ((((unsigned short) _registers [0x01].value ()) << 8) + 
												  ((unsigned short) _registers [0x00].value ())) 
												 * (double) _chipFrequency / 16777216.0f));
			}

			break;

		// Pulse wave duty cycle width for voice 1: PWLO1, PWHI1
		case 0x02:
		case 0x03:
			{
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [0]) -> setPulseUpPercentage
					((double) ((((unsigned short) _registers [0x03].value ()) << 8) + 
								((unsigned short) _registers [0x02].value ())) / 4096.0f);
			}

			break;

		// Voice 1 control regiter: VCREG1
		case 0x04:
			{
				_voices [0] -> setStart (v.bit (0));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [0]) -> setSync (v.bit (1)); 
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [0]) -> setRingModulation (v.bit (2));
				_voices [0] -> setActive (!v.bit (3));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (v.bit (6));
				_voices [0] -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (v.bit (7));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [0]) -> setWavesActive (v.value () & 0xf0);
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
				_voices [0] -> setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f /** between 0 an 1. */);
				_voices [0] -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;

		// Frequency for voice 2: FRELO2, FREHI2
		case 0x07:
		case 0x08:
			{
				_voices [1] -> setFrequency
					((unsigned short) ((double) ((((unsigned short) _registers [0x08].value ()) << 8) + 
												  ((unsigned short) _registers [0x07].value ())) 
												 * (double) _chipFrequency / 16777216.0f));
			}

			break;

		// Pulse wave duty cycle width for voice 2: PWLO2, PWHI2
		case 0x09:
		case 0x0a:
			{
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [1]) -> setPulseUpPercentage
						((double) ((((unsigned short) _registers [0x0a].value ()) << 8) + 
								((unsigned short) _registers [0x09].value ())) / 4096.0f);
			}

			break;

		// Voice 2 control regiter: VCREG2
		case 0x0b:
			{
				_voices [1] -> setStart (v.bit (0));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [1]) -> setSync (v.bit (1)); 
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [1]) -> setRingModulation (v.bit (2));
				_voices [1] -> setActive (!v.bit (3));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (v.bit (6));
				_voices [1] -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (v.bit (7));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [1]) -> setWavesActive (v.value () & 0xf0);
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
				_voices [1] -> setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f);
				_voices [1] -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;
		
		// Frequency for voice 2: FRELO3, FREHI3
		case 0x0e:
		case 0x0f:
			{
				_voices [2] -> setFrequency
					((unsigned short) ((double) ((((unsigned short) _registers [0x0f].value ()) << 8) + 
												  ((unsigned short) _registers [0x0e].value ())) 
												 * (double) _chipFrequency / 16777216.0f));
			}

			break;

		// Pulse wave duty cycle width for voice 3: PWLO3, PWHI3
		case 0x10:
		case 0x11:
			{
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [2]) -> setPulseUpPercentage
					((double) ((((unsigned short) _registers [0x11].value ()) << 8) + 
								((unsigned short) _registers [0x10].value ())) / 4096.0f);
			}

			break;

		// Voice 3 control regiter: VCREG3
		case 0x12:
			{
				_voices [2] -> setStart (v.bit (0));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [2]) -> setSync (v.bit (1)); 
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [2]) -> setRingModulation (v.bit (2));
				_voices [2] -> setActive (!v.bit (3));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (v.bit (6));
				_voices [2] -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (v.bit (7));
				static_cast <COMMODORE::SoundSimpleWrapper::Voice*> 
					(_voices [2]) -> setWavesActive (v.value () & 0xf0);
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
				_voices [2] -> setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f);
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
				setVolumen ((double) (v.value () & 0x0f) / 15.0f /** between 0 and 1. */);
			}

			break;

		// Game paddle are not still implemented
		// They have some king of connection with CIA1
		case 0x19:
		case 0x1a:
			break;

		// Random number generator...
		// Info when reading, not when writting...
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

// ---
const MCHEmul::UByte& COMMODORE::SoundSimpleWrapper::readValue (size_t p) const
{ 
	size_t pp = p % 0x20;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		// Random number generator (oscillator 3): RANDOM
		case 0x1b:
			result = dynamic_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [2]) -> oscillatorValue ();
			break;

		// Envelope generator 3: ENV3
		case 0x1c:
			result = dynamic_cast <COMMODORE::SoundSimpleWrapper::Voice*> (_voices [2]) -> envelopeValue ();
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
void COMMODORE::SoundSimpleWrapper::initialize ()
{ 
	SIDLibWrapper::initialize ();
							  
	_volumen = 0.0f;

	_counterClocksPerSample = 0;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::SoundSimpleWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
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
void COMMODORE::SoundSimpleWrapper::Voice::initialize ()
{
	MCHEmul::SoundVoice::initialize ();

	_ringModulation = false;
}

// ---
double COMMODORE::SoundSimpleWrapper::Voice::data () const
{ 
	// When the wave is active or is in test active and the selected wave is a pulse...
	// ...the sound has to be produced
	if (!(active () || (!active () && _wavesActive == 0x40 /** pulse. */)))
		return (0.0f);

	// When sync the internal clocks of the waves are synchronized...
	if (_sync && wavesClockRestarted ())
		_voiceRelated -> initializeWavesInternalCounters ();

	double result = 0.0f;

	// The way the different waves is merged in the SID
	// is not the standard way of just adding the values...
	// ...because the way they are played are not the same...
	// ..and definetively is not adding data!
	switch (_wavesActive)
	{
		// triangle
		case 0x10:
			// Depending whether the voice is modulated or not, 
			// ...just happens on the triangle wave...
			result = waves ()[(size_t) MCHEmul::SoundWave::Type::_TRIANGLE] -> data () *
				(_ringModulation ? _voiceRelated -> data () : 1.0f); // Multiplying two waves...
			break;

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

		// sawtooth & triangle
		case 0x30:
			result = (double) _SAWTRIWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & triangle
		case 0x50:
			if (static_cast <MCHEmul::PulseSoundWave*> 
					(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUp ())
				result = (double) _PULSETRIWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & sawtooth
		case 0x60:
			if (static_cast <MCHEmul::PulseSoundWave*> 
					(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUp ())
				result = (double) _PULSESAWWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & sawtooth & triangle
		case 0x70:
			if (static_cast <MCHEmul::PulseSoundWave*> 
					(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUp ())
				result = (double) _PULSESAWTRIWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// With this other combinations no output is produced...
		// noise & triangle
		case 0x90:
		// noise & sawtooth
		case 0xa0:
		// noise & sawtooth & triangle
		case 0xb0:
		// noise & pulse
		case 0xc0:
		// noise & pulse & triangle
		case 0xd0:
		// noise & pulse & sawtooth
		case 0xe0:
		// noise & pulse & sawtooth & triangle 
		case 0xf0:
		// This sitution is not possible but just in case!
		default:
			break;
	}

	if (result != 0.0f)
		result *= ADSRData ();

	return ((result > 1.0f) ? 1.0f : result);
}

// ---
MCHEmul::InfoStructure COMMODORE::SoundSimpleWrapper::Voice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundVoice::getInfoStructure ());

	result.add ("RINGMODULATION", _ringModulation);
	result.add ("VOICERELATED", _voiceRelated -> id ());
	result.add ("SYNC", _sync);

	return (result);
}
