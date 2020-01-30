#include "typedefsTGDS.h"
#include "dsregs.h"
#include "limitsTGDS.h"
#include "fatfslayerTGDS.h"
#include "defs.h"
#include "file.h"
#include "xmem.h"
#include "utilsTGDS.h"
#include "biosTGDS.h"

void *R_LoadFile(char *filename, void *out){
	unsigned char *temp,*outx;
	unsigned int i;
	FILE *file = fopen(filename, "r");
	if (!file) return 0;	
	int fileLen = FS_getFileSizeFromOpenHandle(file);
	temp = (unsigned char *) Xmalloc(fileLen);
	fread(temp, 1, fileLen, file);
	out=(void*)temp;
	fclose(file);
	return (void*)out;
}

void *R_LoadLZ77FromFile(char *filename, void *out){
	// same as loadraw... but decompresses!
	u8 *e;
	u32 s;
	e = (u8*) R_LoadFile(filename, NULL);
	s = e[1]+(e[2]*256)+(e[3]*65536);
	if (out == NULL) {
		out = Xcalloc(s,1);
	}
	swiDecompressLZSSWram(e, out);	
	Xfree(e);
	return (void*)out;
}

void *R_LoadLZ77FromBuffer(u8* inputBuffer, void *out){
	// same as loadraw... but decompresses!
	u8 *e;
	u32 s;
	e = (u8*)inputBuffer;
	s = e[1]+(e[2]*256)+(e[3]*65536);
	if (out == NULL) {
		out = Xcalloc(s,1);
	}
	swiDecompressLZSSWram(e, out);	
	return (void*)out;
}