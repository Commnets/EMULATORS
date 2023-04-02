#ifndef _SCUMMPLAYER_H_
#define _SCUMMPLAYER_H_

#undef DUMP_WAVE
#undef ENABLE_LOGGING
#undef USE_PLAYER_ZAK

#ifdef WIN32
#define LOGDIR	"C:/"
#else
#define LOGDIR	"./"
#endif

typedef unsigned char byte;
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef unsigned int uint;

inline uint16 READ_LE_UINT16(const void *ptr) {
	return *(const uint16 *)(ptr);
}

#endif
