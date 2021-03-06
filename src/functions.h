
#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "audio.h"
/* REAL TIME CACHE 
 * ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨
 * the real time cache keeps data generated by a function for
 * a certain time. New samples can be added and the old ones
 * will be discarded. 
 * params : 
 * 	size : the size in samples of the cache.
 * 	time : the time of the last sample inserted in cache
 * 	val_init: the value for past uninitialized values
 * */
typedef struct rtcache_s rtcache_t;
rtcache_t *rtcache_new();
void rtcache_init(rtcache_t *rtc, long size, long time, float val_init); 
float rtcache_get(rtcache_t *rtc, long time); /*returns 0 outside of cache*/
void  rtcache_set(rtcache_t *rtc, long time, float val); /*no effect if
	time isn't last_time + 1 */
/* STATIC CACHE 
 * ¨¨¨¨¨¨¨¨¨¨¨¨
 * the static cache keeps generated data in a static interval
 * you cannot retrieve or add data at times outside that interval.
 * params: 
 * 	size: the size in samples of the cache
 * 	start_time : the time of the first sample in cache 
 * */
typedef struct stcache_s stcache_t;
stcache_t *stcache_new(long size, long sample_start_time);
int stcache_is_sample_in(stcache_t* stc, long time);
float stcache_get(stcache_t *stc, long time);
void  stcache_set(stache_t *stc, long time, float val);

/* SIGNAL FUNCTIONS 
 * ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨
 * Signal functions return a single float value 
 * at every sample. Some signals acts as operators
 * as they need base signals to work.
 *
 * periodic signals have the w_prefix (w for wave)
 * for example 'w_sin' is a sine wave with certain frequency
 * and phase while 'win' is an operator returning the
 * sin of the function in parameter. 
 *
 * Signal functions never crash : in case of
 * divide by zero they return 1, and in case of
 * indetermination they return 0. 
 *
 * w_functions return values between -1 and 1 
 */
typedef struct fun_s fun_t;

/* 	EFFECT AND OPERATORS 
 * 	==================== 
 */

float funSampleGet(fun_t *fun, long sample);
	/* returns the sample at time sample */
float funSampleGetLin(fun_t *fun, double sample);
	/* returns the sample at time sample by computing
	 * the linear interpolation of two discrete sample
	 * values */
fun_t *name(fun_t *a, char *name);
	/* sets a name to the signal for convenient printing
	 * and debugging */
fun_t *mult(fun_t *a, fun_t *b);
	/* returns the product of a and b, computes a first,
	 * and if zero doesn't compute b.*/
fun_t *add(fun_t *a, fun_t *b);
	/* returns the addition of a and b */
fun_t *mix(fun_t *a, fun_t *b);
	/* mixes a and b together */
fun_t *neg(fun_t *a);
	/* returns '-a'*/
fun_t *absolute(fun_t *a);
	/* returns absolute value of a*/
fun_t *max(fun_t *a, fun_t *b);
	/* returns the maximum value of a and b if positive,
	 * minimum value of a and b if negative */
fun_t *min(fun_t *a, fun_t *b);
	/* returns the minimum value of a and b if positive,
	 * maximum value of a and b if negative */
fun_t *rmax(fun_t *a, fun_t *b);
	/* returns the maximum of a and b */
fun_t *rmin(fun_t *a, fun_t *b);
	/* returns the minimum of a and b */
fun_t *clamp(fun_t *limit, fun_t *exp, fun_t*a);
	/* returns a clamped to [-1 1] if 
	 * a is above limit, it is exposed
	 * by exp to smooth the cutoff */
fun_t *delay(msec_t phase, fun_t *a); 
	/* returns a with a delay of phase */
fun_t *fdelay(fun_t *msec_phase, fun_t *a);
	/* returns a with a delay of msec_phase. Uses 
	 * linear interpolation of samples */
fun_t *speed(fun_t *speedup_factor, long origin, fun_t*a);
	/* returns a scaled around origin.
	 * speedup_factor == 2 means a is played
	 * two times faster.
	 * speedup_factor < 0 -> plays in reverse.
	 * speedup_factor == 0 -> returns 0;
	 */
fun_t *raise(fun_t *low, fun_t *high, fun_t *a){
	/* a is considered to be between -1 and 1
	 * and returned function is between low and high
	 */
fun_t *diff(fun_t *a);
	/* returns the derivative of a, = variation of amplitude by sec */
fun_t *cache(msec_t start, msec_t end, fun_t* a);
	/* returns the same value as 'a', but values
	 * at times between start and end are stored for faster
	 * acess later. 'a' must be deterministic if you don't
	 * want strange things to happen */
fun_t *rtcache(msec_t length, float val, fun_t *a){
	/* returns same as a, but will cache past requested results 
	 * for length msec_t. 
	 * val is the value returned for past values that are too
	 * not in cache or before first time a was called. 
	 * 
	 * rtcache are supposed to be used when playing a sound in
	 * real time with filters that only need past values of a
	 * for a certain duration.
	 */


/* 	NON PERIODIC BASE SIGNALS
 * 	=========================
 */

fun_t *c(float value);
fun_t *pulse(msec_t t, fun_t* ground, fun_t* pulse); 
fun_t *unit(msec_t t, float before, float after) ;
fun_t *ramp(msec_t t1, msec_t t2, float before, float after);
fun_t *sincc(float a);
fun_t *trapeze( msec_t time, msec_t release, float before, float after);
fun_t *note(	msec_t time, 
		msec_t attack,
		msec_t decay,
		msec_t sustain, 
		msec_t release, 
		float peak, 
		float amplitude);
fun_t *window(  msec_t start, msec_t end, msec_t alias, fun_t *fun);

/* 	PERIODIC BASE SIGNALS
 *  	===================== */

fun_t w_sin(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_square(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_sawtooth(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_triangle(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_pulse(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_circle(fun_t *freq, fun_t *msec_phase, fun_t *amp);
fun_t w_noise(fun_t *amp);

/* 	PERIODIC SIGNALS 
 *  	================
 */
fun_t w_repeat(fun_t *hz_freq, fun_t *msec_phase, fun_t *a);
fun_t w_repeatadd(fun_t *hz_freq, fun_t *msec_phase, fun_t *a, fun_t *a_length) 

fun_t *sine(fun_t * freq,  msec_t phase, float amplitude);
fun_t *square(fun_t *freq, fun_t *up, msec_t phase, float amplitude);
fun_t *sawtooth(fun_t *freq, fun_t *exp, msec_t phase, float amplitude);
fun_t *triangle(fun_t *freq, msec_t phase, float amplitude);
fun_t *decay(hz_t freq, msec_t phase, float up, float amplitude);
fun_t *pulses(hz_t freq, msec_t phase, float ground, float amplitude);
fun_t *noise(fun_t* volume);
fun_t *loopCut(msec_t period, msec_t phase, fun_t * sample);
fun_t *loopAdd(msec_t period, msec_t phase, fun_t * sample);

/*effect*/

void trackMult(track_t *t, fun_t *f1, fun_t *f2);
void trackAdd(track_t *t,  fun_t*f1, fun_t*f2);



#endif
