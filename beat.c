#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "beat.h"
msec_t beat_to_msec(float bpm, float beat){
	double bpmsec = bpm /60 /1000;
	return beat * 1.0 / bpmsec;
}
beatloop_t *beatloopNew(int beatCount){
	beatloop_t *bl = (beatloop_t*)malloc(sizeof(beatloop_t));
	memset((char*)bl,0,sizeof(beatloop_t));
	bl->beatCount = beatCount;
	bl->subtrackCount = 1;
	bl->mixmode[0] = MIXMODE_MIX;
	bl->subtrack[0] = NULL;
	bl->subtrackVol[0] = 1.0;
	bl->track = NULL;
	return bl;
}

int subtrack(beatloop_t *bl, int mixmode,float volume){
	if (bl->subtrackCount > MAX_SUBTRACKS){
		return -1;
	}else{
		bl->subtrackCount++;
		bl->mixmode[bl->subtrackCount -1] = mixmode;
		bl->subtrackVol[bl->subtrackCount -1] = volume;
		return bl->subtrackCount -1;
	}
}
void drum(beatloop_t *bl, int subtrack, drum_f drum, float beat, float vol){
	float * dt;
	float * dv;
	drum_f * dr;
	int   *ds;
	bl->drumCount++;
	dt = (float*)malloc(bl->drumCount*sizeof(float));
	dv = (float*)malloc(bl->drumCount*sizeof(float));
	dr = (drum_f*)malloc(bl->drumCount*sizeof(drum_f));
	ds = (int*)malloc(bl->drumCount*sizeof(int));
	memcpy((char*)dt,
		(char*)bl->drumTime,
		(bl->drumCount -1)*sizeof(float));
	memcpy((char*)dv,
		(char*)bl->drumVol,
		(bl->drumCount -1)*sizeof(float));
	memcpy((char*)dr,
		(char*)bl->drums,
		(bl->drumCount -1)*sizeof(drum_f));
	memcpy((char*)ds,
		(char*)bl->drumSubtrack,
		(bl->drumCount -1)*sizeof(int));
	free(bl->drumTime);
	free(bl->drumVol);
	free(bl->drums);
	free(bl->drumSubtrack);
	bl->drumTime = dt;
	bl->drumVol = dv;
	bl->drums = dr;
	bl->drumSubtrack = ds;
	dt[bl->drumCount -1] = beat;
	dv[bl->drumCount -1] = vol;
	dr[bl->drumCount -1] = drum;
	if (subtrack >= 0 && subtrack < bl->subtrackCount){
		ds[bl->drumCount -1] = subtrack;
	}else{
		ds[bl->drumCount -1] = 0;
	}
	if(bl->track){
		//free(bl->track);
		bl->track = NULL;
	}
}
void beatloop_self_mix(beatloop_t *bl, float bpm){
	int i;
	track_t *tmp ;
	tmp = trackNew(beat_to_msec(bpm,bl->beatCount));
	bl->bpm = bpm;
	/* removing cache data */
	i = bl->subtrackCount;
	while(i--){
		if(bl->subtrack[i] != NULL){
			trackZero(bl->subtrack[i]);
		}else{
			bl->subtrack[i] = trackNew(2*beat_to_msec(bpm,bl->beatCount));
		}
	}
	if(bl->track){
		trackZero(bl->track);
	}else{
		bl->track = trackNew(2*beat_to_msec(bpm,bl->beatCount));
	}
	/* mixing drums on subtracks */
	i = bl->drumCount;
	while(i--){
		trackZero(tmp);
		bl->drums[i](tmp,bl->drumVol[i]);	
		if(bl->mixmode[bl->drumSubtrack[i]] == MIXMODE_MIX){
			trackMix( bl->subtrack[bl->drumSubtrack[i]],
				  tmp,
				  beat_to_msec(bpm,bl->drumTime[i]),
				  1.0);
		}else{
			trackPaste(bl->subtrack[bl->drumSubtrack[i]],
				   tmp,
				   beat_to_msec(bpm,bl->drumTime[i]),
				   1.0);
		}
	}
	/* mixing subtracks together on track */
	i = bl->subtrackCount;
	while(i--){
		trackMix(bl->track,
			 bl->subtrack[i],
			 0,
			 bl->subtrackVol[i]	);
	}
}
void beatloopMix(track_t *t, beatloop_t *bl, int beat, float bpm, float vol){
	if (bl->track == NULL || bl->bpm != bpm){
		beatloop_self_mix(bl,bpm);
	}
	trackMix(t,bl->track,beat_to_msec(bpm,beat),vol);
}


	
	
