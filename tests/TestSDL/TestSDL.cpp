#include "stdafx.h"

#include <CORE/SDL.hpp>
#include <CORE/SDLImage.hpp>
#include <atomic>
#include <iostream>

#define FREQUENCY 44100

#if MCHEMUL_SDL_VERSION == 2
#define FORMAT (SDL_AudioFormat) (AUDIO_F32SYS)
#define INPUTFORMAT AUDIO_S8
#elif MCHEMUL_SDL_VERSION == 3
#define FORMAT SDL_AUDIO_F32
#define INPUTFORMAT SDL_AUDIO_S8
#endif

#define CHANNELS 2

static std::atomic <int> _maxCount { 0 };

#if MCHEMUL_SDL_VERSION == 2
static SDL_AudioSpec _audioSpec;
static SDL_AudioDeviceID _deviceId = 0;
#endif

static char _getDataSin ()
{
	static double ANGLE = 0.0f;
	static int COUNT = 0;
	static char data = 0;
	
	if (++COUNT > _maxCount)
	{
		COUNT = 0;
		ANGLE += 0.1f;
		data = (char) (std::sin (ANGLE) /** between -1 and 1. */ * 
			(double) (std::numeric_limits <char>::max () - 1));
	}

	return (data);
}

static char _getDataSquare ()
{
	static int status = 0;
	static int COUNT = 0;
	static char data = 0;

	if (++COUNT > _maxCount)
	{
		COUNT = 0;
		data = (status == 0) ? 0 : ((status == 1) ? 100 : -100);
		if (status++ > 2) status = 0;
	}

	return (data);
}

static int _getDataSquare2 ()
{
	static int status = 0;
	static int COUNT = 0;
	static int data = 0;

	if (++COUNT > _maxCount)
	{
		COUNT = 0;
		data = (status == 0) ? 0 : ((status == 1) ? 12800 : -12800);
		if (status++ > 2) status = 0;
	}

	return (data);
}

static char _getDataTriangle ()
{
	static int status = 0;
	static int COUNT = 0;
	static char data = 0;

	if (++COUNT > _maxCount)
	{
		COUNT = 0;
		if (status == 0) { data = 0; status++; }
		else if (status == 1) { if (++data > 100) status = 2; }
		else if (status == 2) { if (--data < 0) status = 0; }
	}

	return (data);
}

#if MCHEMUL_SDL_VERSION == 2
void _audio_callback (void* userdata, Uint8* stream, int len)
{
	char* dC = new char [_audioSpec.samples];
//	int* dC = new int [_audioSpec.samples];
//	for (int i = 0; i < _audioSpec.samples; dC [i++] = _getDataSin ());
//	for (int i = 0; i < _audioSpec.samples; dC [i++] = _getDataSquare ());
	for (int i = 0; i < _audioSpec.samples; dC [i++] = _getDataSquare2 () >> 8);
//	for (int i = 0; i < _audioSpec.samples; dC [i++] = _getDataTriangle ());

	// The conversion...
	int tC = 0;
	SDL_AudioCVT cvt;
	SDL_memset (&cvt, 0, sizeof (cvt));
	tC = SDL_BuildAudioCVT (&cvt, INPUTFORMAT, 1, 44100, FORMAT, CHANNELS, FREQUENCY);
	if (tC < 0)
	{ 
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

	// To copy it into the buffer...
	cvt.len = _audioSpec.samples;
	int cvt_lencvt = cvt.len * cvt.len_mult;
	cvt.buf = (Uint8*) SDL_malloc (cvt_lencvt);
	memcpy ((void*) cvt.buf, (void*) dC, cvt.len);
	SDL_ConvertAudio (&cvt);
	memcpy ((void*) stream, (void*) cvt.buf, len);
	SDL_free (cvt.buf);

	delete [](dC);
}

#endif

void testAudio ()
{
#if MCHEMUL_SDL_VERSION == 2
	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
#elif MCHEMUL_SDL_VERSION == 3
	if (!SDL_Init (SDL_INIT_AUDIO))
#endif
	{
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

	_maxCount = 5;

#if MCHEMUL_SDL_VERSION == 2
	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = FREQUENCY;
	specIn.format = FORMAT;
	specIn.channels = CHANNELS;
	specIn.callback = _audio_callback;
	specIn.userdata = nullptr;

	_deviceId = SDL_OpenAudioDevice (
		nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (!_deviceId)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_Quit ();

		exit (1);
	}

	bool changed =
		(_audioSpec.freq != FREQUENCY) ||
		(_audioSpec.format != FORMAT) ||
		(_audioSpec.channels != CHANNELS);
	if (changed)
		std::cout << "Original sound format has been changed" << std::endl;

	SDL_PauseAudioDevice (_deviceId, 0);
#elif MCHEMUL_SDL_VERSION == 3
	std::atomic <bool> audioFailed { false };

	auto callback = [](
		void* userdata, SDL_AudioStream* stream,
		int additionalAmount, int totalAmount) -> void
		{
			std::atomic <bool>* failed =
				static_cast <std::atomic <bool>*> (userdata);

			if (failed -> load ())
				return;

			// The stream receives signed 8-bit mono samples.
			// Feed bounded blocks without allocating a buffer per callback.
			char samples [1024];
			while (additionalAmount > 0)
			{
				int count = (additionalAmount > (int) (sizeof (samples)))
					? (int) (sizeof (samples)) : additionalAmount;

				for (int i = 0; i < count; i++)
					samples [i] = (char) (_getDataSquare2 () >> 8);

				if (!SDL_PutAudioStreamData (stream, samples, count))
				{
					failed -> store (true);

					return;
				}

				additionalAmount -= count;
			}
		};

	SDL_AudioSpec specIn = { };
	specIn.freq = FREQUENCY;
	specIn.format = INPUTFORMAT;
	specIn.channels = 1;

	SDL_AudioStream* stream = SDL_OpenAudioDeviceStream (
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&specIn, callback, &audioFailed);

	if (stream == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_Quit ();

		exit (1);
	}

	if (!SDL_ResumeAudioStreamDevice (stream))
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_DestroyAudioStream (stream);
		SDL_Quit ();

		exit (1);
	}
#endif

	while (_maxCount != 0)
	{
		std::cout << "?:";

		int value = 0;
		if (!(std::cin >> value))
			break;

		_maxCount = value;

#if MCHEMUL_SDL_VERSION == 3
		if (audioFailed.load ())
			break;
#endif
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_CloseAudioDevice (_deviceId);
	_deviceId = 0;
#elif MCHEMUL_SDL_VERSION == 3
	// Destruction also closes the device and finishes its callbacks.
	SDL_DestroyAudioStream (stream);

	bool failed = audioFailed.load ();
	if (failed)
		std::cout << "SDL audio stream submission failed" << std::endl;
#endif

	SDL_Quit ();

#if MCHEMUL_SDL_VERSION == 3
	if (failed)
		exit (1);
#endif
}

void testGraphics_I ()
{
	const int XSIZE = 800;
	const int YSIZE = 627;

#if MCHEMUL_SDL_VERSION == 2
	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
#elif MCHEMUL_SDL_VERSION == 3
	if (!SDL_Init (SDL_INIT_VIDEO))
#endif
	{
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_Window* window = SDL_CreateWindow (
		"Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_Window* window = SDL_CreateWindow (
		"Test", XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#endif

	if (window == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_Quit ();

		exit (1);
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_Renderer* renderer =
		SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_Renderer* renderer = SDL_CreateRenderer (window, nullptr);
#endif

	if (renderer == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_DestroyWindow (window);
		SDL_Quit ();

		exit (1);
	}

	SDL_Surface* surface = IMG_Load ("C64.jpg");
	SDL_Texture* texture1 = SDL_CreateTextureFromSurface (renderer, surface);
	// This blend mode will affect the way the texture 1 is blended with the texture 2...
	SDL_SetTextureBlendMode (texture1, SDL_BLENDMODE_ADD);
#if MCHEMUL_SDL_VERSION == 2
	SDL_FreeSurface (surface);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_DestroySurface (surface);
#endif

	SDL_Texture* texture2 = SDL_CreateTexture 
		(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, XSIZE, YSIZE);
	SDL_SetRenderTarget (renderer, texture2);
	SDL_SetRenderDrawColor (renderer, 50, 50, 50, 255);
	for (int i = 0; i < YSIZE; i += 3)
#if MCHEMUL_SDL_VERSION == 2
		SDL_RenderDrawLine (renderer, 0, i, XSIZE, i);
#elif MCHEMUL_SDL_VERSION == 3
		SDL_RenderLine (
			renderer, 0.0f, (float) (i), (float) (XSIZE), (float) (i));
#endif

	// This blend mode will affect the way the texture 2 is blended with the window...
	SDL_SetTextureBlendMode (texture2, SDL_BLENDMODE_ADD);
	SDL_SetRenderTarget (renderer, nullptr);

#if MCHEMUL_SDL_VERSION == 2
	SDL_RenderCopy (renderer, texture1, nullptr, nullptr);
	SDL_RenderCopy (renderer, texture2, nullptr, nullptr);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_RenderTexture (renderer, texture1, nullptr, nullptr);
	SDL_RenderTexture (renderer, texture2, nullptr, nullptr);
#endif
	SDL_RenderPresent (renderer);

	bool e = false;
	while (!e)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
#if MCHEMUL_SDL_VERSION == 2
			if (event.type == SDL_QUIT)
#elif MCHEMUL_SDL_VERSION == 3
			if (event.type == SDL_EVENT_QUIT)
#endif
				e = true;
		}

		SDL_Delay (1);
	}

	SDL_DestroyTexture (texture1);
	SDL_DestroyTexture (texture2);
	SDL_DestroyRenderer (renderer);
	SDL_DestroyWindow (window);

	SDL_Quit ();
}

void testGraphics_II ()
{
	const int XSIZE = 800;
	const int YSIZE = 627;

#if MCHEMUL_SDL_VERSION == 2
	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
#elif MCHEMUL_SDL_VERSION == 3
	if (!SDL_Init (SDL_INIT_VIDEO))
#endif
	{
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_PixelFormat* format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
#elif MCHEMUL_SDL_VERSION == 3
	const SDL_PixelFormatDetails* format =
		SDL_GetPixelFormatDetails (SDL_PIXELFORMAT_ARGB8888);
#endif

	if (format == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_Quit ();

		exit (1);
	}

#if MCHEMUL_SDL_VERSION == 2
	unsigned int cRed = SDL_MapRGBA (format, 0xff, 0x00, 0x00, 0xff);
	unsigned int cBlk = SDL_MapRGBA (format, 0x00, 0x00, 0x00, 0xff);
	unsigned int cWht = SDL_MapRGBA (format, 0xff, 0xff, 0xff, 0xff);
	unsigned int cGry = SDL_MapRGBA (format, 0xa0, 0xa0, 0xa0, 0xff);
#elif MCHEMUL_SDL_VERSION == 3
	unsigned int cRed = SDL_MapRGBA (format, nullptr, 0xff, 0x00, 0x00, 0xff);
	unsigned int cBlk = SDL_MapRGBA (format, nullptr, 0x00, 0x00, 0x00, 0xff);
	unsigned int cWht = SDL_MapRGBA (format, nullptr, 0xff, 0xff, 0xff, 0xff);
	unsigned int cGry = SDL_MapRGBA (format, nullptr, 0xa0, 0xa0, 0xa0, 0xff);
#endif

	unsigned int* frame = new unsigned int [YSIZE * XSIZE];
	for (size_t i = 0; i < (YSIZE * XSIZE); frame [i++] = cBlk);
	for (size_t i = 0; i < YSIZE; i++)
	{
		frame [(i * XSIZE) + i] = cRed;
		frame [((rand () % YSIZE) * XSIZE) + (rand () % XSIZE)] = cRed;
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_Window* window = SDL_CreateWindow (
		"Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_Window* window = SDL_CreateWindow (
		"Test", XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#endif

	if (window == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_Quit ();

		exit (1);
	}

#if MCHEMUL_SDL_VERSION == 2
	SDL_Renderer* renderer =
		SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_Renderer* renderer = SDL_CreateRenderer (window, nullptr);
#endif

	if (renderer == nullptr)
	{
		std::cout << SDL_GetError () << std::endl;
		SDL_DestroyWindow (window);
		SDL_Quit ();

		exit (1);
	}

	SDL_Texture* texture1 = SDL_CreateTexture 
		(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, XSIZE, YSIZE);
	SDL_SetTextureBlendMode (texture1, SDL_BLENDMODE_ADD);
	SDL_UpdateTexture (texture1, nullptr, (void*) frame, XSIZE * sizeof (unsigned int));

	SDL_Texture* texture2 = SDL_CreateTexture 
		(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, XSIZE, YSIZE);
	SDL_SetRenderTarget (renderer, texture2);
	SDL_SetRenderDrawColor (renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear (renderer);
	SDL_SetRenderDrawColor (renderer, 0xa0, 0xa0, 0xa0, 0xff);
	for (int i = 0; i < YSIZE; i += 5)
#if MCHEMUL_SDL_VERSION == 2
		SDL_RenderDrawLine (renderer, 0, i, XSIZE, i);
#elif MCHEMUL_SDL_VERSION == 3
		SDL_RenderLine (
			renderer, 0.0f, (float) (i), (float) (XSIZE), (float) (i));
#endif
	SDL_SetTextureBlendMode (texture2, SDL_BLENDMODE_ADD);
	SDL_SetRenderTarget (renderer, nullptr);

	SDL_SetRenderDrawColor (renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear (renderer);
#if MCHEMUL_SDL_VERSION == 2
	SDL_RenderCopy (renderer, texture1, nullptr, nullptr);
	SDL_RenderCopy (renderer, texture2, nullptr, nullptr);
#elif MCHEMUL_SDL_VERSION == 3
	SDL_RenderTexture (renderer, texture1, nullptr, nullptr);
	SDL_RenderTexture (renderer, texture2, nullptr, nullptr);
#endif
	SDL_RenderPresent (renderer);

	bool e = false;
	while (!e)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
#if MCHEMUL_SDL_VERSION == 2
			if (event.type == SDL_QUIT)
#elif MCHEMUL_SDL_VERSION == 3
			if (event.type == SDL_EVENT_QUIT)
#endif
				e = true;
		}

		SDL_Delay (1);
	}

	SDL_DestroyTexture (texture1);
	SDL_DestroyTexture (texture2);
	SDL_DestroyRenderer (renderer);
	SDL_DestroyWindow (window);
#if MCHEMUL_SDL_VERSION == 2
	SDL_FreeFormat (format);
#endif
	delete [] frame;

	SDL_Quit ();
}

// ---
int _tmain (int argc, _TCHAR* argv [])
{
//	testAudio ();
	
//	testGraphics_I ();

	testGraphics_II ();

	return (0);
}
