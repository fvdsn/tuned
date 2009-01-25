#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h> 
#include <math.h>
#include <stdarg.h>
#include "function.h"

fun_t *f_bpm = NULL;
fun_t *f_period = NULL;

sample_t current_sample;

static float get_bpm(sample_t s){
	float f = 0.0;
	if(f_bpm){
		f = fun_compute(f_bpm,0,s);
		if (f > 0.001){
			return f;
		}else{
			return 0.001;
		}
	}else{
		return 120.0;
	}
}
static float get_period(sample_t s){
	float f = 0.0;
	if(f_period){
		f = fun_compute(f_period,0,s);
		if (f > 0.001){
			return f;
		}else{
			return 0.001;
		}
	}else{
		return 4.0;
	}
}
void set_bpm(fun_t *bpm){
	f_bpm = bpm;
}
void set_period(fun_t *period){
	f_period = period;
}

channel_t *channel_new(fun_f fun,int ch, int f_param_count){
	channel_t *chan = (channel_t*)malloc(sizeof(channel_t));
	memset(chan,0,sizeof(channel_t));
	chan->f_param_count = f_param_count;
	chan->f_param = (fun_t**)malloc(f_param_count*sizeof(fun_t*));
	chan->channel = ch;
	chan->fun = fun;
	return chan;
}
void channel_free(channel_t *chan){
	free(chan->f_param);
	free(chan);
	return;
}
void 	channel_set_f_param(channel_t *chan, int index, fun_t*f){
	if (index > 0 && index < chan->f_param_count){
		chan->f_param[index] = f;
	}
}		
float 	channel_compute(channel_t *chan, int c, sample_t s){
	switch(chan->constant){
		case CHAN_CONSTANT : 
			return chan->last_value;
		case CHAN_FLAT:
			if( 	   chan->uptodate 
				&& s >= chan->first_sample 
				&& s <= chan->last_sample	){
				return chan->last_value;
			}else{
				chan->last_value = chan->fun(chan,c,s);
				return chan->last_value;
			}
		default :
			if ( s == chan->last_sample){
				return chan->last_value;
			}else{
				chan->last_sample = s;
				chan->last_value = chan->fun(chan,c,s);
				return chan->last_value;
			}
	}
}
fun_t *fun_new(int channel_count){
	fun_t *fun = (fun_t*)malloc(sizeof(fun_t));
	memset(fun,0,sizeof(fun_t));
	fun->channel_count = channel_count;
	fun->channel = (channel_t**)malloc(channel_count*sizeof(channel_t*));
	return fun;
}
fun_t *fun_user(fun_t* fun){
	fun->user_count++;
	return fun;
}
fun_t *fun_free(fun_t* fun){
	int i = fun->channel_count;
	fun->user_count--;
	if(fun->user_count <= 0){
		while(i--){
			if(fun->channel[i]){
				channel_free(fun->channel[i]);
			}
		}
		free(fun->channel);
		free(fun);
		return NULL;
	}else{
		return fun;
	}
}

float  fun_compute(fun_t*fun, int chan, sample_t s){
	int rchan = chan % fun->channel_count;
	if (rchan >= 0 && fun->channel[rchan] ){
		return channel_compute(fun->channel[rchan],chan,s);
	}else{
		printf("WARNING : channel out of range \n");
		return 0.0;
	}
}
int  	fun_channel_count(fun_t *fun){
	if(fun->virtual_channel_count > fun->channel_count){
		return fun->virtual_channel_count;
	}else{
		return fun->channel_count;
	}
}
unit_t fun_get_unit(fun_t* fun){
	return fun->unit;
}
void fun_set_name(fun_t *fun,const char*name){
	strncpy(fun->name,name,FUN_NAME_LENGTH);
}
static void printt(int i){
	while(i--){
		printf("\t");
	}
}
static void fun_print_l(fun_t *fun, int level){
	int i = 0;
	int j = 0;
	printt(level); printf("fun : %s\n",fun->name);
	printt(level); printf("  unit : %d\n",fun->unit);
	printt(level); printf("  isarg : %d\n",fun->isarg);
	printt(level); printf("  arg   : %d\n",(int)fun->arg);
	printt(level); printf("  user : %d\n",fun->user_count);
	printt(level); printf("  vchan : %d\n",fun->virtual_channel_count);
	printt(level); printf("  rchan : %d\n",fun->channel_count);
	while(i < fun->channel_count){
		printt(level); printf("  CHAN : %d\n",fun->channel[i]->channel);
		printt(level); printf("     cst : %d\n",fun->channel[i]->constant);
		printt(level); printf("     u2d : %d\n",fun->channel[i]->uptodate);
		printt(level); printf("     fs : %d\n",
					(int)fun->channel[i]->first_sample );
		printt(level); printf("     ls : %d\n",
					(int)fun->channel[i]->last_sample );
		printt(level); printf("     lv : %f\n",
					fun->channel[i]->last_value );
		printt(level); printf("     fpc : %d\n",fun->channel[i]->f_param_count);
		j = 0;
		while(j < fun->channel[i]->f_param_count){
			printt(level); printf("     FUN : %d\n",j);
			fun_print_l(fun->channel[i]->f_param[j],level+1);
			j++;
		}
		i++;
	}
}
void fun_print(fun_t *fun){
	fun_print_l(fun,0);
}
void fun_record_16b(	fun_t *fun,
			sample_t start,
			sample_t end,
			int channel_count,
			char *buffer){
	int16_t *out = (int16_t*)buffer;
	float v;
	long i = 0;
	int  c = 0;
	while( i + start < end){
		current_sample = start + i;
		c = 0;
		while(c < channel_count){
			v = fun_compute(fun,c,start+i);
			if (v > 1.0){
				v = 1.0;
			}else if (v < -1.0){
				v = -1.0;
			}
			out[i*channel_count + c] = 
				(int16_t)((65355/2)*v); 
			c++;
		}
		i++;
	}
}
static void fun_setup_channels(fun_t *fun, fun_f f, int f_param_count, ...){
	va_list va;
	fun_t *tmp;
	int vchan_count = 0;
	int i = 0;
	int j = fun_channel_count(fun);
	va_start(va,f_param_count);
	while(i < f_param_count){
		tmp = va_arg(va,fun_t*);
		fun->arg |= tmp->arg;
		if(fun_channel_count(tmp) > vchan_count){
			vchan_count = fun_channel_count(tmp);
		}
		j = fun_channel_count(fun);
		while(j--){
			if(fun->channel[j] == NULL){
				fun->channel[j] = channel_new(f,j,f_param_count);
			}
			fun->channel[j]->f_param[i] = tmp;
		}	
		i++;
	}
	fun->virtual_channel_count = vchan_count;
}

/*-------------------------*\
 * BASEE FUNCTIONS
\*-------------------------*/

static fun_t *set_unit(unit_t unit, fun_t *fun){
 	fun->unit = unit;
	return fun;
}
static float identity_fun(channel_t *chan, int c, sample_t s){
	return fun_compute(chan->f_param[0],c,s);
}
fun_t *f_identity(fun_t*a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,identity_fun,1,a);
	fun_set_name(f,"identity");
	f->unit = fun_get_unit(a);
	return f;
}
static fun_t *to_unit(unit_t unit, fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,identity_fun,1,a);
	f->unit = unit;
	fun_set_name(f,"to unit");
	return f;
}
static float cmult_fun(channel_t *chan, int c,sample_t s){
	return fun_compute(chan->f_param[0],c,s)*chan->n_param[0]; 
}
static fun_t *cmult(float v, fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,cmult_fun,1,a);
	f->channel[0]->n_param[0] = v;
	f->unit = fun_get_unit(a);
	fun_set_name(f,"constant mult");
	return f;
}
static float cdiv_fun(channel_t *chan, int c,sample_t s){
	float f = fun_compute(chan->f_param[0],c,s);
	if (f == 0.0){
		return 10000*chan->n_param[0];
	}else{
		return chan->n_param[0] / fun_compute(chan->f_param[0],c,s);
	}
}
static fun_t *cdiv(float v, fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,cdiv_fun,1,a);
	f->channel[0]->n_param[0] = v;
	f->unit = fun_get_unit(a);
	fun_set_name(f,"constant div");
	return f;
}

static float period_to_sample_fun(channel_t *chan, int c, sample_t s){
	return  get_period(current_sample)* 
		SAMPLING_RATE*60.0 / get_bpm(current_sample)
		*fun_compute(chan->f_param[0],c,s);
}
static fun_t *period_to_sample(fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,period_to_sample_fun,1,a);
	f->unit = SAMPLE;
	fun_set_name(f,"period to sample");
	return f;
}
static float beat_to_sample_fun(channel_t *chan, int c, sample_t s){
	return SAMPLING_RATE*60.0 / get_bpm(current_sample)
		*fun_compute(chan->f_param[0],c,s);
}
static fun_t *beat_to_sample(fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,beat_to_sample_fun,1,a);
	f->unit = SAMPLE;
	fun_set_name(f,"beat to sample");
	return f;
}
fun_t *f_convert(fun_t *a, unit_t unit){
	unit_t aunit = fun_get_unit(a);
	if(fun_get_unit(a) == unit){
		return a;
	}else if(unit == NO_UNIT){
		return to_unit(NO_UNIT,a);
	}
	switch(aunit){
		case SEC:
			switch(unit){
				case MSEC:   return set_unit(MSEC,cmult(1000,a));
				case HZ:     return set_unit(HZ,cdiv(1.0,a));
				case SAMPLE: return set_unit(SAMPLE,
						cmult(SAMPLING_RATE,a));
				default :    return to_unit(unit,a);
			}
		case MSEC:
			switch(unit){
				case SEC:    return set_unit(SEC,cmult(0.001,a));
				case HZ:     return set_unit(HZ,cdiv(1000,a));
				case SAMPLE: return set_unit(SAMPLE,
						cmult(SAMPLING_RATE*0.001,a));
				default:     return to_unit(unit,a);
			}
		case HZ:
			switch(unit){
				case SEC:    return set_unit(SEC,cdiv(1.0,a));
				case MSEC:   return set_unit(MSEC,cdiv(1000,a));
				case SAMPLE: return set_unit(SAMPLE,
						cdiv(SAMPLING_RATE,a));
				default :    return to_unit(unit,a);
			}
		case SAMPLE:
			switch(unit){
				case SEC:  return set_unit(SEC,
						cmult(1.0/SAMPLING_RATE,a));
				case MSEC: return set_unit(MSEC,
						cmult(1000.0/SAMPLING_RATE,a));
				case HZ:   return set_unit(HZ,
						cdiv(SAMPLING_RATE,a));	/*TODO*/
				default :  return to_unit(unit,a);
			}
		case PERIOD:
			switch(unit){
				case SEC :  return f_convert(period_to_sample(a),SEC);
				case MSEC : return f_convert(period_to_sample(a),MSEC);
				case HZ :   return f_convert(period_to_sample(a),HZ);
				case SAMPLE : return period_to_sample(a);
				default :    return to_unit(unit,a);
			}
		case BEAT:
			switch(unit){
				case SEC :  return f_convert(beat_to_sample(a),SEC);
				case MSEC : return f_convert(beat_to_sample(a),MSEC);
				case HZ :   return f_convert(beat_to_sample(a),HZ);
				case SAMPLE : return beat_to_sample(a);
				default :    return to_unit(unit,a);
			}
		case HBEAT:
			return cmult(0.5,f_convert(to_unit(BEAT,a),unit));
		case QBEAT:
			return cmult(0.25,f_convert(to_unit(BEAT,a),unit));
		case EBEAT:
			return cmult(0.125,f_convert(to_unit(BEAT,a),unit));
		default :
			return to_unit(unit,a);
	}
}
static float now_fun(channel_t *chan, int c, sample_t s){
	return fun_compute(chan->f_param[0],c,current_sample);
}
fun_t * f_now(fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,now_fun,1,a);
	f->unit = fun_get_unit(a);
	fun_set_name(f,"time is now");
	return f;
}
fun_t *f_c(float value){
	fun_t *f = fun_new(1);
	channel_t *chan = channel_new(NULL,0,0);
	chan->constant = CHAN_CONSTANT;
	chan->last_value = value;
	f->unit = NO_UNIT;
	f->channel[0] = chan;
	fun_set_name(f,"no unit constant");
	return f;
}
fun_t *f_cu(float value,unit_t unit){
	fun_t *f = fun_new(1);
	channel_t *chan = channel_new(NULL,0,0);
	chan->constant = CHAN_CONSTANT;
	chan->last_value = value;
	f->unit = unit;
	f->channel[0] = chan;
	fun_set_name(f,"constant");
	return f;
}

float pulse_fun(channel_t *chan, int c, sample_t s){
	sample_t time = (sample_t)fun_compute(chan->f_param[0],c,s);
	if (s == time ){
		return 1.0;
	}else{
		return 0.0;
	}
}
fun_t *f_pulse(fun_t *time){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,pulse_fun,1,f_convert(time,SAMPLE)	);
	f->unit = AMP;
	fun_set_name(f,"pulse");
	return f;
}
float unit_fun(channel_t *chan, int c, sample_t s){
	sample_t time = (sample_t)fun_compute(chan->f_param[0],c,s);
	if (s >= time ){
		return 1.0;
	}else{
		return 0.0;
	}
}
fun_t *f_unit(fun_t *time){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,unit_fun,1,f_convert(time,SAMPLE)	);
	f->unit = AMP;
	fun_set_name(f,"unit");
	return f;
}
float ramp_fun(channel_t *chan, int c, sample_t s){
	sample_t start = (sample_t)fun_compute(chan->f_param[0],c,s);
	sample_t end   = (sample_t)fun_compute(chan->f_param[1],c,s);
	float fact;
	if(s < start){
		return fun_compute(chan->f_param[2],c,s);
	}else if(s >= end){
		return fun_compute(chan->f_param[3],c,s);
	}else{
		fact = (float)( s - start) / (float)(end - start);
		return (1.0 - fact) * fun_compute(chan->f_param[2],c,s) +
			fact * fun_compute(chan->f_param[3],c,s);
	}
}
fun_t * f_ramp(fun_t *start, fun_t *end, fun_t *start_v, fun_t *end_v){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,ramp_fun,4,
				f_convert(start,SAMPLE),
				f_convert(end,SAMPLE),
				start_v,
				end_v );
	f->unit = fun_get_unit(start_v);
	fun_set_name(f,"ramp");
	return f;
}
float adsr_fun(channel_t *chan, int c, sample_t s){
	sample_t attack = (sample_t)fun_compute(chan->f_param[0],c,s);
	sample_t decay   = (sample_t)fun_compute(chan->f_param[1],c,s) + attack;
	sample_t sustain   = (sample_t)fun_compute(chan->f_param[2],c,s) + decay;
	sample_t release   = (sample_t)fun_compute(chan->f_param[4],c,s) + sustain;
	float fact;
	if(s < 0 || s >= release){
		return 0.0;
	}else if (s < attack){
		fact = (float)(s - attack) / (float)(attack);
		return 1.0 + fact ;
	}else if (s < decay){
		if(decay == attack){
			fact = 1.0;
		}else{
			fact = 1.0 + (float)(s - decay) / (float)(decay - attack);
		}
		return 1.0 - fact + fact * fun_compute(chan->f_param[3],c,s);
	}else if ( s < sustain){
		return fun_compute(chan->f_param[3],c,s);
	}else if (s < release){
		if(sustain == release){
			return 0.0;
		}
		fact = 1.0 + (float)(s- release) / (float)(release - sustain);
		return (1.0 - fact) * fun_compute(chan->f_param[3],c,s);
	}
	return 0.0;
}

fun_t * f_adsr(fun_t *attack, fun_t *decay, fun_t *sustain, fun_t *sustain_v ,fun_t *release){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,adsr_fun,5,
				f_convert(attack,SAMPLE),
				f_convert(decay,SAMPLE),
				f_convert(sustain,SAMPLE),
				f_convert(sustain_v,AMP),
				f_convert(release,SAMPLE)
				);
	f->unit = AMP;
	fun_set_name(f,"adsr envelope");
	return f;
}

float window_fun(channel_t *chan, int c, sample_t s){
	sample_t start = (sample_t)fun_compute(chan->f_param[0],c,s);
	sample_t end   = (sample_t)fun_compute(chan->f_param[1],c,s);
	if (s < start || s >= end){
		return 0.0;
	}else{
		return 1.0;
	}
}
fun_t *f_window(fun_t *start, fun_t *end){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,window_fun,2,
			f_convert(start,SAMPLE),
			f_convert(end,SAMPLE)	);
	f->unit = AMP;
	fun_set_name(f,"window");
	return f;
}

float delay_fun(channel_t *chan,int c,sample_t s){
	return fun_compute(chan->f_param[0],c,
			s - fun_compute(chan->f_param[1],c,s) );
}
fun_t *f_delay(fun_t *a, fun_t *delay){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,delay_fun,2,a,f_convert(delay,SAMPLE));
	f->unit = fun_get_unit(a);
	fun_set_name(f,"delay");
	return f;
}
float scale_fun(channel_t *chan, int c,sample_t s){
	return fun_compute(chan->f_param[0],c, 
			s * fun_compute(chan->f_param[1],c,s) );
}
fun_t *f_scale(fun_t *a, fun_t *factor){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,scale_fun,2,a,factor);
	f->unit = fun_get_unit(a);
	fun_set_name(f,"scale");
	return f;
}
float neg_fun(channel_t *chan, int c, sample_t s){
	return - fun_compute(chan->f_param[0],c,s);
}
fun_t *f_neg(fun_t *a){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,neg_fun,1,a);
	f->unit = fun_get_unit(a);
	fun_set_name(f,"negation");
	return f;
}
float add_fun(channel_t *chan, int c, sample_t s){
	return fun_compute(chan->f_param[0],c,s) +
		fun_compute(chan->f_param[1],c,s);
}
fun_t *f_add(fun_t *a, fun_t*b){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,add_fun,2,a,f_convert(b,fun_get_unit(a)));
	f->unit = fun_get_unit(a);
	fun_set_name(f,"addition");
	return f;
}
float mult_fun(channel_t *chan, int c, sample_t s){
	float f = fun_compute(chan->f_param[0],c,s);
	if(f != 0.0){
		return f * fun_compute(chan->f_param[1],c,s);
	}else{
		return 0.0;
	}
}
fun_t *f_mult(fun_t *a, fun_t*b){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,mult_fun,2,a,f_convert(b,fun_get_unit(a)));
	f->unit = fun_get_unit(a);
	fun_set_name(f,"multiply");
	return f;
}
float sin_fun(channel_t *chan, int c, sample_t s){
	float amp = fun_compute(chan->f_param[0],c,s);
	float hz  = fun_compute(chan->f_param[1],c,s);
	float phase = fun_compute(chan->f_param[2],c,s);
	double I = (2*3.141592 *hz*s) + phase;
	I /= SAMPLING_RATE;
	return amp*sin(I);
}
fun_t *f_sin(fun_t *amp, fun_t*hz,fun_t *phase){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,sin_fun,3,
			f_convert(amp,AMP),
			f_convert(hz,HZ),
			f_convert(phase,SAMPLE)	); 
	f->unit = AMP;
	fun_set_name(f,"sinusoid");
	return f;
}
float loop_fun(channel_t *chan, int c, sample_t s){
	sample_t period = (sample_t)fun_compute(chan->f_param[1],c,s);
	return fun_compute(chan->f_param[0],c,s % period);
}
fun_t *f_loop(fun_t *a, fun_t *period){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,loop_fun,2,a,f_convert(period,SAMPLE));
	f->unit = fun_get_unit(a);
	fun_set_name(f,"loop");
	return f;
}
fun_t *f_synsin(fun_t *amp,fun_t*hz,fun_t*phase){
	return f_loop(	f_sin(amp,f_convert(hz,HZ),f_convert(phase,SAMPLE)),
			f_convert(f_convert(hz,HZ),SAMPLE));
}
float tri_fun(channel_t *chan, int c, sample_t s){
	float amp = fun_compute(chan->f_param[0],c,s);
	sample_t period = (sample_t)fun_compute(chan->f_param[1],c,s);
	sample_t phase =  (sample_t)fun_compute(chan->f_param[2],c,s);
	s = (s + phase) % period;
	return amp*( (float)( -2.0 * s) / (float)period +1.0); 
}
fun_t *f_tri(fun_t*amp,fun_t*hz,fun_t*phase){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,tri_fun,3,
			f_convert(amp,AMP),
			f_convert(hz,SAMPLE),
			f_convert(phase,SAMPLE) );
	f->unit = AMP;
	fun_set_name(f,"triangle");
	return f;
}
float square_fun(channel_t *chan, int c, sample_t s){
	float amp = fun_compute(chan->f_param[0],c,s);
	float asy = fun_compute(chan->f_param[3],c,s);
	sample_t period = (sample_t)fun_compute(chan->f_param[1],c,s);
	sample_t phase =  (sample_t)fun_compute(chan->f_param[2],c,s);
	s = (s + phase) % period;
	if ( s < period *asy){
		return -amp;
	}else{
		return  amp;
	}
}
fun_t *f_square(fun_t*amp,fun_t*hz,fun_t*phase,fun_t *asymetry){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,square_fun,4,
			f_convert(amp,AMP),
			f_convert(hz,SAMPLE),
			f_convert(phase,SAMPLE),
			asymetry);
	f->unit = AMP;
	fun_set_name(f,"square");
	return f;
}
float noise_fun(channel_t *chan, int c, sample_t s){
	float rnd = (double)(random() - RAND_MAX/2) / (double)(RAND_MAX/2);
	rnd*= fun_compute(chan->f_param[0],c,s);
	return rnd;
}
fun_t *s_noise(fun_t *amp){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,noise_fun,1,f_convert(amp,AMP));
	f->unit = AMP;
	fun_set_name(f,"noise");
	return f;
}
float brownian_noise_fun(channel_t *chan, int c, sample_t s){
	float rnd = (double)(random() - RAND_MAX/2) / (double)(RAND_MAX/2);
	rnd *= fun_compute(chan->f_param[0],c,s);
	rnd += chan->n_param[0];
	chan->n_param[0] = rnd * fun_compute(chan->f_param[1],c,s);
	return rnd;
}
fun_t *s_brownian_noise(fun_t *var, fun_t *damp){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,brownian_noise_fun,2,
				f_convert(var,AMP),
				f_convert(damp,AMP));
	f->unit = AMP;
	fun_set_name(f,"brownian noise");
	return f;
}
float iir_two_fun(channel_t *chan, int c, sample_t s){
	float f = fun_compute(chan->f_param[0],c,s);
	f += fun_compute(chan->f_param[1],c,s)*chan->n_param[0];
	f += fun_compute(chan->f_param[2],c,s)*chan->n_param[1];
	chan->n_param[1] = chan->n_param[0];
	chan->n_param[0] = f;
	return f;
}
fun_t * s_iir_two(fun_t *in, fun_t*fac1, fun_t*fac2){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,iir_two_fun,3,in,fac1,fac2);
	f->unit = fun_get_unit(in);
	fun_set_name(f,"2o inf. impulse filter");
	return f;
}

float fir_two_fun(channel_t *chan, int c, sample_t s){
	float f = fun_compute(chan->f_param[0],c,s);
	float f2 = f;
	f += fun_compute(chan->f_param[1],c,s)*chan->n_param[0];
	f += fun_compute(chan->f_param[2],c,s)*chan->n_param[1];
	chan->n_param[1] = chan->n_param[0];
	chan->n_param[0] = f2;
	return f;
}

fun_t * s_fir_two(fun_t *in, fun_t*fac1, fun_t*fac2){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,fir_two_fun,3,in,fac1,fac2);
	f->unit = fun_get_unit(in);
	fun_set_name(f,"2o fin. impulse filter");
	return f;
}

	


	

	




