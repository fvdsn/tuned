#include <math.h>
#include "unit.h"
static float bpm = 132;
void bpm_set(float _bpm){
	bpm = _bpm;
float bpm_get(){
	return bpm;
}
msec_t to_msec(double value,unit_t u){
	switch(u){
		case U_NO_UNIT: return value;
		case U_SAMPLE: return  value * 1000 / AUDIO_RATE; 
		case U_SEC: return value * 1000.0;
		case U_MSEC: return value;
		case U_NSEC: return value / 1000.0;
		case U_METER: return value / SOUND_SPEED * 1000;
		case U_HZ :
			if (value >= AUDIO_RATE){
				return 1000.0 / AUDIO_RATE; /*1 sample*/
			}else if (value <= 1.0 / AUDIO_RATE ){
				printf("WARNING : frequency too low\n");
				return 1000 * 44100; 
			}else{
				return 1000.0 / value;
			}
		case U_BEAT: 
		case U_BPM:
			return 60000.0/bpm_get();
		default :
			printf("WARNING : cannot convert volume to msec\n");
			return value;
	}
}
long to_sample(double value, unit_t u){
	switch(u){
		case U_NO_UNIT:
		case U_SAMPLE: 
			return value;
		case U_METER: return value / SOUND_SPEED * AUDIO_RATE:
		case U_SEC: return value * AUDIO_RATE;
		case U_MSEC: return value * AUDIO_RATE / 1000.0;
		case U_NSEC: return value * AUDIO_RATE / 100000.0;
		case U_HZ :
			if ( value >= AUDIO_RATE){
				return 1;
			}else if (value <= 1.0 / AUDIO_RATE){
				return AUDIO_RATE*AUDIO_RATE;
			}else{
				return AUDIO_RATE/value;
			}
		case U_BEAT:
		case U_BPM:
			return 60.0/bpm_get()*AUDIO_RATE;
		default:
			printf("WARNING : cannot convert volume to sample \n");
			return value;
	}
}
long to_byte(double value, unit_t u){
	return to_sample(value,u)*BYTE_RATE;
}
float to_volume(double value, unit_t u){
	switch(u){
		case U_NO_UNIT: 
		case U_VOLUME : 
			return value;
		case U_DB : 
			return 20*log10(pow(2,16)*value);
		default : 
			printf("WARNING : cannot convert time to volume \n");
			return value;
	}
}
float to_db(double value, unit_t u){
	switch(u){
		case U_NO_UNIT:
		case U_DB:
			return value;
		case U_VOLUME :
			return pow(10,db/20.0) / pow(2,16);
		default : 
			printf("WARNING : cannot convert time to decibels \n");
			return value;
	}
}
hz_t to_hz(double value, unit_t u){
	switch(u){
		case U_NO_UNIT: return value;
		case U_HZ : return value;
		case U_NSEC:
			value /=1000;
		case U_MSEC:
			value /=1000;
		case U_SEC : 
			if (value == 0){
				return AUDIO_RATE;
			}else{
				return 1.0/value;
			}
		default : 
			printf("WARNING: cannot convert Volume to Hertz\n");
			return value;
	}
}
