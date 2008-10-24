#include <stdbool.h>
#include "audio.h"
#ifndef __BEAT_H__
#define __BEAT_H__
msec_t beat_to_msec(float bpm,float beat);
typedef void (*drum_f)(track_t *t, float vol);
#define MAX_SUBTRACKS 32
typedef struct beatloop_s{
	float bpm;	  
	float beatCount; /* length of the loop in beats*/
	int   drumCount;  /* total amount of drums in the loop*/
	float *drumTime;  /* [drumCount] time where the drum is hit in the loop*/
	float *drumVol;	  /* [drumCount] volume of the related drum */
	drum_f *drums;    /* [drumCount] the drums */
	int   *drumSubtrack;   /* [drumCount] subtrack of the related drum */
	int  subtrackCount; /*total amount of subtracks in the beatloop */
	char     mixmode[MAX_SUBTRACKS];       /*[subtrackCount]*/
	float    subtrackVol[MAX_SUBTRACKS]; /*[subtrackCount]*/
	track_t *subtrack[MAX_SUBTRACKS]; /* [subtrackCount]*/
	track_t *track;	   
}beatloop_t;

beatloop_t *beatloopNew(int beatcount); /*eg 140bpm, 4 beatcount for techno*/
/* adds a drum sample to the beat loop at "beat" time.
 * beat starts at 1 and finishes at beatcount, fractional beats are ok 
 */
#define MIXMODE_MIX 0
#define MIXMODE_CUT 1
int subtrack(beatloop_t *bl, int mixmode, float volume);
void drum(beatloop_t*bl, int subtrack, drum_f drum, float beat, float vol);
/* mix the beatloop on the track at "beat", in bpm unit. */
void beatloopMix(track_t *t,beatloop_t *bl,int beat,float bpm,float vol);

typedef struct beatTrack_s{
	float bpm;
	int   measure;
	track_t *track;
	struct beatTrack_s *next;
}btrack_t;

btrack_t *btrackNew(float bpm, int measure,int measure_count);
void btrackAdd(	btrack_t *bt,
		beatloop_t*bl,
		int measure_start, 
		int measure_end,	/* if <= 0 -> until the end*/
		bool fill_measure, 	/*if beatloop is shorter 
					than measure, it will repeat 
					to fill the measure, if longer,
					it will be cut */
		float vol);
/* puts the btrack to be mixed after dst.
 * if insert is true, it will be inserted just
 * after b
void btrackAppend(btrack_t *dst,btrack_t *src, bool insert);*/
#endif
