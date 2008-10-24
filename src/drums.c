#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "drums.h"
	
void tom(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/pah.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void kickH(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/bom.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void kickL(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/bohhm.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void hihatCH(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/hihat_closed_hi.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void hihatCL(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/hihat_closed_low.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void hihatO(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/hihat_opened.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void snareH(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/snare_hi.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}
void snareL(track_t *t, float vol){
	static track_t *n = NULL;
	int l;
	fun_t *f;
	if( n == NULL){
		n = trackRead("samples/batterie/snare_low.raw");
		l = trackMsecLength(n);
		printf("%d\n",l);
		f = ramp(l/2,l,1,0);
		trackMult(n,f,f);
	}
	trackMix(t,n,0,vol);
}

