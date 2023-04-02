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

#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

#ifdef SIDPLAYER_STANDALONE
#include <assert.h>
#include "player_zak.h"
#include "config.h"
#else
#include "engines/engine.h"
#include "scumm/player_zak.h"
#include "scumm/scumm.h"
#include "sound/mixer.h"
#endif

namespace Scumm {

/*
 * The player's update() routine is called once per (NTSC/PAL) frame as it is 
 * called by the VIC Rasterline interrupt handler which is in turn called 
 * approx. 50 (PAL) or 60 (NTSC) times per second. 
 * The SCUMM V0/V1 music playback routines or sound data have not been adjusted 
 * to PAL systems. As a consequence, music is played audibly (-16%) slower 
 * on PAL systems.
 * In addition, the SID oscillator frequency depends on the video clock too.
 * As SCUMM games use an NTSC frequency table for both NTSC and PAL versions
 * all tone frequencies on PAL systems are slightly (-4%) lower than on NTSC ones.
 *
 * For more info on the SID chip see:
 * - http://www.dopeconnection.net/C64_SID.htm (German)
 * For more info on the VIC chip see:
 * - http://www.htu.tugraz.at/~herwig/c64/man-vic.php (German)
 * - http://www.c64-wiki.de/index.php/VIC (German)
 */

struct TimingProps {
	double clockFreq;
	int cyclesPerFrame;
};

static const TimingProps timingProps[2] = {
	{ 17734472.0 / 18, 312 * 63 }, // PAL:  312*63 cycles/frame @  985248 Hz (~50Hz)
	{ 14318180.0 / 14, 263 * 65 }  // NTSC: 263*65 cycles/frame @ 1022727 Hz (~60Hz)
};

static const uint8 BITMASK[7] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40
};
static const uint8 BITMASK_INV[7] = {
	0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF
};

static const uint8 SOUNDBYTE5_CHANNELBIT[4] = {
	0x01, 0x04, 0x10, 0x00
};

static const uint8 FILE_OFFSET_INDEX[4] = {
	0x3F, 0x01, 0x07, 0x1F
};

static const uint8 FILE_OFFSET_TBL[64] = {
	0x06,0x08,0x08,0x0A,0x08,0x0A,0x0A,0x0C,
	0x08,0x0A,0x0A,0x0C,0x0A,0x0C,0x0C,0x0E,
	0x08,0x0A,0x0A,0x0C,0x0A,0x0C,0x0C,0x0E,
	0x0A,0x0C,0x0C,0x0E,0x0C,0x0E,0x0E,0x10,
	0x08,0x0A,0x0A,0x0C,0x0A,0x0C,0x0C,0x0E,
	0x0A,0x0C,0x0C,0x0E,0x0C,0x0E,0x0E,0x10,
	0x06,0x08,0x08,0x0A,0x08,0x0A,0x0A,0x0C,
	0x06,0x08,0x08,0x0A,0x08,0x0A,0x0A,0x0C
};

static const int SID_REG_OFFSET[7] = {
	0, 7, 14, 21, 2, 9, 16
};

// NTSC frequency table (also used for PAL versions).
// FREQ_TBL[i] = tone_freq[i] * 2^24 / clockFreq
static const uint16 FREQ_TBL[97] = {
  0x0000, 0x010C, 0x011C, 0x012D, 0x013E, 0x0151, 0x0166, 0x017B, 
  0x0191, 0x01A9, 0x01C3, 0x01DD, 0x01FA, 0x0218, 0x0238, 0x025A, 
  0x027D, 0x02A3, 0x02CC, 0x02F6, 0x0323, 0x0353, 0x0386, 0x03BB, 
  0x03F4, 0x0430, 0x0470, 0x04B4, 0x04FB, 0x0547, 0x0598, 0x05ED, 
  0x0647, 0x06A7, 0x070C, 0x0777, 0x07E9, 0x0861, 0x08E1, 0x0968, 
  0x09F7, 0x0A8F, 0x0B30, 0x0BDA, 0x0C8F, 0x0D4E, 0x0E18, 0x0EEF, 
  0x0FD2, 0x10C3, 0x11C3, 0x12D1, 0x13EF, 0x151F, 0x1660, 0x17B5, 
  0x191E, 0x1A9C, 0x1C31, 0x1DDF, 0x1FA5, 0x2187, 0x2386, 0x25A2, 
  0x27DF, 0x2A3E, 0x2CC1, 0x2F6B, 0x323C, 0x3539, 0x3863, 0x3BBE, 
  0x3F4B, 0x430F, 0x470C, 0x4B45, 0x4FBF, 0x547D, 0x5983, 0x5ED6, 
  0x6479, 0x6A73, 0x70C7, 0x777C, 0x7E97, 0x861E, 0x8E18, 0x968B, 
  0x9F7E, 0xA8FA, 0xB306, 0xBDAC, 0xC8F3, 0xD4E6, 0xE18F, 0xEEF8, 
  0xFD2E
};

static const int SONG_CHANNEL_OFFSET[3] = { 6, 8, 10 };
static const int RES_ID_CHANNEL[3] = { 3, 4, 5 };

#define LOBYTE(a) ((a) & 0xFF)
#define HIBYTE(a) (((a) >> 8) & 0xFF)

#define GETBIT(var, pos) ((var) & (1<<(pos)))

static void log(char* fmt, ...) {
#ifdef ENABLE_LOGGING
	static FILE *f = NULL;
	if (!f) {
		f = fopen(LOGDIR"sidout.txt", "w");
	}
	va_list args;
	va_start(args, fmt);
	vfprintf(f, fmt, args);
	va_end(args);
#endif
}

void Player_Zak::handleMusicBuffer() { // $33cd
	int channel = 2;
	while (channel >= 0) {
		if ((statusBits1A & BITMASK[channel]) == 0 ||
		    (busyChannelBits & BITMASK[channel]) != 0) 
		{			
			--channel;
			continue;
		}

		setupSongFileData();
		
		uint8* l_chanFileDataPtr = chanFileData[channel];
		
		uint16 l_freq = 0;
		bool l_keepFreq = false;

		int y = 0;
		uint8 curByte = l_chanFileDataPtr[y++];

		// freq or 0/0xFF
		if (curByte == 0) {
			func_3674(channel);
			if (!isMusicPlaying)
				return;		
			continue;
		} else if (curByte == 0xFF) {
			l_keepFreq = true;
		} else {
			l_freq = FREQ_TBL[curByte];
		}

		uint8 local1 = 0;
		curByte = l_chanFileDataPtr[y++];
		bool isLastCmdByte = (curByte & 0x80) != 0;
		uint16 curStepSum = stepTbl[curByte & 0x7f];
		
		for (int i = 0; !isLastCmdByte && (i < 2); ++i) {
			curByte = l_chanFileDataPtr[y++];
			isLastCmdByte = (curByte & 0x80) != 0;
			if (curByte & 0x40) {
				// note: bit used in zak theme (95) only (not used/handled in MM)
				_music_timer = curByte & 0x3f;
			} else {
				local1 = curByte & 0x3f;
			}
		} 
		
		chanFileData[channel] += y;
		chanDataOffset[channel] += y;

		uint8 *l_chanBuf = getResource(RES_ID_CHANNEL[channel]);

		if (local1 != 0) {
			// TODO: signed or unsigned?
			uint16 offset = READ_LE_UINT16(&actSongFileData[local1*2 + 12]);
			l_chanFileDataPtr = actSongFileData + offset;
			
			// next five bytes: freqDelta, attack, sustain and phase bit
			for (int i = 0; i < 5; ++i) {
				l_chanBuf[15 + i] = l_chanFileDataPtr[i];
			}
			phaseBit[channel] = l_chanFileDataPtr[4];

			for (int i = 0; i < 17; ++i) {		
				l_chanBuf[25 + i] = l_chanFileDataPtr[5 + i];
			}		
		}
		
		if (l_keepFreq) {
			if (!releasePhase[channel]) {
				l_chanBuf[10] &= 0xfe; // release phase
			}
			releasePhase[channel] = true;
		} else {
			if (releasePhase[channel]) {
				l_chanBuf[19] = phaseBit[channel];
				l_chanBuf[10] |= 0x01; // attack phase
			}
			l_chanBuf[11] = LOBYTE(l_freq);
			l_chanBuf[12] = HIBYTE(l_freq);
			releasePhase[channel] = false;
		}
		
		// set counter value for frequency update (freqDeltaCounter)
		l_chanBuf[13] = LOBYTE(curStepSum);
		l_chanBuf[14] = HIBYTE(curStepSum);

		busyChannelBits |= BITMASK[channel];

		channelMap[channel] = RES_ID_CHANNEL[channel];
		channelMap[channel+4] = RES_ID_CHANNEL[channel];

		chanPrio[channel] = 127;
		chanNeedsFilter[channel] = false;
		songFileOrChanBufOffset[channel] = 0x08;
		--freeChannelCount;
		processSongData(channel);

		busyChannelBits |= BITMASK[channel+4];
		chanPrio[channel+4] = 127;
		chanNeedsFilter[channel+4] = false;
		songFileOrChanBufOffset[channel+4] = 0x19;
		processSongData(channel+4);

		--channel;
	}
}

void Player_Zak::setupSongFileData() { // $36cb
	actSongFileData = _music;
	for (int i = 0; i < 3; ++i) {
		chanFileData[i] = _music + chanDataOffset[i];
	}
}

//x:0..2
void Player_Zak::func_3674(int channel) { // $3674
	statusBits1B &= BITMASK_INV[channel];
	if (statusBits1B == 0) {
		isMusicPlaying = false;
		--resStatus[resID_song];
		for (int i = 0; i < 3; ++i) {
			--resStatus[RES_ID_CHANNEL[i]];
		}	
	}
	
	statusBits1A &= BITMASK_INV[channel];
	phaseBit[channel] = 0;
}

void Player_Zak::resetPlayerState() {
	for (int i = 6; i >= 0; --i) 
		releaseChannel(i);
	
	isMusicPlaying = false;
	_soundQueueLen = 0;
	bgSoundPrio = 0;
	statusBits1B = 0;
	statusBits1A = 0;
	freeChannelCount = 3;
	minChanPrio = 0xFF;
}

void Player_Zak::resetSID() { // $48D8
	SIDReg24 = 0x0f;

	for (int i = 0; i < 24; ++i)
		SID_Write(i, 0);
	SID_Write(24, SIDReg24);
	
	resetPlayerState();
}

void Player_Zak::update() { // $481B
	SID_Write(24, SIDReg24);
	SID_Write(23, SIDReg23);
	for (int i = 6; i >= 0; --i) {
		int reg = SID_REG_OFFSET[i];
		SID_Write(reg,   LOBYTE(freqReg[i]));
		SID_Write(reg+1, HIBYTE(freqReg[i]));
		if (i < 3) {
			SID_Write(reg+4, waveCtrlReg[i]);
			SID_Write(reg+5, attackReg[i]);
			SID_Write(reg+6, sustainReg[i]);
		}
	}

	if (isMusicPlaying) {
		handleMusicBuffer();
	}

	if (_soundInQueue) {
		for (int i = _soundQueueLen-1; i >= 0; --i) {
			if (_soundQueue[i] != -1) {
				int resID = _soundQueue[i];
				_soundQueue[i] = -1;
				unlockSound(resID);
				initSound(resID);
			}
		}
		_soundQueueLen = 0;
		_soundInQueue = false;
	}

	for (int i = 6; i >= 0; --i) {
		if (busyChannelBits & BITMASK[i]) {
			updateFreq(i);
		}
	}
		
	if ((busyChannelBits & 0x07) != 0x07) {
		int nextSound = bgSoundResID;
		bgSoundResID = 0;
		bgSoundPrio = 0;
		if (nextSound != 0) {
			initSound(nextSound);
		}
	};	
}

// channel: 0..6
void Player_Zak::processSongData(int channel) { // $4939
	isVoiceChannel = (channel < 3);
		
	setupSongPtr(channel);
	uint8* ptr1 = songPosPtr[channel];
		
	int y = -1;
	if (channel < 4) {
		++y;
		if (channel == 3) {
			readSetSIDFilterAndProps(&y, ptr1);
		} else {
			waveCtrlReg[channel] = ptr1[y];
			
			++y;
			if (ptr1[y] & 0x0f) {
				// filter on for voice channel
				SIDReg23 |= BITMASK[channel];
			} else {
				// filter off for voice channel
				SIDReg23 &= BITMASK_INV[channel];
			}
		}
	}
	
	saveSongPos(y, channel);
	readSongChunk(channel);
}

void Player_Zak::readSetSIDFilterAndProps(int *offset, uint8* dataPtr) {  // $49e7
	SIDReg23 &= 0x0F;
	SIDReg23 |= dataPtr[*offset] & 0xF0;
	SID_Write(23, SIDReg23);
	++*offset;
	SIDReg24 = dataPtr[*offset];
	SID_Write(24, SIDReg24);
}

void Player_Zak::saveSongPos(int y, int channel) {
	++y;
	songPosPtr[channel] += y;
	songFileOrChanBufOffset[channel] += y;
}

// channel: 0..6
void Player_Zak::updateFreq(int channel) {
	isVoiceChannel = (channel < 3);

	--freqDeltaCounter[channel];
	if (freqDeltaCounter[channel] < 0) {
		readSongChunk(channel);
	} else if (freqDeltaCounter[channel] % 256 != 255) {
		freqReg[channel] += freqDelta[channel];
	}
}

void Player_Zak::resetFreqDelta(int channel) {
	freqDeltaCounter[channel] = 0;
	freqDelta[channel] = 0;
}

void Player_Zak::readSongChunk(int channel) { // $4a6b
	while (true) {
		setupSongPtr(channel);
		
		uint8* ptr1 = songPosPtr[channel];
		
		//curChannelActive = true;	
		
		uint8 l_cmdByte = ptr1[0];
		if (l_cmdByte == 0) {
			//curChannelActive = false;
			songPosUpdateCounter[channel] = 0;
			releaseChannel(channel);
			return;
		}
		
		//vec19[channel] = l_cmdByte;
		
		// attack (1) / release (0) phase
		if (isVoiceChannel) {
			if (GETBIT(l_cmdByte, 0))
				waveCtrlReg[channel] |= 0x01; // start attack phase
			else
				waveCtrlReg[channel] &= 0xfe; // start release phase
		}
		
		// channel finished bit
		if (GETBIT(l_cmdByte, 1)) {
			releaseChannel(channel);
			return;
		}

		int y = 0;

		// frequency
		if (GETBIT(l_cmdByte, 2)) {
			y += 2;
			freqReg[channel] = READ_LE_UINT16(&ptr1[y-1]);
			if (!GETBIT(l_cmdByte, 6)) {
				y += 2;
				freqDeltaCounter[channel] = READ_LE_UINT16(&ptr1[y-1]);
				y += 2;
				freqDelta[channel] = READ_LE_UINT16(&ptr1[y-1]);
			} else {
				resetFreqDelta(channel);
			}
		} else {
			resetFreqDelta(channel);
		}
		
		// attack / release
		if (isVoiceChannel && GETBIT(l_cmdByte, 3)) {
			if (channel < 3) {
				int reg = SID_REG_OFFSET[channel];
				SID_Write(reg+4, waveCtrlReg[channel] & 0xFE);
			}

			++y;
			attackReg[channel] = ptr1[y];
			++y;
			sustainReg[channel] = ptr1[y];
			
			// set attack (1) or release (0) phase
			waveCtrlReg[channel]  |= (l_cmdByte & 0x01);
		}
		
		if (GETBIT(l_cmdByte, 4)) {
			++y;
			uint8 curByte = ptr1[y];
			
			// pulse width
			if (isVoiceChannel && GETBIT(curByte, 0)) {
				// FIXME: seems to be a bug at $4276, shouldn't this be "y += 2"?
				y += 1;
				freqReg[channel+4] = READ_LE_UINT16(&ptr1[y-1]);
			}
			
			if (GETBIT(curByte, 1)) {
				++y;
				readSetSIDFilterAndProps(&y, ptr1);
				
				y += 2;
				freqReg[3] = READ_LE_UINT16(&ptr1[y-1]);
			}
			
			if (GETBIT(curByte, 2)) {
				resetFreqDelta(channel);
				
				y += 2;
				freqDeltaCounter[channel] = READ_LE_UINT16(&ptr1[y-1]);
			}
		}
		
		// set waveform (?)
		if (GETBIT(l_cmdByte, 5)) {
			++y;
			waveCtrlReg[channel] = (waveCtrlReg[channel] & 0x0f) | ptr1[y];		
		}
		
		// song position
		if (GETBIT(l_cmdByte, 7)) {
			if (songPosUpdateCounter[channel] == 1) {
				y += 2;
				--songPosUpdateCounter[channel];			
				saveSongPos(y, channel);
			} else {
				// looping / skipping / ...
				++y;
				songPosPtr[channel] -= ptr1[y];
				songFileOrChanBufOffset[channel] -= ptr1[y];
					
				++y;
				if (songPosUpdateCounter[channel] == 0) {
					songPosUpdateCounter[channel] = ptr1[y];
				} else {
					--songPosUpdateCounter[channel];
				}
			}
		} else {	
			saveSongPos(y, channel);
			return;
		}
	}
}

// channel: 0..6
void Player_Zak::setupSongPtr(int channel) { // $4C1C
	//resID:5,4,3,songid
	int resID = channelMap[channel];
	songPosPtr[channel] = getResource(resID) + songFileOrChanBufOffset[channel];
}

void Player_Zak::unlockSound(int resID) { // $4328
	if (resID > 5)
        --resStatus[resID];
}

// a: 0..6
void Player_Zak::releaseChannel(int channel) {
	int oldChannelMap = channelMap[channel];
	stopChannel(channel);

	if (channel >= 3) {
		return;
	}

	++freeChannelCount;
	waveCtrlReg[channel] = 0;

	if (oldChannelMap != 0) {
		unlockSound(oldChannelMap);
	}

	if ((resStatus[oldChannelMap] & 0x7F) == 0 &&
		oldChannelMap == filterResChanID) 
	{
		filterResChanID = 0;
	}

	int reg = SID_REG_OFFSET[channel];
	SID_Write(reg+4, 0);
	SID_Write(reg,   0);
	SID_Write(reg+1, 0);
	
	if (chanNeedsFilter[channel]) {
		SIDReg23 &= BITMASK_INV[channel];
	}

	stopChannel(channel + 4);
}

void Player_Zak::stopChannel(int channel) {
	songPosUpdateCounter[channel] = 0;
	busyChannelBits &= BITMASK_INV[channel];
	channelMap[channel] = 0;

	chanPrio[channel] = 255;
	minChanPrio = 255;
	minChanPrioResID = 255;

	for (int i = 2; i >= 0; --i) {
		if (chanPrio[i] < minChanPrio) {
			minChanPrioResID = channelMap[i];
			minChanPrio = chanPrio[i];
		}
	}
}

void Player_Zak::reserveChannel( // $4ffe
	int channel, int channel2, uint8 prioValue, int chanResIndex, 
	uint8 soundByte5, bool filterNeeded, uint8 *songFilePtr) 
{
	if (channel < 3) {
		--freeChannelCount;
		++resStatus[chanResIndex];

		chanNeedsFilter[channel] = filterNeeded;
		chanPrio[channel] = prioValue;
	}

	busyChannelBits |= BITMASK[channel];
	channelMap[channel] = chanResIndex;

	int offset = FILE_OFFSET_TBL[soundByte5 & FILE_OFFSET_INDEX[channel2]];

	if (channel2) {
		songFileOrChanBufOffset[channel] = READ_LE_UINT16(&songFilePtr[offset]);
		offset += 2;
	} else {
		songFileOrChanBufOffset[channel] = offset;
		offset = 6;
	}

	if (soundByte5 & SOUNDBYTE5_CHANNELBIT[channel2]) {
		songFileOrChanBufOffset[channel+4] = READ_LE_UINT16(&songFilePtr[offset]);
		busyChannelBits |= BITMASK[channel+4];
		chanNeedsFilter[channel+4] = filterNeeded;
		chanPrio[channel+4] = prioValue;
		channelMap[channel+4] = chanResIndex;

		processSongData(channel+4);
	}
	processSongData(channel);
}

void Player_Zak::initMusic(int songResIndex) { // $7de6
	resID_song = songResIndex;
	_music = getResource(resID_song);
	
	// song base address
	uint8* songFileDataPtr = _music;
	actSongFileData = _music;

	initializing = true;
	isMusicPlaying = false;
	
	resetPlayerState();
	++resStatus[resID_song];
	buildStepTbl(songFileDataPtr[5]);
	
	// fetch sound
	songChannelBits = songFileDataPtr[4];
	for (int i = 2; i >= 0; --i) {
		if ((songChannelBits & BITMASK[i]) != 0) {
			func_7eae(i, songFileDataPtr);
		}
	}

	isMusicPlaying = true;
	
	SIDReg23 &= 0xf0;
	
	handleMusicBuffer();
	
	initializing = false;
	_soundInQueue = true;
}

// params:
//   channel: channel 0..2
void Player_Zak::func_7eae(int channel, uint8* songFileDataPtr) {
	int pos = SONG_CHANNEL_OFFSET[channel];
	chanDataOffset[channel] = READ_LE_UINT16(&songFileDataPtr[pos]);
	chanFileData[channel] = songFileDataPtr + chanDataOffset[channel];
	
	func_819b(channel);
	
	waveCtrlReg[channel] = 0;
}

void Player_Zak::func_819b(int channel) {
	statusBits1B |= BITMASK[channel];
	statusBits1A |= BITMASK[channel];
}

void Player_Zak::buildStepTbl(int step) { // $82B4
	stepTbl[0] = 0;	
	stepTbl[1] = step - 2;	
	for (int i = 2; i < 33; ++i) {
		stepTbl[i] = stepTbl[i-1] + step;
	}
}

void Player_Zak::enqueueSound(int resID) { // $3FCE
	if (_soundQueueLen < 10) {
		_soundQueue[_soundQueueLen++] = resID;
	}
	++resStatus[resID];
}

int Player_Zak::initSound(int soundResID) { // $4D0A
	if (isMusicPlaying && (statusBits1A & 0x07) == 0x07) {
		return -1;
	}
	
	uint8 *songFilePtr = getResource(soundResID);
	uint8 soundPrio = songFilePtr[4] & 0x7F;
	bool isBgSound = (songFilePtr[4] & 0x80) != 0;

	uint8 soundByte5 = songFilePtr[5];
	bool filterNeeded = (soundByte5 & 0x20) != 0;
	if (filterNeeded) {
		if (filterResChanID != 0) {
			if (soundPrio < actFilterPrio)
				return -1;
			stopSound_intern(filterResChanID);
		}
		filterResChanID = soundResID;
		actFilterPrio = soundPrio;
	}


	uint8 requestedChannels = 1;
	if (songFilePtr[5] & 0x02)
		++requestedChannels;
	if (songFilePtr[5] & 0x08)
		++requestedChannels;

	while (requestedChannels > freeChannelCount) {
		if (soundPrio >= minChanPrio) {
			uint8 *otherSongData = getResource(minChanPrioResID);
			bool otherIsBgSound = (otherSongData[4] & 0x80) != 0;
			uint8 otherPrio = otherSongData[4] & 0x7F;
			if (otherIsBgSound && otherPrio > bgSoundPrio) {
				bgSoundResID = minChanPrioResID;
				bgSoundPrio = minChanPrio;
			}
			releaseResource(minChanPrioResID);
		} else {
			if (!isBgSound || (bgSoundResID != 0 && soundPrio < bgSoundPrio)) {
				return -1;
			}
			bgSoundResID = soundResID;
			bgSoundPrio = soundPrio;
			return 0;
		}
	}
	
	soundPrio &= 0x7F;
	if (soundPrio <= minChanPrio) {
		minChanPrio = soundPrio;
		minChanPrioResID = soundResID;
	}
	
	for (int i = 0; i < requestedChannels; ++i) {
		if ((busyChannelBits & 0x07) == 2)
			reserveChannel(2, i, soundPrio, soundResID, soundByte5, filterNeeded, songFilePtr);
		else if ((busyChannelBits & 0x01) == 0)
			reserveChannel(0, i, soundPrio, soundResID, soundByte5, filterNeeded, songFilePtr);
		else if (busyChannelBits & 0x04)
			reserveChannel(1, i, soundPrio, soundResID, soundByte5, filterNeeded, songFilePtr);
		else
			reserveChannel(2, i, soundPrio, soundResID, soundByte5, filterNeeded, songFilePtr);
	}

	if (soundByte5 & 0x20) {
		reserveChannel(3, 3, soundPrio, soundResID, soundByte5, filterNeeded, songFilePtr);
	}

	return 0;
}

void Player_Zak::releaseResource(int resID) {
	for (int i = 3; i >= 0; --i) {
		if (channelMap[i] == resID) {
			releaseChannel(i);
		}
	}

	for (int i = _soundQueueLen; i > 0; --i) {
		if (_soundQueue[i] == resID) {
			_soundQueue[i] = -1;
			unlockSound(resID);
		}
	}
}

void Player_Zak::stopSound_intern(int soundResID) {
	if (soundResID == bgSoundResID) {
		bgSoundResID = 0;
		bgSoundPrio = 0;
	}
	releaseResource(soundResID);
}

void Player_Zak::stopAllSounds_intern() { // $4332
	statusBits1B = 0;
	isMusicPlaying = false;
	if (resID_song != 0) {
		resStatus[resID_song] = 0;
	}

	statusBits1A = 0;
	phaseBit[0] = 0;
	phaseBit[1] = 0;
	phaseBit[2] = 0;
}

///////////////////////////
///////////////////////////

#define ZEROMEM(a) memset(a, 0, sizeof(a))

Player_Zak::Player_Zak(ScummEngine *scumm, Audio::Mixer *mixer) {
	/*
	 * clear memory
	 */

	resID_song = 0;
	statusBits1A = 0;
	statusBits1B = 0;
	busyChannelBits = 0;
	SIDReg23 = 0;
	SIDReg24 = 0;
	freeChannelCount = 0;
	songChannelBits = 0;
	minChanPrio = 0;
	filterResChanID = 0;
	minChanPrioResID = 0;
	actFilterPrio = 0;
	bgSoundPrio = 0;
	bgSoundResID = 0;
	_soundQueueLen = 0;

	_music = NULL;
	actSongFileData = NULL;

	initializing = false;
	_soundInQueue = false;
	isVoiceChannel = false;
	isMusicPlaying = false;

	ZEROMEM(chanFileData);
	ZEROMEM(chanDataOffset);
	ZEROMEM(songPosPtr);
	ZEROMEM(freqReg);
	ZEROMEM(songFileOrChanBufOffset);
	ZEROMEM(freqDelta);
	ZEROMEM(freqDeltaCounter);
	ZEROMEM(resStatus);
	ZEROMEM(attackReg);
	ZEROMEM(sustainReg);
	ZEROMEM(phaseBit);
	ZEROMEM(releasePhase);
	ZEROMEM(_soundQueue);
	ZEROMEM(channelMap);
	ZEROMEM(songPosUpdateCounter);
	ZEROMEM(chanPrio);
	ZEROMEM(waveCtrlReg);
	ZEROMEM(stepTbl);
	ZEROMEM(chanNeedsFilter);

	/*
	 * initialize data
	 */

	const uint8 chanBuffer_const[3][45] = {{
		0x00,0x00,0x00,0x00,0x7f,0x01,0x19,0x00,
		0x00,0x00,0x2d,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xf0,0x40,0x10,0x04,0x00,0x00,
		0x00,0x04,0x27,0x03,0xff,0xff,0x01,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00
	},{		
		0x00,0x00,0x00,0x00,0x7f,0x01,0x19,0x00,
		0x00,0x00,0x2d,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xf0,0x20,0x10,0x04,0x00,0x00,
		0x00,0x04,0x27,0x03,0xff,0xff,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00
	},{						
		0x00,0x00,0x00,0x00,0x7f,0x01,0x19,0x00,
		0x00,0x00,0x2d,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xf0,0x20,0x10,0x04,0x00,0x00,
		0x00,0x04,0x27,0x03,0xff,0xff,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00
	}};
	memcpy(chanBuffer, chanBuffer_const, sizeof(chanBuffer_const));

	for (int i = 0; i < 7; ++i) {
		_soundQueue[i] = -1;
	};

	_music_timer = 0;

	_mixer = mixer;
	_sample_rate = _mixer->getOutputRate();
	_vm = scumm;

	_videoSystem = NTSC;
	_cpuCyclesLeft = 0;

	initSID();
	resetSID();

	_mixer->playInputStream(Audio::Mixer::kPlainSoundType, &_soundHandle, this, -1, Audio::Mixer::kMaxChannelVolume, 0, false, true);
}

Player_Zak::~Player_Zak() {
	_mixer->stopHandle(_soundHandle);
	delete _sid;
}

uint8 *Player_Zak::getResource(int resID) {
	switch (resID) {
		case 0:
			return NULL;
		case 3:
		case 4:
		case 5:
			return 	chanBuffer[resID-3];
		default:
			return _vm->getResourceAddress(rtSound, resID);
	}
}

int Player_Zak::readBuffer(int16 *buffer, const int numSamples) {
	int samplesLeft = numSamples;

	_mutex.lock();

	while (samplesLeft > 0) {
		// update SID status after each frame
		if (_cpuCyclesLeft <= 0) {
			update();
			_cpuCyclesLeft = timingProps[_videoSystem].cyclesPerFrame;
		}
		// fetch samples
		int sampleCount = _sid->clock(_cpuCyclesLeft, (short*)buffer, samplesLeft);
		samplesLeft -= sampleCount;
		buffer += sampleCount;
	}

	_mutex.unlock();

	return numSamples;
}

void Player_Zak::SID_Write(int reg, uint8 data) {
	log("SID[%d]: 0x%02x\n", reg, data);
	_sid->write(reg, data);
}

void Player_Zak::initSID() {
	_sid = new Resid::SID();
	_sid->set_sampling_parameters(
		timingProps[_videoSystem].clockFreq, 
		_sample_rate);
	_sid->enable_filter(true);

	_sid->reset();
	// Synchronize the waveform generators (must occur after reset)
	_sid->write( 4, 0x08);
	_sid->write(11, 0x08);
	_sid->write(18, 0x08);
	_sid->write( 4, 0x00);
	_sid->write(11, 0x00);
	_sid->write(18, 0x00);
}

void Player_Zak::startSound(int nr) {
	byte *data = _vm->getResourceAddress(rtSound, nr);
	assert(data);

	// WORKAROUND:
	// data[4] contains either a song prio or a music channel usage byte.
	// As music channel usage is always 0x07 for all music files and 
	// prio 7 is never used in any sound file use this byte for auto-detection.
	bool isMusic = (data[4] == 0x07);

	_mutex.lock();

	if (isMusic) {
		initMusic(nr);
	} else {
		stopSound_intern(nr);
		enqueueSound(nr);
		initSound(nr);
	}
	
	_mutex.unlock();
}

void Player_Zak::stopSound(int nr) {
	if (nr == -1)
		return;

	_mutex.lock();
	stopSound_intern(nr);
	_mutex.unlock();
}

void Player_Zak::stopAllSounds() {
	_mutex.lock();
	stopAllSounds_intern();
	_mutex.unlock();
}

int Player_Zak::getSoundStatus(int nr) const {
	int result = 0;

	//_mutex.lock();

	if (resID_song == nr && isMusicPlaying) {
		result = 1;
	}

	for (int i = 0; (i < 4) && (result == 0); ++i) {
		if (nr == _soundQueue[i] ||
			nr == channelMap[i]) 
		{
			result = 1;
		}
	}
	
	//_mutex.unlock();

	return result;
}

int Player_Zak::getMusicTimer() const {
	int result = _music_timer;
	// WORKAROUND: very, very dirty hack
	*((int*)&_music_timer) = 0;
	return result;
}

} // End of namespace Scumm
