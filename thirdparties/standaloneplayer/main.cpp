#include <iostream>
#include <string>
#include <sstream>
#include "SDL.h"
#include "config.h"
#ifdef USE_PLAYER_ZAK
#include "player_zak.h"
#else
#include "player_sid.h"
#endif

/*
 * music (Maniac Mansion):
 *   (49)/50: piano
 *   58: theme
 *   (51)/(52)/(55): theme (short version)
 *
 * sounds (Maniac Mansion):
 *    8: door to basement
 *    9: door to kitchen
 *   12: lamp on
 *   17: faucet (prio 1)
 *   28: grandfather clock
 *   38: cricket
 *   39: doorbell
 *   44: old record (prio 1)
 *   54: click
 *   56: meteor
 *   57: meteor crash
 *
 * music (Zak):
 *   95: theme
 *   102/103/104: theme end
 *
 * sounds (Zak):
 *    2: clock
 *    6: walk
 *   10: open door
 *   18: disposal
 *   23: bell
 *   24: bread flight
 *   33: kazoo
 *   49: music: razor on mars
 *   60: bread crash
 *   63: open window shade
 *   64: close window shade
 *   68: open drawer
 *   69: close drawer
 *   81: snoring
 *   85: music: kinshasa
 *   87: music: airport
 *   96: music: devotee
 *   112: music: mars
 *   114: music: airport 2
 */

#define SOUNDPATH "D:/daten/Projekte/Diverse/sidscumm/sounds/"

// scummvm sound/music dump filename 
const std::string FILENAME_DEFAULT = SOUNDPATH"maniac-c64-58.bin";
//const std::string FILENAME_DEFAULT = SOUNDPATH"zak-v1-c64-95.bin";

const int SAMPLE_FREQ = 44100;

#ifdef USE_PLAYER_ZAK
static Scumm::Player_Zak *player;
#else
static Scumm::Player_SID *player;
#endif
static Scumm::ScummEngine vm;
static uint8 *fileBuf;

void SDLCALL audioCallback(void *userdata, Uint8 *buffer, int len) {
	if (player) {
		player->readBuffer((int16*)buffer, len/2);
	}

	#ifdef DUMP_WAVE
	FILE* f = fopen(LOGDIR"sample.wav", "wb");
	fwrite(buffer, 1, len, f);
	fclose(f);
	#endif
}

bool initAudio() {
	SDL_AudioSpec specDesired;
	specDesired.freq     = SAMPLE_FREQ;
	specDesired.format   = AUDIO_S16;
	specDesired.channels = 1;
	specDesired.samples  = 4096;
	specDesired.callback = audioCallback;
	specDesired.userdata = NULL;

	if (SDL_OpenAudio(&specDesired, NULL) != 0) {
		std::cerr << "SDL_OpenAudio: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_PauseAudio(0);

	return true;
}

uint8* loadSound(const std::string &fname) {
	FILE* f = fopen(fname.c_str(), "rb");
	if (!f) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8 *buffer = (uint8*)malloc(size);
	if (!buffer) {
		return NULL;
	}
	
	size_t nRead;
	size_t bufPos = 0;
	while ((nRead = fread(&buffer[bufPos], 1, 1024, f)) > 0)
		bufPos += nRead;

	if (ferror(f)) {
		perror("Read error");
	}
	fclose(f);

	if (bufPos != size) {
		return NULL;
	}

	return buffer;
}

int startBuffer(uint8 *buf) {
	// sound[4] contains either a song prio or a music channel usage byte.
	// As music channel usage is always 0x07 for all music files and 
	// prio 7 is never used in any sound file use this byte for auto-detection.
	bool isMusic = (buf[4] == 0x07);

	int resID;

	// initialize sound/music
	if (isMusic) {
		resID = 58;
		vm.setResourceAddress(resID, buf);
	} else {
		// the sound resource ID is not really important, just use 39
		resID = 39;
		vm.setResourceAddress(resID, buf);
		player->stopSound(resID);
	}
	player->startSound(resID);
	return resID;
}

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
		return 1;
	}
	atexit(SDL_Quit);

	Audio::Mixer mixer(SAMPLE_FREQ);
	
	#ifdef USE_PLAYER_ZAK
	player = new Scumm::Player_Zak(&vm, &mixer);
	#else
	player = new Scumm::Player_SID(&vm, &mixer);
	#endif

	if (!initAudio()) {
		return 1;
	}

	std::string fname = (argc >= 2) ? argv[1] : FILENAME_DEFAULT;

	fileBuf = loadSound(fname);
	if (!fileBuf) {
		std::cerr << "Could not open sound: " << fname << std::endl;
		return 1;
	}
	int resID = startBuffer(fileBuf);

	while(player->getSoundStatus(resID))
		SDL_Delay(100);

	SDL_Quit();
	delete player;
	free(fileBuf);

	return 0;
}
