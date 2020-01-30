#ifndef __file9_h__
#define __file9_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "limitsTGDS.h"
#include "dldi.h"
#include "defs.h"

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void *R_LoadFile(char *filename, void *out);
extern void *R_LoadLZ77FromFile(char *filename, void *out);
extern void *R_LoadLZ77FromBuffer(u8* inputBuffer, void *out);

#ifdef __cplusplus
}
#endif
