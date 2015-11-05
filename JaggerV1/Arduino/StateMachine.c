#define BYTE unsigned char
#define STARTHEADING 0x01U
#define ENDTRANSMISSION 0x04U

#define CMD_CONFIG_ETHERNET 0x00
#define CMD_CONFIG_DEST_IP  0x01

enum STATES
{
HEADER1,
HEADER2,
SESSION,
CMD1,
STATUS,
NUMDATA,
DATA,
CHECKSUM,
END
}eSTATES;

typedef struct
{
BYTE bData,     
BYTE bPckgReady, 
BYTE bNumData,   
BYTE bRcvData,   
BYTE bSession,   
BYTE bCmd,       	
BYTE bStatus,    	
BYTE bRXData[127],  
BYTE bCRC
}sPackage



sPackage sRcvPackage;
EthernetUDP UDP;
BYTE bEstado = HEADER1; 
/*
BYTE bData      = 0x00U;
BYTE bPckgReady = 0x00U;
BYTE bNumData   = 0x00U;
BYTE bRcvData   = 0x00U;
BYTE bSession   = 0x00U;
BYTE bCmd       = 0x00U;
BYTE bStatus    = 0x00U;
BYTE bRXData    [255];
*/

while(1)
{
   if (Serial.available() > 0) 
   {
   	switch( bEstado )
   	{
   		bData = Serial.read();
   		case HEADER1:
   		if( bData == STARTHEADING)
   		{
   			bEstado = HEADER2;
   		}		
   		break;
   		
   		case HEADER2:
   		if( bData == STARTHEADING)
   		{
   			bEstado = SESSION;
   		}	
   		else
   		{
   			bEstado = HEADER1;
   		}
   		
   		break;
   		
   		case SESSION:
   		sRcvPackage.bSession = bData;		
   		bEstado = CMD1;
   		break;
   		
   		case CMD1:
   		sRcvPackage.bCmd = bData;
   		bEstado = STATUS;
   		break;
   		
   		case STATUS:
   		sRcvPackage.bStatus = bData;
   		bEstado = NUMDATA;
   		break;
   		
   		case NUMDATA:
   		sRcvPackage.bNumData = bData;
   		bEstado = DATA;
   		break;
   		
   		case DATA:
   		if( bRcvData < bNumData)
   		{
   			sRcvPackage.bRXData[bRcvData] = bData;
   			bRXData++;
   		}
   		else
   		{
   			bEstado = CHECKSUM;
   		}		
   		break;
   		
   		case CHECKSUM:
   		bEstado = END;
   		
   		break;
   		
   		case END:
   		if( bData == ENDTRANSMISSION)
   		{
   			bPckgReady = 0x01U;
   		}
   		else
   		{
   			sRcvPackage.bData      = 0x00U;
   			sRcvPackage.bPckgReady = 0x00U;
   			sRcvPackage.bNumData   = 0x00U;
   			sRcvPackage.bRcvData   = 0x00U;
   			sRcvPackage.bSession   = 0x00U;
   			sRcvPackage.bCmd       = 0x00U;
   			sRcvPackage.bStatus    = 0x00U;
   			
   			bEstado = HEADER1;
   		}
   		break;
   		
   		default:
   		bEstado = HEADER1;
   		break;
   		
   	}
   	
   }
   
   if( bPckgReady == 0x01 )
   {
	    
   }
   
}

BYTE ProcessPackage( sPackage * psRcvPackage )
{
	BYTE bRet = 0;
	
	if( psRcvPackage == NULL )
	{
		return (BYTE)-1;
	}
	
	switch ( psRcvPackage->bCmd )
	{
		case CMD_CONFIG_ETHERNET:
		
		break;
		
		case CMD_CONFIG_DEST_IP:
		
		unsigned short usPort = (psRcvPackage->bRXData[4] << 8) | (psRcvPackage->bRXData[5]);		
		bRet = UDP.beginPacket(IPAddress(psRcvPackage->bRXData[0],psRcvPackage->bRXData[1],psRcvPackage->bRXData[2],psRcvPackage->bRXData[3]),usPort);
		
		break;
		
		default:
		SendUDPPackage( psRcvPackage );
		break;
		
		
	}
	
	return 0;
	
}

BYTE SendUDPPackage ( sPackage * psRcvPackage )
{
	BYTE Buffer[135];
	
	MakePackage( psRcvPackage, bBuffer );
	
	UDP.write(const uint8_t *buffer, size_t size);
}

BYTE MakePackage( sPackage * psRcvPackage, BYTE bBuffer[] )
{
	if( ( psRcvPackage == NULL ) || ( bBuffer == NULL ) )
	{		
		return (BYTE)-1;		
	}
	
	bBuffer[0] = STARTHEADING;
	bBuffer[1] = STARTHEADING;
	bBuffer[2] = psRcvPackage->bSession;
	bBuffer[3] = psRcvPackage->bCmd;
	bBuffer[4] = psRcvPackage->bStatus;
	bBuffer[5] = psRcvPackage->bNumData;
	
	memcpy( &bBuffer[6], psRcvPackage->bRXData, ( psRcvPackage->bNumData ));
	
	bBuffer[ psRcvPackage->bNumData + 5] = psRcvPackage->bCRC;
	bBuffer[ psRcvPackage->bNumData + 6] = ENDTRANSMISSION;
	
	
	
}





















