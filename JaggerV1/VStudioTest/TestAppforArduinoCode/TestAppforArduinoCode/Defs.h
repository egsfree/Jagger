#ifndef DEFS_H
#define DEFS_H

#define BYTE unsigned char
#define STARTHEADING 0x01U
#define ENDTRANSMISSION 0x04U

#define SEND_ERROR 0xFAU

#define NULL 0

#define CMD_CONFIG_ETHERNET 0x00
#define CMD_CONFIG_DEST_IP  0x01



enum STATES
{
	HEADER1,
	HEADER2,
	SESSION,
	DEST,
	CMD1,
	STATUS,
	NUMDATA,
	DATA,
	CHECKSUM,
	END
}eSTATES;

enum TYPECOM
{
   SERIAL,
   ETHERNET
}eTYPECOM;

typedef struct
{
	BYTE bData;
	BYTE bPckgReady;
	BYTE bNumData;
	BYTE bRcvData;
	BYTE bSession;
	BYTE bCmd;
	BYTE bDest;
	BYTE bStatus;
	BYTE bRXData[127];
	BYTE bCRC;
}sPackage;

#endif
