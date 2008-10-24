#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "audio.h"
#include "functions.h"
#include "effect.h"
#include "notes.h"
#include "beat.h"
#include "drums.h"

void pouu(track_t *t, int pos, int length, int freq, float vol){
	track_t *n = trackNew(110 + 2*length);
	float hi_vol  = 0.125	*vol;
	float med_vol = 0.25	*vol;
	float low_vol = 0.5	*vol;
	int   hi_sust = 0.25	*length;
	int med_sust  = 0.5	*length;
	int low_sust  = 1.0	*length;
	fun_t *nte;
	fun_t *hi_note = note(0,10,100,hi_sust,med_sust,med_vol,hi_vol);
	fun_t *med_note = note(0,10,100,med_sust,low_sust,low_vol,med_vol);
	fun_t *low_note = note(0,10,100,low_sust,low_sust,low_vol,low_vol);
	fun_t *vhi_note = note(0,10,100,hi_sust,hi_sust,med_vol/2,hi_vol/2);
	freq/=2;
	
	
	nte = 	
			add( 	mult(low_note, sine(trapeze(0,low_sust*2,freq,freq),0,1.0)),
			add( 	mult(med_note, sine(trapeze(0,low_sust*2,freq*2,freq*2),0,1.0)),
			add(	mult(hi_note, sine(trapeze(0,low_sust*2,freq*4,freq*4),0,0.3)),
				mult(vhi_note,sine(c(freq*8),0,0.05)) )));
	nte = mult(nte,add(c(0.9),sine(c(5),100,0.1)));
	nte = mult(unit(0,0.0,1.0),mult(unit(low_sust*2,1.0,0.0),nte));
	trackAdd(n,nte,nte);
	trackMix(t,n,pos,1.0);
}
void bassloop(track_t *t,int base,float speed){
	int duration = 300;
	pouu(t,base,duration,ff("la0"),0.5);
	pouu(t,base + (1*speed),duration,ff("la0"),0.5);
	pouu(t,base + (2*speed),duration,ff("do1"),0.5);
	pouu(t,base + (3*speed),duration,ff("la0"),0.5);
	pouu(t,base + (4*speed),duration,ff("la0"),0.5);
	pouu(t,base + (5*speed),duration,ff("la0"),0.5);
	pouu(t,base + (6*speed),duration,ff("re1"),0.5);
	pouu(t,base + (7*speed),duration,ff("do1"),0.5);
}
void tshh(track_t *t, int pos, int length,int echo, float vol){
	track_t *n = trackNew(20+20+length+echo+60);
	fun_t * f = noise(note(0,1,20,20,length,vol,vol/2));
	f = 	add( f, 
		add( delay(echo,mult(c(0.2),f)),
		add( delay(echo + 10,mult(c(0.2),f)),
		add( delay(echo + 20,mult(c(0.2),f)),
		add( delay(echo + 30,mult(c(0.1),f)),
		add( delay(echo + 40,mult(c(0.1),f)),
		add( delay(echo + 50,mult(c(0.05),f)),
		     delay(echo + 60,mult(c(0.05),f))	)))))));

	trackAdd(n,f,f);
	trackMix(t,n,pos,1.0);
}
void tshhloop(track_t *t, int base, float speed){
	int toc = 10;
	int tish = 300;
	tshh(t,base +(0*speed),toc,10,0.24);
	tshh(t,base +(2*speed),toc,10,0.24);
	tshh(t,base +(4*speed),toc,10,0.24);
	tshh(t,base +(6*speed),tish,100,1.0);
}
void bonk(track_t *t, int base, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/pok.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l*0.75,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,base,vol);
}
void sq_bass(track_t *t, int base, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/basse/sq-basse.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l*0.75,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,base,vol);
}

void song(track_t*t){
	track_t *s = trackNew(8000);
	track_t *b = trackNew(8000);
	fun_t *f;
	fun_t *f2;
	bassloop(t,0,200);
	bassloop(t,8*200,200);
	bassloop(t,16*200,200);
	bassloop(t,24*200,200);
	bassloop(t,32*200,200);
	bassloop(t,46*200,200);
	bassloop(t,54*200,200);
	bassloop(t,62*200,200);
	f = min(c(1.0),max(ramp(4000,8400,0,1),sine(ramp(2000,8400,5,8),0,10)));
	f2 =max(unit(2400,0.2,0),unit(2400,0,1.0));
		
	trackMult(t,f,f);
	f = max(unit(5000,1,0),max(unit(8500,0,1),
		min(c(1.0),max(ramp(5000,8500,1,0),sine(ramp(5000,8000,20,200),0,10)))
		));
	trackMult(t,f,f);
	//trackMult(t,f2,f2);
	//fx_reverb(t);
	fx_disto(t,WAVE_BOTH);

	tshhloop(s,0*200,200);
	tshhloop(s,8*200,200);
	tshhloop(s,16*200,200);
	tshhloop(s,24*200,200);
	tshhloop(s,32*200,200);
	tshhloop(s,40*200,200);
	tshhloop(s,46*200,200);
	tshhloop(s,54*200,200);
	tshhloop(s,56*200,200);
	tshhloop(s,62*200,200);
	f = ramp(0,8000,0.2,0.7);
	trackMult(s,f,f);
	trackMix(t,s,0,1);
		
	//fx_reverb(b);
	//fx_disto(b);
	//trackMix(t,b,0,0.5);	
}
fun_t *cool_sound(fun_t *freq, fun_t *envelope, int pos, float vol){
	fun_t *freq8 = mult(freq,c(8)); //original cool noise freq is 55
	fun_t *freq54 = mult(freq,c(0.545454));
	fun_t *f = add(mult(sine(freq8,0,0.2),add(sawtooth(freq,add(c(4.0),sawtooth(c(1),c(1),0,2.0)),0,0.5),c(0.5))),
			square(freq54,add(sine(c(0.5),0,0.3),c(0.4)),0,0.2));
	f = mult(envelope,f);
	f = fdelay(c(pos),f);
	f =   mult(c(vol),f);
	return f;
}

int main(int argc, char **argv){
	track_t *t = trackRead("samples/test-guitare-raw.raw");
	/*
	fun_t *env = note(0,10,700,500,2000,1,0.2);
	fun_t *f = add( cool_sound(c(880),env,0,0.5),
		   add( cool_sound(c(140),env,500,1),
		   add( cool_sound(c(130),env,1000,1),
		   add( cool_sound(c(110),env,1500,1),
		   add( cool_sound(c(130),env,2000,1),
		   add( cool_sound(c(440),env,2500,1),
		   add( cool_sound(c(130),env,3000,1),
		   add( cool_sound(c(110),env,3500,1), c(0)
		   		))))))));
	*/
	//fun_t *f2 = add(c(0.5), square(c(5),c(0.5),0,0.5));
	//f = sine(c(440.0),0.0,0.5);
	//trackAdd(t,f,f);
	
	//trackMult(t,c(0.2),c(0.2));
	//fx_reverb(t);
	
	fx_echo(t,c(500),c(0.2),3);
	fx_cutoff(t,c(0.09),c(0.2),WAVE_BOTH); 
	//trackMult(t,f2,f2);
	//fx_echo(t,c(500),c(0.3),4);
	fx_flange(t,sine(c(0.2),0,500));
	//void fx_echo(track_t *t, fun_t *delay, fun_t *amp, int count);
	//void fx_power_dist(track_t*t, fun_t *gain, fun_t*clip, fun_t *power,
	//	int wave);
	//fx_disto(t,WAVE_BOTH);
	initAudio();
	//trackPlay(t);
	trackWrite(t,"test01.wav");
	return 1;
}
/*
int main0ld(int argc, char**argv){
	track_t *t2 = trackNew(1000);
	track_t *t3 = trackNew(15000);
	track_t *t = trackNew(15000);
	fun_t* f2;
	fun_t* f3;
	fun_t* hz;
	fun_t* f = sine(c(120),0,0.2);
	float bpm = 90;
	beatloop_t *bl = beatloopNew(4);
	int st1 = subtrack(bl,MIXMODE_MIX,0.5);
	int st2 = subtrack(bl,MIXMODE_MIX,0.25);
	int st3 = subtrack(bl,MIXMODE_MIX,0.3);

	drum(bl,st1, kickL, 0, 1); 	drum(bl,st2,hihatO, 0,   1);
	
	drum(bl,st1, kickL, 0.5, 0);	drum(bl,st2,hihatO, 0.5, 1);
	 
					drum(bl,st2,hihatO, 1  , 1);  	drum(bl,st3, snareL, 1,   1);
	
	drum(bl,st1, kickL, 1.5,   0);	drum(bl,st2,hihatO, 1.5, 1);
	//								drum(bl,st3, snareL, 1.75,   1);	
	drum(bl,st1, kickL, 2,   1);	drum(bl,st2,hihatO, 2,   1);
	//								drum(bl,st3, snareL, 2.25,   1);	
	drum(bl,st1, kickL, 2.5,   0);  drum(bl,st2,hihatO, 2.5, 1);
	//drum(bl,st1, kickL, 2.75,  1);
					drum(bl,st2,hihatO, 3,   1);	drum(bl,st3, snareL, 3,   1);

					drum(bl,st2,hihatO, 3.5, 1);	
	
	int freq = ff("la6");
	float vol = 70;
	int i = 52;
	char * n = "do3";
	//hz = expramp(0,15000,ff("do1"),ff("do4"));
	hz = c(ff("do2"));
	t2 = trackRead("samples/batterie/hihat_closed_low.raw");
	f3 = square(hz,ramp(10000,15000,0.5,0.9),0,0.5);
	f2 = sawtooth(hz,ramp(5000,10000,0.5,2),0,0.5);
	f = sine(hz,0,0.5);
	f = add(c(0),window(0,15000,0,f));
	//f = add(f,window(5000,10000,0,f2));
	//f = add(f,window(10000,15000,0,f3));
	//trackAdd(t,f,f);
	f = add(c(0.5),sawtooth(mult(hz,c(1/8.0)),c(3),0,0.5));
	f = mult(c(0.3),f);
	//trackMult(t,f,f);
	f = square(mult(hz,c(1/16.0)),c(0.5),0,0.2);
	f = fdelay(sawtooth(c(5),rsine(c(2),0.3,3),0,50),f);
	//f = cache(0,15000,f);
//	f = lowpass(expramp(100,15000,100,15000),50,f);
	//f = expramp(1000,5000,0.1,0.9);
	f = mult(ramp(0,5000,0,1.5),f);
	//trackAdd(t,f,f);
	
	beatloopMix(t,bl,0, bpm,1);
	beatloopMix(t,bl,4, bpm,1);
	beatloopMix(t,bl,8, bpm,1);
	beatloopMix(t,bl,12,bpm,1);
	beatloopMix(t,bl,16,bpm,1);
	beatloopMix(t,bl,20,bpm,1);
	beatloopMix(t,bl,24,bpm,1);
	beatloopMix(t,bl,28,bpm,1);
	beatloopMix(t,bl,32,bpm,1);
	beatloopMix(t,bl,36,bpm,1);

	printf("playing song\n");
	initAudio();
	trackWrite(t,"song.wav");
	trackResize(t,1000);
	trackPlay(t);
	trackPlay(t);
	trackPlay(t);
	trackPlay(t);
	trackPlay(t);
	trackPlay(t);
	
	
	struct timespec ts;
	ts.tv_sec = 1;
	//nanosleep(&ts,NULL);
	t2 = trackCopy(t);
	trackAdd(t2,sine(c(440),0,0.2),c(0));
	trackPlay(t2);
	
	closeAudio();
	return 0;
}*/
/*
techno beat 	
	drum(bl,st1, kickL, 		0,   1);
	 drum(bl,st2,hihatO,	 	0.5, 1);
	drum(bl,st1, kickL, 		1,   1);
	 drum(bl,st2,hihatO,		1.5, 1);
	drum(bl,st1, kickL, 		2,   1);
	 drum(bl,st2,hihatO, 		2.5, 1);
	drum(bl,st1, kickL, 		3,   1);
	 drum(bl,st2,hihatO,		3.5, 1);
*/
	/*
	pouu(t,200	,1000,	freq,db(vol));
	pouu(t,200	,5000,	freq*0.15,db(vol -5));
	pouu(t,1200	,1000,	freq*0.2,db(vol));
	pouu(t,2200	,1000,	freq*0.9,db(vol));
	pouu(t,3200	,1000,	freq*2,db(vol));
	pouu(t,4200	,1000,	freq*0.6,db(vol));
	pouu(t,5200	,1000,	freq,db(vol));
	pouu(t,6200	,1000,	freq*1.2,db(vol));
	pouu(t,200	,5000,	freq*0.15,db(vol));
	pouu(t,7200	,1000,	freq,db(vol));
	pouu(t,8200	,1000,	freq*0.4,db(vol));
	pouu(t,9200	,1000,	freq,db(vol));
	pouu(t,10200	,1000,	freq*0.8,db(vol));
	pouu(t,11200	,1000,	freq*0.6,db(vol));
	pouu(t,12200	,1000,	freq*0.2,db(vol));
		
	sq_bass(t,200,db(89));
	sq_bass(t,1200,db(89));
	sq_bass(t,2200,db(89));
	sq_bass(t,3200,db(89));
	sq_bass(t,4200,db(89));
	sq_bass(t,5200,db(89));
	sq_bass(t,6200,db(89));
	sq_bass(t,7200,db(89));
	sq_bass(t,8200,db(89));
	sq_bass(t,9200,db(89));
	sq_bass(t,10200,db(89));
	sq_bass(t,11200,db(89));
	sq_bass(t,12200,db(89));
	//fx_reverb(t);
	fx_disto(t,WAVE_DOWN);
	//trackAdd(t,f,f);
	//	fx_phase_dist(t,ramp(0,10000,0,0.2),rsine(c(43.21),100,1000),WAVE_UP);
	fx_flange(t,rsine(c(0.2134),0,100));
	while(i--){
		trackMix(t,t2,200+i*250,0.15);
	}
	f = ftrack(t,0);
	f = lowpass(ramp(0,13000,0,10000),50,f);
	trackAdd(t3,f,f);
	*/
	
	/*
	int freq = ff("la6");
	float vol = 70;
	int i = 52;
	char * n = "do3";
	hz = expramp(0,15000,ff("do1"),ff("do7"));
	hz = c(ff("do2"));
	t2 = trackRead("samples/batterie/hihat_closed_low.raw");
	f3 = square(hz,ramp(10000,15000,0.5,0.9),0,0.5);
	f2 = sawtooth(hz,ramp(5000,10000,0.5,2),0,0.5);
	f = sine(hz,0,0.5);
	f = add(c(0),window(0,15000,0,f));
	//f = add(f,window(5000,10000,0,f2));
	//f = add(f,window(10000,15000,0,f3));
	//trackAdd(t,f,f);
	f = add(c(0.5),sawtooth(mult(hz,c(1/8.0)),c(3),0,0.5));
	f = mult(c(0.3),f);
	//trackMult(t,f,f);
	f = square(mult(hz,c(1/16.0)),c(0.5),0,0.2);
	f = fdelay(sawtooth(c(5),rsine(c(2),0.3,3),0,50),f);
	f = cache(0,15000,f);
	f = lowpass(expramp(100,15000,100,15000),50,f);
	//f = expramp(1000,5000,0.1,0.9);
	f = sawtooth(c(100),c(1),0,0.2);
	f = resonance(2000,3,c(0.2),f);
	trackAdd(t,f,f);
	*/
