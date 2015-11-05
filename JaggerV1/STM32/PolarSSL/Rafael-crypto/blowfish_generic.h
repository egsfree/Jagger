/**
 *  file   blowfish_generic.h
 *  brief  definitions about the blowfish crypto engine
 *  author Rafael Dias <rafael.dias@silicaconsulting.com>
 *  date   dez/2014
 */

#ifndef GENERICBLOWFISH
#define GENERICBLOWFISH

#include "defs.h"
     
typedef struct BlowFish BlowFish;     

BlowFish* BlowFishFactory( void );

/**
 *  brief initialize the blowfish crypto engine
 */
void BlowFishInit( BlowFish* pBlowFish );

/**
 *  brief  set the 
 */
DWORD BlowFishSetKey( BlowFish* pBlowFish, BYTE* pbKey, size_t szLen );
     
const char* BlowFishEncrypt( BlowFish* pBlowFish, char *pcBufferIn, size_t szBufferIn );

const char* BlowFishDecrypt( BlowFish* pBlowFish, char *pcBufferIn, size_t szBufferIn );

#endif /* ~GENERICBLOWFISH */
