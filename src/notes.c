#include <string.h>
#include <stdio.h>
#include <math.h>
#include "notes.h"
float ff(char *note){
	float base = 0;
	int len = 0;
	int exp = 0;
	if(!strncmp(note,"dod",3)){
		base = 277.18/4;
		len = 3;
	}else if (!strncmp(note,"do",2)){
		base = 261.63/4;
		len = 2;
	}else if (!strncmp(note,"re",2)){
		base = 293.66/4;
		len = 2;
	}else if (!strncmp(note,"mib",3)){
		base = 311.13/4;
		len = 3;
	}else if (!strncmp(note,"mi",2)){
		base = 329.63/4;
		len = 2;
	}else if (!strncmp(note,"fad",3)){
		base = 369.99/4;
		len = 3 ;
	}else if (!strncmp(note,"fa",2)){
		base = 349.23/4; 
		len = 2;
	}else if (!strncmp(note,"sod",3)){
		base = 415.3/4;
		len = 3;
	}else if (!strncmp(note,"sol",3)){
		base = 392.0/4;
		len = 3;
	}else if (!strncmp(note,"la",2)){
		base = 440.0/4;
		len = 2;
	}else if (!strncmp(note,"sib",3)){
		base = 466.16/4;
		len = 3;
	}else if (!strncmp(note,"si",2)){
		base = 493.88/4;
		len = 2;
	}else { return 440;}
	exp = note[len] -'0';
	printf("%s : %f,%d\n",note,base,exp);
	return base *pow(2,exp);
}
/* format partition :
 * "# 
 *  - do4 1 1	| si2 8 1 |  ... ->
 *  - do4 1 1
 *  -
 *  #
 *  - noteOctave volume (en huitièmes) durée ( en 16 ièmes de tmps)
 *  -
 *  -
 *  #
 *  .
 *  .
 *  -
 *  .
 *  -
 *  .
 *  -
 *  .
 *  #
 *  
*/
