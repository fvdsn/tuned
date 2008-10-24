#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "functions.h"

/* STATIC CACHE */
typedef struct stcache_s{
	long start;	/*starting time of the cache, in sample*/ 
	long size;	/*size of the cache in samples*/
	float *cache;	/*[size]*/
	char *isincache;/*[size]*/ /*TODO reduce memory*/
}stcache_t;
stcache_t *stcache_new(long size, long start_time){
	stcache_t *stc = (stcache_t*)malloc(sizeof(stcache_t));
	stc->start = start_time;
	stc->size = size;
	if(size > 0){
		stc->cache = (float*)malloc(size*sizeof(float));
		stc->isincache = (char*)malloc(size*sizeof(char));
		memset(stc->isincache,0,size*sizeof(char));
	}
	else{
		printf("WARNING : creating cache with size %d\n"(int)size);
	}
	return stc;
}
int stcache_is_sample_in(stcache_t*stc, long time){
	if(time < stc->start || time >= stc->start+stc->size){
		return 0;
	}else{
		return stc->isincache[time-stc->start];
	}
}
float stcache_get(stcache_t *stc, long time){
	if(!stache_is_sample_in(stc,time)){
		return 0;
	}else{
		return stc->cache[time-stc->start];
	}
}
void stcache_set(stcache_t *stc, long time, float val){
	if( time < stc->start || time >= stc->start+stc->size ){
		return;
	}else{
		stc->cache[time-stc->start] = val;
		stc->isincache[time-stc->start] = 1;
	}
}



#define MAX_PARAM 5
#define FUN_NAME_LENGTH 16
struct fun_s{
	char name[FUN_NAME_LENGTH];
	struct 	fun_s * fun[MAX_PARAM];
	float  	fval[MAX_PARAM];
	double 	dval[MAX_PARAM];
	long   	lval[MAX_PARAM];
	msec_t 	msecval[MAX_PARAM];
	hz_t   	hzval[MAX_PARAM];	
	track_t *track;
	rtcache_t *rtcache;	/*real time cache*/
	stcache_t *stcache;	/*static cache */
	float (*compute)(struct fun_s *self,long sample);
	/*struct fun_s * a;
	struct fun_s * b;
	struct fun_s * c;
	float peak;
	float ground;
	float A;
	float B;
	long  time;
	long time2;
	long attack;
	long sustain;
	long release;
	hz_t freq;
	long period;
	long phase;
	double ftime;
	double ftime2;
	int channel;
	int quality;
	long  cache_size;
	float *cache;    [cache_size]
	char *isincache; [cache_size]
	rtcache_t rtc1;*/
};
float funSampleGet(fun_t *fun, long sample){
	if (fun == NULL){
		return 0;
	}else{
		return fun->compute(fun,sample);
	}
}
float funSampleGetLin(fun_t *fun, double sample){
	long lsample = (long)sample;
	double fact  = sample - lsample;
	return 	funSampleGet(fun,lsample)*(1.0 - fact) +
		funSampleGet(fun,lsample + 1) * fact;
}
float sinc(float a){
	if( a > -0.0001 && a < 0.0001){
		return 1;
	}else{
		return sinf( M_PI * a) / (M_PI * a);
	}
}
fun_t *fun_new(){
	fun_t *fun = (fun_t*)malloc(sizeof(fun_t));
	memset((char*)fun,0,sizeof(fun_t));
	return fun;
}
void  fun_name_set(fun_t* f, char *name){
	strncpy(f->name,name,FUN_NAME_LENGTH);
}

float fun_mult(fun_t *f,  long time){
	return funSampleGet(f->fun[0],time) * funSampleGet(f->fun[1],time);
}
fun_t *mult(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"mult");
	f->compute = fun_mult;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
float fun_add(fun_t *f, long time){
	return funSampleGet(f->fun[0],time)+funSampleGet(f->fun[1],time);
}
fun_t *add(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"add");
	f->compute = fun_add;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
float fun_max(fun_t*f, long time){
	float a = funSampleGet(f->fun[0],time);
	float b = funSampleGet(f->fun[1],time);
	if(a < 0){
		if (b < 0){
			if (a < b){
				return a;
			}else{
				return b;
			}
		}else{ /*b >= 0*/
			return b;
		}
	}else{ /*a >=0*/
		if(b > 0){
			if(a > b){
				return a;
			}else{
				return b;
			}
		}else{
			return a;
		}
	}
}
fun_t *max(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"max");
	f->compute = fun_max;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
float fun_min(fun_t*f, long time){
	float a = funSampleGet(f->fun[0],time);
	float b = funSampleGet(f->fun[1],time);
	if(a < 0){
		if (b < 0){
			if (a < b){
				return b;
			}else{
				return a;
			}
		}else{ /*b >= 0*/
			return a;
		}
	}else{ /*a >=0*/
		if(b > 0){
			if(a > b){
				return b;
			}else{
				return a;
			}
		}else{
			return b;
		}
	}
}
fun_t *min(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"min");
	f->compute = fun_min;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
float fun_rmax(fun_t *f, long time){
	float a = funSampleGet(f->fun[0],time);
	float b = funSampleGet(f->fun[1],time);
	if (a >= b){
		return a;
	}else{
		return b;
	}
}
fun_t *rmax(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"rmax");
	f->compute = fun_rmax;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
float fun_rmin(fun_t *f, long time){
	float a = funSampleGet(f->fun[0],time);
	float b = funSampleGet(f->fun[1],time);
	if (a < b){
		return a;
	}else{
		return b;
	}
}
fun_t *rmin(fun_t *a, fun_t *b){
	fun_t *f = fun_new();
	fun_name_set(f,"rmin");
	f->compute = fun_min;
	f->fun[0] = a;
	f->fun[1] = b;
	return f;
}
/*system functions */
float fun_delay(fun_t *f, long time){
	return funSampleGet(f->fun[0], time - f->lval[0]);
}
fun_t *delay(msec_t phase,fun_t *a){
	fun_t *f = fun_new();
	fun_name_set(f,"delay");
	f->compute = fun_delay;
	f->fun[0] = a;
	f->lval[0] = msec_to_sample(phase);
	return f;
}
float fun_fdelay(fun_t *f, long time){
	double ftime = time - msec_to_sample(funSampleGet(f->fun[1],time));
	return funSampleGet(f->fun[0],(long)ftime);
}
fun_t *fdelay(fun_t *msec_phase, fun_t *a){
	fun_t *f = fun_new();
	fun_name_set(f,"fdelay");
	f->compute = fun_fdelay;
	f->fun[0] = a;
	f->fun[1] = msec_phase;
	return f;
}
float fun_raise(fun_t*f, long time){
	float hi = funSampleGet(f->fun[0],time);
	float lo = funSampleGet(f->fun[1],time);
	float range = hi - lo;
	float v = funSampleGet(f->fun[2],time);
	v*=range/2;
	v+= lo + range/4;
	return v;
}
fun_t *raise(fun_t *low, fun_t *hi, fun_t *a){
	fun_t *f = fun_new();
	fun_name_set(f,"raise");
	f->compute = fun_raise;
	f->fun[0] = hi;
	f->fun[1] = low;
	f->fun[2] = a;
	return f;
}
float fun_diff(fun_t *f, long time){
	float v = funSampleGet(f->fun[0],time) 
			- funSampleGet(f->fun[0],time -1);
	return v;
}
fun_t *diff(fun_t *a){
	fun_t *f = fun_new();
	fun_name_set(f,"diff");
	f->compute = fun_diff;
	f->fun[0] = a;
	return f;
}
/* non periodic */
float fun_c(fun_t*f, long time){
	time++; /*compiler warning*/
	return f->fval[0];
}
fun_t *c(float value){
	fun_t *f = fun_new();
	f->compute = fun_c;
	f->fval[0] = value;
	return f;
}
float fun_pulse(fun_t *f, long time){
	if(time == f->time){
		return f->fval[0];
	}else{
		return f->fval[1];
	}
}
fun_t *pulse(msec_t t, float ground, float pulse){
	fun_t *f = fun_new();
	f->lval[0] = msec_to_sample(t); 
	f->compute = fun_pulse;
	f->fval[0] = pulse;
	f->fval[1] = ground;
	return f;
}
float fun_unit(fun_t *f, long time){
	if(time < f->time){
		return f->fval[1];
	}else{
		return f->fval[0];
	}
}
fun_t *unit(msec_t t, float before, float after){
	fun_t *f = fun_new();
	f->time = msec_to_sample(t);
	f->compute = fun_unit;
	f->fval[0] = after;
	f->fval[1] = before;
	return f;
}
float fun_sincc(fun_t *f, long time){
	return sinc(f->fval[0]*(float)time);
}
fun_t *sincc(float a){
	fun_t *f = fun_new();
	f->compute = fun_sincc;
	f->fval[0] = a;
	return f;
}
float fun_window(fun_t *f, long time){
	float v = 0;
	if(time < f->lval[0] - f->lval[2] / 2 ||
		time >= f->lval[1] + f->lval[2] / 2){
		return 0;
	}else if (time < f->lval[0] + f->lval[2]/2) {
		v = funSampleGet(f->fun[0],time);
		v *= (time - (f->lval[0] - f->lval[2] /2 )) 
			/ (float)(f->lval[2]);
		return v;
	}else if (time < f->lval[1] - f->lval[2] / 2){
		return funSampleGet(f->fun[0],time);
	}else{
		v = funSampleGet(f->fun[0],time);
		v *= ((f->lval[1] + f->lval[2] /2) - time)
			/ (float)(f->lval[2]);
		return v;
	}
}
fun_t *window(msec_t start, msec_t end, msec_t alias, fun_t *fun){
	fun_t *f = fun_new();
	f->compute = fun_window;
	f->lval[0] = msec_to_sample(start);	//time
	f->lval[1] = msec_to_sample(end);	//time2
	f->lval[2] = msec_to_sample(alias);	//attack
	f->fun[0] = fun;
	return f;
}
float fun_trapeze(fun_t *f, long time){
	long ftime = f->lval[0];
	long frel  = f->lval[1];
	float fb   = f->fval[0];
	float fa   = f->fval[1];
	float factor;
	if (time < ftime || time >= ftime + frel || frel <= 0){
		return 0;
	}else{
		if(fa== fb){
			return fa;
		}else{
			factor = (float)(time - ftime)/(float)(frel);
			return fb*(1.0 - factor)+ fa *factor;
		}
	}
}
fun_t *trapeze(msec_t time, msec_t release, float before, float after){
	fun_t *f = fun_new();
	f->compute = fun_trapeze;
	f->lval[0] = msec_to_sample(time);
	f->lval[1] = msec_to_sample(release);
	f->fval[0] = before;
	f->fval[1] = after;
	return f;
}
fun_t *ramp(msec_t t1, msec_t t2, float before, float after){
	return 	add( unit(t1,before,0),
		add( trapeze(t1,t2-t1,before,after),
		     unit(t2,0,after)	));
}
fun_t *note( msec_t time, msec_t attack, msec_t decay, msec_t sustain, msec_t release, float peak,float amp){
	return  add(trapeze(time,attack,0,peak),
		add(trapeze(time+attack,decay,peak,amp),
		add(trapeze(time+attack+decay,sustain,amp,amp),
		trapeze(time+attack+decay+sustain,release,amp,0)
			)));
}
/*periodic*/
float fun_sine(fun_t *f, long time){
	double I = 2*M_PI*time*funSampleGet(f->a,time) + f->phase;
	I /= AUDIO_RATE;
	return f->peak*sin(I);
}
fun_t *sine(fun_t * freq, msec_t phase, float amplitude){
	fun_t *f = fun_new();
	f->compute = fun_sine;
	f->a = freq;
	f->phase = msec_to_sample(phase);
	f->peak = amplitude;
	return f;
}
fun_t *rsine(fun_t*freq, float low, float hi){
	return add(c((low+hi)/2),sine(freq,0,(hi-low)/2));
}
float fun_square(fun_t *f, long time){
	long period = hz_to_sample(funSampleGet(f->a,time));
	long ptime;
	if(period < 2){
		return 1.0;
	}else{
		ptime = (time + f->phase)% period;
		if (ptime < period * funSampleGet(f->b,time)){
			return f->peak;
		}else{
			return -f->peak;
		}
	}
}
fun_t *square(fun_t *freq, fun_t *up, msec_t phase, float amplitude){
	fun_t *f = fun_new();
	f->a = freq;
	f->b = up;
	f->phase = msec_to_sample(phase);
	f->peak = amplitude;
	f->compute = fun_square;
	return f;
}
float fun_sawtooth(fun_t *f, long time){
	long period = hz_to_sample(funSampleGet(f->a,time));
	long ptime;
	float p;
	float v;
	if(period < 2){
		return 1.0;
	}
	ptime = (time + f->phase) % period;
	p = 1.0 - (float)ptime / (float)period;
	if (p <= 0.00001){
		v = 1.0;
	}else{
		v = powf(p,funSampleGet(f->b,time));
	}
	return (2.0*v -1.0)*f->peak;
}
fun_t *sawtooth(fun_t *freq, fun_t *exp, msec_t phase, float amplitude){
	fun_t *f = fun_new();
	f->a = freq;
	f->b = exp;
	f->phase = msec_to_sample(phase);
	f->peak = amplitude;
	f->compute = fun_sawtooth;
	return f;
}

float fun_noise(fun_t *f, long time){
	float vol = funSampleGet(f->a,time);
	double i = random()%65355 ;
	i -= 65355 /2;
	i /= (65355.0/2.0);
	i *= vol;
	return i;
}
fun_t *noise(fun_t*volume){
	fun_t *f = fun_new();
	f->compute = fun_noise;
	f->a = volume;
	return f;
}
void trackMult(track_t *t, fun_t *f1, fun_t *f2){
	long i = trackSampleLength(t);
	float c;
	while(i--){
		c = funSampleGet(f1,i);
		trackSampleSet(t,0,i,c*trackSampleGet(t,0,i));
		if(trackChannels(t) > 1 && f2 != NULL){
			if(f1 != f2){
				c = funSampleGet(f2,i);
			}
			trackSampleSet(t,1,i,c*trackSampleGet(t,1,i));
		}
	}
}

void trackAdd(track_t *t, fun_t *f1, fun_t *f2){
	long i = trackSampleLength(t);
	float c;
	while(i--){
		c = funSampleGet(f1,i);
		trackSampleSet(t,0,i,c+trackSampleGet(t,0,i));
		if(trackChannels(t) > 1 && f2 != NULL){
			if(f1 != f2){
				c = funSampleGet(f2,i);
			}
			trackSampleSet(t,1,i,c+trackSampleGet(t,1,i));
		}
	}
}
