#include <math.h>
#include "function.h"
float lin_compress_fun(channel_t *chan, int c, sample_t s){
	float limit = fun_compute(chan->f_param[0],c,s);
	float gain = fun_compute(chan->f_param[1],c,s);
	float val = fun_compute(chan->f_param[2],c,s);
	if (val < limit && - val < limit){
		return val;
	}else if (val > 0){
		return limit + (val-limit)* (1.0-gain)*val;
	}else{
		return -limit - (val+limit)* (1.0-gain)*-val;
	}
}
fun_t *f_lin_compress(fun_t *limit, fun_t *gain, fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,lin_compress_fun,3,limit,gain,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"linear compressor");
	return f;
}
float pow_compress_fun(channel_t*chan,int c, sample_t s){
	float p = fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val > 0){
		return powf(val,p);
	}else{
		return -powf(-val,p);
	}
}
fun_t *f_pow_compress(fun_t *power, fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,pow_compress_fun,2,power,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"power compressor");
	return f;
}
float log_compress_fun(channel_t*chan,int c, sample_t s){
	float val = fun_compute(chan->f_param[0],c,s);
	if(val > 0){
		return log(1.0+val)/log(2);
	}else{
		return -log(1.0-val)/log(2);
	}
}
fun_t *f_log_compress(fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,log_compress_fun,1,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"logarithm compressor");
	return f;
}
float sin_compress_fun(channel_t*chan,int c,sample_t s){
	float val = fun_compute(chan->f_param[0],c,s);
	if(val > 0){
		return sin(val*3.141592/2.0);
	}else{
		return -sin(-val*3.141592/2.0);
	}
}
fun_t *f_sin_compress(fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,sin_compress_fun,1,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"sinus compressor");
	return f;
}
float sigmoid_compress_fun(channel_t*chan,int c,sample_t s){
	float fact = -fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val > 0){
		return 1.0 / (1.0 + expf(fact*val)) - 0.5;
	}else{
		return - (1.0 / (1.0 +expf(fact*-val)) - 0.5);
	}
}
fun_t *f_sigmoid_compress(fun_t* factor,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,sigmoid_compress_fun,2,factor,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"sigmoid compressor");
	return f;
}
float cutoff_fun(channel_t*chan, int c, sample_t s){
	float limit = fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val < limit && -val < limit){
		return 0.0;
	}else{
		return val;
	}
}
fun_t *f_cutoff(fun_t* limit,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,cutoff_fun,2,limit,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"simple cutoff");
	return f;
}
float sigmoid_limit_fun(channel_t*chan, int c, sample_t s){
	float fact = -fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val > 0){
		return 1.0 / (1.0 + expf(fact*val - 0.5));
	}else{
		return -1.0 / (1.0 + expf(fact*-val -0.5));
	}
}
fun_t *f_sigmoid_limit(fun_t* fact,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,sigmoid_limit_fun,2,fact,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"sigmoid limiter");
	return f;
}
float atan_limit_fun(channel_t *chan, int c, sample_t s){
	float fact = fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val > 0){
		return atanf(fact*val)*2.0/3.141592;
	}else{
		return -atanf(-fact*val)*2.0/3.141592;
	}
}
fun_t *f_atan_limit(fun_t* fact,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,atan_limit_fun,2,fact,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"atan limiter");
	return f;
}
	
float hard_limit_fun(channel_t*chan,int c, sample_t s){
	float limit = fun_compute(chan->f_param[0],c,s);
	float val = fun_compute(chan->f_param[1],c,s);
	if (val < limit && -val < limit){
		return val;
	}else{
		return limit;
	}
}
fun_t *f_hard_limit(fun_t* limit,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,hard_limit_fun,2,limit,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"hard limiter");
	return f;
}
float down_sample_fun(channel_t *chan, int c, sample_t s){
	sample_t interval = (sample_t)fun_compute(chan->f_param[0],c,s);
	if( s - chan->s_param[0] >= interval ){
		chan->n_param[0] = fun_compute(chan->f_param[1],c,s);
		chan->s_param[0] = s;
	}
	return chan->n_param[0];
}
fun_t *s_down_sample(fun_t* sampling_interval,fun_t* input){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,down_sample_fun,2,sampling_interval,input);
	f->unit = fun_get_unit(input);
	fun_set_name(f,"downsampler");
	return f;
}
float fade_fun(channel_t *chan, int c, sample_t s){
	float f = fun_compute(chan->f_param[0],c,s);
	if (f == 0.0){
		return fun_compute(chan->f_param[1],c,s);
	}else if (f == 1.0){
		return fun_compute(chan->f_param[2],c,s);
	}else{
		return f*fun_compute(chan->f_param[2],c,s)
			+(1.0-f)*fun_compute(chan->f_param[1],c,s);
	}
}
fun_t *f_fade(fun_t *factor, fun_t*a,fun_t*b){
	fun_t *f = fun_new(1);
	fun_setup_channels(f,fade_fun,3,factor,a,b);
	f->unit = fun_get_unit(a);
	fun_set_name(f,"fader");
	return f;
}




