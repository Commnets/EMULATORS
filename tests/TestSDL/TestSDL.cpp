#include "stdafx.h"

#include <SDL.h>
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
//	tC = SDL_BuildAudioCVT (&cvt, AUDIO_S16, 1, 44100, FORMAT, CHANNELS, FREQUENCY);
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

int _tmain (int argc, _TCHAR* argv [])
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
