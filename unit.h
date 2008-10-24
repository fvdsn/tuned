#ifndef __UNITS_H__
#define __UNITS_H__

#define AUDIO_RATE 44100 /* samples per second */
#define BYTE_RATE 2 /* bytes per channel per sample */
#define SOUND_SPEED 300 /*meter per second*/

typedef double msec_t;
typedef double hz_t;

typedef enum unit_e{
	U_NO_UNIT,
	U_SAMPLE,
	U_SEC,
	U_METER,
	U_MSEC,
	U_NSEC,
	U_HZ,
	U_BEAT,
	U_BPM,
	U_DB,
	U_VOLUME
}unit_t;

float bpm_get();
void bpm_set(float bpm);

msec_t to_msec(double value,unit_t u);
long   to_sample(double value, unit_t u);
long   to_byte(double value, unit_t u, int channels);
float  to_volume(double value, unit_t u);
float  to_db(double value, unit_t u);
hz_t   to_hz(double value, unit_t u);




/* UNITS ======================================================= */
typedef double msec_t; 
msec_t sec(double sec); /*seconds to msec */
msec_t nsec(double nsec); /*nanoseconds to msec */
msec_t sample(double samples); /*samples to msec */
long msec_to_sample(msec_t msec);
long msec_to_byte(msec_t msec, int channels); /*memory use*/

msec_t  hz_to_period(hz_t hz); /* period of freq. -1 if hz == 0, 0 if hz is higher than AUDIO_RATE / 2 */
long hz_to_sample(hz_t hz);   /* same as above */

float db(float db); /*db to vol */
float vol(float vol); /*vol to db*/
#endif
