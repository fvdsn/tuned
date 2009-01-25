#ifndef __TUNED_FUNCTION_H__
#define __TUNED_FUNCTION_H__

typedef long int sample_t;
typedef struct channel_s channel_t;
typedef struct fun_s fun_t;
typedef float (*fun_f)(channel_t *chan, int c, sample_t sample);

#define SAMPLING_RATE 44100

typedef enum unit_e{
	/* time, period and hz */
	SEC,
	MSEC,
	HZ,
	SAMPLE,
	PERIOD,	/* 3,4,5 or 8 beats */
	BEAT,
	HBEAT,	/* Half a beat*/
	QBEAT,	/* 1/4  a beat*/
	EBEAT,  /* 1/8  a beat*/

	/* amplitude */
	AMP,	/* amplitude */
	DB,	/* Decibels */
	UNIT,	/* value normalized in [-1,1] */
	NO_UNIT	/* not a unit */
}unit_t;
#define FUN_NAME_LENGTH 32
#define CHAN_MAX_PARAM 8

#define CHAN_VARIABLE 0
#define CHAN_CONSTANT 1
#define CHAN_FLAT     2
#define CHAN_INCREASE 3
#define CHAN_DECREASE 4
#define CHAN_POSITIVE 5
#define CHAN_NEGATIVE 6

#define ARG_FREQ 0
#define ARG_VOLUME 1
#define ARG_DELAY 2
#define ARG_SUSTAIN 3

struct channel_s{
	fun_f fun;
	int channel;

	/*cache */
	int constant;
	int uptodate;
	sample_t first_sample;
	sample_t last_sample;
	float last_value;

	/*params*/
	float n_param[CHAN_MAX_PARAM];
	sample_t s_param[CHAN_MAX_PARAM];
	int f_param_count;
	fun_t **f_param; /*[F_PARAM_COUNT]*/
};

struct fun_s{
	unit_t unit;
	int constant;
	int isarg;
	long arg;
	int user_count;
	sample_t start;
	sample_t end;
	int virtual_channel_count;
	int channel_count;
	char name[FUN_NAME_LENGTH];
	channel_t **channel; /*[channel_count]*/
};

channel_t *channel_new(fun_f fun, int chan, int f_param_count);
void	channel_free(channel_t *chan);
void 	channel_set_f_param(channel_t *chan, int index, fun_t*f);
float 	channel_compute(channel_t *chan,int c, sample_t s);

fun_t *fun_new(int channel_count);
fun_t *fun_user(fun_t* fun);
fun_t *fun_free(fun_t* fun);
float  fun_compute(fun_t*fun, int chan, sample_t s);
unit_t fun_get_unit(fun_t* fun);
int  	fun_channel_count(fun_t *fun);
void	fun_set_name(fun_t *fun,const char*name);
void fun_print(fun_t *fun);
void fun_record_16b(	fun_t *fun, 
			sample_t start, 
			sample_t end, 
			int channel_count,
			char *buffer );
void set_bpm(fun_t *bpm);
void set_period(fun_t *period);
/* functions */
fun_t * f_identity(fun_t *f);
fun_t * f_arg(int arg,fun_t *argv);
fun_t * f_make(int arg, fun_t *argv,fun_t *f);
fun_t * f_now(fun_t *a);
fun_t * f_n(const char *note);
fun_t * f_c(float value);
fun_t * f_cu(float value, unit_t unit);
fun_t * f_ctab(int count, unit_t unit, float value, ... );
fun_t * f_tab(int count, fun_t * fun, ... );
fun_t * f_ch(int channel, fun_t * fun);
fun_t * f_convert(fun_t *fun, unit_t unit);
fun_t * f_delay(fun_t *fun, fun_t *delay);
fun_t * f_scale(fun_t *fun, fun_t *factor);
fun_t * f_past(fun_t *fun, fun_t *delay);
fun_t * f_neg(fun_t *a);
fun_t * f_abs(fun_t *a);
fun_t * f_sign(fun_t *a, fun_t *b);
fun_t * f_max(fun_t *a, fun_t *b);
fun_t * f_min(fun_t *a, fun_t *b);
fun_t * f_limit(fun_t *a, fun_t *b);
fun_t * f_mult(fun_t *a, fun_t *b);
fun_t * f_add(fun_t *a, fun_t *b);
fun_t * f_mix(int count, fun_t *a, ... );
fun_t * f_div(fun_t *a, fun_t *b);
fun_t * f_log(fun_t *base, fun_t *val);
fun_t * f_exp(fun_t *base, fun_t *val);
fun_t * f_map(fun_t *op, fun_t *scale, fun_t *input);
fun_t * f_unit(fun_t *time);
fun_t * f_pulse(fun_t *time);
fun_t * f_window(fun_t *start, fun_t *end);
fun_t * f_ramp(fun_t *start, fun_t *end, fun_t *start_v, fun_t *end_v);
fun_t * f_adsr(fun_t *attack, fun_t *decay, fun_t *sustain, fun_t *sustain_v ,fun_t *release);
fun_t * f_expramp(fun_t *base,fun_t *start, fun_t *end, fun_t *start_v, fun_t *end_v);
fun_t * f_loop(fun_t *fun, fun_t *period);
fun_t * f_sin(fun_t *amp,fun_t *hz, fun_t *phase);
fun_t * f_synsin(fun_t *amp,fun_t *hz, fun_t *phase);
fun_t * f_rsine(fun_t *vala, fun_t *valb, fun_t *hz, fun_t *phase);
fun_t * f_tri(fun_t *amp, fun_t *hz, fun_t *phase);
fun_t * f_rtri(fun_t *vala, fun_t *valb, fun_t *hz, fun_t *phase);
fun_t * f_square(fun_t *amp, fun_t *hz, fun_t *phase, fun_t *asymetry);
fun_t * f_rsquare(fun_t *vala, fun_t *valb, fun_t *hz, fun_t *phase, fun_t *asymetry);
fun_t * f_echo(fun_t *a, fun_t *delay, fun_t *ampl);
fun_t * f_downsample(fun_t *a, fun_t *sample);
fun_t * f_solarize(fun_t *a, fun_t *factor);
fun_t * f_linear_compress(fun_t *in, fun_t *control, fun_t *treshold, fun_t *gain);
fun_t * f_quad_compress(fun_t *in, fun_t *control, fun_t *treshold, fun_t *gain);

fun_t * s_noise(fun_t *amp);
fun_t * s_average(fun_t *in);
fun_t * s_brownian_noise(fun_t *var, fun_t *damp);
fun_t * s_iir_two(fun_t *in, fun_t*fac1, fun_t*fac2);
fun_t * s_fir_two(fun_t *in, fun_t*fac1, fun_t*fac2);
fun_t * s_lowpass(fun_t *a, fun_t *freq);
fun_t * s_hipass(fun_t *a, fun_t *freq);
fun_t * s_resonance(fun_t *a, fun_t *amp, fun_t *freq);
fun_t * s_nbody(int body, fun_t *k, fun_t *d, fun_t *in);
fun_t * s_noise_gate(fun_t *a, fun_t *treshold, fun_t *tolerance);

fun_t * a_freq(fun_t *hz);
fun_t * a_vol(fun_t *vol);
fun_t * a_delay(fun_t *sample);
fun_t * a_sustain(fun_t *sample);

fun_t * f_note(fun_t *delay, fun_t*freq, fun_t *vol, fun_t *sust, fun_t *instr);

#endif

