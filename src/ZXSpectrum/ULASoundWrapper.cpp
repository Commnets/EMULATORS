#include <ZXSpectrum/ULASoundWrapper.hpp>

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ULASoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundLibWrapper::getInfoStructure ());

	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 4; i++)
		vDt.add (std::to_string (i), std::move (getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}

// ---
ZXSPECTRUM::ULASoundSimpleLibWrapper::ULASoundSimpleLibWrapper (unsigned int cF, unsigned int sF)
	: ULASoundLibWrapper (
		{
			{ "Name", "ULASound" },
			{ "Programer", "Ignacio Cea" },
			{ "Year", "2025" }
		}),
	  _chipFrequency (cF), _samplingFrequency (sF),
	  // The volumen in the ULA is always 1.0f at the beginning, 
	  // but it could be change by software. The volumen in the original ULA is not changeable,
	  _volumen (1.0f), 
	  _voice (0, cF),
	  _cyclesPerSample ((double) cF / (double (sF))),
	  _counterCyclesPerSample (0.0f)
{
	// The voice has only a "plain wave" and it is always active!
	static_cast <MCHEmul::PlainSoundWave*> (_voice.wave (MCHEmul::SoundWave::Type::_PLAIN)) -> 
		setActive (true);
}

// ---
void ZXSPECTRUM::ULASoundSimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	// In this simulation there is no registers at all
	// so all of the behave on the same way: activating or desactivating the voice...
	// Remeber that the onlky wave is a plain wave, so no frequency is needed, and it always up!
	_voice.setActive (v.value () != 0x00); 
}

// ---
const MCHEmul::UByte& ZXSPECTRUM::ULASoundSimpleLibWrapper::readValue (size_t p) const
{ 
	return (_lastValueRead = (unsigned char) 
		((_voice.frequency () != 0.0f) ? 1 : 0));
}

// ---
void ZXSPECTRUM::ULASoundSimpleLibWrapper::initialize ()
{ 
	ULASoundLibWrapper::initialize ();
							  
	_volumen = 1.0f;

	_cyclesPerSample = 0.0f;
	_counterCyclesPerSample = 0.0f;
	
	_voice.initialize ();
	// Active the right wave for the voice...
	_voice.wave (MCHEmul::SoundWave::Type::_PLAIN) -> setActive (true);
}

// ---
bool ZXSPECTRUM::ULASoundSimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
{
	bool result = false;

	// If doesn't make anything actually,...
	// It is here just for consistency between other wrappers!
	_voice.clock ();

	_counterCyclesPerSample += 1.0;
	if ((result = 
			(_cyclesPerSample > 0.0f &&
			 _counterCyclesPerSample >= _cyclesPerSample)))
	{
		_counterCyclesPerSample = 
			std::fmod (_counterCyclesPerSample, _cyclesPerSample);

		dt = MCHEmul::UBytes 
			({ (unsigned char) ((_voice.data () * _volumen) * 255.0f /** between 0 and 255. */) });
	}

	return (result);
}

// ---
ZXSPECTRUM::ULASoundSimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF, 
		{ new MCHEmul::PlainSoundWave (/** No frquency needed. */) }, // Only one type of wave possible!
		nullptr) // There is no envelope around this sound!
{ 
	setClassName ("ULAVoice");
}

// ---
double ZXSPECTRUM::ULASoundSimpleLibWrapper::Voice::data () const
{ 
	if (!active ())
		return (0.0f); // Not active, no sound!

	return (wave (MCHEmul::SoundWave::Type::_PLAIN) -> data ());
}
