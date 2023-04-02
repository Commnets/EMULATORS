/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/scumm/actor.cpp $
 * $Id: actor.cpp 45438 2009-10-27 12:08:19Z Kirben $
 *
 */

#ifndef SCUMM_PLAYER_ZAK_H
#define SCUMM_PLAYER_ZAK_H

#include "sid.h"

#include "common/scummsys.h"
#ifndef SIDPLAYER_STANDALONE
#include "scumm/music.h"
#include "sound/audiostream.h"
#include "sound/mixer.h"
#include "common/mutex.h"
#else
#include "config.h"
#endif

namespace Scumm {

// the "channel" parameters seem to be in fact SID register 
// offsets. Should be replaced.
enum sid_reg_t {
	FREQ_VOICE1,
	FREQ_VOICE2,
	FREQ_VOICE3,
	FREQ_FILTER,
	PULSE_VOICE1,
	PULSE_VOICE2,
	PULSE_VOICE3
};

enum VideoStandard {
	PAL,
	NTSC
};

class ScummEngine;

class Player_Zak : public Audio::AudioStream, public MusicEngine {
public:
	Player_Zak(ScummEngine *scumm, Audio::Mixer *mixer);
	virtual ~Player_Zak();

	virtual void setMusicVolume(int vol) { _maxvol = vol; };
	void startMusic(int songResIndex);
	virtual void startSound(int sound);
	virtual void stopSound(int sound);
	virtual void stopAllSounds();
	virtual int  getSoundStatus(int sound) const;
	virtual int  getMusicTimer() const;

	// AudioStream API
	int readBuffer(int16 *buffer, const int numSamples);
	bool isStereo() const { return false; }
	bool endOfData() const { return false; }
	int getRate() const { return _sample_rate; }

private:
	Resid::SID *_sid;
	void SID_Write(int reg, uint8 data);
	void initSID();
	uint8 *getResource(int resID);

	// number of cpu cycles until next frame update
	Resid::cycle_count _cpuCyclesLeft;

	ScummEngine *_vm;
	Audio::Mixer *_mixer;
	Audio::SoundHandle _soundHandle;
	int _sample_rate;
	int _maxvol;
	Common::Mutex _mutex;

	VideoStandard _videoSystem;

	int _music_timer;
	uint8* _music;

private:
	void initMusic(int songResIndex); // $7de6
	int initSound(int soundResID); // $4D0A
	void stopSound_intern(int soundResID); // $5093
	void stopAllSounds_intern(); // $4332

	void resetSID(); // $48D8
	void update(); // $481B
	void handleMusicBuffer();
	void setupSongFileData(); // $36cb
	void func_3674(int channel); // $3674
	void resetPlayerState();
	void processSongData(int channel); // $4939
	void readSetSIDFilterAndProps(int *offset, uint8* dataPtr);  // $49e7
	void saveSongPos(int y, int channel);
	void updateFreq(int channel);
	void resetFreqDelta(int channel);
	void readSongChunk(int channel); // $4a6b
	void setupSongPtr(int channel); // $4C1C
	void unlockSound(int resID);
	void releaseResource(int resID);
	void releaseChannel(int channel);
	void stopChannel(int channel);
	void reserveChannel(int channel, int channel2, uint8 prioValue, int chanResIndex, uint8 soundByte5, bool filterNeeded, uint8 *songFilePtr); // $4ffe
	void func_7eae(int channel, uint8* songFileDataPtr); // $7eae
	void func_819b(int channel); // $819b
	void buildStepTbl(int step); // $82B4
	void enqueueSound(int resID); // $3FCE

	uint8 chanBuffer[3][45];

	int resID_song;

	// statusBits1A/1B are always equal
	uint8 statusBits1A;
	uint8 statusBits1B;

	uint8 busyChannelBits;

	uint8 SIDReg23;
	uint8 SIDReg24;

	uint8* chanFileData[3];
	uint16 chanDataOffset[3];
	uint8* songPosPtr[7];

	// 0..2: freq value voice1/2/3
	// 3:    filter freq
	// 4..6: pulse width
	uint16 freqReg[7];

	// current offset[i] for songFileOrChanBufData to obtain songPosPtr[i] (starts with vec6[i], increased later)
	uint16 songFileOrChanBufOffset[7];

	uint16 freqDelta[7];
	int freqDeltaCounter[7];

	// never read
	//uint8 vec19[7];
	// never read (needed by scumm engine?)
	//bool curChannelActive;	

	// resource status (never read)
	// bit7: some flag
	// bit6..0: counter (use-count?), maybe just bit0 as flag (used/unused?)
	uint8 resStatus[127];

	uint8* actSongFileData;

	uint16 stepTbl[33];

	bool initializing;
	bool _soundInQueue;
	bool isVoiceChannel;

	bool isMusicPlaying;

	uint8 freeChannelCount;

	uint8 attackReg[3];
	uint8 sustainReg[3];

	// bit-array: 00000cba
	// a/b/c: channel1/2/3
	uint8 songChannelBits;

	uint8 phaseBit[3];
	bool releasePhase[3];

	// values: a resID or -1
	// resIDs: 3, 4, 5 or song-number
	int _soundQueue[10];

	// values: a resID or 0
	// resIDs: 3, 4, 5 or song-number
	int channelMap[7];

	uint8 songPosUpdateCounter[7];

	// priortity of channel contents
	// MM:  1: lowest .. 120: highest (1,2,A,64,6E,73,78)
	// Zak: -???: lowest .. 120: highest (5,32,64,65,66,6E,78, A5,A6,AF,D7)
	uint8 chanPrio[7];

	// only [0..2] used?
	uint8 waveCtrlReg[7];

	int filterResChanID;

	uint8 actFilterPrio;
	uint8 minChanPrio;
	int minChanPrioResID;

	uint8 bgSoundPrio;
	uint8 bgSoundResID;

	// always 0?
	uint8 _soundQueueLen;

	bool chanNeedsFilter[7];
};

} // End of namespace Scumm

#endif
