#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "audio.h"
msec_t sec(double sec){
	return sec * 1000;
}
msec_t nsec(double nsec){
	return nsec / 1000;
}
msec_t samples(double samples){
	return samples * 1000 / AUDIO_RATE;
}
long msec_to_sample(msec_t msec){
	double b = AUDIO_RATE *msec / 1000;
	return (long)b;
}
long msec_to_byte(msec_t length, int channels){
	double b = AUDIO_RATE*BYTE_RATE;
	b *= channels;
	b *= length;
	b /= 1000;
	return (int)b;
}
msec_t hz_to_period(hz_t hz){
	if (hz >= AUDIO_RATE / 2)
		return 0;
	else if (hz <= 1.0 / AUDIO_RATE)
		return -1;
	else
		return 1.0 / hz * 1000.0;
}
long hz_to_sample(hz_t hz){
	double v = 0;
	if (hz >= AUDIO_RATE / 2)
		return 0;
	else if (hz <= 1.0 / AUDIO_RATE){
		return -1;
	}else{
		v = 1/hz;
		v *= AUDIO_RATE;
		return (long)v;
	}
}
float vol(float vol){
	return 20*log10(pow(2,16)*vol);
}
float db(float db){
	double d = pow(10,db/20.0);
	d /= pow(2,16);
	return d;
}

track_t *trackNew(msec_t length){
	track_t *t = (track_t*)malloc(sizeof(track_t));
	t->channels = 2;
	t->length = msec_to_sample(length);
	t->byte_length = msec_to_byte(length,t->channels);
	t->buffer = (char*)malloc(t->byte_length);
	if(t->buffer == NULL){
		printf("error allocating memory\n");
	}
	trackZero(t);
	return t;
}
track_t *trackCopy(const track_t *t){
	track_t *cpy = (track_t*)malloc(sizeof(track_t));
	cpy->channels = t->channels;
	cpy->length = t->length;
	cpy->byte_length = t->byte_length;
	cpy->buffer = (char*)malloc(t->byte_length);
	memcpy(cpy->buffer,t->buffer,t->byte_length);
	return cpy;
}
void track_resize(track_t *t, long samples){
	char *newbuffer = NULL;
	long   newlength = samples;
	long   newbyte_length = samples * t->channels * BYTE_RATE;
	if(t->length == newlength || newlength <= 0){
		return;
	}else{
		newbuffer = (char*)malloc(newbyte_length);
		if(t->length > newlength){
			memcpy(newbuffer,t->buffer,newbyte_length);
		}else{
			memcpy(newbuffer,t->buffer,t->byte_length);
			memset(	newbuffer+t->byte_length,
				0,
				newbyte_length - t->byte_length);
		}
		free(t->buffer);
		t->length = newlength;
		t->byte_length = newbyte_length;
		t->buffer = newbuffer;
	}
}
void trackResize(track_t *t, msec_t new_length){
	track_resize(t,msec_to_sample(new_length));
}
void trackMix(track_t *dst, const track_t *src, msec_t pos, float volume){
	long i = trackSampleLength(src);
	int c = 0;
	float v = 0;
	long I = msec_to_sample(pos);
	if(dst->length < I + src->length ){
		track_resize(dst,I+src->length);
	}
	while(i--){
		c = trackChannels(src);
		while(c--){
			v = trackSampleGet(dst,c,I+i);
			v += trackSampleGet(src,c,i)*volume;
			trackSampleSet(dst,c,I+i,v);
		}
	}
}

void trackPaste(track_t *dst, const track_t *src, msec_t pos, float volume){
	long i = trackSampleLength(src);
	int c = 0;
	float v = 0;
	long I = msec_to_sample(pos);
	if(dst->length < I + src->length ){
		track_resize(dst,I+src->length);
	}
	while(i--){
		c = trackChannels(src);
		while(c--){
			v = trackSampleGet(src,c,i)*volume;
			trackSampleSet(dst,c,I+i,v);
		}
	}
}
		

		
long trackSampleLength(const track_t *t){
	return t->length;
}
msec_t trackMsecLength(const track_t *t){
	long l = trackSampleLength(t);
	l *= 1000;
	l /= AUDIO_RATE;
	return (msec_t)l;
}
int trackChannels(const track_t *t){
	return t->channels;
}
float trackSampleGet(const track_t* t, int channel, long sample){
	int16_t *samples = (int16_t*)t->buffer; /*TODO 24 & 32 bit audio rate*/
	if(sample < 0 || sample >= t->length){
		return 0;
	}else if (channel < 0 || channel >= t->channels){
		return 0;
	}else{
		return (float)(samples[sample*t->channels+channel])/65535*2;
	}
}
float trackSampleGetLin(const track_t* t, int channel,double index){
	double fact = index - (long)index;
	return 	trackSampleGet(t,channel,index)*(1.0-fact) +
		trackSampleGet(t,channel,index)*fact;
}
float trackSampleGetNorm(const track_t*t, int channel, long sample){
	return (trackSampleGet(t,channel,sample)+1)/2;
}
void trackSampleSet(track_t *t, int channel, long index, float sample){
	int16_t *samples = (int16_t*)t->buffer;
	if(index >= 0 && index < t->length && channel >= 0 && channel < t->channels){
		
		if (sample > 1.0){
			sample = 1.0;
		}else if( sample < 0 && sample < -1.0){
			sample = -1.0;
		}
		samples[index*t->channels+channel] = (int16_t)(sample*65535/2);
	}
}
void trackSampleSetNorm(track_t*t, int channel, long index, float s){
	trackSampleSet(t,channel,index,(s*2)-1);
}
void trackZero(track_t *t){
	int i = trackSampleLength(t);
	while(i--){
		trackSampleSet(t,0,i,0);
		trackSampleSet(t,1,i,0);
	}
}
