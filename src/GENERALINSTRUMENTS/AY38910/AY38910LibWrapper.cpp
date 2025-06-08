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
	  // There are onky 3 voices available in the AY38910,
	  // but the 4th is just used to be mixed with the rest...
	  _voices (
		{ new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (0, cF), 
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (0, cF), 
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (0, cF),
		  new GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice (1, cF), }),
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
	_voices [3] -> wave (MCHEmul::SoundWave::Type::_NOISE) -> setActive (true);
}

// ---
void GENERALINSTRUMENTS::AY38910SimpleLibWrapper::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	_registers [pp] = v;

	switch (pp)
	{
		// TODO

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
		// TODO

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
}

// ---
bool GENERALINSTRUMENTS::AY38910SimpleLibWrapper::getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt)
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
		
		// This number could be greater than 1!
		if (iR > 1.0f) iR = 1.0f; // ...so it is needed to correct.

		dt = MCHEmul::UBytes ({ (unsigned char) (iR * 255.0f /** between 0 and 255. */) });
	}

	return (result);
}

// ---
GENERALINSTRUMENTS::AY38910SimpleLibWrapper::Voice::Voice (int id, unsigned int cF)
	: MCHEmul::SoundVoice (id, cF,
		{
			new MCHEmul::SquareSoundWave (cF),
			new MCHEmul::NoiseSoundWave (cF) // This is just like a fourth wave to be mixed with the rest...
		}, nullptr), // No envelope so far...
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
