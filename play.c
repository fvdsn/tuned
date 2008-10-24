#include <ao/ao.h>
#include <stdio.h>

#include "audio.h"

static	ao_device *device;
static	ao_sample_format format;
static	int default_driver;
void initAudio(){
	
	ao_initialize();
	
	default_driver = ao_default_driver_id();

	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;
	device = ao_open_live(default_driver, &format, NULL /* no options */);
}
void closeAudio(){
	ao_close(device);
	ao_shutdown();
}
void trackWrite(const track_t *t, char *file){
	ao_device *f = ao_open_file(ao_driver_id("wav"),file,1,&format,NULL);
	ao_play(f,t->buffer,t->byte_length);
	ao_close(f);
	if(!t->buffer || !t->byte_length){
		printf("couldn't write empty track\n");
	}else{
		printf("writen %d bytes of buffer %p\n",(int)t->byte_length,t->buffer);
	}
}
void trackPlay(const track_t *t){
	ao_play(device,t->buffer,t->byte_length);
	if(!t->buffer || !t->byte_length){
		printf("couldn't play empty track\n");
	}
}
long file_size(FILE *f){
	long curpos;
	long endpos;
	if (f == NULL){
		return 0;
	}else{
		curpos = ftell(f);
		fseek(f,0,SEEK_END);
		endpos = ftell(f);
		fseek(f,curpos,SEEK_SET);
		return endpos;
	}
}
track_t *trackRead(char *path){
	FILE *f = fopen(path,"r");
	long size = file_size(f);
	long length = size / (2 *BYTE_RATE); /* 2 channels */
	track_t * t = (track_t*)malloc(sizeof(track_t));
	t->length = (int)length;
	t->byte_length = size;
	t->channels = 2;
	t->buffer = (char*)malloc(size);
	fread(t->buffer,size,1,f);
	return t;
}

