#include<math.h>
#include<stdlib.h>
#include<string.h>
#include "functions.h"

struct rtcache_s{
	int initialized;
	long cache_size;
	float val_init;
	float *val_cache; /*sample value*/
	long current_time;
	long current; /*rotating buffer val_cache[current]*/
};

void rtcache_init(rtcache_t *rtc, long size, long time, float val_init){
	int i = size;
	if(rtc->initialized == 0){
		rtc->initialized = 1;
		rtc->val_cache = (float*)malloc(size*sizeof(float));
	}else if(size != rtc->cache_size){
		free(rtc->val_cache);
		rtc->val_cache = (float*)malloc(size*sizeof(float));
	}
	rtc->cache_size = size;
	rtc->current = 0;
	rtc->current_time = time;
	rtc->val_init = val_init;
	while(i--){
		rtc->val_cache[i] = val_init;
	}
	return;
}
float rtcache_get(rtcache_t *rtc, long time){
	if(time > rtc->current_time ||
		rtc->current_time - time >= rtc->cache_size){
		return rtc->val_init;
	}else{
		return rtc->val_cache[
		(rtc->current - ( rtc->current_time - time)) % rtc->cache_size];
	}
}
void rtcache_set(rtcache_t *rtc, long time, float val){
	if(time != rtc->current_time + 1){
		return;
	}else{
		rtc->current_time++;
		rtc->current = (rtc->current + 1) % rtc->cache_size;
		rtc->val_cache[rtc->current] = val;
		return;
	}
}
		
	
