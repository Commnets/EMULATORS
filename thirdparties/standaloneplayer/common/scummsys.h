#ifndef _UTIL_H_
#define _UTIL_H_

#include "SDL.h"
#include "config.h"

const int rtSound = 4;

namespace Common {

class Mutex {
private:
	SDL_mutex *_mutex;

public:
	Mutex() {
		_mutex = SDL_CreateMutex();
	}

	~Mutex() {
		SDL_DestroyMutex(_mutex);
	}

	void lock() {
		SDL_mutexP(_mutex);
	}

	void unlock() {
		SDL_mutexV(_mutex);
	}
};

}

namespace Scumm {
	
class ScummEngine {
	uint8* resTbl[128];

public:
	ScummEngine() {
		memset(resTbl, 0, sizeof(resTbl));
	}

	byte *getResourceAddress(int type, int idx) {
		return resTbl[idx];
	}

	void setResourceAddress(int idx, uint8* ptr) {
		resTbl[idx] = ptr;
	}
};

class MusicEngine {
public:
	virtual ~MusicEngine() {}
	virtual void setMusicVolume(int vol) = 0;
	virtual void startSound(int sound) = 0;
	virtual void stopSound(int sound) = 0;
	virtual void stopAllSounds() = 0;
	virtual int  getSoundStatus(int sound) const = 0;
	virtual void terminate() {}
};

}

namespace Audio {

class AudioStream {
public:
	virtual ~AudioStream() {}
	virtual int readBuffer(int16 *buffer, const int numSamples) = 0;
	virtual bool isStereo() const = 0;
	virtual int getRate() const = 0;
	virtual bool endOfData() const = 0;
};

class SoundHandle {};

class Mixer {
	int _sample_rate;
public:
	enum SoundType {
		kPlainSoundType = 0
	};

	enum {
		kMaxChannelVolume = 255,
		kMaxMixerVolume = 256
	};

	Mixer(int sample_rate) : _sample_rate(sample_rate) {}

	virtual void stopHandle(SoundHandle handle) {}

	virtual uint getOutputRate() const {
		return _sample_rate;
	}
	
	void playInputStream(SoundType type, SoundHandle *handle, AudioStream *input,
		int id = -1, byte volume = kMaxChannelVolume, int8 balance = 0,
		bool autofreeStream = true, bool permanent = false, bool reverseStereo = false) 
	{
	}
};

}

#endif
