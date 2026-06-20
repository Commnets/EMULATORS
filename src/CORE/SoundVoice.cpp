#include <CORE/SoundVoice.hpp>

// ---
MCHEmul::SoundVoice::SoundVoice (int id, unsigned int cF, 
		const MCHEmul::SoundWaves& sw, 
		MCHEmul::SoundEnvelope* sE, 
		const MCHEmul::SoundFilters& sF)
	: MCHEmul::InfoClass ("SoundVoice"),
	  _id (id),
	  _chipFrequency (cF),
	  _active (false), // No active by default...
	  _envelope (sE),
	  _filters (sF),
	  _waves (sw),
	  _wavesPlain (nullptr)
{ 
	// Plain the waves vector to facilitate later looking for them...
	size_t mW = 0;
	for (const auto& i : _waves) 
		if ((size_t) i -> type () > mW) mW = (size_t) i -> type ();
	_wavesPlain = new MCHEmul::SoundWave* [mW + 1];
	for (size_t i = 0; i <= mW; ++i) _wavesPlain [i] = nullptr;
	for (const auto& i : _waves) _wavesPlain [(size_t) i -> type ()] = i;
}

// ---
MCHEmul::SoundVoice::~SoundVoice ()
{ 
	for (auto i : _waves) 
		delete (i); 

	delete [] _wavesPlain; // Delete the plain waves...

	// Delete the envelope...
	delete (_envelope);

	// Delete the filters...
	for (auto i : _filters)
		delete (i);
}

// ---
void MCHEmul::SoundVoice::initialize ()
{ 
	setActive (false); // By default...
	
	for (const auto& i : _waves) 
		i -> initialize (); 

	if (_envelope != nullptr)
		_envelope -> initialize ();

	for (const auto& i: _filters)
		i -> initialize ();
}

// ---
void MCHEmul::SoundVoice::initializeInternalCounters ()
{ 
	if (_envelope != nullptr) 
		_envelope -> initializeInternalCounters ();

	// Notice that the filter doesn't have any internal counter to be initialized, 
	// so it is not invoked here.

	// The waves are initialized too...
	for (auto i : _waves) 
		i -> initializeInternalCounters ();
}

// ---
void MCHEmul::SoundVoice::clock (unsigned int nC)
{ 
	for (const auto& i : _waves)
		i -> clock (nC);

	if (_envelope != nullptr)
		_envelope -> clock (nC);
}

// ---
double MCHEmul::SoundVoice::data () const
{ 
	if (!_active)
		return (0.0f);

	double result = 
		wavesData () * 
		((_envelope != nullptr && _envelope -> active ()) 
			? _envelope -> envelopeData () : 1.0);

	for (const auto& i : _filters)
		if (i != nullptr /** Just in case, althought it shouldn't. */ && i -> active ())
			result = i -> process (result);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundVoice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID", _id);
	result.add ("ACTIVE", _active);
	result.add ("SoundEnvelope", std::move ((_envelope != nullptr) 
		? _envelope -> getInfoStructure () : MCHEmul::InfoStructure ()));	// Empty when there is no envelope...

	size_t ct = 0;
	MCHEmul::InfoStructure fDt;
	for (const auto& i : _filters)
		fDt.add (std::to_string (ct++), std::move (i -> getInfoStructure ()));
	result.add ("FILTERS", fDt);

	ct = 0;
	MCHEmul::InfoStructure wDt;
	for (auto i : _waves)
		wDt.add (std::to_string (ct++), std::move (i -> getInfoStructure ()));
	result.add ("WAVES", wDt);

	return (result);
}

// ---
double MCHEmul::SoundVoice::wavesData () const
{
	double result = 0.0; 
			
	size_t nAW = 0; // Number of active waves...
	for (auto i : _waves) 
	{
		if (i -> active ()) // Only when the wave active...
		{
			result += i -> data (); // Pure adding...

			nAW++;
		}
	}

	// To have a number between 0 and 1.0, 
	// as the data of each wave is between 0 and 1.0.
	result /= (nAW == 0) ? 1.0f : (double) nAW; 

	// It can not be bigger that 1.0f...
	// ...it should be, but just in cese, 
	// to avoid problems with the sound output, it is limited to 1.0f.
	return ((result > 1.0f) ? 1.0f : result);
}
