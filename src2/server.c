#include <stdlib.h> 
#include "function.h"
#include "readwrite.h"


char buffer[SAMPLING_RATE*2*2*10];

int main(int argc, char**argv){
	fun_t *f;
	f = f_mult(f_window(f_cu(0,SEC),f_cu(3,SEC)),
			f_sin(f_c(1),f_cu(50,HZ),f_c(0)) );
	f = f_add(f,
	    f_mult(f_window(f_cu(3,SEC),f_cu(6,SEC)),
			f_tri(f_c(1),f_cu(50,HZ),f_c(0)) ));
	f = f_add(f,
	    f_mult(f_window(f_cu(6,SEC),f_cu(9,SEC)),
			f_square(f_c(1),f_cu(50,HZ),f_c(0),f_c(0.5)) ));
	
	set_bpm(f_ramp(f_cu(2,SEC),f_cu(8,SEC),f_c(60),f_c(70)));
	set_period(f_c(4));
	/*
	fun_print(f);
	*/

	f = s_down_sample(f_c(10),f);

	fun_record_16b(f,0,SAMPLING_RATE*10,2,buffer);
	audio_write_stereo_16b(buffer,SAMPLING_RATE*2*2*10,"test.wav");

	return 0;
}
