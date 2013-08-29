/*
 * Copyright 2010 Reef Angel / Roberto Imai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /*
  * Updated by:  Curt Binder
  * Updates Released under Apache License, Version 2.0
  */

#ifndef __RA_WIFI_H__
#define __RA_WIFI_H__

#include <Globals.h>
#include <avr/pgmspace.h>

#if defined wifi || defined ethernet
const prog_char XML_ID[] PROGMEM = "<RA><ID>";
const prog_char XML_T1[] PROGMEM = "</ID><T1>";
const prog_char XML_T2[] PROGMEM = "</T1><T2>";
const prog_char XML_T3[] PROGMEM = "</T2><T3>";
const prog_char XML_PH[] PROGMEM = "</T3><PH>";
const prog_char XML_R[] PROGMEM = "</PH><R>";
const prog_char XML_RON[] PROGMEM = "</R><RON>";
const prog_char XML_ROFF[] PROGMEM = "</RON><ROFF>";
const prog_char XML_RE_OPEN[] PROGMEM = "<R";
const prog_char XML_RE_CLOSE[] PROGMEM = "</R";
const prog_char XML_RE_ON[] PROGMEM = "ON";
const prog_char XML_RE_OFF[] PROGMEM = "OFF";
const prog_char XML_ATOLOW[] PROGMEM = "<ATOLOW>";
const prog_char XML_ATOHIGH[] PROGMEM = "</ATOLOW><ATOHIGH>";
const prog_char XML_EM[] PROGMEM = "</ATOHIGH><EM>";
const prog_char XML_EM1[] PROGMEM = "</EM><EM1>";
const prog_char XML_REM[] PROGMEM = "</EM1><REM>";
const prog_char XML_FLAG[] PROGMEM = "</REM><FLAG>";
const prog_char XML_FLAG_END[] PROGMEM = "</FLAG>";

#ifdef DisplayLEDPWM
const prog_char XML_PWMA[] PROGMEM = "<PWMA>";
const prog_char XML_PWMD[] PROGMEM = "</PWMA><PWMD>";
const prog_char XML_PWMAO[] PROGMEM = "</PWMD><PWMAO>";
const prog_char XML_PWMDO[] PROGMEM = "</PWMAO><PWMDO>";
const prog_char XML_PWMDO_END[] PROGMEM = "</PWMDO>";
#endif  // DisplayLEDPWM
#ifdef ORPEXPANSION
const prog_char XML_ORP[] PROGMEM = "<ORP>";
const prog_char XML_ORP_END[] PROGMEM = "</ORP>";
#endif  // ORPEXPANSION
#ifdef SALINITYEXPANSION
const prog_char XML_SAL[] PROGMEM = "<SAL>";
const prog_char XML_SAL_END[] PROGMEM = "</SAL>";
#endif  // SALINITYEXPANSION
#ifdef PHEXPANSION
const prog_char XML_PHEXP[] PROGMEM = "<PHE>";
const prog_char XML_PHEXP_END[] PROGMEM = "</PHE>";
#endif  // PHEXPANSION
#ifdef WATERLEVELEXPANSION
const prog_char XML_WL[] PROGMEM = "<WL>";
const prog_char XML_WL_END[] PROGMEM = "</WL>";
#endif  // WATERLEVELEXPANSION
#ifdef HUMIDITYEXPANSION
const prog_char XML_HUM[] PROGMEM = "<HUM>";
const prog_char XML_HUM_END[] PROGMEM = "</HUM>";
#endif  // HUMIDITYEXPANSION
#ifdef DCPUMPCONTROL
const prog_char XML_DCM[] PROGMEM = "<DCM>";
const prog_char XML_DCM_END[] PROGMEM = "</DCM><DCS>";
const prog_char XML_DCS_END[] PROGMEM = "</DCS><DCD>";
const prog_char XML_DCD_END[] PROGMEM = "</DCD>";
#endif  // DCPUMPCONTROL
#ifdef PWMEXPANSION
const prog_char XML_PWME[] PROGMEM = "<PWME";
const prog_char XML_PWME_END[] PROGMEM = "</PWME";
#endif  // PWMEXPANSION
#ifdef AI_LED
const prog_char XML_AIW[] PROGMEM = "<AIW>";
const prog_char XML_AIW_END[] PROGMEM = "</AIW><AIB>";
const prog_char XML_AIB_END[] PROGMEM = "</AIB><AIRB>";
const prog_char XML_AIRB_END[] PROGMEM = "</AIRB><AIWO>";
const prog_char XML_AIWO_END[] PROGMEM = "</AIWO><AIBO>";
const prog_char XML_AIBO_END[] PROGMEM = "</AIBO><AIRBO>";
const prog_char XML_AIRBO_END[] PROGMEM = "</AIRBO>";
#endif  // AI_LED
#ifdef RFEXPANSION
const prog_char XML_RFM[] PROGMEM = "<RFM>";
const prog_char XML_RFM_END[] PROGMEM = "</RFM><RFS>";
const prog_char XML_RFS_END[] PROGMEM = "</RFS><RFD>";
const prog_char XML_RFD_END[] PROGMEM = "</RFD><RFW>";
const prog_char XML_RFW_END[] PROGMEM = "</RFW><RFRB>";
const prog_char XML_RFRB_END[] PROGMEM = "</RFRB><RFR>";
const prog_char XML_RFR_END[] PROGMEM = "</RFR><RFG>";
const prog_char XML_RFG_END[] PROGMEM = "</RFG><RFB>";
const prog_char XML_RFB_END[] PROGMEM = "</RFB><RFI>";
const prog_char XML_RFI_END[] PROGMEM = "</RFI><RFWO>";
const prog_char XML_RFWO_END[] PROGMEM = "</RFWO><RFRBO>";
const prog_char XML_RFRBO_END[] PROGMEM = "</RFRBO><RFRO>";
const prog_char XML_RFRO_END[] PROGMEM = "</RFRO><RFGO>";
const prog_char XML_RFGO_END[] PROGMEM = "</RFGO><RFBO>";
const prog_char XML_RFBO_END[] PROGMEM = "</RFBO><RFIO>";
const prog_char XML_RFIO_END[] PROGMEM = "</RFIO>";
#endif  // RFEXPANSION
#ifdef IOEXPANSION
const prog_char XML_IO[] PROGMEM = "<IO>";
const prog_char XML_IO_END[] PROGMEM = "</IO>";
#endif  // IOEXPANSION
#ifdef CUSTOM_VARIABLES
const prog_char XML_C[] PROGMEM = "<C";
const prog_char XML_C_END[] PROGMEM = "</C";
#endif  // CUSTOM_VARIABLES
#ifdef ENABLE_ATO_LOGGING
const prog_char XML_ATOLOW_LOG_OPEN[] PROGMEM = "<AL";
const prog_char XML_ATOLOW_LOG_CLOSE[] PROGMEM = "</AL";
const prog_char XML_ATOHIGH_LOG_OPEN[] PROGMEM = "<AH";
const prog_char XML_ATOHIGH_LOG_CLOSE[] PROGMEM = "</AH";
#endif  // ENABLE_ATO_LOGGING
const prog_char XML_END[] PROGMEM = "</RA>";
const prog_char XML_CLOSE_TAG[] PROGMEM = ">";


const prog_char XML_P_OPEN[] PROGMEM = "<P";
const prog_char XML_P_CLOSE[] PROGMEM = "</P";
const prog_char XML_M_OPEN[] PROGMEM = "<M";
const prog_char XML_M_CLOSE[] PROGMEM = "</M";
const prog_char XML_MEM_OPEN[] PROGMEM = "<MEM>";
const prog_char XML_MEM_CLOSE[] PROGMEM = "</MEM>";
const prog_char XML_DATE_OPEN[] PROGMEM = "<D>";
const prog_char XML_DATE_CLOSE[] PROGMEM = "</D>";
const prog_char XML_MODE_OPEN[] PROGMEM = "<MODE>";
const prog_char XML_MODE_CLOSE[] PROGMEM = "</MODE>";
const prog_char XML_OK[] PROGMEM = "OK";
const prog_char XML_ERR[] PROGMEM = "ERR";

const prog_char SERVER_HEADER1[] PROGMEM = "HTTP/1.1 200 OK\r\nServer: ReefAngel\r\nCache-Control: no-store, no-cache, must-revalidate\r\nPragma: no-cache\r\nConnection: close\r\nContent-Type: text/";
const prog_char SERVER_HEADER2[] PROGMEM = "\r\nContent-Length: ";
const prog_char SERVER_DENY[] PROGMEM = "HTTP/1.1 401 Access Denied\r\nWWW-Authenticate: Basic realm=Reef Angel Controller\r\nContent-Length: 0\r\n";
const prog_char SERVER_DEFAULT[] PROGMEM = "<h1>Reef Angel Controller Web Server</h1>";

// REQUEST TYPES
#define REQ_ROOT		1		// Default page
#define REQ_WIFI		2		// Wifi page
#define REQ_RELAY		3		// Relay status information
#define REQ_M_BYTE		4  		// Memory read/write byte
#define REQ_M_INT		5  		// Memory read/write int
#define REQ_M_ALL		6  		// All memory values
#define REQ_VERSION		7  		// Library version
#define REQ_DATE		8  		// Date and time
#define REQ_R_STATUS	9		// Relay status information
#define REQ_RA_STATUS	10		// Relay status information + ato logging
#define REQ_FEEDING		11		// Start feeding mode
#define REQ_WATER		12		// Start water change mode
#define REQ_BTN_PRESS	13		// Simulate a button press, to end feeding & water change modes
#define REQ_CAL_RELOAD	14		// Reload calibration values from memory
#define REQ_ALARM_ATO	15		// Clears the ATO alarm
#define REQ_ALARM_OVERHEAT	16	// Clears the Overheat alarm
#define REQ_M_RAW		17  	// All memory raw values
#define REQ_LIGHTSON	18		// Turn Lights On
#define REQ_LIGHTSOFF	19		// Turn Lights Off
#define REQ_ALARM_LEAK	20		// Clears the Leak alarm
#define REQ_OVERRIDE	21		// Channel Override
#define REQ_HTTP		127		// HTTP get request from  external server
#define REQ_UNKNOWN		128	 	// Unknown request

#define P(name)   static const prog_char name[] PROGMEM
const prog_char SERVER_RA[] PROGMEM = "<script language='javascript' src='http://www.reefangel.com/wifi/ra1.js'></script>";
//const prog_char SERVER_RA[] PROGMEM = "<!DOCTYPE html><script src='http://www.reefangel.com/wifi/ra2.js'></script>";
const prog_char EncodingChars[] PROGMEM = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
const prog_char BannerGET[] PROGMEM = "GET /status/submitp.aspx?t1=";
const prog_char BannerT2[] PROGMEM = "&t2=";
const prog_char BannerT3[] PROGMEM = "&t3=";
const prog_char BannerPH[] PROGMEM = "&ph=";
const prog_char BannerATOHIGH[] PROGMEM = "&atohigh=";
const prog_char BannerATOLOW[] PROGMEM = "&atolow=";
const prog_char BannerRelayData[] PROGMEM = "&r";
const prog_char BannerRelayMaskOn[] PROGMEM = "&ron";
const prog_char BannerRelayMaskOff[] PROGMEM = "&roff";
const prog_char BannerID[] PROGMEM = "&id=";
const prog_char BannerEM[] PROGMEM = "&em=";
const prog_char BannerEM1[] PROGMEM = "&em1=";
const prog_char BannerREM[] PROGMEM = "&rem=";
const prog_char BannerCustom[] PROGMEM = "&c";
const prog_char BannerKey[] PROGMEM = "&key=";
const prog_char BannerFlag[] PROGMEM = "&flag=";

#if defined DisplayLEDPWM && ! defined RemoveAllLights
	const prog_char BannerPWMA[] PROGMEM = "&pwma=";
	const prog_char BannerPWMD[] PROGMEM = "&pwmd=";
#endif  // DisplayLEDPWM && ! defined RemoveAllLights

#ifdef PWMEXPANSION
	const prog_char BannerPWME[] PROGMEM = "&pwme";
#endif  // PWMEXPANSION

#ifdef RFEXPANSION
	const prog_char BannerRFM[] PROGMEM = "&rfm=";
	const prog_char BannerRFS[] PROGMEM = "&rfs=";
	const prog_char BannerRFD[] PROGMEM = "&rfd=";
	const prog_char BannerRFW[] PROGMEM = "&rfw=";
	const prog_char BannerRFRB[] PROGMEM = "&rfrb=";
	const prog_char BannerRFR[] PROGMEM = "&rfr=";
	const prog_char BannerRFG[] PROGMEM = "&rfg=";
	const prog_char BannerRFB[] PROGMEM = "&rfb=";
	const prog_char BannerRFI[] PROGMEM = "&rfi=";
#endif  // RFEXPANSION

#ifdef AI_LED
	const prog_char BannerAIW[] PROGMEM = "&aiw=";
	const prog_char BannerAIB[] PROGMEM = "&aib=";
	const prog_char BannerAIRB[] PROGMEM = "&airb=";
#endif  // AI_LED

#ifdef SALINITYEXPANSION
	const prog_char BannerSal[] PROGMEM = "&sal=";
#endif  // SALINITYEXPANSION

#ifdef ORPEXPANSION
	const prog_char BannerORP[] PROGMEM = "&orp=";
#endif  // ORPEXPANSION

#ifdef IOEXPANSION
	const prog_char BannerIO[] PROGMEM = "&io=";
#endif  // IOEXPANSION

#ifdef PHEXPANSION
	const prog_char BannerPHE[] PROGMEM = "&phe=";
#endif  // PHEXPANSION

#ifdef WATERLEVELEXPANSION
	const prog_char BannerWL[] PROGMEM = "&wl=";
#endif  // WATERLEVELEXPANSION
	
#ifdef HUMIDITYEXPANSION
	const prog_char BannerHumidity[] PROGMEM = "&hum=";
#endif  // HUMIDITYEXPANSION

#ifdef DCPUMPCONTROL
	const prog_char BannerDCM[] PROGMEM = "&dcm=";
	const prog_char BannerDCS[] PROGMEM = "&dcs=";
	const prog_char BannerDCD[] PROGMEM = "&dcd=";
#endif  // DCPUMPCONTROL


#ifdef CUSTOM_VARIABLES
	const prog_char BannerCustomVar[] PROGMEM = "&c";
#endif //CUSTOM_VARIABLES

static char m_pushback[32];
static byte m_pushbackindex=0;
static byte reqtype=0;
static unsigned long timeout;
static boolean bIncoming=false;
static boolean auth=false;
static char authStr[32];
static int weboption=0;
static int weboption2=-1;
static int weboption3=-1;
static byte bHasSecondValue = false;
//static byte bHasComma = false;
static byte bCommaCount = 0;
static boolean webnegoption=false;

#endif  // wifi || ethernet

#ifdef wifi

void WebResponse (const prog_char *response, long strsize);
void pushbuffer(byte inStr);
void PrintHeader(int s, byte type);
char GetC(int c);
void ConvertC(char* strIn, char* strOut, byte len);
void WifiAuthentication(char* userpass);
void SendXMLData(bool fAtoLog = false);
void processHTTP();
void CheckWifi();
void pingSerial();
void PROGMEMprint(const prog_char str[]);

#endif  // wifi

#endif  // __RA_WIFI_H__
