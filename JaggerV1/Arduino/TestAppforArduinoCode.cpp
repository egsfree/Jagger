// TestAppforArduinoCode.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include "Defs.h"

BYTE ProcessPackage(sPackage * psRcvPackage, TYPECOM FromCOM);
BYTE SendPackage(sPackage * psRcvPackage, TYPECOM FromCOM);
BYTE MakePackage(sPackage * psRcvPackage, BYTE bBuffer[]);
size_t GetPkgSize(sPackage * psRcvPackage);


BYTE bTestPkg[] = { STARTHEADING, STARTHEADING, 0x00U, 0x0AU, 0x00U, 0x05U, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, ENDTRANSMISSION };

int main(int argc, char * argv[])
{	
	sPackage * psRcvPackage;

	while (1)
	{
		if (udp.available() > 0)
		{
			psRcvPackage = NULL;

			psRcvPackage = RecebePacote(ETHERNET);

			if (psRcvPackage == NULL)
			{
				continue;
			}

			ProcessPackage(psRcvPackage);
        }

		if (Serial.available() > 0)
		{
			psRcvPackage = NULL;

			psRcvPackage = RecebePacote(SERIAL);

			if (psRcvPackage == NULL)
			{
				continue;
			}

			ProcessPackage(psRcvPackage);
		}





	}




}



sPackage * RecebePacote(TYPECOM FromCOM)
{
	static sPackage sRcvPackage;



	static BYTE bEstado = HEADER1;
	BYTE bData = 0x00U;
	BYTE bPckgReady = 0x00U;
	BYTE bRcvData = 0x00U;

	BYTE bCnt = 0;

	while (1)
	{
		if (1)
		{
			switch (FromCOM)
			{
			case SERIAL:
				
				if (Serial.available() > 0)
				{
					*bData = Serial.read(); 
				}
				else
				{
					return NULL;
				}
				
				break;
			case ETHERNET:
				
				if (udp.available()) > 0)
				{
					bData = UDP.read();
                }
				else
				{
					return NULL;
				}
			
				break;
			default:
				return NULL;
				break;
			}
			
			bData = bTestPkg[bCnt++];

			switch (bEstado)
			{
			case HEADER1:
				if (bData == STARTHEADING)
				{
					bEstado = HEADER2;
					return NULL;
				}
				break;

			case HEADER2:
				if (bData == STARTHEADING)
				{
					bEstado = SESSION;
				}
				else
				{
					bEstado = HEADER1;
					return NULL;
				}

				break;

			case SESSION:
				sRcvPackage.bSession = bData;
				bEstado = CMD1;
				break;

			case CMD1:
				sRcvPackage.bCmd = bData;
				bEstado = DEST;
				break;
			case DEST:
				sRcvPackage.bDest = bData;
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
				
				sRcvPackage.bRXData[bRcvData] = bData;
				bRcvData++;
				
				if (bRcvData == sRcvPackage.bNumData)
				{
					bEstado = CHECKSUM;
				}
				break;

			case CHECKSUM:

				sRcvPackage.bCRC = bData;
				bEstado = END;
				break;

			case END:
				if (bData == ENDTRANSMISSION)
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
					return NULL;
				}
				break;

			default:
				bEstado = HEADER1;
				break;
			}
		}

		if (bPckgReady == 0x01)
		{
			
			return &sRcvPackage;

		}

	}	
}

BYTE ProcessPackage(sPackage * psRcvPackage, TYPECOM FromCOM)
{
	BYTE bRet = 0;
	unsigned short usPort;

	if (psRcvPackage == NULL)
	{
		return (BYTE)-1;
	}

	switch (psRcvPackage->bCmd)
	{
	
	case CMD_CONFIG_ETHERNET:

		break;

	case CMD_CONFIG_DEST_IP:

		usPort = (psRcvPackage->bRXData[4] << 8) | (psRcvPackage->bRXData[5]);
		/*bRet = UDP.beginPacket(IPAddress(psRcvPackage->bRXData[0], psRcvPackage->bRXData[1], psRcvPackage->bRXData[2], psRcvPackage->bRXData[3]), usPort); */

		break;

	default:
		SendPackage(psRcvPackage, FromCOM);
		break;


	}

	return 0;

}

BYTE SendPackage(sPackage * psRcvPackage, TYPECOM FromCOM)
{
	BYTE bBuffer[135];
	BYTE bSentData = 0;
	BYTE bReceiveData = 0;
	BYTE bRet = 0;
	BYTE bTimeout = 0;

	memset(bBuffer, 0, sizeof(bBuffer));

	MakePackage(psRcvPackage, bBuffer);


	switch (FromCOM)
	{
	case ETHERNET:
		
		Serial.write(bBuffer, GetPkgSize(psRcvPackage));
		break;

	case SERIAL:
		bSentData = UDP.write(const uint8_t *buffer, size_t size); */
			bRet = bSentData ? 0 : 1;

		udp.endPacket();

		if (bRet != 0)
		{
			return SEND_ERROR;
		}


		do
		{
			bRet = UDP.parsePacket() ? 0 : 1;
			delay(10);
		} while (((bTimeout++) < 5) && (bRet != 0));
		break;

	}


	if (bRet != 0)
	{
		return bRet;
	}
    
	return bRet;
}

BYTE MakePackage(sPackage * psRcvPackage, BYTE bBuffer[])
{
	if ((psRcvPackage == NULL) || (bBuffer == NULL))
	{
		return (BYTE)-1;
	}

	bBuffer[0] = STARTHEADING;
	bBuffer[1] = STARTHEADING;
	bBuffer[2] = psRcvPackage->bSession;
	bBuffer[3] = psRcvPackage->bCmd;
	bBuffer[4] = psRcvPackage->bStatus;
	bBuffer[5] = psRcvPackage->bNumData;

	memcpy(&bBuffer[6], psRcvPackage->bRXData, (psRcvPackage->bNumData));

	bBuffer[psRcvPackage->bNumData + 6] = psRcvPackage->bCRC;
	bBuffer[psRcvPackage->bNumData + 7] = ENDTRANSMISSION;
	return 0;

}

size_t GetPkgSize(sPackage * psRcvPackage)
{
	if ((psRcvPackage == NULL))
	{
		return (BYTE)-1;
	}
	return psRcvPackage->bNumData + 9;
}


