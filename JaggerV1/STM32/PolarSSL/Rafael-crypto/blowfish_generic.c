#include "blowfish_generic.h"
#include "polarssl/blowfish.h"
#include <string.h>
#include "util.h"

#define MAX_BLOWFISH_ENGINE 2

struct BlowFish
{
    char    pcInputCryptoBuffer[BLOWFISH_BLOCKSIZE];        /*! input crypto buffer     **/
    char    pcOutputCryptoBuffer[BLOWFISH_BLOCKSIZE];       /*! output crypto buffer    **/        
    char    pcKeyCrypto[64];                                /*! crypto key              **/        
    blowfish_context blowfish;
    DWORD   id;
    BYTE    bInit;
};

/*----------------------------------------------------------------------------*/

struct BlowFish stBlowFish[MAX_BLOWFISH_ENGINE];

/*----------------------------------------------------------------------------*/

BlowFish* BlowFishFactory( void )
{
    static DWORD dwEngineCount = 0;
    BlowFish* pB;
    
    pB = &stBlowFish[dwEngineCount];
    
    if ( dwEngineCount++ > GET_ARRAY_LEN( stBlowFish ) )
    {
        pB = NULL;
    }
    return pB;
}

/*----------------------------------------------------------------------------*/

void BlowFishInit( BlowFish* pBlowFish )
{
    if ( pBlowFish != NULL )
    {
        if ( !pBlowFish->bInit )
        {
            (void)memset ( pBlowFish->pcInputCryptoBuffer     , 0, sizeof( pBlowFish->pcInputCryptoBuffer    ) );
            (void)memset ( pBlowFish->pcOutputCryptoBuffer    , 0, sizeof( pBlowFish->pcOutputCryptoBuffer   ) );
            (void)memset ( pBlowFish->pcKeyCrypto             , 0, sizeof( pBlowFish->pcKeyCrypto            ) );
            blowfish_init( &pBlowFish->blowfish );    
            pBlowFish->bInit = 1;
        }
    }
    return;
}

/*----------------------------------------------------------------------------*/

DWORD BlowFishSetKey( BlowFish* pBlowFish, BYTE* pbKey, size_t szLen )
{
    DWORD dwRet = -1;

    if ( pBlowFish != NULL )
    {
        if ( pBlowFish->bInit )
        {
            if ( szLen )
            {
                memcpy( pBlowFish->pcKeyCrypto, pbKey, szLen );
                blowfish_setkey( &pBlowFish->blowfish, (unsigned char const*)pBlowFish->pcKeyCrypto, (szLen)*8 );
                dwRet = 0;
            }
        }
    }
    
    return dwRet;
}

/*----------------------------------------------------------------------------*/

const char* BlowFishEncrypt( BlowFish* pBlowFish, char *pcBufferIn, size_t szBufferIn )
{
    DWORD dwRet = -1;
    size_t szBufIn = 0;
    char *pcBufferOut = NULL;

    if ( pBlowFish != NULL )
    {
        if ( pBlowFish->bInit )
        {
            (void)memset ( pBlowFish->pcInputCryptoBuffer, 0, BLOWFISH_BLOCKSIZE );
            szBufIn = (szBufferIn > BLOWFISH_BLOCKSIZE) ? BLOWFISH_BLOCKSIZE : szBufferIn;
            memcpy( pBlowFish->pcInputCryptoBuffer, pcBufferIn, szBufIn );
            
            dwRet = blowfish_crypt_ecb( &pBlowFish->blowfish, 
                                        BLOWFISH_DECRYPT, 
                                        (const unsigned char*)pBlowFish->pcInputCryptoBuffer, 
                                        (unsigned char*)pBlowFish->pcOutputCryptoBuffer );
            if ( dwRet == 0 )
            {
                pcBufferOut = pBlowFish->pcOutputCryptoBuffer;
            } 
        }
    }

    return (const char*)pcBufferOut;
}

/*----------------------------------------------------------------------------*/

const char* BlowFishDecrypt( BlowFish* pBlowFish, char *pcBufferIn, size_t szBufferIn )
{
    DWORD dwRet = -1;
    size_t szBufIn = 0;
    char *pcBufferOut = NULL;    
    
    if ( pBlowFish->bInit )
    {
        (void)memset ( pBlowFish->pcInputCryptoBuffer, 0, BLOWFISH_BLOCKSIZE );
        szBufIn = (szBufferIn > BLOWFISH_BLOCKSIZE) ? BLOWFISH_BLOCKSIZE : szBufferIn;
        memcpy( pBlowFish->pcInputCryptoBuffer, pcBufferIn, szBufIn );
        
        dwRet = blowfish_crypt_ecb( &pBlowFish->blowfish, 
                                    BLOWFISH_ENCRYPT, 
                                    (const unsigned char*)pBlowFish->pcInputCryptoBuffer, 
                                    (unsigned char*)pBlowFish->pcOutputCryptoBuffer );
        if ( dwRet == 0 )
        {
            pcBufferOut = pBlowFish->pcOutputCryptoBuffer;
        }                
    }
    return (const char*)pcBufferOut;
}

/*----------------------------------------------------------------------------*/
