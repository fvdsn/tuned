#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "function.h"

fun_t *f_arg(int arg,fun_t *argv){
	fun_t *f;
	if(arg < 0 || arg >= 64){
		printf("ERROR: f_arg() : arg out of range [0,64[ : %d\n",arg);
		return argv;
	}else{
		f = f_identity(argv);
		f->isarg = 1;
		f->arg |= (1 << arg);
		fun_set_name(f,"argument");
		return f;
	}
}
static channel_t * channel_dup(channel_t *chan){
	channel_t *chand = channel_new(chan->fun,chan->channel,chan->f_param_count);
	fun_t ** f_param = chand->f_param;
	memcpy(chand,chan,sizeof(channel_t));
	chand->f_param = f_param;
	memcpy(chand->f_param,chan->f_param,chan->f_param_count*sizeof(fun_t**));
	return chand;
}
static fun_t *fun_dup(fun_t *f){
	int i = 0;
	fun_t *fd = fun_new(f->channel_count);
	fd->unit = f->unit;
	fd->constant = f->constant;
	fd->isarg = f->isarg;
	fd->arg = f->arg;
	fd->user_count = 0;
	fd->virtual_channel_count = f->virtual_channel_count;
	fd->channel_count = fd->channel_count;
	memcpy(fd->name,f->name,FUN_NAME_LENGTH);
	while(i < fd->channel_count){
		fd->channel[i] = channel_dup(f->channel[i]);
		i++;
	}
	return fd;
}
fun_t *f_make(int arg, fun_t *argv, fun_t *f){
	fun_t *fd;
	fun_t *fc;
	channel_t *chan;
	int i = 0;
	int j = 0;
	if(arg < 0 || arg > 64){
		printf("ERROR: f_make() : arg out of range[0,64[ : %d\n",arg);
		return f;
	}else if(f->arg & (1 << arg)){
		if(f->isarg){
			return argv;
		}
		fd = fun_dup(f);
		fd->arg &= ~(1 << arg);
		if(fun_channel_count(fd) < fun_channel_count(argv)){
			fd->virtual_channel_count = fun_channel_count(argv);
		}
		while(i < fd->channel_count){
			j = 0;
			chan = fd->channel[i];
			while( j < chan->f_param_count){
				fc = chan->f_param[j];
				if(fc->arg & (1 << arg)){
					chan->f_param[j] = f_make(arg,argv,fc);
				}
				j++;
			}
			i++;
		}
		return fd;
	}else{
		return f;
	}
}

fun_t * a_freq(fun_t *hz){
	return f_arg(ARG_FREQ,hz);
}
fun_t * a_vol(fun_t *vol){
	return f_arg(ARG_VOLUME,vol);
}
fun_t * a_delay(fun_t *sample){
	return f_arg(ARG_DELAY,sample);
}
fun_t * a_sustain(fun_t *sample){
	return f_arg(ARG_SUSTAIN,sample);
}
fun_t * f_note(fun_t *delay, fun_t*freq, fun_t *vol, fun_t *sust, fun_t * instr){
	return f_make(ARG_DELAY,f_convert(delay,SAMPLE),
		f_make(ARG_FREQ,f_convert(freq,HZ),
		 f_make(ARG_VOLUME,f_convert(vol,AMP),
		  f_make(ARG_SUSTAIN,f_convert(sust,SAMPLE),instr ))));
}
