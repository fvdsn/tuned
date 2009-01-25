#include <stdlib.h> 
#include "function.h"
#include "readwrite.h"


char buffer[SAMPLING_RATE*2*2*10];

int main(int argc, char**argv){
	fun_t *f;
	fun_t *instr = f_sin(f_c(0.5), a_freq(f_n("la3")),f_c(0));
	instr = f_add( instr, f_sin(f_c(0.5), f_mult(f_c(2),a_freq(f_n("la5"))),f_c(0)));
	instr = f_mult( f_adsr( f_cu(10,MSEC),
				f_cu(20,MSEC),
				a_sustain(f_c(100)),
				f_c(0.66),
				f_cu(500,MSEC) ), instr );
	instr = f_mult(instr, a_vol(f_c(1)));
	instr = f_delay(instr,a_delay(f_c(0)));






	set_bpm(f_ramp(f_cu(2,SEC),f_cu(8,SEC),f_c(60),f_c(70)));
	set_period(f_c(4));
	f = 		f_note(f_cu(1,SEC),f_n("do2"),f_c(0.6), f_cu(1,SEC),instr);
	f = f_add(f,	f_note(f_cu(2,BEAT),f_n("re2"),f_c(0.6),f_cu(200,MSEC),instr));
	f = f_add(f,	f_note(f_cu(3,BEAT),f_n("mi2"),f_c(0.5),f_cu(1,SEC),instr));
	f = f_add(f,	f_note(f_cu(4,BEAT),f_n("fa2"),f_c(0.5),f_cu(10,MSEC),instr));
	f = f_add(f,	f_note(f_cu(5,BEAT),f_n("sol2"),f_c(0.7),f_cu(10,MSEC),instr));
	f = f_add(f,	f_note(f_cu(6,BEAT),f_n("la2"),f_c(0.5),f_cu(10,MSEC),instr));
	f = f_add(f,	f_note(f_cu(7,BEAT),f_n("si2"),f_c(0.5),f_cu(10,MSEC),instr));
	/*
	fun_print(f);
	*/
	fun_record_16b(f,0,SAMPLING_RATE*10,2,buffer);
	audio_write_stereo_16b(buffer,SAMPLING_RATE*2*2*10,"test.wav");

	return 0;
}
