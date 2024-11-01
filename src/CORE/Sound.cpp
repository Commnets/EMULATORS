#include <CORE/Sound.hpp>
#include <CORE/LogChannel.hpp>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, 
		unsigned short tp, int sF, unsigned char nC,
		const MCHEmul::Attributes& attrs)
	: MCHEmul::IODevice (Type::_OUTPUT, id, attrs),
	  _type (tp),
	  _samplingFrequency (sF),
	  _numberChannels (nC),
	  _silence (false), // By default it always sound!
	  _soundChip (nullptr),
	  _soundReady (false),
	  _audioSpec (), _deviceId (0), // Defined later...
	  _conversionNeeded (false),
	  _conversionData () // Set when the system is initialized (becuase is when the sound chip will be finally linked)...
{ 
	setClassName ("SoundSystem");

	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = _samplingFrequency;
	specIn.format = _type;
	specIn.channels = (Uint8) _numberChannels;
	specIn.callback = nullptr; // SDL_QueueAudio will be used intead...
	specIn.userdata = nullptr; // No callback no special data to be passed...
	_deviceId = SDL_OpenAudioDevice (nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	
	// After openning the device the initial variables can be changed...
	bool changed = false;
	if (_audioSpec.freq != _samplingFrequency) { _samplingFrequency = _audioSpec.freq; changed = true; }
	if (_audioSpec.format != _type) { _type = _audioSpec.format; changed = true; }
	if (_audioSpec.channels != _numberChannels) { _numberChannels = _audioSpec.channels; changed = true; }
	if (changed)
		_LOG ("Original sound format has been changed");

	SDL_PauseAudioDevice (_deviceId, 0); // 0 to start...
}

// ---
MCHEmul::SoundSystem::~SoundSystem ()
{
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

	SDL_ClearQueuedAudio (_deviceId);

	SDL_memset (&_conversionData, 0, sizeof (_conversionData));
	int tC = SDL_BuildAudioCVT 
			(&_conversionData, 
			 _soundChip -> type (), (Uint8) _soundChip -> numberChannels (), _soundChip -> samplingFrecuency (),
			 _type, _numberChannels, _samplingFrequency);

	result = (tC >= 0);

	_conversionNeeded = (tC != 0);

	// If the conversion action is not possible the initialization doesn't progress...
	return (result ? MCHEmul::IODevice::initialize () : result);
}

// ---
bool MCHEmul::SoundSystem::simulate (MCHEmul::CPU* cpu)
{
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

			if (_conversionNeeded)
			{
				_conversionData.len = _soundChip -> soundBufferSize ();
				int cvt_lencvt = _conversionData.len * _conversionData.len_mult;
				_conversionData.buf = (Uint8*) SDL_malloc (cvt_lencvt);
				memcpy ((void*) _conversionData.buf, (void*) _soundChip -> soundMemory () -> samplingData (), _conversionData.len);
				SDL_ConvertAudio (&_conversionData);
				result = (SDL_QueueAudio (_deviceId, (void*) _conversionData.buf, cvt_lencvt) != -1);
				SDL_free (_conversionData.buf);
			}
			else
			{
				result = (SDL_QueueAudio (_deviceId, (void*) _soundChip -> soundMemory () -> samplingData (), 
							(Uint32) _soundChip -> soundBufferSize ()) != -1);
			}
		}

		_soundReady = false;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IODevice::getInfoStructure ());

	result.add ("DATA", _type);
	result.add ("FREQUENCY", std::to_string ((double) _samplingFrequency / 1000.0f));
	result.add ("CHANNELS", _numberChannels);

	return (result);
}

// ---
void MCHEmul::SoundSystem::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::SoundChip::_SOUNDREADY)
		_soundReady = true; // To be processed in simulation...
}
