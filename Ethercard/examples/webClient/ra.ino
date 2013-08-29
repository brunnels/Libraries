// ReefDruino (c)PAVSoftworks 
//P Viscovich
// 10/21/2011
// added test for relay update to only update after deley has been met 
// problem was that the main loop would loop more that one time a sec 
// and that would toggle the bits incorectly so main lights were toggled off 
// when led lights were turned off. (03/22/2012)

//#include <avr/wdt.h>
#include <Wire.h>
#include <LCDI2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EtherCard.h>
#include <avr/eeprom.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 7

// Address of 24L32 eeprom chip
#define ROM_ADDRESS 0x50    

#define LAMPOFFADDR 0x00FA
#define FANOFFADDR  0x00FB
#define MINHADDR    0x00FC
#define MAXHADDR    0x00FD
#define BRIADDR     0x00FE
#define CONADDR     0x00FF

#define  MAINONHADDR  0x00F2
#define  MAINONMADDR  0x00F3
#define  MAINOFFHADDR 0x00F4
#define  MAINOFFMADDR 0x00F5
#define  LEDONHADDR   0x00F6
#define  LEDONMADDR   0x00F7
#define  LEDOFFHADDR  0x00F8
#define  LEDOFFMADDR  0x00F9

//relay map to port bit
#define ATORELAY    0
#define MAINLAMPS   1
#define HEATER      2
#define RELAY4      3
#define TANKFAN     4
#define LEDLAMPS    5
#define INTERNALFAN 6
#define RELAY8      7
byte RelayBanks = B11111111;

//keypad defines
#define KEY_PAD_PORT   A3
#define KEY_PAD_NONE   0
#define KEY_PAD_LEFT   1
#define KEY_PAD_UP     2
#define KEY_PAD_RIGHT  3
#define KEY_PAD_SELECT 4
#define KEY_PAD_DOWN   5

//menu and keypad declarations
byte MenuFlag1 = 0;
byte KeyPressed = 0;

#define REQUEST_RATE 30000 // milliseconds

// ethernet interface mac address
static byte mymac[] = {
  0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 
  192,168,1,15 };
static byte gwip[] = { 
  192,168,1,1 };
static byte dnsip[] = {
  68,87,76,182};


// remote website name
char website[] PROGMEM = "www.yourwebaddress.com";
char tbuff[30];
byte Ethernet::buffer[400];   // a very small tcp/ip buffer is enough here
static long timer;
BufferFiller bfill;
char cbuff[]= "UID=XXX&T1=00.00&T2=00.00&T3=00.00&T4=00.00&RD=000&AO=0&FIN=0&EX=0&ML=0&LL=0&HE=0";

//RTC
#define DS1307_I2C_ADDRESS 0x68
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// We'll use this variable to store a found device address
DeviceAddress TP1 = {
  0x28, 0x39, 0xEF, 0x32, 0x03, 0x00, 0x00, 0x7B};
DeviceAddress TP2 = {
  0x28, 0xC0, 0x81, 0xAA, 0x03, 0x00, 0x00, 0xD1};
DeviceAddress TP3 = {
  0x28, 0x31, 0x7F, 0xAA, 0x03, 0x00, 0x00, 0x90};
DeviceAddress TP4 = {
  0x28, 0x13, 0x65, 0xAA, 0x03, 0x00, 0x00, 0x89};

unsigned long lastTempRequest = 0;
unsigned long lastRelaySet = 0;
int  delayInMillis = 0;
float ClockBoardTempTP1 = 0;
float AmbiantNearBoxTP2 = 0;
float AmbiantNearTankTP3 = 0;
float TankTP4 = 0;
float ClockBoardTempTP1a = 0;
float AmbiantNearBoxTP2a = 0;
float AmbiantNearTankTP3a = 0;
float TankTP4a = 0;

int  MinHeater = 0;
int  MaxHeater = 0;
int  FanOnTemp = 0;
int  LampOffTemp = 0;

//timmer variables
int MainOnHour;
int MainOnMinute;
int MainOffHour;
int MainOffMinute ;
int LEDOnHour ;
int LEDOnMinute;
int LEDOffHour ;
int LEDOffMinute ;
int LEDStart;
int LEDStop;
int MainStart;
int MainStop;


int idle = 0;
byte flag1 = 0;
byte flag2 = 0;

byte err;

//ATO digital port
#define ATOPin  8

//chr row and column for lcd display
int g_rows = 4;
int g_cols = 20;

// Number of lines and i2c address of the display
LCDI2C lcd = LCDI2C(g_rows,g_cols,0x4C,1);             

void setup() { 

  Wire.begin (); //needs to be run before first eeprom read/write
  //set I2C digital buss for output 
  wireout (0x4C, 0xFE, 0x1C, 0x03);
  //may not need this depending on state of of digital ports on start up
  wireout (0x4C, 0xFE, 0x1E, RelayBanks);


  BackLight(readEEPROM1 (ROM_ADDRESS,BRIADDR));
  Contrast (readEEPROM1 (ROM_ADDRESS,CONADDR));

  MinHeater = readEEPROM1 (ROM_ADDRESS, MINHADDR);
  MaxHeater = readEEPROM1 (ROM_ADDRESS, MAXHADDR);
  FanOnTemp =readEEPROM1 (ROM_ADDRESS, FANOFFADDR);
  LampOffTemp = readEEPROM1 (ROM_ADDRESS, LAMPOFFADDR);

  MainOnHour  = readEEPROM1 (ROM_ADDRESS, MAINONHADDR);
  MainOnMinute  = readEEPROM1 (ROM_ADDRESS, MAINONMADDR);
  MainOffHour  = readEEPROM1 (ROM_ADDRESS, MAINOFFHADDR);
  MainOffMinute =  readEEPROM1 (ROM_ADDRESS, MAINOFFMADDR);
  LEDOnHour =  readEEPROM1 (ROM_ADDRESS, LEDONHADDR);
  LEDOnMinute =  readEEPROM1 (ROM_ADDRESS, LEDONMADDR);
  LEDOffHour =  readEEPROM1 (ROM_ADDRESS, LEDOFFHADDR);
  LEDOffMinute =  readEEPROM1 (ROM_ADDRESS, LEDOFFMADDR);
  LEDStart  = (LEDOnHour*60)+(LEDOnMinute);
  LEDStop   = (LEDOffHour*60)+(LEDOffMinute);
  MainStart = (MainOnHour*60)+(MainOnMinute);
  MainStop  = (MainOffHour*60)+(MainOffMinute);

  lcd.init();      // Init the display, clears the display
  ReadScreens(0x0000); //splash screen 0x0000  

    // Start up OneWire library
  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  delayInMillis = 750 / (1 << (12 - 12)); //resolution defaul is 12 so we dont set it
  lastTempRequest = millis(); 
  lastRelaySet = millis();

  // config port for ato read
  pinMode(ATOPin, INPUT);           // set pin to input
  digitalWrite(ATOPin, HIGH);       // turn on pullup resistors

  //Serial.begin(57600); 
  MenuFlag1 = 0;

  ether.begin(sizeof Ethernet::buffer, mymac);
  ether.staticSetup(myip, gwip, dnsip);
  !ether.dnsLookup(website);
  while (ether.clientWaitingGw()) ether.packetLoop(ether.packetReceive());
  //Serial.println("Gateway found");
  timer = - REQUEST_RATE; // start timing out right away

  //wdt_enable(WDTO_8S);// set watchdog timer to 8 seconds

}
//#################################################################################
//
//#################################################################################
void loop()
{
  //wdt_reset();

  //if menuflag = 0 then reprint the main display
  if(!MenuFlag1) ReadScreens(0x0100);
  DisplayDateTime();

  if (millis() - lastTempRequest >= delayInMillis) // waited long enough??
  {
    ClockBoardTempTP1 = sensors.getTempF(TP1);
    AmbiantNearBoxTP2 = sensors.getTempF(TP2);
    AmbiantNearTankTP3 = sensors.getTempF(TP3);      
    TankTP4 = sensors.getTempF(TP4);  
    sensors.requestTemperatures(); 
    lastTempRequest = millis(); 
  }
  DisplayTemps();

  if (millis() - lastRelaySet >= delayInMillis) // waited long enough??
  {
    // internal fan two lines of code for gaurd band 
    if (ClockBoardTempTP1  > 95) bitClear (RelayBanks,INTERNALFAN); //fan on
    if (ClockBoardTempTP1  < 77)  bitSet  (RelayBanks,INTERNALFAN); //fan off

    bitClear (RelayBanks,ATORELAY); //ATO Pump On
    if (digitalRead(ATOPin)) bitSet (RelayBanks,ATORELAY);//ATO Pump Off

    bitSet (RelayBanks,MAINLAMPS );
    if (IsTimeOfDayBetween((hour*60)+minute+1,MainStart,MainStop)) bitClear (RelayBanks,MAINLAMPS ); 

    bitSet (RelayBanks,LEDLAMPS );
    if (IsTimeOfDayBetween((hour*60)+minute+1,LEDStart,LEDStop)) bitClear (RelayBanks,LEDLAMPS ); 

    //bit set turns it off
    if (TankTP4 < MinHeater) bitClear (RelayBanks,HEATER);//on
    if (TankTP4 > MaxHeater) bitSet   (RelayBanks,HEATER);//off
    if (TankTP4 > FanOnTemp) bitClear (RelayBanks,TANKFAN); //on
    if (TankTP4 < FanOnTemp-2) bitSet (RelayBanks,TANKFAN);
    if (TankTP4 > LampOffTemp) bitSet (RelayBanks,MAINLAMPS); //on
    if (TankTP4 > LampOffTemp) bitSet (RelayBanks,LEDLAMPS); //on

    //turn on or off the relays in one byte
    wireout (0x4C, 0xFE, 0x1E, RelayBanks);
    lastRelaySet = millis(); 
  }

  //update screen relay control bits
  DisplayIOFlags();

  //poll keypad for key press
  if(Key_Pad_Read () != KEY_PAD_NONE) {
    MenuFlag1 = 0; 
    Menu1();
  }

  // present web pages if page request is made from server.
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos) { // check if valid tcp data is received
    bfill = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
    // receive buf hasn't been clobbered by reply yet
    if (strncmp("GET /raw/", data, 9) == 0) ether.httpServerReply(homePage1()); // send web page data
    if (strncmp("GET /con/ ", data, 10) == 0) ether.httpServerReply(homePage2()); // send web page data
    ether.httpServerReply(homePage0()); // send web page data
  } 

  // out put data to database 
  if (millis() > timer + REQUEST_RATE) {
    ether.packetLoop(ether.packetReceive());
    timer = millis();
    FillOutBuff (ClockBoardTempTP1, 11, 5, 2);
    FillOutBuff (AmbiantNearBoxTP2, 20, 5, 2);
    FillOutBuff (AmbiantNearTankTP3, 29, 5, 2);    
    FillOutBuff (TankTP4, 38, 5, 2);
    FillOutBuff (RelayBanks, 47, 3, 0);
    FillOutBuff (!bitRead(RelayBanks, ATORELAY), 54, 1, 0);    
    FillOutBuff (!bitRead(RelayBanks, INTERNALFAN), 60, 1, 0);
    FillOutBuff (!bitRead(RelayBanks, TANKFAN), 65, 1, 0);
    FillOutBuff (!bitRead(RelayBanks, MAINLAMPS), 70, 1, 0);    
    FillOutBuff (!bitRead(RelayBanks, LEDLAMPS), 75, 1, 0);
    FillOutBuff (!bitRead(RelayBanks, LEDLAMPS), 80, 1, 0); 
    ether.browseUrl(PSTR("/rc/status/submit.asp?"),cbuff,website, my_result_cb);
  }
}

//######################################################################################
// Start of all things Sub
//######################################################################################
int IsTimeOfDayBetween(int time, int startTime, int endTime) 
{ 
  if (endTime == startTime) 
  { 
    return true;    
  } 
  else if (endTime < startTime) 
  { 
    return time <= endTime || time >= startTime; 
  } 
  else 
  { 
    return time >= startTime && time <= endTime; 
  } 

}

void Menu1 (){
  if (MenuFlag1 == 1) {
    MenuFlag1 = 0; 
    return;
  }
  int SelectX = 0;
  ReadScreens(0x0200); //first menu =0x0200
  byte l = 0;
  lcd.setCursor(0,19);  
  lcd.blink_on();
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++;
    } 
    else {
      l = 0;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 0;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) SelectX++;
    if (KeyPressed == KEY_PAD_UP) SelectX--;
    if (SelectX < 0) SelectX = 3;
    if (SelectX > 3 ) SelectX = 0;
    if (KeyPressed == KEY_PAD_SELECT){//SelectX is the menu item 0 - 3 when select is pressed
      switch (SelectX) {
      case 0:  
        MenuFlag1 = 1;
        SetLightTime();
        LEDStart  = (LEDOnHour*60)+(LEDOnMinute);
        LEDStop   = (LEDOffHour*60)+(LEDOffMinute);
        MainStart = (MainOnHour*60)+(MainOnMinute);
        MainStop  = (MainOffHour*60)+(MainOffMinute);

        if (MenuFlag1 == 2) {
          ReadScreens(0x0200); //first menu =0x0200
          byte l = 0;
          lcd.setCursor(0,19);  
          lcd.blink_on();
          break;
        }
        MenuFlag1 = 0;
        return;

      case 1:  
        MenuFlag1 = 1;
        SetTempratures();
        if (MenuFlag1 == 2) {
          ReadScreens(0x0200); //first menu =0x0200
          byte l = 0;
          lcd.setCursor(0,19);  
          lcd.blink_on();
          break;
        }
        MenuFlag1 = 0;
        return;

      case 2:  
        MenuFlag1 = 1;
        SetBackCont();
        if (MenuFlag1 == 2) {
          ReadScreens(0x0200); //first menu =0x0200
          byte l = 0;
          lcd.setCursor(0,19);  
          lcd.blink_on();
          break;
        }
        MenuFlag1 = 0;
        return;      

      case 3:  
        MenuFlag1 = 1;
        SetTimeDate();
        if (MenuFlag1 == 2) {
          ReadScreens(0x0200); //first menu =0x0200
          byte l = 0;
          lcd.setCursor(0,19);  
          lcd.blink_on();
          break;
        }
        MenuFlag1 = 0;
        return;
      }
    }
    lcd.setCursor(SelectX,19);
    delay(100);
  }
  MenuFlag1 = 0;
  return;
}
//#######################################################################################
//  brightness and contrast adjustments
//
//#######################################################################################
void SetBackCont()
{
  int bright = readEEPROM1 (ROM_ADDRESS,BRIADDR);
  int contrast = readEEPROM1 (ROM_ADDRESS,CONADDR );
  int SelectX = 0;
  byte l = 0;
  ReadScreens(0x0500);
  lcd.setCursor(0,15);
  lcd.print(contrast);   
  lcd.setCursor(1,15);
  lcd.print(bright);    
  lcd.setCursor(0,19);
  lcd.blink_on();
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
    } 
    else {
      l = 0;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) SelectX++;
    if (KeyPressed == KEY_PAD_UP) SelectX--;
    if (SelectX < 0) SelectX = 1;
    if (SelectX > 1 ) SelectX = 0;
    if (KeyPressed == KEY_PAD_SELECT){//SelectX is the menu item 0 - 3 when select is pressed
      switch (SelectX) {
      case 0:  
        SetConBri(0,1,65);
        MenuFlag1 = 2;
        Contrast (readEEPROM1 (ROM_ADDRESS,CONADDR)); 
        return;

      case 1:  
        SetConBri(1,0,255);
        MenuFlag1 = 2;
        BackLight(readEEPROM1 (ROM_ADDRESS,BRIADDR));
        return;
      }
    }
    lcd.setCursor(SelectX,19);
    delay(100);    
  }
}

void SetConBri(int flag, int min1, int max1){
  int Value = 0;
  int inc1 = 0;
  if (flag==0) {
    Value = readEEPROM1 (ROM_ADDRESS,CONADDR );
    Contrast(Value);  
    inc1 = 1;
  }
  if (flag==1){
    Value = readEEPROM1 (ROM_ADDRESS,BRIADDR);
    BackLight(Value);
    inc1 = 5;
  }

  byte l = 0;
  int test1 = 0;
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
      test1 = 0;
    } 
    else {
      l = 0;
      test1++;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) Value=Value-inc1;
    if (KeyPressed == KEY_PAD_UP) Value=Value+inc1;
    if (KeyPressed == KEY_PAD_SELECT and test1 == 2){
      if (flag==0) writeEEPROM1 (ROM_ADDRESS, CONADDR, Value);
      if (flag==1) writeEEPROM1 (ROM_ADDRESS, BRIADDR, Value);  
      return;
    }    
    if (Value < min1) Value = max1;
    if (Value > max1 ) Value = min1;    
    lcd.setCursor(flag,15);
    lcd.print(Value); 
    lcd.print("  ");
    lcd.setCursor(flag,15);
    if (flag==0) Contrast(Value); 
    if (flag==1) BackLight(Value);
  }
}
//#######################################################################################
//  Set Temp min max and lamps
//
//#######################################################################################
void SetTempratures()
{
  int SelectX = 0;
  byte l = 0;   
  ReadScreens(0x0600);
  lcd.setCursor(0,12);
  lcd.print(MinHeater);
  lcd.setCursor(1,12);
  lcd.print(MaxHeater);
  lcd.setCursor(2,12);
  lcd.print(FanOnTemp);
  lcd.setCursor(3,12);
  lcd.print(LampOffTemp);
  lcd.setCursor(0,19);
  lcd.blink_on();
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
    } 
    else {
      l = 0;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) SelectX++;
    if (KeyPressed == KEY_PAD_UP) SelectX--;
    if (SelectX < 0) SelectX = 3;
    if (SelectX > 3 ) SelectX = 0;
    if (KeyPressed == KEY_PAD_SELECT){//SelectX is the menu item 0 - 3 when select is pressed
      switch (SelectX) {
      case 0:  
        MenuFlag1=0;
        lcd.setCursor(0,12);
        SetHeating(0, 73, 85, MinHeater);
        MinHeater = readEEPROM1 (ROM_ADDRESS, MINHADDR);
        MenuFlag1 = 2;
        return;
        MinHeater = readEEPROM1 (ROM_ADDRESS, MINHADDR);
      case 1:  
        MenuFlag1=0;
        lcd.setCursor(1,12);
        SetHeating(1, 73, 85, MaxHeater);
        MaxHeater = readEEPROM1 (ROM_ADDRESS, MAXHADDR);
        MenuFlag1 = 2;
        return;
      case 2:  
        MenuFlag1=0;
        lcd.setCursor(2,12);
        SetHeating(2, 73, 85, FanOnTemp);
        FanOnTemp =readEEPROM1 (ROM_ADDRESS, FANOFFADDR);
        MenuFlag1 = 2;
        return;
      case 3:  
        MenuFlag1=0;
        lcd.setCursor(3,12);
        SetHeating(3, 73, 85, LampOffTemp);
        LampOffTemp = readEEPROM1 (ROM_ADDRESS, LAMPOFFADDR);
        MenuFlag1 = 2;
        return;        
      }
    }
    lcd.setCursor(SelectX,19);
    delay(100);    
  }
}

void SetHeating(int flag, int min1, int max1, int Value){
  byte l = 0;
  int test1 = 0;
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
      test1 = 0;
    } 
    else {
      l = 0;
      test1++;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) Value=Value--;
    if (KeyPressed == KEY_PAD_UP) Value=Value++;
    if (KeyPressed == KEY_PAD_SELECT and test1 == 2){
      if (flag==0) writeEEPROM1 (ROM_ADDRESS, MINHADDR, Value);
      if (flag==1) writeEEPROM1 (ROM_ADDRESS, MAXHADDR, Value);  
      if (flag==2) writeEEPROM1 (ROM_ADDRESS, FANOFFADDR, Value); 
      if (flag==3) writeEEPROM1 (ROM_ADDRESS, LAMPOFFADDR, Value); 
      return;
    }    
    if (Value < min1) Value = max1;
    if (Value > max1 ) Value = min1;    
    lcd.setCursor(flag,12);
    lcd.print(Value); 
    lcd.print("  ");
    lcd.setCursor(flag,12);
    delay (100);
  }
}
//#######################################################################################
//  Set Date and Time
//
//#######################################################################################
void SetTimeDate(){
  byte SelectX = 0;
  byte l = 0;
  lcd.clear();
  lcd.print("Set Time");
  lcd.setCursor(0,9);
  printDigits(hour, 0);
  printDigits(minute, 1);
  printDigits(second, 1);
  lcd.setCursor(1,0);
  lcd.print("Set Date");
  lcd.setCursor(1,9);
  printDigits(month, 0);  
  printDigits(dayOfMonth,2); 
  printDigits(year, 2); 
  lcd.setCursor(0,19);
  lcd.blink_on();  
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
    }
    else{ 
      l = 0;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) SelectX++;
    if (KeyPressed == KEY_PAD_UP) SelectX--;
    if (SelectX < 0) SelectX = 1;
    if (SelectX > 1 ) SelectX = 0;
    if (KeyPressed == KEY_PAD_SELECT){//SelectX is the menu item 0 - 3 when select is pressed
      switch (SelectX) {
      case 0:  
        SetDandT(0, 0, 23, hour,9);
        SetDandT(1, 0, 59, minute,12);
        MenuFlag1 = 2;
        return;

      case 1:  
        SetDandT(2, 1, 12, month,9);
        SetDandT(3, 1, 31, dayOfMonth,12);
        SetDandT(4, 0, 17, year,15);
        MenuFlag1 = 2;
        return;
      }
    }
    lcd.setCursor(SelectX,19);
    delay(100);    
  }
}


void SetDandT(int flag, int min1, int max1, int Value, int Tab){
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  byte l = 0;
  int test1 = 0;
  int flag1=0;
  if(flag<2) flag1=0;
  if(flag>1) flag1=1;

  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
      test1 = 0;
    } 
    else {
      l = 0;
      test1++;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) Value=Value--;
    if (KeyPressed == KEY_PAD_UP) Value=Value++;
    if (KeyPressed == KEY_PAD_SELECT and test1 == 2){
      if(flag==0) hour=Value;
      if(flag==1) minute=Value;
      if(flag==2) month=Value;
      if(flag==3) dayOfMonth=Value;
      if(flag==4) year=Value;
      setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
      return;
    }    
    if (Value < min1) Value = max1;
    if (Value > max1 ) Value = min1;    
    lcd.setCursor(flag1,Tab);
    if(Value <10){
      lcd.print("0");
    }
    lcd.print(Value); 
    lcd.setCursor(flag1,Tab);
    delay (100);
  }
}
//###################################################################################################
// This is used to set the light times for on and off
//
//###################################################################################################
void SetLightTime()
{
  int SelectX = 0;
  MainOnHour  = readEEPROM1 (ROM_ADDRESS, MAINONHADDR);
  MainOnMinute  = readEEPROM1 (ROM_ADDRESS, MAINONMADDR);
  MainOffHour  = readEEPROM1 (ROM_ADDRESS, MAINOFFHADDR);
  MainOffMinute =  readEEPROM1 (ROM_ADDRESS, MAINOFFMADDR);
  LEDOnHour =  readEEPROM1 (ROM_ADDRESS, LEDONHADDR);
  LEDOnMinute =  readEEPROM1 (ROM_ADDRESS, LEDONMADDR);
  LEDOffHour =  readEEPROM1 (ROM_ADDRESS, LEDOFFHADDR);
  LEDOffMinute =  readEEPROM1 (ROM_ADDRESS, LEDOFFMADDR);

  ReadScreens(0x0300); //first menu =0x0200
  lcd.setCursor(0,9);
  if (MainOnHour < 10)lcd.print("0");
  lcd.print(MainOnHour);
  lcd.print(":");
  if (MainOnMinute < 10) lcd.print("0");
  lcd.print(MainOnMinute);
  lcd.setCursor(1,9);
  if (MainOffHour < 10)lcd.print("0");
  lcd.print(MainOffHour);
  lcd.print(":");
  if (MainOffMinute < 10) lcd.print("0");
  lcd.print(MainOffMinute);
  lcd.setCursor(2,9);
  if (LEDOnHour < 10)lcd.print("0");
  lcd.print(LEDOnHour);
  lcd.print(":");
  if (LEDOnMinute < 10) lcd.print("0");
  lcd.print(LEDOnMinute);
  lcd.setCursor(3,9);
  if (LEDOffHour < 10)lcd.print("0");
  lcd.print(LEDOffHour);
  lcd.print(":");
  if (LEDOffMinute < 10) lcd.print("0");
  lcd.print(LEDOffMinute);  
  lcd.blink_on();
  byte l = 0;
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++;
    } 
    else {
      l = 0;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) SelectX++;
    if (KeyPressed == KEY_PAD_UP) SelectX--;
    if (SelectX < 0) SelectX = 3;
    if (SelectX > 3 ) SelectX = 0;
    if (KeyPressed == KEY_PAD_SELECT){//SelectX is the menu item 0 - 3 when select is pressed
      switch (SelectX) {
      case 0:  
        SetLampHours(0, 0, 23, MainOnHour,9);
        SetLampHours(1, 0, 59, MainOnMinute,12);
        MainOnHour  = readEEPROM1 (ROM_ADDRESS, MAINONHADDR);
        MainOnMinute  = readEEPROM1 (ROM_ADDRESS, MAINONMADDR);

        MenuFlag1 = 2;
        return;

      case 1:  
        SetLampHours(2, 0, 23, MainOffHour,9);
        SetLampHours(3, 0, 59, MainOffMinute,12);
        MainOffHour  = readEEPROM1 (ROM_ADDRESS, MAINOFFHADDR);
        MainOffMinute =  readEEPROM1 (ROM_ADDRESS, MAINOFFMADDR);

        MenuFlag1 = 2;
        return;

      case 2:  
        SetLampHours(4, 0, 23, LEDOnHour,9);
        SetLampHours(5, 0, 59, LEDOnMinute,12);
        LEDOnHour =  readEEPROM1 (ROM_ADDRESS, LEDONHADDR);
        LEDOnMinute =  readEEPROM1 (ROM_ADDRESS, LEDONMADDR);

        MenuFlag1 = 2;
        return;        

      case 3:  
        SetLampHours(6, 0, 23, LEDOffHour,9);
        SetLampHours(7, 0, 59, LEDOffMinute,12);
        LEDOffHour =  readEEPROM1 (ROM_ADDRESS, LEDOFFHADDR);
        LEDOffMinute =  readEEPROM1 (ROM_ADDRESS, LEDOFFMADDR);
        MenuFlag1 = 2;
        return;
      }
    }
    lcd.setCursor(SelectX,19);
    delay(50);    
  }
}


void SetLampHours(int flag, int min1, int max1, int Value, int Tab){
  byte l = 0;
  int test1 = 0;
  int flag1 = 0;
  if ((flag==2)||(flag==3)) flag1 = 1;
  if ((flag==4)||(flag==5)) flag1 = 2;
  if ((flag==6)||(flag==7)) flag1 = 3;
  while (l < 45)
  {
    KeyPressed = Key_Pad_Read ();
    if (KeyPressed == KEY_PAD_NONE) {
      l++; 
      test1 = 0;
    } 
    else {
      l = 0;
      test1++;
    }
    if (KeyPressed == KEY_PAD_LEFT) {
      MenuFlag1 = 2;
      return;
    } //Home
    if (KeyPressed == KEY_PAD_DOWN) Value=Value--;
    if (KeyPressed == KEY_PAD_UP) Value=Value++;
    if (KeyPressed == KEY_PAD_SELECT and test1 == 2){
      if (flag==0) writeEEPROM1 (ROM_ADDRESS, MAINONHADDR,Value);
      if (flag==1) writeEEPROM1 (ROM_ADDRESS, MAINONMADDR,Value);
      if (flag==2) writeEEPROM1 (ROM_ADDRESS, MAINOFFHADDR,Value);
      if (flag==3) writeEEPROM1 (ROM_ADDRESS, MAINOFFMADDR,Value);
      if (flag==4) writeEEPROM1 (ROM_ADDRESS, LEDONHADDR,Value);
      if (flag==5) writeEEPROM1 (ROM_ADDRESS, LEDONMADDR,Value);
      if (flag==6) writeEEPROM1 (ROM_ADDRESS, LEDOFFHADDR,Value);
      if (flag==7) writeEEPROM1 (ROM_ADDRESS, LEDOFFMADDR,Value);
      return;
    }    
    if (Value < min1) Value = max1;
    if (Value > max1 ) Value = min1;    
    lcd.setCursor(flag1,Tab);
    if(Value <10){
      lcd.print("0");
    }
    lcd.print(Value); 
    lcd.setCursor(flag1,Tab);
    delay (50);
  }
}

//END OF MENUS #############################################################################################################

void BackLight(uint8_t bright){
  wireout (0x4C, 0xFE, 0x03, bright);
}

void Contrast(uint8_t Contrast)
{
  wireout (0x4C, 0xFE, 0x04, Contrast);
}

int Key_Pad_Read ()
{
  //    wdt_reset();

  // read keypad port and then map the keypad
  // range to a range of 6 options:
  int KEY_Pad = analogRead(KEY_PAD_PORT);
  //add test for same key pressed and only move on if value if diffrent 
  //as when it would be if the key is no longer pressed
  byte KEY_Value = map(KEY_Pad,0, 1023, 0,128);
  switch (KEY_Value)
  {
  case 118 ... 138:    // Center value 128
    return KEY_PAD_NONE;

  case 53 ... 73:      // Center value  63
    //    delay(50); 
    return KEY_PAD_LEFT;    

  case 8 ... 27:       // Center value  17
    //    delay(50);
    return KEY_PAD_UP;

  case 0 ... 7:        // Center value  0 
    //    delay(50);
    return KEY_PAD_RIGHT;

  case 81 ... 101:    // Center value  91 
    //    delay(50);
    return KEY_PAD_SELECT;    

  case 31 ... 51:     // Center value  41 
    //    delay(50);
    return KEY_PAD_DOWN;
  }
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void setDateDs1307(byte second, byte minute,byte hour,byte dayOfWeek,byte dayOfMonth,byte month,byte year)          
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.send(0);
  Wire.send(decToBcd(second));    // 0 to bit 7 starts the clock
  Wire.send(decToBcd(minute));
  Wire.send(decToBcd(hour));      // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.send(decToBcd(dayOfWeek));
  Wire.send(decToBcd(dayOfMonth));
  Wire.send(decToBcd(month));
  Wire.send(decToBcd(year));
  Wire.endTransmission();
}

// Gets the date and time from the ds1307
void getDateDs1307(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.send(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  // A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.receive() & 0x7f);
  *minute     = bcdToDec(Wire.receive());
  *hour       = bcdToDec(Wire.receive() & 0x3f);  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.receive());
  *dayOfMonth = bcdToDec(Wire.receive());
  *month      = bcdToDec(Wire.receive());
  *year       = bcdToDec(Wire.receive());
}

void DisplayDateTime(){
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  flag2 = !flag2;
  lcd.setCursor(0,0);
  printDigits(hour, 0);
  printDigits(minute, 1);
  printDigits(second, 1);
  if (flag2){
    lcd.setCursor(0,12);
    printDigits(month, 0);  
    printDigits(dayOfMonth,2); 
    printDigits(year, 2); 
  }
}

void DisplayTemps(){
  flag1++;
  if (flag1 == 1) {
    lcd.setCursor(1,4);
    lcd.print(ClockBoardTempTP1);
  }
  if (flag1 == 2){      
    lcd.setCursor(1,15);
    lcd.print(AmbiantNearBoxTP2);
  }
  if (flag1 == 3) {
    lcd.setCursor(2,4);
    lcd.print(AmbiantNearTankTP3);
  }
  if (flag1 == 4) {    
    lcd.setCursor(2,15);
    lcd.print(TankTP4);    
  }
  if (flag1 == 4) flag1 = 0;
}

void DisplayIOFlags(){
  lcd.setCursor(3,2);
  lcd.print(!bitRead(RelayBanks, ATORELAY));
  lcd.setCursor(3,6);
  lcd.print(!bitRead(RelayBanks, MAINLAMPS));
  lcd.setCursor(3,10);
  lcd.print(!bitRead(RelayBanks, LEDLAMPS));
  lcd.setCursor(3,14);
  lcd.print(!bitRead(RelayBanks, TANKFAN));
  lcd.setCursor(3,19);
  lcd.print(!bitRead(RelayBanks, HEATER));
}

byte ReadScreens(unsigned int MenuBase)
{
  lcd.clear();
  lcd.blink_off();
  // read back to confirm
  byte LCDBuffer [4] [21];
  byte err;
  err = readEEPROM (ROM_ADDRESS, MenuBase+0x00, LCDBuffer[0], 21);
  lcd.setCursor(0,0);
  lcd.print ((char *) LCDBuffer[0]);  
  err = readEEPROM (ROM_ADDRESS, MenuBase+0x20, LCDBuffer[1], 21);
  lcd.setCursor(1,0);
  lcd.print ((char *) LCDBuffer[1]); 
  err = readEEPROM (ROM_ADDRESS, MenuBase+0x40, LCDBuffer[2], 21);
  lcd.setCursor(2,0);
  lcd.print ((char *) LCDBuffer[2]);  
  err = readEEPROM (ROM_ADDRESS, MenuBase+0x60, LCDBuffer[3], 21);
  lcd.setCursor(3,0);
  lcd.print ((char *) LCDBuffer[3]);  
  MenuFlag1 = 1;
}

byte writeEEPROM (byte device, unsigned int addr, byte * data, byte len ) 
{
  byte err;
  byte counter;
  if (len > BUFFER_LENGTH) return 0xFF;  // too long // 32 (in Wire.h)
  Wire.beginTransmission(device);
  Wire.send ((byte) (addr >> 8));    // high order byte
  Wire.send ((byte) (addr & 0xFF));  // low-order byte
  Wire.send (data, len);
  err = Wire.endTransmission ();
  if (err != 0)   return err;  // cannot write to device
  // wait for write to finish by sending address again
  //  ... give up after 100 attempts (1/10 of a second)
  for (counter = 0; counter < 100; counter++)
  {
    delayMicroseconds (300);  // give it a moment
    Wire.beginTransmission (device);
    Wire.send ((byte) (addr >> 8));    // high order byte
    Wire.send ((byte) (addr & 0xFF));  // low-order byte
    err = Wire.endTransmission ();
    if (err == 0) break;
  }
  return err;
} // end of writeEEPROM

byte readEEPROM (byte device, unsigned int addr, byte * data, byte len ) 
{
  byte err;
  byte counter;
  if (len > BUFFER_LENGTH)  return 0xFF;  // too long// 32 (in Wire.h)
  Wire.beginTransmission (device);
  Wire.send ((byte) (addr >> 8));    // high order byte
  Wire.send ((byte) (addr & 0xFF));  // low-order byte
  err = Wire.endTransmission ();
  if (err != 0)  return err;  // cannot read from device
  // initiate blocking read into internal buffer
  Wire.requestFrom (device, len);
  // pull data out of Wire buffer into our buffer
  for (counter = 0; counter < len; counter++)
  {
    if (Wire.available ()) 
      data [counter] = Wire.receive();
    else
      return 0xFE;  // did not get all bytes
  }
  return 0;  // OK
}  // end of readEEPROM

// read one byte from device, returns 0xFF on error
byte readEEPROM1 (byte device, unsigned int addr ) 
{
  byte temp;
  if (readEEPROM (device, addr, &temp, 1) == 0) return temp;
  return 0xFF;
}  // end of readEEPROM

// write one byte to device, returns non-zero on error
byte writeEEPROM1 (byte device, unsigned int addr, byte data ) 
{
  return writeEEPROM (device, addr, &data, 1);
} // end of writeEEPROM


void printDigits(int digits, int temp2){
  // utility function for digital clock display: prints preceding colon and leading 0
  if (temp2==1) lcd.print (":");
  if (temp2==2) lcd.print ("/");
  if(digits < 10) lcd.print ("0");
  lcd.print (digits);
}

void wireout (byte a1, byte a2, byte a3, byte a4){
  //i2c communincations
  Wire.beginTransmission(a1);
  Wire.send(a2);
  Wire.send(a3);
  Wire.send(a4);
  Wire.endTransmission();
  delay(50);
}

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  //  Serial.print("<<< reply ");
  //  Serial.print(millis() - timer);
  //  Serial.println(" ms");
  //  Serial.println((const char*) Ethernet::buffer + off);
}

static word homePage0() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\nE404"));
  return bfill.position();
}
static word homePage1() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/xml\r\nPragma: no-cache\r\n\r\n"
    "<RC>"
    "<TP1>$D</TP1><TP2>$D</TP2><TP3>$D</TP3><TP4>$D</TP4>"
    "<IFAN>$D</IFAN><ATO>$D</ATO><MLAMP>$D</MLAMP><LLAMP>$D</LLAMP><TFAN>$D</TFAN><HEAT>$D</HEAT>"
    "</RC>"),
  int(ClockBoardTempTP1*100),int(AmbiantNearBoxTP2*100),int(AmbiantNearTankTP3*100),int(TankTP4*100),
  !bitRead(RelayBanks, INTERNALFAN),!bitRead(RelayBanks, ATORELAY),
  !bitRead(RelayBanks, MAINLAMPS),!bitRead(RelayBanks, LEDLAMPS),
  !bitRead(RelayBanks, TANKFAN),!bitRead(RelayBanks, HEATER)  
    );
  return bfill.position(); 
}

static word homePage2() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"
    "<script language='javascript' src='http://www.yourwebaddres.com/rc/rc1.js'></script>"))  ;
  return bfill.position();
}

void  FillOutBuff (float temp1, int dstart, int dlen, int zeros){
  dtostrf(temp1,dlen,zeros,tbuff);
  for (int i=0+dstart; i <= dstart+dlen-1; i++){
    cbuff[i] = tbuff[i-dstart];
    if(cbuff[i] == ' ') cbuff[i] = '0';
  }  
}
