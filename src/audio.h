#ifndef __AUDIO_H__
#define __AUDIO_H__



typedef struct track_s{
	long length;	/* in samples */
	long byte_length;
	int channels;	
	char *buffer;
}track_t;


/* TRACK LOW LEVEL =============================================*/

long   	trackSampleLength(const track_t *t);
msec_t 	trackMsecLength(const track_t *t);
int 	trackChannels(const track_t *t);
float 	trackSampleGet(const track_t *t, int channel, long sample);
void 	trackSampleSet(track_t *t, int channel, long sample, float val);
float 	trackSampleGetLin(const track_t *t, int channel, double sample);
float 	trackSampleGetNorm(const track_t *t, int channel, long sample);
void 	trackSampleSetNorm(track_t *t, int channel, long sample,float value);

/* TRACK MANIPULATION */
track_t *trackNew(msec_t length);
track_t *trackCopy(const track_t *t);
void trackVol(track_t *t,int channel,float vol);
void trackResize(track_t *dst, msec_t new_length);
/*TRACK MIX : mix new track with old, keep the old sound data, 
 * resizes dst so that the new data can fit the track */
void trackMix(track_t *dest, const track_t *src, msec_t pos, float volume);
/*TRACK APPEND : resize dest and puts src at the end of dest */
void trackAppend(track_t *dest, const track_t *src);
/*TRACK PASTE : same as mix but the original sound data is overwriten
 * by the new one */
void trackPaste(track_t *dest, const track_t *src, msec_t pos, float volume);

void trackZero(track_t *t);

/*TRACK IO */
void trackPlay(const track_t *t);
void trackWrite(const track_t *t, char *file);
track_t *trackRead(char *file);
void initAudio();
void closeAudio();
#endif

