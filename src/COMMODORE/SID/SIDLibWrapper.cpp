#include <COMMODORE/SID/SIDLibWrapper.hpp>
#include <cmath>

unsigned short COMMODORE::SoundSIDSimpleWrapper::_ATTACKTIMES [0x10] =
	{ 2, 8, 16, 24, 38, 56, 68, 80, 100, 250, 500, 800, 1000, 3000, 5000, 8000 };
unsigned short COMMODORE::SoundSIDSimpleWrapper::_DECAYTIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };
unsigned short COMMODORE::SoundSIDSimpleWrapper::_RELEASETIMES [0x10] =
	{ 6, 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000 };

// Sampled from a 6581R4
// When the triangle and sawtooth waves are mixed...
const unsigned char COMMODORE::SoundSIDSimpleWrapper::Voice::_SAWTRIWAVE_6581 [0x100] =
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
const unsigned char COMMODORE::SoundSIDSimpleWrapper::Voice::_PULSETRIWAVE_6581 [0x100] = 
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

const unsigned char COMMODORE::SoundSIDSimpleWrapper::Voice::_PULSESAWWAVE_6581 [0x100] = 
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

const unsigned char COMMODORE::SoundSIDSimpleWrapper::Voice::_PULSESAWTRIWAVE_6581 [0x100] = 
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
MCHEmul::InfoStructure COMMODORE::SIDLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 3; i++)
		vDt.add (std::to_string (i), std::move (getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}

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

	int at, dc, sn, rl;
	size_t iP = nV * 7;
	result.add ("CLASSNAME", std::string ("SIDVoice"));
	result.add ("ID", nV);
	result.add ("ACTIVE", (st.sid_register [iP + 4] & 0x01) != 0);
	result.add ("ATTACK", 
		at = (int) (COMMODORE::SoundSIDSimpleWrapper::_ATTACKTIMES [(st.sid_register [iP + 5] & 0xf0) >> 4]));
	result.add ("DECAY", 
		dc = (int) (COMMODORE::SoundSIDSimpleWrapper::_DECAYTIMES [st.sid_register [iP + 5] & 0x0f]));
	result.add ("SUSTAIN", 
		sn = (int) ((st.sid_register [iP + 6] & 0xf0) >> 4));
	result.add ("RELEASE", 
		rl = (int) (COMMODORE::SoundSIDSimpleWrapper::_RELEASETIMES [st.sid_register [iP + 6] & 0x0f]));
	result.add ("ADSR", "+" + std::to_string (at) + "," +
						"-" + std::to_string (dc) + "," +
						"=" + std::to_string (sn) + "," +
						"--" + std::to_string (rl)); // A summary...
	result.add ("RINGMODULATION", (st.sid_register [iP + 4] & 0x04) != 0);
	result.add ("VOICERELATED", (nV == 0) ? 2 : ((nV == 1) ? 0 : 1));
	result.add ("SYNC", (st.sid_register [iP + 4] & 0x02) != 0);

	int tp;
	int fq;
	MCHEmul::InfoStructure wDt, wDt1;
	wDt1.add ("CLASSNAME", std::string ("SoundWave"));
	wDt1.add ("ACTIVE", true);
	wDt1.add ("TYPE", tp = ((int) ((st.sid_register [iP + 4] & 0xf0) >> 4)));
	wDt1.add ("FREQUENCY", 
		fq = (int (double (st.sid_register [iP + 1] * 256 + st.sid_register [iP + 0]) * (double) _chipFrequency / 16777216.0f)));
	wDt1.add ("TYPEANDFREQUENCY", std::to_string (tp) + "(" + std::to_string (fq) + ")");
	wDt.add ("0", std::move (wDt1));
	result.add ("WAVES", wDt);

	return (result);
}

// ---
COMMODORE::SoundSIDSimpleWrapper::SoundSIDSimpleWrapper (unsigned int cF, unsigned int sF)
	: COMMODORE::SIDLibWrapper (
		{
			{ "Name", "SimpleSID" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2023" }
		}),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  _volumen (0.0f), // There is no volumen at the beginning...
	  _voices (
		{ new COMMODORE::SoundSIDSimpleWrapper::Voice (0, cF, sF), // Desactivated by default...
		  new COMMODORE::SoundSIDSimpleWrapper::Voice (1, cF, sF), 
		  new COMMODORE::SoundSIDSimpleWrapper::Voice (2, cF, sF) }),
	  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
	  _cyclesPerSample ((double) cF / (double (sF))), // Whether the value sF == 0 is not checked...
	  _counterCyclesPerSample (0.0f)
{ 
	// Link the different voices to make complex effects when requested...
	static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [0]) -> 
		setRelation (static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]));
	static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [1]) -> 
		setRelation (static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [0]));
	static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]) -> 
		setRelation (static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [1]));

	// The voices are desactivated by default...
	// ...ans also the enevlope in each...
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x20;

	MCHEmul::UByte oldV = _registers [pp];
	_registers [pp] = v;

	switch (pp)
	{
		// Frequency for voice 1: FRELO1, FREHI1
		case 0x00:
		case 0x01:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [0]);
				const unsigned short freqRegister =
					(((unsigned short) _registers [0x01].value ()) << 8) |
					 ((unsigned short) _registers [0x00].value ());
				voice -> setFrequency (
					((double) freqRegister * (double) _chipFrequency) / 16777216.0f);
			}

			break;

		// Pulse wave duty cycle width for voice 1: PWLO1, PWHI1
		case 0x02:
		case 0x03:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [0]);
				voice -> setPulseUpPercentage
					((double) ((((unsigned short) (_registers [0x03].value () & 0x0f)) << 8) + 
								((unsigned short) _registers [0x02].value ())) / 4096.0f);
			}

			break;

		// Voice 1 control regiter: VCREG1
		case 0x04:
			{
				auto* voice =
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [0]);
				if (v.bit (0) != oldV.bit (0)) 
					voice -> setStart (v.bit (0));
				voice -> setSync (v.bit (1)); 
				voice -> setRingModulation (v.bit (2));
				voice -> setTest (v.bit (3));
				voice -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				voice -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				voice -> wave (MCHEmul::SoundWave::Type::_PULSE)	-> setActive (v.bit (6));
				voice -> wave (MCHEmul::SoundWave::Type::_NOISE)	-> setActive (v.bit (7));
				voice -> setWavesActive (v.value () & 0xf0);
			}

			break;

		// Voice 1 Attack/Decay register: ATDCY1
		case 0x05:
			{
				auto* envelope = 
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [0] -> envelope ());
				envelope -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				envelope -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 1 Sustain/Release register: SUREL1
		case 0x06:
			{
				auto* envelope = 
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [0] -> envelope ());
				envelope -> setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f /** between 0 an 1. */);
				envelope -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;

		// Frequency for voice 2: FRELO2, FREHI2
		case 0x07:
		case 0x08:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [1]);
				const unsigned short freqRegister =
					(((unsigned short) _registers [0x08].value ()) << 8) |
					 ((unsigned short) _registers [0x07].value ());
				voice -> setFrequency (
					((double) freqRegister * (double) _chipFrequency) / 16777216.0f);
			}

			break;

		// Pulse wave duty cycle width for voice 2: PWLO2, PWHI2
		case 0x09:
		case 0x0a:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [1]);
				voice -> setPulseUpPercentage
						((double) ((((unsigned short) (_registers [0x0a].value () & 0x0f)) << 8) + 
								((unsigned short) _registers [0x09].value ())) / 4096.0f);
			}

			break;

		// Voice 2 control regiter: VCREG2
		case 0x0b:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [1]);
				if (v.bit (0) != oldV.bit (0))
					voice -> setStart (v.bit (0));
				voice -> setSync (v.bit (1)); 
				voice -> setRingModulation (v.bit (2));
				voice -> setTest (v.bit (3));
				voice -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				voice -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				voice -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (v.bit (6));
				voice -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (v.bit (7));
				voice -> setWavesActive (v.value () & 0xf0);
			}

			break;

		// Voice 2 Attack/Decay register: ATDCY2
		case 0x0c:
			{
				auto* envelope =
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [1] -> envelope ());
				envelope -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				envelope -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 2 Sustain/Release register: SUREL2
		case 0x0d:
			{
				auto* envelope =
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [1] -> envelope ());
				envelope ->	setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f);
				envelope -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;
		
		// Frequency for voice 3: FRELO3, FREHI3
		case 0x0e:
		case 0x0f:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]);
				const unsigned short freqRegister =
					(((unsigned short) _registers [0x0f].value ()) << 8) |
					 ((unsigned short) _registers [0x0e].value ());
				voice -> setFrequency (
					((double) freqRegister * (double) _chipFrequency) / 16777216.0f);
			}

			break;

		// Pulse wave duty cycle width for voice 3: PWLO3, PWHI3
		case 0x10:
		case 0x11:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]);
				voice -> setPulseUpPercentage
					((double) ((((unsigned short) (_registers [0x11].value () & 0x0f)) << 8) + 
								((unsigned short) _registers [0x10].value ())) / 4096.0f);
			}

			break;

		// Voice 3 control regiter: VCREG3
		case 0x12:
			{
				auto* voice = 
					static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]);
				if (v.bit (0) != oldV.bit (0))
					voice -> setStart (v.bit (0));
				voice -> setSync (v.bit (1)); 
				voice -> setRingModulation (v.bit (2));
				voice -> setTest (v.bit (3));
				voice -> wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> setActive (v.bit (4));
				voice -> wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> setActive (v.bit (5));
				voice -> wave (MCHEmul::SoundWave::Type::_PULSE) -> setActive (v.bit (6));
				voice -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (v.bit (7));
				voice -> setWavesActive (v.value () & 0xf0);
			}

			break;

		// Voice 3 Attack/Decay register: ATDCY3
		case 0x13:
			{
				auto* envelope =
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [2] -> envelope ());
				envelope -> setAttack (_ATTACKTIMES [(v.value () & 0xf0) >> 4]);
				envelope -> setDecay (_DECAYTIMES [v.value () & 0x0f]);
			}

			break;

		// Voice 3 Sustain/Release register: SUREL3
		case 0x14:
			{
				auto* envelope =
					static_cast <MCHEmul::SoundADSREnvelope*> (_voices [2] -> envelope ());
				envelope -> setSustainVolumen ((double) ((v.value () & 0xf0) >> 4) / 15.0f);
				envelope -> setRelease (_RELEASETIMES [v.value () & 0x0f]);
			}

			break;

		// Filter cutoff: CUTLOW & CUTHI
		// The frequency is transmitted to all filters (4 per voice = 12 in total)
		// The ones active and desactive is not changed...
		case 0x15:
		case 0x16:
			{
				setFrecuencyInFilters (
					((unsigned short) _registers [0x16].value () << 3) |
					 (unsigned short) _registers [0x15].value () & 0x03);
			}

			break;

		// Filter Resonance Control Register: RESON
		case 0x17:
			{
				activateFiltersPerVoice 
					(v & 0x0f, (v & 0xf0) >> 4, (_registers [0x18] & 0xf0) >> 4);
			}

			break;

		// Volumen: SIGVOL
		case 0x18:
			{
				setVolumen ((double) (v.value () & 0x0f) / 15.0f /** between 0 and 1. */);
				// The rest of the info...
				activateFiltersPerVoice 
					(_registers [0x17] & 0x0f, (_registers [0x17] & 0xf0) >> 4, (v & 0xf0) >> 4);
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
const MCHEmul::UByte& COMMODORE::SoundSIDSimpleWrapper::readValue (size_t p) const
{ 
	size_t pp = p % 0x20;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		// Random number generator (oscillator 3): RANDOM
		case 0x1b:
			result = dynamic_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]) -> oscillatorValue ();
			break;

		// Envelope generator 3: ENV3
		case 0x1c:
			result = dynamic_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [2]) -> envelopeValue ();
			break;

		// This registers always return FF
		case 0x1d:
		case 0x1e:
		case 0x1f:
			result = MCHEmul::UByte::_FF;
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
void COMMODORE::SoundSIDSimpleWrapper::initialize ()
{ 
	SIDLibWrapper::initialize ();
							  
	_volumen = 0.0f;

	_counterCyclesPerSample = 0.0f;

	// All voices are active in this emulation...
	for (auto i : _voices)
		i -> initialize ();

	// All registers are 0 by default...
	_registers = std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0); 
}

// ---
bool COMMODORE::SoundSIDSimpleWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	for (auto i : _voices)
		i -> clock (); // just one...
	for (auto i : _voices)
		static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (i) -> applySync ();
	for (auto i : _voices)
		static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (i) -> clearOscillatorRestarted ();

	_counterCyclesPerSample += 1.0f;
	if ((result = 
			(_cyclesPerSample > 0.0f &&
			 _counterCyclesPerSample >= _cyclesPerSample)))
	{
		_counterCyclesPerSample = 
			std::fmod (_counterCyclesPerSample, _cyclesPerSample);

		// In the emulation of the SID
		// the voices are "added" producing more "signal"....
		double iR = 0.0f; // No sound...
		for (auto i : _voices)
			iR += i -> data (); // but the values are added...
		iR *= _volumen; // ...and adjusted to the volumen...
		if (iR > 1.0f) // ..but the outcome can never be finally more than 1.0f!
			iR = 1.0f;

		dt = MCHEmul::UBytes ({ (unsigned char) (iR * 255.0f /** between 0 and 255 finally. */) });
	}

	return (result);
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::setFrecuencyInFilters (unsigned short nR)
{
	double cF = 30.0f + ((double) nR * 5.85f);
	for (size_t i = 0; i < 3; i++)
	{
		COMMODORE::SoundSIDSimpleWrapper::Voice* voice = 
			static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [i]);
		for (size_t j = 0; j < 4; j++)
		{
			// Each filter in each voice belogs to a specific type...
			switch (j)
			{
				case 0: voice -> filters ()[0] -> setLowPassCutFrequency	(cF); break;
				case 1:	voice -> filters ()[1] -> setHighPassCutFrequency	(cF); break;
				case 2: voice -> filters ()[2] -> setBandPassFrequency		(cF); break;
				case 3: voice -> filters ()[3] -> setNotchFrequency			(cF); break;
				default: break; // It shouldn't be here, just in case...
			}
		}
	}
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::activateFiltersPerVoice
	(const MCHEmul::UByte& vF, const MCHEmul::UByte& tF, const MCHEmul::UByte& nL)
{
	// The three voices...
	for (size_t i = 0; i < 3; i++)
	{
		COMMODORE::SoundSIDSimpleWrapper::Voice* voice = 
			static_cast <COMMODORE::SoundSIDSimpleWrapper::Voice*> (_voices [i]);
		// The filters are active depending onthe value of the bytes...
		for (size_t j = 0; j < 3; j++)
			voice -> filter (j) -> setActive 
				(vF.bit (i) /** Active or not. */ && tF.bit (j) /** Which type is active. */);
		// ...but the filter 3 is always active (notch)
		// What is adjustable is the level of this...
		voice -> filter (3) -> setActive (true); // The notch is always active...
		voice -> filter (3) -> setNotchFrequency ((double) (nL.value ()));
	}
}

// ---
COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::SIDNoiseSoundWave (unsigned int cF)
	: MCHEmul::SoundWave (MCHEmul::SoundWave::Type::_NOISE, cF),
	  _phaseAccumulator (0),
	  _phaseIncrement (0),
	  _shiftRegister (0x7ffff8)
{
	setClassName ("SIDNoiseSoundWave");
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::setTest (bool t)
{
	initializeInternalCounters ();

	// reSID approximates TEST by clearing the register immediately and reseeding it when TEST ends.
	_shiftRegister = t ? 0 : 0x7ffff8;
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::initialize ()
{
	MCHEmul::SoundWave::initialize ();

	_phaseAccumulator = 0;
	_phaseIncrement = 0;
	_shiftRegister = 0x7ffff8;
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::initializeInternalCounters ()
{
	MCHEmul::SoundWave::initializeInternalCounters ();

	// Hard sync resets the oscillator phase without restarting the noise sequence.
	_phaseAccumulator = 0;
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::clock (unsigned int nC)
{
	MCHEmul::SoundWave::clock (nC);

	for (unsigned int i = 0; i < nC; i++)
	{
		const unsigned int previousAccumulator = _phaseAccumulator;

		_phaseAccumulator =
			(_phaseAccumulator + _phaseIncrement) & 0xffffff;

		// The SID clocks the noise register on the rising edge of accumulator bit 19.
		if (!(previousAccumulator & 0x080000) &&
			(_phaseAccumulator & 0x080000))
			stepShiftRegister ();
	}
}

// ---
double COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::data () const
{
	// FREQ zero freezes the register but does not force its DAC output to zero.
	return (_active ? (double) outputValue () / 255.0f : 0.0f);
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave::calculateWaveSamplingData ()
{
	MCHEmul::SoundWave::calculateWaveSamplingData ();

	_phaseIncrement = (_chipFrequency != 0)
		? (static_cast <unsigned int> (std::llround (
			(_frequency * 16777216.0) / (double) _chipFrequency)) & 0xffff)
		: 0;
}

// ---
COMMODORE::SoundSIDSimpleWrapper::Voice::Voice (int id, unsigned int cF, unsigned int sF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::TriangleSoundWave (cF),
			new MCHEmul::SawSmoothSoundWave (cF),
			new MCHEmul::PulseSoundWave (cF),
			new COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave (cF) // The noise is special...
		}, 
		// the most typical one!
		new MCHEmul::SoundADSREnvelope (cF),
		// There are four filters (one per type) per voice 
		// that might be active simultaneously...
		{ 
			new MCHEmul::BiquadSoundFilter (cF, MCHEmul::SoundFilter::Type::_LOWPASS),
			new MCHEmul::BiquadSoundFilter (cF, MCHEmul::SoundFilter::Type::_HIGHPASS),
			new MCHEmul::BiquadSoundFilter (cF, MCHEmul::SoundFilter::Type::_BANDPASS),
			new MCHEmul::BiquadSoundFilter (cF, MCHEmul::SoundFilter::Type::_NOTCH)
		}),
		_voiceRelated (nullptr), // set when Emulation is built (it is guarentted that it is not nullptr when running)
		_ringModulation (false), // Not modulated by default...
		_sync (false), // Not sync by default...
		_test (false), // Not in test mode by default...
		_wavesActive (0),
		_oscillatorRestarted (false)
{ 
	setClassName ("SIDVoice");

	// By default none of the filters are active..
	// ...but just in case tis method is invoked...
	setFiltersActive (false);
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::Voice::setTest (bool t)
{
	if (_test == t)
	{
		_active = !t;

		return;
	}

	static_cast <COMMODORE::SoundSIDSimpleWrapper::SIDNoiseSoundWave*>
		(wave (MCHEmul::SoundWave::Type::_NOISE)) -> setTest (t);

	_test = t;
	if (_test)
	{
		_active = false;

		// Approximation:
		// TEST disables oscillator output and resets internal oscillator state.
		initializeOscillatorCounters ();
	}
	else
		_active = true;
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::Voice::initialize ()
{
	MCHEmul::SoundVoice::initialize ();

	_ringModulation = false;
	_sync = false;
	_test = false;
	_wavesActive = 0;
	_oscillatorRestarted = false;

	if (_envelope != nullptr)
		_envelope -> setActive (true);

	// None is active by default...
	setFiltersActive (false);
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::Voice::clock (unsigned int nC)
{
	_oscillatorRestarted = false;

	if (!_test)
	{
		for (auto i : _waves)
		{
			i -> clock (nC);

			_oscillatorRestarted |= i -> clockRestarted ();
		}
	}

	// The envelope is controlled by GATE, not by TEST.
	// Therefore it must keep evolving even when TEST is active.
	if (_envelope != nullptr)
		_envelope -> clock (nC);
}

// ---
double COMMODORE::SoundSIDSimpleWrapper::Voice::data () const
{ 
	// When the wave is active or is in test active and the selected wave is a pulse...
	// ...the sound has to be produced
    if (!active () && 
		!(_test && (_wavesActive == 0x40)))
        return (0.0f);

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
			result = wave (MCHEmul::SoundWave::Type::_TRIANGLE) -> data () *
				(_ringModulation ? _voiceRelated -> data () : 1.0f); // Multiplying two waves...
			break;

		// sawtooth
		case 0x20:
			result = wave (MCHEmul::SoundWave::Type::_SAWTOOTH) -> data ();
			break;

		// pulse
		case 0x40:
			result = wave (MCHEmul::SoundWave::Type::_PULSE) -> data ();
			break;

		// noise
		case 0x80:
			result = wave (MCHEmul::SoundWave::Type::_NOISE) -> data ();
			break;

		// sawtooth & triangle
		case 0x30:
			result = (double) _SAWTRIWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & triangle
		case 0x50:
			if (static_cast <const MCHEmul::PulseSoundWave*> 
					(wave (MCHEmul::SoundWave::Type::_PULSE)) -> pulseUp ())
				result = (double) _PULSETRIWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & sawtooth
		case 0x60:
			if (static_cast <const MCHEmul::PulseSoundWave*> 
					(wave (MCHEmul::SoundWave::Type::_PULSE)) -> pulseUp ())
				result = (double) _PULSESAWWAVE_6581 [(size_t) wavesClockValue ()] / 256.0f;
			break;

		// pulse & sawtooth & triangle
		case 0x70:
			if (static_cast <const MCHEmul::PulseSoundWave*> 
					(wave (MCHEmul::SoundWave::Type::_PULSE)) -> pulseUp ())
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
		result *= _envelope -> envelopeData ();

	return ((result > 1.0f) ? 1.0f : result);
}

// ---
MCHEmul::InfoStructure COMMODORE::SoundSIDSimpleWrapper::Voice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundVoice::getInfoStructure ());

	result.add ("RINGMODULATION", _ringModulation);
	result.add ("VOICERELATED", _voiceRelated -> id ());
	result.add ("SYNC", _sync);

	return (result);
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::Voice::applySync ()
{
	if (!_sync || 
		_voiceRelated == nullptr)
		return;

	// Correct direction:
	// If the related/source voice oscillator has restarted,
	// this voice is reset.
	if (_voiceRelated -> oscillatorRestarted ())
		initializeOscillatorCounters ();
}

// ---
void COMMODORE::SoundSIDSimpleWrapper::Voice::initializeOscillatorCounters ()
{
	for (auto i : _waves)
		i -> initializeInternalCounters ();

	_oscillatorRestarted = false;
}
