/*
 * RA_NET.h
 *
 *  Created on: Sep 2, 2013
 *      Author: Benjamin Runnels
 */

#ifndef RA_NET_H_
#define RA_NET_H_

#include <Globals.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <DS1307RTC.h>
#include <Print.h>

// REQUEST TYPES
#define REQ_ROOT    1   // Default page
#define REQ_WIFI    2   // Wifi page
#define REQ_RELAY   3   // Relay status information
#define REQ_M_BYTE    4     // Memory read/write byte
#define REQ_M_INT   5     // Memory read/write int
#define REQ_M_ALL   6     // All memory values
#define REQ_VERSION   7     // Library version
#define REQ_DATE    8     // Date and time
#define REQ_R_STATUS  9   // Relay status information
#define REQ_RA_STATUS 10    // Relay status information + ato logging
#define REQ_FEEDING   11    // Start feeding mode
#define REQ_WATER   12    // Start water change mode
#define REQ_BTN_PRESS 13    // Simulate a button press, to end feeding & water change modes
#define REQ_CAL_RELOAD  14    // Reload calibration values from memory
#define REQ_ALARM_ATO 15    // Clears the ATO alarm
#define REQ_ALARM_OVERHEAT  16  // Clears the Overheat alarm
#define REQ_M_RAW   17    // All memory raw values
#define REQ_LIGHTSON  18    // Turn Lights On
#define REQ_LIGHTSOFF 19    // Turn Lights Off
#define REQ_ALARM_LEAK  20    // Clears the Leak alarm
#define REQ_OVERRIDE  21    // Channel Override
#define REQ_REBOOT    22    // Reboot
#define REQ_HTTP    127   // HTTP get request from  external server
#define REQ_UNKNOWN   128   // Unknown request

#define P(name) static const prog_char name[] PROGMEM

class RA_NET: public Print
{
  public:
    RA_NET();
    void WebResponse (const prog_char *response, long strsize);
    void ModeResponse(bool fOk);
    void PushBuffer(byte inStr);
    void ProcessHTTP();
    void PrintHeader(int s, byte type);
    char GetC(int c);
    void ConvertC(char* strIn, char* strOut, byte len);
    void Authentication(char* userpass);
    void SendXMLData(bool fAtoLog = false);
    void PROGMEMprint(const prog_char str[]);

    void LoadWebBanner(int pointer, byte qty);
    void Portal(char *username);
    void Portal(char *username, char *key);
    void SendPortal(char *username, char*key);
    using Print::write;

    char *portalusername;

  protected:
    void _init();
    char m_pushback[32];
    byte m_pushbackindex;
    byte reqtype;
    unsigned long timeout;
    boolean bIncoming;
    boolean auth;
    char authStr[32];
    int weboption;
    int weboption2;
    int weboption3;
    byte bHasSecondValue;
    //byte bHasComma;
    byte bCommaCount;
    boolean webnegoption;

    prog_char XML_ID[];
    prog_char XML_T1[];
    prog_char XML_T2[];
    prog_char XML_T3[];
    prog_char XML_PH[];
    prog_char XML_R[];
    prog_char XML_RON[];
    prog_char XML_ROFF[];
    prog_char XML_RE_OPEN[];
    prog_char XML_RE_CLOSE[];
    prog_char XML_RE_ON[];
    prog_char XML_RE_OFF[];
    prog_char XML_ATOLOW[];
    prog_char XML_ATOHIGH[];
    prog_char XML_EM[];
    prog_char XML_EM1[];
    prog_char XML_REM[];
    prog_char XML_ALERTFLAG[];
    prog_char XML_STATUSFLAG[];
    prog_char XML_STATUSFLAG_END[];

#ifdef DisplayLEDPWM
    prog_char XML_PWMA[];
    prog_char XML_PWMD[];
    prog_char XML_PWMAO[];
    prog_char XML_PWMDO[];
    prog_char XML_PWMDO_END[];
#endif  // DisplayLEDPWM
#ifdef ORPEXPANSION
    prog_char XML_ORP[];
    prog_char XML_ORP_END[];
#endif  // ORPEXPANSION
#ifdef SALINITYEXPANSION
    prog_char XML_SAL[];
    prog_char XML_SAL_END[];
#endif  // SALINITYEXPANSION
#ifdef PHEXPANSION
    prog_char XML_PHEXP[];
    prog_char XML_PHEXP_END[];
#endif  // PHEXPANSION
#ifdef WATERLEVELEXPANSION
    prog_char XML_WL[];
    prog_char XML_WL_END[];
#endif  // WATERLEVELEXPANSION
#ifdef HUMIDITYEXPANSION
    prog_char XML_HUM[];
    prog_char XML_HUM_END[];
#endif  // HUMIDITYEXPANSION
#ifdef DCPUMPCONTROL
    prog_char XML_DCM[];
    prog_char XML_DCM_END[];
    prog_char XML_DCS_END[];
    prog_char XML_DCD_END[];
#endif  // DCPUMPCONTROL
#ifdef PWMEXPANSION
    prog_char XML_PWME[];
    prog_char XML_PWME_END[];
#endif  // PWMEXPANSION
#ifdef AI_LED
    prog_char XML_AIW[];
    prog_char XML_AIW_END[];
    prog_char XML_AIB_END[];
    prog_char XML_AIRB_END[];
    prog_char XML_AIWO_END[];
    prog_char XML_AIBO_END[];
    prog_char XML_AIRBO_END[];
#endif  // AI_LED
#ifdef RFEXPANSION
    prog_char XML_RFM[];
    prog_char XML_RFM_END[];
    prog_char XML_RFS_END[];
    prog_char XML_RFD_END[];
    prog_char XML_RFW_END[];
    prog_char XML_RFRB_END[];
    prog_char XML_RFR_END[];
    prog_char XML_RFG_END[];
    prog_char XML_RFB_END[];
    prog_char XML_RFI_END[];
    prog_char XML_RFWO_END[];
    prog_char XML_RFRBO_END[];
    prog_char XML_RFRO_END[];
    prog_char XML_RFGO_END[];
    prog_char XML_RFBO_END[];
    prog_char XML_RFIO_END[];
#endif  // RFEXPANSION
#ifdef IOEXPANSION
    prog_char XML_IO[];
    prog_char XML_IO_END[];
#endif  // IOEXPANSION
#ifdef CUSTOM_VARIABLES
    prog_char XML_C[];
    prog_char XML_C_END[];
#endif  // CUSTOM_VARIABLES
#ifdef ENABLE_ATO_LOGGING
    prog_char XML_ATOLOW_LOG_OPEN[];
    prog_char XML_ATOLOW_LOG_CLOSE[];
    prog_char XML_ATOHIGH_LOG_OPEN[];
    prog_char XML_ATOHIGH_LOG_CLOSE[];
#endif  // ENABLE_ATO_LOGGING
    prog_char XML_END[];
    prog_char XML_CLOSE_TAG[];


    prog_char XML_P_OPEN[];
    prog_char XML_P_CLOSE[];
    prog_char XML_M_OPEN[];
    prog_char XML_M_CLOSE[];
    prog_char XML_MEM_OPEN[];
    prog_char XML_MEM_CLOSE[];
    prog_char XML_DATE_OPEN[];
    prog_char XML_DATE_CLOSE[];
    prog_char XML_MODE_OPEN[];
    prog_char XML_MODE_CLOSE[];
    prog_char XML_OK[];
    prog_char XML_ERR[];

    prog_char SERVER_HEADER1[];
    prog_char SERVER_HEADER2[];
    prog_char SERVER_DENY[];
    prog_char SERVER_DEFAULT[];

    prog_char SERVER_RA[];
    //prog_char SERVER_RA[];
    prog_char EncodingChars[];
    prog_char BannerGET[];
    prog_char BannerT2[];
    prog_char BannerT3[];
    prog_char BannerPH[];
    prog_char BannerATOHIGH[];
    prog_char BannerATOLOW[];
    prog_char BannerRelayData[];
    prog_char BannerRelayMaskOn[];
    prog_char BannerRelayMaskOff[];
    prog_char BannerID[];
    prog_char BannerEM[];
    prog_char BannerEM1[];
    prog_char BannerREM[];
    prog_char BannerCustom[];
    prog_char BannerKey[];
    prog_char BannerAlertFlag[];
    prog_char BannerStatusFlag[];

#if defined DisplayLEDPWM && ! defined RemoveAllLights
    prog_char BannerPWMA[];
    prog_char BannerPWMD[];
    prog_char BannerPWMAO[];
    prog_char BannerPWMDO[];
#endif  // DisplayLEDPWM && ! defined RemoveAllLights

#ifdef PWMEXPANSION
    prog_char BannerPWME[];
#endif  // PWMEXPANSION

#ifdef RFEXPANSION
    prog_char BannerRFM[];
    prog_char BannerRFS[];
    prog_char BannerRFD[];
    prog_char BannerRFW[];
    prog_char BannerRFRB[];
    prog_char BannerRFR[];
    prog_char BannerRFG[];
    prog_char BannerRFB[];
    prog_char BannerRFI[];
#endif  // RFEXPANSION

#ifdef AI_LED
    prog_char BannerAIW[];
    prog_char BannerAIB[];
    prog_char BannerAIRB[];
#endif  // AI_LED

#ifdef SALINITYEXPANSION
    prog_char BannerSal[];
#endif  // SALINITYEXPANSION

#ifdef ORPEXPANSION
    prog_char BannerORP[];
#endif  // ORPEXPANSION

#ifdef IOEXPANSION
    prog_char BannerIO[];
#endif  // IOEXPANSION

#ifdef PHEXPANSION
    prog_char BannerPHE[];
#endif  // PHEXPANSION

#ifdef WATERLEVELEXPANSION
    prog_char BannerWL[];
#endif  // WATERLEVELEXPANSION

#ifdef HUMIDITYEXPANSION
    prog_char BannerHumidity[];
#endif  // HUMIDITYEXPANSION

#ifdef DCPUMPCONTROL
    prog_char BannerDCM[];
    prog_char BannerDCS[];
    prog_char BannerDCD[];
#endif  // DCPUMPCONTROL


#ifdef CUSTOM_VARIABLES
    prog_char BannerCustomVar[];
#endif //CUSTOM_VARIABLES

};

#endif /* RA_NET_H_ */
