#include <CORE/Sound.hpp>

// All initialized when SoiundSystem is created...
static bool SOUND_bufferEmpty = true;
static int SOUND_maxLenBuffer = 0;		
static Uint8* SOUND_sBuffer = nullptr;	
static int SOUND_posReadBuffer = 0;
static int SOUND_posWriteBuffer = 0;

static void SoundSystem_audioCallback (void* userData, Uint8* stream, int len)
{
	if (SOUND_bufferEmpty || (SOUND_posReadBuffer + len) > SOUND_posWriteBuffer)
		memset (stream, 0, len);
	else
	{
		memcpy (stream, (void*) (SOUND_sBuffer + SOUND_posReadBuffer), len);
		if ((SOUND_posReadBuffer += len) >= SOUND_maxLenBuffer)
		{ 
			SOUND_posReadBuffer = 0;

			SOUND_bufferEmpty = true;
		}
	}
}

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
	  _audioSpec (), _deviceId (0) // Defined later...
{ 
	setClassName ("SoundSystem");

	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = _samplingFrequency;
	specIn.format = _type;
	specIn.channels = (Uint8) _numberChannels;
	specIn.callback = SoundSystem_audioCallback;
	specIn.userdata = (void*) this;
	_deviceId = SDL_OpenAudioDevice (nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	
	// After openning the device the initial variables can be changed...
	bool changed = false;
	if (_audioSpec.freq != _samplingFrequency) { _samplingFrequency = _audioSpec.freq; changed = true; }
	if (_audioSpec.format != _type) { _type = _audioSpec.format; changed = true; }
	if (_audioSpec.channels != _numberChannels) { _numberChannels = _audioSpec.channels; changed = true; }
	if (changed)
		std::cout << "Original sound format has been changed" << std::endl;

	// Iniitialize the buffer management...
	SOUND_bufferEmpty = true;
	SOUND_sBuffer = new Uint8 [SOUND_maxLenBuffer = (_audioSpec.size * 10)];
	SOUND_posReadBuffer = SOUND_posWriteBuffer = 0;

	SDL_PauseAudioDevice (_deviceId, 0); // 0 to start...
}

// ---
MCHEmul::SoundSystem::~SoundSystem ()
{
	SDL_CloseAudioDevice (_deviceId);

	delete [] SOUND_sBuffer;
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
		if (!_silence)
		{
			int tC = 0;
			SDL_AudioCVT cvt;
			SDL_memset (&cvt, 0, sizeof (cvt));
			result = (tC = SDL_BuildAudioCVT (&cvt, 
				_soundChip -> type (), (Uint8) _soundChip -> numberChannels (), _soundChip -> samplingFrecuency (),
				_type, _numberChannels, _samplingFrequency)) >= 0; // Change from the chip format to the card sound format...
			if (!result) // And error has happened when conversion is planned...
				std::cout << SDL_GetError () << std::endl;
			else
			{
				if (SOUND_posWriteBuffer >= SOUND_maxLenBuffer) 
					SOUND_posWriteBuffer = 0;

				if (tC != 0) // Conversion is needed...
				{
					cvt.len = _soundChip -> soundBufferSize ();
					int cvt_lencvt = cvt.len * cvt.len_mult;
					cvt.buf = (Uint8*) SDL_malloc (cvt_lencvt);
					std::memcpy ((void*) cvt.buf, (void*) _soundChip -> soundMemory () -> samplingData (), cvt.len);
					SDL_ConvertAudio (&cvt);

					std::memcpy ((void*) (SOUND_sBuffer + SOUND_posWriteBuffer), cvt.buf, cvt.len_cvt);
					SOUND_posWriteBuffer += cvt_lencvt;
					SOUND_bufferEmpty = false;

					SDL_free (cvt.buf);
				}
				else
					std::memcpy ((void*) (SOUND_sBuffer + SOUND_posWriteBuffer), 
						(void*) _soundChip -> soundMemory () -> samplingData (), (Uint32) _soundChip -> soundBufferSize ());
			}
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
