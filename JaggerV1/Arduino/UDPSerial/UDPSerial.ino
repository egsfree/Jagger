/*
 * UIPEthernet UdpClient example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This UdpClient example tries to send a packet via udp to 192.168.0.1
 * on port 5000 every 5 seconds. After successfully sending the packet it
 * waits for up to 5 seconds for a response on the local port that has been
 * implicitly opened when sending the packet.
 *
 * Copyright (C) 2013 by Norbert Truchsess (norbert.truchsess@t-online.de)
 */

#include <UIPEthernet.h>
#include "Defs.h"
#include <string.h>

#define DEBUG_SERIAL
EthernetUDP udp;
unsigned long next;
sPackage * psRcvPackage;

void setup() {

  Serial.begin(57600);

#ifdef DEBUG_SERIAL
  Serial.println("Initializing...");
#endif

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

  Ethernet.begin(mac,IPAddress(192,168,0,110));

  int success = udp.begin(5000);

  psRcvPackage = NULL;
}

// TestAppforArduinoCode.cpp : Defines the entry point for the console application.
//




BYTE ProcessPackage(sPackage * psRcvPackage, TYPECOM FromCOM);
BYTE SendPackage(sPackage * psRcvPackage, TYPECOM FromCOM);
BYTE MakePackage(sPackage * psRcvPackage, BYTE bBuffer[]);
size_t GetPkgSize(sPackage * psRcvPackage);


BYTE bTestPkg[] = { STARTHEADING, STARTHEADING, 0x00, 0x0A, 0x00, 0x00, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, ENDTRANSMISSION };

void loop()
{
	while (1)
	{
		udp.begin(5000);
		int size = udp.parsePacket();
		if (size > 0)
		{
#ifdef DEBUG_SERIAL
			Serial.println("UDP Data available");
#endif
			psRcvPackage = NULL;

			psRcvPackage = RecebePacote(TETHERNET);

			if (psRcvPackage == NULL)
			{
#ifdef DEBUG_SERIAL
				Serial.println("UDP Package error.");
#endif			
				udp.flush();
				

				continue;
			}

			ProcessPackage(psRcvPackage, TETHERNET);
			udp.flush();
			//udp.stop();
			//udp.begin(5000);
		}

		if (Serial.available() > 0)
		{
#ifdef DEBUG_SERIAL
			Serial.println("Serial Data available");
#endif
			psRcvPackage = NULL;
			psRcvPackage = NULL;

			psRcvPackage = RecebePacote(TSERIAL);

			if (psRcvPackage == NULL)
			{
#ifdef DEBUG_SERIAL
				Serial.println("Serial Package error.");
#endif          
				Serial.end();
				Serial.begin(57600);
				continue;
			}

			ProcessPackage(psRcvPackage, TSERIAL);
			Serial.flush();
		}

		udp.stop();
	}

}








sPackage * RecebePacote(TYPECOM FromCOM)
{
	static sPackage sRcvPackage;



	BYTE bEstado = HEADER1;
	static BYTE bData = 0x00U;
	BYTE bPckgReady = 0x00U;
	BYTE bRcvData = 0x00U;


	memset((BYTE*)&sRcvPackage, 0, sizeof(sRcvPackage));

	while (1)
	{
		if (1)
		{
			switch (FromCOM)
			{
			case TSERIAL:			

					//bData = Serial.read();
					Serial.readBytes((char *)&bData, (size_t)1);
					//Serial.print("Data: ");
					//Serial.println(bData, HEX);
			
				break;
			case TETHERNET:

				if (udp.available() > 0)
				{
					bData = udp.read();

				}
				else
				{
#ifdef DEBUG_SERIAL
					Serial.println("Read Error");
#endif
					psRcvPackage = NULL;
					return NULL;
				}

				break;
			default:
				return NULL;
				break;
			}

			

			switch (bEstado)
			{
			case HEADER1:
				if (bData == STARTHEADING)
				{
					bEstado = HEADER2;
					
				}
				else
				{
#ifdef DEBUG_SERIAL
					Serial.println("HEADER 1 Error");
#endif
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
#ifdef DEBUG_SERIAL
					Serial.println("HEADER 2 Error");
#endif
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
#ifdef DEBUG_SERIAL
				Serial.print("Data size: ");
				Serial.println(bData, DEC);

#endif
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
					sRcvPackage.bData = 0x00U;
					sRcvPackage.bPckgReady = 0x00U;
					sRcvPackage.bNumData = 0x00U;
					sRcvPackage.bRcvData = 0x00U;
					sRcvPackage.bSession = 0x00U;
					sRcvPackage.bCmd = 0x00U;
					sRcvPackage.bStatus = 0x00U;

					bEstado = HEADER1;
#ifdef DEBUG_SERIAL
					Serial.print("End-Of-Data Error. End char: ");
					Serial.println(bData, HEX);
					
#endif
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
#ifdef DEBUG_SERIAL
			Serial.println("PackageReceived");
#endif
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
#ifdef DEBUG_SERIAL
		Serial.println("Config Serial.");
#endif
		break;

	case CMD_CONFIG_DEST_IP:
#ifdef DEBUG_SERIAL
		Serial.println("Config UDP.");
#endif
		usPort = (psRcvPackage->bRXData[4] << 8) | (psRcvPackage->bRXData[5]);
		/*bRet = UDP.beginPacket(IPAddress(psRcvPackage->bRXData[0], psRcvPackage->bRXData[1], psRcvPackage->bRXData[2], psRcvPackage->bRXData[3]), usPort); */

		break;

	default:
#ifdef DEBUG_SERIAL
		Serial.println("Generic Data. Retransmitting...");
#endif
		SendPackage(psRcvPackage, FromCOM);
		break;


	}
#ifdef DEBUG_SERIAL
	Serial.println("Done Process Package.");
#endif
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
	case TETHERNET:		
		Serial.write(&bBuffer[0], GetPkgSize(psRcvPackage));
		break;

	case TSERIAL:
		bSentData = udp.write(bBuffer, GetPkgSize(psRcvPackage));
			bRet = bSentData ? 0 : 1;

		udp.endPacket();

		if (bRet != 0)
		{
			return SEND_ERROR;
		}

		/*
		do
		{
			bRet = udp.parsePacket() ? 0 : 1;
			delay(10);
		} while (((bTimeout++) < 5) && (bRet != 0));
		break;
		*/
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


