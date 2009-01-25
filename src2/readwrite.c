#include <ao/ao.h>
#include <stdio.h>
#include "readwrite.h"

int initialized = 0;
static	ao_device *device;
static	ao_sample_format format;
static	int default_driver;


static void audio_init(void){
	if(initialized){
		return;
	}
	initialized = 1;
	ao_initialize();
	
	default_driver = ao_default_driver_id();

	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;
	device = ao_open_live(default_driver, &format, NULL /* no options */);
}
void audio_close_devices(){
	ao_close(device);
	ao_shutdown();
}
void audio_write_stereo_16b(char*buffer, long length, const char* path){
	ao_device *f;
	audio_init();
	f = ao_open_file(ao_driver_id("wav"),path,1,&format,NULL);
	ao_play(f,buffer,length);
	ao_close(f);
}
void audio_play_stereo_16b(char*buffer, long length){
	audio_init();
	ao_play(device,buffer,length);
}

