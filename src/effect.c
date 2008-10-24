#include <math.h>
#include "effect.h"

void fx_cutoff(track_t *t, fun_t*max, fun_t *fact, int wave){
	long i = trackSampleLength(t);
	int c = trackChannels(t);
	float v;
	float fmax;
	float ffact;
	while(i--){
		c = trackChannels(t);
		while(c--){
			v = trackSampleGet(t,c,i);
			fmax = funSampleGet(max,i);
			ffact = funSampleGet(fact,i);
			if( (wave == WAVE_UP || wave == WAVE_BOTH)
				&& v  > fmax){
				v = fmax + (v-fmax)*ffact;
			}else if ( (wave == WAVE_DOWN || wave == WAVE_BOTH)
			&& v < -fmax){
				v = -fmax + (v+fmax)*ffact;
			}
			trackSampleSet(t,c,i,v);
		}
	}
}
void fx_echo(track_t *t, fun_t *delay, fun_t *amp,int count){
	long i = trackSampleLength(t);
	int c = trackChannels(t);
	float fdelay;
	float norm = 0;
	float famp;
	float v;
	int j = count;
	while(i--){
		c = trackChannels(t);
		while(c--){
			v = trackSampleGet(t,c,i);
			fdelay = msec_to_sample(funSampleGet(delay,i));
			famp = funSampleGet(amp,i);
			j = count;
			norm = 1;
			do{
				fdelay = j*
					msec_to_sample(funSampleGet(delay,i));
				famp = pow(funSampleGet(amp,i),j);
				v+= trackSampleGet(t,c,i - fdelay)*famp;
				norm +=  famp;
			}while(--j > 0);
			trackSampleSet(t,c,i,v/famp);
		}
	}
}
void fx_disto(track_t *t,int wave){
	fx_cutoff(t,c(0.1),c(0.02), wave);
	fx_cutoff(t,c(0.09),c(10), wave);
}
void fx_power_dist(track_t *t,fun_t*gain, fun_t* clip ,fun_t* power,
	int wave){
	long i = trackSampleLength(t);
	int c = trackChannels(t);
	float v;
	float fclip;
	float fpower;
	float fgain;
	while(i--){
		c = trackChannels(t);
		while(c--){
			fclip = funSampleGet(clip,i);
			fpower = funSampleGet(power,i);
			fgain  = funSampleGet(gain,i);
			v = trackSampleGet(t,c,i);
			if((wave == WAVE_UP || wave == WAVE_BOTH)
				&& v >= fclip ){
				v *= fgain;
				v = powf(v,fpower);
			}else if (( wave == WAVE_DOWN ||
					wave == WAVE_BOTH)
					&& v <= -fclip ){
				v *= fgain;
				v = -powf(-v,fpower);
			}
			trackSampleSet(t,c,i,v);
		}
	}
}
void fx_mute(track_t *t, float min, int length){
	long tsl = trackSampleLength(t);
	int c = trackChannels(t);
	long i = 0;
	do{
		c = trackChannels(t);
		while(c--){
			v = trackSampleGet(t,c,i);
			if(v < min)
		}
	}while(++i < tsl);
void fx_phase_dist(track_t *t, fun_t *clip, fun_t *delay, int wave){
	long i = trackSampleLength(t);
	track_t *d = trackCopy(t);
	int c = trackChannels(t);
	float v;
	float fclip;
	float fdelay;
	while(i--){
		c = trackChannels(t);
		while(c--){
			v = trackSampleGet(t,c,i);
			fclip = funSampleGet(clip,i);
			fdelay = funSampleGet(delay,i);
			if((wave == WAVE_UP || wave == WAVE_BOTH)
				&& v >= fclip){
				v -= fclip;
				v = trackSampleGetLin(t,c,
					(double)i + v*fdelay );
			}else if ((wave == WAVE_UP || wave == WAVE_BOTH)
				&& v <= -fclip ){
				v += fclip;
				v = trackSampleGetLin(t,c,
					(double)i+v*fdelay);
			}
			trackSampleSet(d,c,i,v);
		}
	}
	trackZero(t);
	trackMix(t,d,0,1);
}
void fx_flange(track_t*t,fun_t*delay){
	long i = trackSampleLength(t);
	track_t *d = trackCopy(t);
	int c = trackChannels(t);
	float v;
	float v2;
	float fi;
	while(i--){
		c = trackChannels(t);
		fi = funSampleGet(delay,i);
		while(c--){
			v = trackSampleGet(d,c,i);
			v2 = trackSampleGetLin(d,c,
				(double)i+fi);
			trackSampleSet(t,c,i,(v+v2)/2);
		}
	}
}

	

void fx_reverb(track_t *t){
	fx_echo(t,c(100),c(0.2),1);
	fx_echo(t,c(110),c(0.2),1);
	fx_echo(t,c(120),c(0.2),1);
	fx_echo(t,c(130),c(0.2),1);
	fx_echo(t,c(140),c(0.1),1);
	fx_echo(t,c(150),c(0.1),1);
}

					
