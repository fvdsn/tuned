#ifndef __TUNED_READ_WRITE_H__
#define __TUNED_READ_WRITE_H__
void audio_write_stereo_16b(char*buffer, long length, const char* path);
void audio_play_stereo_16b(char*buffer, long length);
void audio_close_devices(void);
#endif

