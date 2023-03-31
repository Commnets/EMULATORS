#include <CORE/Sound.hpp>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, 
		unsigned short tp, int sF, unsigned char nC,
		const MCHEmul::Attributes& attrs)
	: MCHEmul::IODevice (Type::_OUTPUT, id, attrs),
	  _type (tp),
	  _samplingFrequency (sF),
	  _numberChannels (nC),
	  _soundChip (nullptr),
	  _soundReady (false),
	  _audioSpec (), _deviceId (0) // Defined later...
{ 
	setClassName ("SoundSystem");

	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = _samplingFrequency;
	specIn.format = _type;
	specIn.channels = _numberChannels;
	specIn.samples = 0; // It will be determined later...
	specIn.callback = nullptr; // SDL_QueueAudio will be used intead...
	specIn.userdata = nullptr; // No callback no special data to be passed...
	_deviceId = SDL_OpenAudioDevice (nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);

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
	_soundReady = false;

	SDL_ClearQueuedAudio (_deviceId);

	return (MCHEmul::IODevice::initialize ());
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
		if (SDL_GetQueuedAudioSize (_deviceId) >= (_audioSpec.size * 5))
			SDL_ClearQueuedAudio (_deviceId); // If the data queued is too much...it is taken off!

		int tC = 0;
		SDL_AudioCVT cvt;
		SDL_memset (&cvt, 0, sizeof (cvt));
		result = (tC = SDL_BuildAudioCVT (&cvt, AUDIO_S16, 1, 44100, _audioSpec.format, _audioSpec.channels, _audioSpec.freq)) >= 0;
		if (!result) // And error has happened when conversion is planned...
			std::cout << SDL_GetError () << std::endl;
		else
		{
			if (tC != 0) // Conversion is needed...
			{
				cvt.len = (int) _soundChip -> soundMemory () -> size ();
				int cvt_lencvt = cvt.len * cvt.len_mult;
				cvt.buf = (Uint8*) SDL_malloc (cvt_lencvt);
				memcpy (cvt.buf, _soundChip -> soundMemory () -> samplingData (), cvt.len);
				SDL_ConvertAudio (&cvt);
				if (!(result = SDL_QueueAudio (_deviceId, (void*) cvt.buf, cvt_lencvt) != -1))
					std::cout << SDL_GetError () << std::endl;
				SDL_free (cvt.buf);
			}
			else // No conversion is needed...
				if (!(result = SDL_QueueAudio (_deviceId, (void*) _soundChip -> soundMemory () -> samplingData (), 
					(Uint32) _soundChip -> soundMemory () -> size ()) != -1))
					std::cout << SDL_GetError () << std::endl;
		}

		_soundReady = false;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::IODevice::getInfoStructure ();

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
