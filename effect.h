#ifndef __EFFECT_H__
#define __EFFECT_H__
#include "audio.h"
#include "functions.h"
#define WAVE_UP 0
#define WAVE_DOWN 1
#define WAVE_BOTH 2
void fx_cutoff(track_t *t,fun_t* max, fun_t *fact,int wave); 
void fx_echo(track_t *t, fun_t *delay, fun_t *amp, int count);
void fx_power_dist(track_t*t, fun_t *gain, fun_t*clip, fun_t *power,
int wave);
void fx_disto(track_t *t,int wave);
void fx_phase_disto(track_t *t,int wave);
void fx_reverb(track_t *t);
void fx_phase_dist(track_t*t, fun_t*clip,fun_t*delay,int wave);
void fx_fbox(track_t*t,fun_t* boxlength);
void fx_ftent(track_t*t,fun_t* tentlength);
void fx_flange(track_t*t,fun_t*delay);

#endif
