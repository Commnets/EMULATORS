#include "stdafx.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#define FREQUENCY 44100
#define FORMAT (SDL_AudioFormat)AUDIO_F32SYS
#define CHANNELS (Uint8)2

static int _maxCount = 0;
static SDL_AudioSpec _audioSpec;
static int _deviceId = 0;

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
	tC = SDL_BuildAudioCVT (&cvt, AUDIO_S8, 1, 44100, FORMAT, CHANNELS, FREQUENCY);
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

void testAudio ()
{
	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
	{ 
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

	SDL_AudioSpec specIn;
	SDL_zero (specIn);
	specIn.freq = FREQUENCY;
	specIn.format = FORMAT;
	specIn.channels = CHANNELS;
	specIn.callback = _audio_callback; 
	specIn.userdata = nullptr; 
	_deviceId = SDL_OpenAudioDevice (nullptr, 0, &specIn, &_audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	
	if (!_deviceId)
	{
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}
	
	bool changed = 
		(_audioSpec.freq != FREQUENCY) ||
		(_audioSpec.format != FORMAT)  ||
		(_audioSpec.channels != CHANNELS);
	if (changed)
		std::cout << "Original sound format has been changed" << std::endl;

	SDL_PauseAudioDevice (_deviceId, 0); // 0 to start...

	_maxCount = 5;
	while (_maxCount != 0)
	{
		std::cout << "?:";
		std::cin >> _maxCount;
	}

	SDL_Quit ();
}

void testGraphics_I ()
{
	const int XSIZE = 800;
	const int YSIZE = 627;

	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
	{ 
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

	SDL_Window* window = SDL_CreateWindow ("Test", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface* surface = IMG_Load ("C64.jpg");
	SDL_Texture* texture1 = SDL_CreateTextureFromSurface (renderer, surface);
	// This blend mode will affect the way the texture 1 is blended with the texture 2...
	SDL_SetTextureBlendMode (texture1, SDL_BLENDMODE_ADD);
	SDL_FreeSurface (surface);

	SDL_Texture* texture2 = SDL_CreateTexture 
		(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, XSIZE, YSIZE);
	SDL_SetRenderTarget (renderer, texture2);
	SDL_SetRenderDrawColor (renderer, 50, 50, 50, 255);
	for (int i = 0; i < YSIZE; i += 3)
		SDL_RenderDrawLine (renderer, 0, i, XSIZE, i);

	// This blend mode will affect the way the texture 2 is blended with the window...
	SDL_SetTextureBlendMode (texture2, SDL_BLENDMODE_ADD);
	SDL_SetRenderTarget (renderer, nullptr);

	SDL_RenderCopy (renderer, texture1, nullptr, nullptr);
	SDL_RenderCopy (renderer, texture2, nullptr, nullptr);
	SDL_RenderPresent (renderer);

	bool e = false;
	while (!e)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					e = true;
					break;

				default:
					break;
			}
		}
	}

	SDL_DestroyTexture (texture1);
	SDL_DestroyTexture (texture2);
	SDL_DestroyRenderer (renderer);

	SDL_Quit ();
}

void testGraphics_II ()
{
	const int XSIZE = 800;
	const int YSIZE = 627;

	if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
	{ 
		std::cout << SDL_GetError () << std::endl;

		exit (1);
	}

	SDL_PixelFormat* format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
	unsigned int cRed = SDL_MapRGBA (format, 0xff, 0x00, 0x00, 0xff);
	unsigned int cBlk = SDL_MapRGBA (format, 0x00, 0x00, 0x00, 0xff);
	unsigned int cWht = SDL_MapRGBA (format, 0xff, 0xff, 0xff, 0xff);
	unsigned int cGry = SDL_MapRGBA (format, 0xa0, 0xa0, 0xa0, 0xff);

	unsigned int* frame = new unsigned int [YSIZE * XSIZE];
	for (size_t i = 0; i < (YSIZE * XSIZE); frame [i++] = cBlk);
	for (size_t i = 0; i < YSIZE; i++)
	{
		frame [(i * XSIZE) + i] = cRed;
		frame [((rand () % YSIZE) * XSIZE) + (rand () % XSIZE)] = cRed;
	}

	SDL_Window* window = SDL_CreateWindow ("Test", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, XSIZE, YSIZE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);

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
		SDL_RenderDrawLine (renderer, 0, i, XSIZE, i);
	SDL_SetTextureBlendMode (texture2, SDL_BLENDMODE_ADD);
	SDL_SetRenderTarget (renderer, nullptr);

	SDL_SetRenderDrawColor (renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, texture1, nullptr, nullptr);
	SDL_RenderCopy (renderer, texture2, nullptr, nullptr);
	SDL_RenderPresent (renderer);

	bool e = false;
	while (!e)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					e = true;
					break;

				default:
					break;
			}
		}
	}

	SDL_DestroyTexture (texture1);
	SDL_DestroyTexture (texture2);
	SDL_DestroyRenderer (renderer);
	SDL_FreeFormat (format);
	delete [] frame;

	SDL_Quit ();
}

// ---
int _tmain (int argc, _TCHAR* argv [])
{
//	testAudio ();
	
//	testGraphics_I ();

	testGraphics_II ();
}
