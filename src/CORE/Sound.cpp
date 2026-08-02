#include <CORE/Sound.hpp>
#include <CORE/LogChannel.hpp>
#include <cmath>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, 
		unsigned short tp, int sF, unsigned char nC,
		const MCHEmul::Attributes& attrs, double mADelay)
	: MCHEmul::IODevice (Type::_OUTPUT, id, attrs),
	  _type (tp),
	  _samplingFrequency (sF),
	  _numberChannels (nC),
	  _silence (false), // By default it always sound!
	  _soundChip (nullptr),
	  _soundReady (false),
	  _audioSpec (), _deviceId (0), // Defined later...
	  _conversionNeeded (false),
	  _conversionData (), // Set when the system is initialized (becuase is when the sound chip will be finally linked)...
	  _conversionBuffer (),
	  _maxAllowedDelay (mADelay),
	  _outputFrameSize (0),
	  _maximumAdditionalFrames (0),
	  _performanceCounterFrequency (0),
	  _nominalSoundReadyCounters (0),
	  _maximumAllowedDelayCounters (0),
	  _lastSoundReadyCounter (0),
	  _lastSoundReadyElapsedCounters (0),
	  _soundReadyDelayCounters (0),
	  _additionalSoundData (0),
	  _soundReadyDelayed (false),
	  _addAdditionalSoundData (false)
{
	assert (_maxAllowedDelay >= 0.0 &&
			_maxAllowedDelay <= 0.1); // A maximum of a 10% delay is allowed in the buffer.

	setClassName ("SoundSystem");

	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = _samplingFrequency;
	specIn.format = _type;
	specIn.channels = (unsigned char) _numberChannels;
	specIn.callback = nullptr; // SDL_QueueAudio will be used instead...
	specIn.userdata = nullptr; // No callback no special data to be passed...
	if ((_deviceId = // When the device is opened the parameters can be changed by the sound card driver...
			SDL_OpenAudioDevice (nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE)) != 0) // ..and cannot be 0!
	{	
		bool changed = false;
		if (_audioSpec.freq != _samplingFrequency) { _samplingFrequency = _audioSpec.freq; changed = true; }
		if (_audioSpec.format != _type) { _type = _audioSpec.format; changed = true; }
		if (_audioSpec.channels != _numberChannels) { _numberChannels = _audioSpec.channels; changed = true; }
		if (changed)
			_LOG ("Original sound format has been changed");

		SDL_PauseAudioDevice (_deviceId, 0); // 0 to start...
	}
	else
		_LOG ("Failed to open audio device");
}

// ---
MCHEmul::SoundSystem::~SoundSystem ()
{
	if (_deviceId != 0)
		SDL_CloseAudioDevice (_deviceId);
}

// ---
void MCHEmul::SoundSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end () && _soundChip == nullptr; 
		_soundChip = dynamic_cast <MCHEmul::SoundChip*> ((*i++).second));
	// If there is sound device it is mandatory to have a sound chip attached...
	assert (_soundChip != nullptr);

	// The events from the sound chip must be observed to actualize the sound system...
	observe (_soundChip);
}

// ---
bool MCHEmul::SoundSystem::initialize ()
{
	bool result = false;

	_soundReady = false;
	// A new execution cannot use the timestamp or the pending decision from a
	// previous one. The first new sound-ready event will only establish a baseline.
	_lastSoundReadyCounter = 0;
	_lastSoundReadyElapsedCounters = 0;
	_soundReadyDelayCounters = 0;
	_additionalSoundData = 0;
	_soundReadyDelayed = false;
	_addAdditionalSoundData = false;
	_nominalSoundReadyCounters = 0;
	_maximumAllowedDelayCounters = 0;
	_performanceCounterFrequency =
		static_cast <unsigned long long> (SDL_GetPerformanceFrequency ());
	assert (_performanceCounterFrequency != 0);

	SDL_ClearQueuedAudio (_deviceId);

	SDL_memset (&_conversionData, 0, sizeof (_conversionData));
	int tC = SDL_BuildAudioCVT 
			(&_conversionData, 
			 _soundChip -> type (), (unsigned char) _soundChip -> numberChannels (), _soundChip -> samplingFrecuency (),
			 _type, _numberChannels, _samplingFrequency);

	result = (tC >= 0);

	_conversionNeeded = (tC != 0);

	size_t normalBufferCapacity =
		(size_t) _soundChip -> soundBufferSize ();
	if (result && _conversionNeeded)
	{
		assert (_conversionData.len_mult > 0);

		normalBufferCapacity *= (size_t) _conversionData.len_mult;
	}

	if (result)
	{
		const size_t sourceFrameSize =
			(size_t) _soundChip -> sampleSize () *
			(size_t) _soundChip -> numberChannels ();
		assert (sourceFrameSize != 0);
		assert (((size_t) _soundChip -> soundBufferSize () % sourceFrameSize) == 0);
		assert (_soundChip -> samplingFrecuency () > 0);

		// This is the actual duration represented by a complete source block. It is
		// normally 10 ms, but deriving it avoids assuming a fixed buffer duration.
		const double nominalSoundReadyTime =
			(((double) _soundChip -> soundBufferSize () / (double) sourceFrameSize) /
			 (double) _soundChip -> samplingFrecuency ());
		// Runtime comparisons remain in counter units: the conversion from seconds
		// and the rounding cost are therefore paid only once during initialization.
		_nominalSoundReadyCounters =
			static_cast <unsigned long long> (std::ceil (
				nominalSoundReadyTime * (double) _performanceCounterFrequency));
		_maximumAllowedDelayCounters =
			static_cast <unsigned long long> (std::ceil (
				nominalSoundReadyTime * _maxAllowedDelay *
				(double) _performanceCounterFrequency));
		assert (_nominalSoundReadyCounters != 0);

		// The extra capacity is expressed in complete output frames. This keeps the
		// reserved area independent of the sample format and the number of channels.
		_outputFrameSize =
			(size_t) (SDL_AUDIO_BITSIZE (_type) >> 3) *
			(size_t) _numberChannels;
		assert (_outputFrameSize != 0);
		_maximumAdditionalFrames = (size_t) std::ceil (
			nominalSoundReadyTime * (double) _samplingFrequency * _maxAllowedDelay);

		// SDL requires the original size multiplied by len_mult for a conversion.
		// The additional area follows that workspace and remains unused for now.
		_conversionBuffer.resize (
			normalBufferCapacity + (_maximumAdditionalFrames * _outputFrameSize));
		_conversionData.buf = _conversionBuffer.data ();
	}
	else
	{
		_outputFrameSize = 0;
		_maximumAdditionalFrames = 0;
		_conversionBuffer.clear ();
		_conversionData.buf = nullptr;
	}

	// If the conversion action is not possible the initialization doesn't progress...
	return (result ? MCHEmul::IODevice::initialize () : result);
}

// ---
bool MCHEmul::SoundSystem::simulate (MCHEmul::CPU* cpu)
{
	if (_deviceId == 0)
		return (false);

	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	// When a chunck of sound is ready to be sent to the sound card...
	bool result = true;
	if (_soundReady)
	{
		if (!_silence)
		{
 			if (SDL_GetQueuedAudioSize (_deviceId) >= (_audioSpec.size * 20))
				SDL_ClearQueuedAudio (_deviceId); // If the data queued is too much...it is taken off!

			// Both converted and already compatible data use the same reusable buffer.
			// Only the nominal valid size is queued; the reserved tail is untouched.
			_conversionData.len = _soundChip -> soundBufferSize ();
			memcpy ((void*) _conversionBuffer.data (),
				(const void*) _soundChip -> soundMemory () -> samplingData (),
				(size_t) _conversionData.len);
			size_t soundSize = (size_t) _conversionData.len;

			if (_conversionNeeded)
			{
				_conversionData.buf = _conversionBuffer.data ();
				if (SDL_ConvertAudio (&_conversionData) < 0)
					result = false;
				else
					soundSize = (size_t) _conversionData.len_cvt;
			}

			if (result)
			{
				soundSize = fillAdditionalSoundData (
					_conversionBuffer, soundSize, _additionalSoundData);
				result = (SDL_QueueAudio (_deviceId,
					(const void*) _conversionBuffer.data (), (unsigned int) soundSize) != -1);
			}
		}

		_soundReady = false;
	}

	return (result);
}

// ---
size_t MCHEmul::SoundSystem::fillAdditionalSoundData (
	std::vector <unsigned char>& soundBuffer,
	size_t soundSize,
	size_t additionalData)
{
	// This is the usual path. No buffer contents or sizes are modified unless
	// processEvent () marked the last delay as compensable.
	if (!_addAdditionalSoundData || additionalData == 0)
		return (soundSize);

	// TO BE IMPLEMENTED: The following checks are not strictly necessary, but they
	assert (_outputFrameSize != 0);
	assert ((soundSize % _outputFrameSize) == 0);
	assert ((additionalData % _outputFrameSize) == 0);
	assert (soundSize <= soundBuffer.size ());
	assert (additionalData <= (soundBuffer.size () - soundSize));

	// TODO

	// The calculated compensation belongs only to the sound block now queued.
	// Consuming it here prevents it from being applied again to a later block.
	_additionalSoundData = 0;
	_addAdditionalSoundData = false;

	return (soundSize);
}

MCHEmul::InfoStructure MCHEmul::SoundSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IODevice::getInfoStructure ());

	result.add ("DATA",			_type);
	result.add ("FREQUENCY",	std::to_string ((double) _samplingFrequency / 1000.0f));
	result.add ("CHANNELS",		_numberChannels);

	return (result);
}

// ---
void MCHEmul::SoundSystem::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::SoundChip::_SOUNDREADY)
	{
		const unsigned long long currentCounter =
			static_cast <unsigned long long> (SDL_GetPerformanceCounter ());

		// These values always describe only the interval ending at the event just
		// received. No stale correction may survive an early or unsupported interval.
		_lastSoundReadyElapsedCounters = 0;
		_soundReadyDelayCounters = 0;
		_additionalSoundData = 0;
		_soundReadyDelayed = false;
		_addAdditionalSoundData = false;

		// The first event has no preceding timestamp. After initialization succeeds,
		// all later events can be compared directly in high-resolution counter units.
		if (_lastSoundReadyCounter != 0 &&
			_performanceCounterFrequency != 0 &&
			_nominalSoundReadyCounters != 0)
		{
			_lastSoundReadyElapsedCounters =
				currentCounter - _lastSoundReadyCounter;
			if (_lastSoundReadyElapsedCounters > _nominalSoundReadyCounters)
			{
				_soundReadyDelayed = true;
				_soundReadyDelayCounters =
					_lastSoundReadyElapsedCounters - _nominalSoundReadyCounters;
				if (_soundReadyDelayCounters <= _maximumAllowedDelayCounters)
				{
					// Integer ceiling of delaySeconds * outputFrequency. It avoids
					// floating-point work in the event path and produces complete frames.
					const unsigned long long additionalFramesNumerator =
						_soundReadyDelayCounters *
						static_cast <unsigned long long> (_samplingFrequency);
					const size_t additionalFrames = static_cast <size_t> (
						(additionalFramesNumerator + _performanceCounterFrequency - 1) /
						_performanceCounterFrequency);

					// The time and frame checks together guarantee that a later consumer
					// can never address beyond the area reserved in initialize ().
					if (additionalFrames <= _maximumAdditionalFrames)
					{
						_additionalSoundData = additionalFrames * _outputFrameSize;
						_addAdditionalSoundData = (_additionalSoundData != 0);
					}
				}
			}
		}

		_lastSoundReadyCounter = currentCounter;
		_soundReady = true; // To be processed in simulation...
	}
}
