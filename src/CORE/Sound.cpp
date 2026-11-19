#include <CORE/Sound.hpp>
#include <CORE/LogChannel.hpp>
#include <cmath>
#include <limits>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, 
		SDL_AudioFormat tp, int sF, unsigned char nC,
		const MCHEmul::Attributes& attrs, double mADelay)
	: MCHEmul::IODevice (Type::_OUTPUT, id, attrs),
	  _type (tp),
	  _samplingFrequency (sF),
	  _numberChannels (nC),
	  _silence (false), // By default it always sound!
	  _soundChip (nullptr),
	  _soundReady (false),
	  _deviceId (0),
	  _audioSpec (),
#if MCHEMUL_SDL_VERSION == 2
	  _conversionNeeded (false),
	  _conversionData (),
#elif MCHEMUL_SDL_VERSION == 3
	  _conversionStream (nullptr),
	  _playbackStream (nullptr),
#endif
	  _conversionBuffer (),
	  _normalBufferCapacity (0),
	  _maximumQueuedAudioBytes (0),
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

#if MCHEMUL_SDL_VERSION == 2
	bool result = openAudioSDL2 ();
#elif MCHEMUL_SDL_VERSION == 3
	bool result = openAudioSDL3 ();
#endif
	if (!result)
	{
		_error = MCHEmul::_INIT_ERROR;

		_LOG ("Failed to open audio device");
	}
}

// ---
MCHEmul::SoundSystem::~SoundSystem ()
{
#if MCHEMUL_SDL_VERSION == 2
	if (_deviceId != 0)
		SDL_CloseAudioDevice (_deviceId);
#elif MCHEMUL_SDL_VERSION == 3
	if (_conversionStream != nullptr)
		SDL_DestroyAudioStream (_conversionStream);
	if (_playbackStream != nullptr)
		SDL_DestroyAudioStream (_playbackStream);
#endif
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
	_soundReady = false;
	// A new execution must not inherit timestamps or a pending compensation decision.
	_lastSoundReadyCounter = 0;
	_lastSoundReadyElapsedCounters = 0;
	_soundReadyDelayCounters = 0;
	_additionalSoundData = 0;
	_soundReadyDelayed = false;
	_addAdditionalSoundData = false;
	_nominalSoundReadyCounters = 0;
	_maximumAllowedDelayCounters = 0;
	_normalBufferCapacity = 0;
	_maximumQueuedAudioBytes = 0;
	_outputFrameSize = 0;
	_maximumAdditionalFrames = 0;

	if (_deviceId == 0 || _soundChip == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_performanceCounterFrequency =
		(unsigned long long) (SDL_GetPerformanceFrequency ());
	assert (_performanceCounterFrequency != 0);

	const size_t sourceFrameSize =
		(size_t) _soundChip -> sampleSize () *
		(size_t) _soundChip -> numberChannels ();
	assert (sourceFrameSize != 0);
	assert (_soundChip -> soundBufferSize () > 0);
	assert (((size_t) _soundChip -> soundBufferSize () % sourceFrameSize) == 0);
	assert (_soundChip -> samplingFrecuency () > 0);

	// Both backends use the duration represented by one complete chip block.
	const double nominalSoundReadyTime =
		((double) _soundChip -> soundBufferSize () / (double) sourceFrameSize) /
		(double) _soundChip -> samplingFrecuency ();
	// Keep runtime delay calculations in high-resolution counter units.
	_nominalSoundReadyCounters = (unsigned long long) (std::ceil (
		nominalSoundReadyTime * (double) _performanceCounterFrequency));
	_maximumAllowedDelayCounters = (unsigned long long) (std::ceil (
		nominalSoundReadyTime * _maxAllowedDelay *
		(double) _performanceCounterFrequency));
	assert (_nominalSoundReadyCounters != 0);

	_outputFrameSize =
		(size_t) (SDL_AUDIO_BITSIZE (_type) >> 3) *
		(size_t) _numberChannels;
	assert (_outputFrameSize != 0);
	const double maximumAdditionalFrames = std::ceil (
		nominalSoundReadyTime * (double) _samplingFrequency * _maxAllowedDelay);

	// SDL3's transfer lengths are signed ints. Check the full workspace before narrowing.
	const size_t maximumBufferSize = (size_t) std::numeric_limits <int>::max ();
	bool result = maximumAdditionalFrames <= (double) (maximumBufferSize / _outputFrameSize);
	if (result)
	{
		_maximumAdditionalFrames = (size_t) maximumAdditionalFrames;
#if MCHEMUL_SDL_VERSION == 2
		result = initializeAudioSDL2 (_normalBufferCapacity);
#elif MCHEMUL_SDL_VERSION == 3
		result = initializeAudioSDL3 (_normalBufferCapacity);
#endif
	}
	if (result)
		result = _normalBufferCapacity <=
			maximumBufferSize - _maximumAdditionalFrames * _outputFrameSize;

	if (result)
	{
		// The tail is reserved for compensation, never used as conversion workspace.
		_conversionBuffer.resize (
			_normalBufferCapacity + _maximumAdditionalFrames * _outputFrameSize);
#if MCHEMUL_SDL_VERSION == 2
		_conversionData.buf = _conversionBuffer.data ();
#endif
		result = MCHEmul::IODevice::initialize ();
	}

	if (!result)
	{
		_normalBufferCapacity = 0;
		_maximumQueuedAudioBytes = 0;
		_outputFrameSize = 0;
		_maximumAdditionalFrames = 0;
		_nominalSoundReadyCounters = 0;
		_maximumAllowedDelayCounters = 0;
		_conversionBuffer.clear ();
#if MCHEMUL_SDL_VERSION == 2
		_conversionData.buf = nullptr;
#elif MCHEMUL_SDL_VERSION == 3
		if (_conversionStream != nullptr)
		{
			SDL_DestroyAudioStream (_conversionStream);
			_conversionStream = nullptr;
		}
#endif
		_error = MCHEmul::_INIT_ERROR;
	}
	else
		_error = MCHEmul::_NOERROR;

	return (result);
}

// ---
bool MCHEmul::SoundSystem::simulate (MCHEmul::CPU* cpu)
{
	if (_deviceId == 0 || _normalBufferCapacity == 0)
		return (false);

	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	bool result = true;
	if (_soundReady)
	{
		if (!_silence)
		{
#if MCHEMUL_SDL_VERSION == 2
			result = queueSoundSDL2 ();
#elif MCHEMUL_SDL_VERSION == 3
			result = queueSoundSDL3 ();
#endif
		}

		_soundReady = false;
	}

	if (!result)
		_error = MCHEmul::_DEVICE_ERROR;

	return (result);
}

#if MCHEMUL_SDL_VERSION == 2
// ---
bool MCHEmul::SoundSystem::openAudioSDL2 ()
{
	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = _samplingFrequency;
	specIn.format = _type;
	specIn.channels = _numberChannels;
	specIn.callback = nullptr;
	specIn.userdata = nullptr;

	_deviceId = SDL_OpenAudioDevice (
		nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (_deviceId == 0)
		return (false);

	bool changed = false;
	if (_audioSpec.freq != _samplingFrequency)
	{
		_samplingFrequency = _audioSpec.freq;
		changed = true;
	}
	if (_audioSpec.format != _type)
	{
		_type = _audioSpec.format;
		changed = true;
	}
	if (_audioSpec.channels != _numberChannels)
	{
		_numberChannels = _audioSpec.channels;
		changed = true;
	}
	if (changed)
		_LOG ("Original sound format has been changed");

	SDL_PauseAudioDevice (_deviceId, 0);

	return (true);
}

// ---
bool MCHEmul::SoundSystem::initializeAudioSDL2 (size_t& normalBufferCapacity)
{
	SDL_ClearQueuedAudio (_deviceId);

	SDL_memset (&_conversionData, 0, sizeof (_conversionData));
	int conversion = SDL_BuildAudioCVT (
		&_conversionData,
		_soundChip -> type (), _soundChip -> numberChannels (),
		_soundChip -> samplingFrecuency (), _type, _numberChannels, _samplingFrequency);
	if (conversion < 0)
		return (false);

	_conversionNeeded = conversion != 0;
	normalBufferCapacity = (size_t) _soundChip -> soundBufferSize ();
	if (_conversionNeeded)
	{
		assert (_conversionData.len_mult > 0);
		if (normalBufferCapacity >
			(size_t) std::numeric_limits <int>::max () / (size_t) _conversionData.len_mult)
			return (false);

		normalBufferCapacity *= (size_t) _conversionData.len_mult;
	}

	if ((size_t) _audioSpec.size > std::numeric_limits <size_t>::max () / 20)
		return (false);
	_maximumQueuedAudioBytes = (size_t) _audioSpec.size * 20;

	return (true);
}

// ---
bool MCHEmul::SoundSystem::queueSoundSDL2 ()
{
	if ((size_t) SDL_GetQueuedAudioSize (_deviceId) >= _maximumQueuedAudioBytes)
		SDL_ClearQueuedAudio (_deviceId);

	_conversionData.len = _soundChip -> soundBufferSize ();
	memcpy (
		(void*) _conversionBuffer.data (),
		(const void*) _soundChip -> soundMemory () -> samplingData (),
		(size_t) _conversionData.len);

	size_t soundSize = (size_t) _conversionData.len;
	if (_conversionNeeded)
	{
		_conversionData.buf = _conversionBuffer.data ();
		if (SDL_ConvertAudio (&_conversionData) < 0)
			return (false);

		soundSize = (size_t) _conversionData.len_cvt;
	}

	soundSize = fillAdditionalSoundData (
		_conversionBuffer, soundSize, _additionalSoundData);

	return (SDL_QueueAudio (
		_deviceId, (const void*) _conversionBuffer.data (), (unsigned int) soundSize) == 0);
}
#elif MCHEMUL_SDL_VERSION == 3
// ---
bool MCHEmul::SoundSystem::openAudioSDL3 ()
{
	SDL_zero (_audioSpec);
	_audioSpec.freq = _samplingFrequency;
	_audioSpec.format = _type;
	_audioSpec.channels = _numberChannels;

	_playbackStream = SDL_OpenAudioDeviceStream (
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &_audioSpec, nullptr, nullptr);
	if (_playbackStream == nullptr)
		return (false);

	_deviceId = SDL_GetAudioStreamDevice (_playbackStream);
	if (_deviceId == 0 || !SDL_ResumeAudioStreamDevice (_playbackStream))
	{
		SDL_DestroyAudioStream (_playbackStream);
		_playbackStream = nullptr;
		_deviceId = 0;

		return (false);
	}

	return (true);
}

// ---
bool MCHEmul::SoundSystem::initializeAudioSDL3 (size_t& normalBufferCapacity)
{
	if (!SDL_ClearAudioStream (_playbackStream))
		return (false);

	if (_conversionStream != nullptr)
	{
		SDL_DestroyAudioStream (_conversionStream);
		_conversionStream = nullptr;
	}

	SDL_AudioSpec sourceSpec;
	SDL_zero (sourceSpec);
	sourceSpec.freq = _soundChip -> samplingFrecuency ();
	sourceSpec.format = _soundChip -> type ();
	sourceSpec.channels = _soundChip -> numberChannels ();

	_conversionStream = SDL_CreateAudioStream (&sourceSpec, &_audioSpec);
	if (_conversionStream == nullptr)
		return (false);

	const size_t sourceFrames = (size_t) _soundChip -> soundBufferSize () /
		((size_t) _soundChip -> sampleSize () * (size_t) _soundChip -> numberChannels ());
	const double outputFrames = std::ceil (
		(double) sourceFrames * (double) _samplingFrequency / (double) sourceSpec.freq);
	if (outputFrames > (double) ((size_t) std::numeric_limits <int>::max () / _outputFrameSize))
		return (false);
	normalBufferCapacity = (size_t) outputFrames * _outputFrameSize;

	SDL_AudioSpec deviceSpec;
	int deviceFrames = 0;
	if (!SDL_GetAudioDeviceFormat (_deviceId, &deviceSpec, &deviceFrames) ||
		deviceSpec.freq <= 0 || deviceFrames <= 0)
		return (false);

	// Express twenty hardware buffers in the playback stream's input frames.
	const double queuedFrames = std::ceil (
		20.0 * (double) deviceFrames * (double) _samplingFrequency / (double) deviceSpec.freq);
	if (queuedFrames > (double) ((size_t) std::numeric_limits <int>::max () / _outputFrameSize))
		return (false);
	_maximumQueuedAudioBytes = (size_t) queuedFrames * _outputFrameSize;

	return (true);
}

// ---
bool MCHEmul::SoundSystem::queueSoundSDL3 ()
{
	int queued = SDL_GetAudioStreamQueued (_playbackStream);
	if (queued < 0)
		return (false);

	if ((size_t) queued >= _maximumQueuedAudioBytes)
	{
		// Drop both stages so pending conversion cannot reintroduce old audio.
		if (!SDL_ClearAudioStream (_playbackStream))
			return (false);
		if (!SDL_ClearAudioStream (_conversionStream))
			return (false);
	}

	if (!SDL_PutAudioStreamData (_conversionStream,
			(const void*) _soundChip -> soundMemory () -> samplingData (),
			_soundChip -> soundBufferSize ()))
		return (false);

	// Resampling may yield no output yet, or more than one buffer-sized portion.
	for (;;)
	{
		int available = SDL_GetAudioStreamAvailable (_conversionStream);
		if (available < 0)
			return (false);
		if (available == 0)
			return (true);

		size_t requested = (size_t) available;
		if (requested > _normalBufferCapacity)
			requested = _normalBufferCapacity;
		requested -= requested % _outputFrameSize;
		if (requested == 0)
			return (true);

		int received = SDL_GetAudioStreamData (
			_conversionStream, (void*) _conversionBuffer.data (), (int) requested);
		if (received < 0)
			return (false);
		if (received == 0)
			return (true);

		assert (((size_t) received % _outputFrameSize) == 0);
		size_t soundSize = fillAdditionalSoundData (
			_conversionBuffer, (size_t) received, _additionalSoundData);
		// SDL copies this data, allowing immediate reuse of the conversion buffer.
		if (!SDL_PutAudioStreamData (
				_playbackStream, (const void*) _conversionBuffer.data (), (int) soundSize))
			return (false);
	}
}
#endif

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
