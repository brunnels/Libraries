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

#include <Globals.h>
#include <Time.h>
#include <ReefAngel.h>
#include "RA_NokiaLCD.h"
#ifdef wifi
#include <RA_Wifi.h>
#endif  // wifi
#include <Wire.h>
#include <InternalEEPROM.h>
#include <Memory.h>
#include <avr/pgmspace.h>
#if defined WDT || defined WDT_FORCE
#include <avr/wdt.h>
#endif  // defined WDT || defined WDT_FORCE

RA_NokiaLCD::RA_NokiaLCD()
{
  LCDID=255;
  DDRE = ((1 << CS) | (1 << SDATA)| (1 << BL)); //Set CS, SDATA, and BL pins on PORTE as outputs
  DDRG = (1 << CLK);                           //Set SDATA pin on PORTG as output
  DDRH = (1 << RESET);                          //Set RESET pin on PORTB as output
}

void RA_NokiaLCD::ShiftBits(byte b)
{
    byte Bit;

    for (Bit = 0; Bit < 8; Bit++)     // 8 Bit Write
    {
        CLK0          // Standby SCLK
        if((b&0x80)>>7)
        {
            SDA1
        }
        else
        {
            SDA0
        }
        CLK1          // Strobe signal bit
        b <<= 1;   // Next bit data
    }
}

void RA_NokiaLCD::SendData(byte data)
{
    CLK0
    SDA1
    CLK1
    ShiftBits(data);
#ifdef wifi
    pingSerial();
#endif  // wifi
}

void RA_NokiaLCD::SendCMD(byte data)
{
    CLK0
    SDA0
    CLK1
    ShiftBits(data);
}


void RA_NokiaLCD::SendColor12Bit(byte color)
{
	int R=(color&0xE0)>>5;
	if (R>=4) R=((R*2)+1); else R=R<<1;
	int G=(color&0x1C)>>2;
	if (G>=4) G=((G*2)+1)<<4; else G=G<<5;
	int B=(color&0x03)*5;
	SendData(R);
	SendData(G+B);
}

void RA_NokiaLCD::SetBox(byte x1, byte y1, byte x2, byte y2)
{
#if defined WDT || defined WDT_FORCE
	wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
	if (LCDID==0)
	{
		SendCMD(0x42);
		SendCMD(y1);     // for some reason starts at 2
		SendCMD(y2);

	    SendCMD(0x43);   // column start/end ram
	    SendCMD(x1);
	    SendCMD(x2);
	}
	else
	{
	    SendCMD(CASET);   // page start/end ram
	    SendData(x1);     // for some reason starts at 2
	    SendData(x2);

	    SendCMD(PASET);   // column start/end ram
	    SendData(y1);
	    SendData(y2);
	}
}

void RA_NokiaLCD::Clear(byte color, byte x1, byte y1, byte x2, byte y2)
{
    uint8_t xmin, xmax, ymin, ymax;
    uint16_t i;

    // best way to create a filled rectangle is to define a drawing box
    // and loop two pixels at a time
    // calculate the min and max for x and y directions
    xmin = (x1 <= x2) ? x1 : x2;
    xmax = (x1 > x2) ? x1 : x2;
    ymin = (y1 <= y2) ? y1 : y2;
    ymax = (y1 > y2) ? y1 : y2;

    SetBox(xmin,ymin,xmax,ymax);

    // WRITE MEMORY
	if (LCDID!=0) SendCMD(RAMWR);

    // loop on total number of pixels / 2
    for (i = 0; i<(xmax - xmin + 1); i++)
    {
        // use the color value to output three data bytes covering two pixels
        // For some reason, it has to send blue first then green and red
        //SendData((color << 4) | ((color & 0xF0) >> 4));
        //SendData(((color >> 4) & 0xF0) | (color & 0x0F));
        //SendData((color & 0xF0) | (color >> 8));
#if defined WDT || defined WDT_FORCE
	wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
    	for (int j=0; j<(ymax - ymin + 1); j++)
    	{
			if (LCDID==0)
				SendColor12Bit(color);
			else
				SendData(~color);
    	}
    }
}

void RA_NokiaLCD::Init()
{
    // Initial state
    CS1
    CS0

    // Hardware Reset LCD
    RESET0
    delay(100);
    RESET1
    delay(100);

    if (LCDID==0)
    {
        for (int a=0;a<SIZE(init_code_S6B33B);a++)
        	SendCMD(pgm_read_byte_near(init_code_S6B33B + a));
        Clear(DefaultBGColor,2,2,129,129);
    }
    else
    {
        for (int a=0;a<SIZE(init_code_Nokia);a++)
        	SendCMD(pgm_read_byte_near(init_code_Nokia + a));

        SendCMD(SETCON);
        SendData(0x38);

        //Memory data access control
        SendCMD(MADCTL);

        //SendData(8|64);   //rgb + MirrorX
        //SendData(8|128);   //rgb + MirrorY
        SendData(0xc0);

        SendCMD(COLMOD);
        SendData(2);   //16-Bit per Pixel
        Clear(DefaultBGColor,0,0,131,131);
    }


}

void RA_NokiaLCD::Sleep()
{
    SendCMD(DISPOFF);
    SendCMD(SLEEPIN);
    BacklightOff();
}

void RA_NokiaLCD::Wake()
{
    BacklightOn();
    SendCMD(SLEEPOUT);
    SendCMD(DISPON);
}

void RA_NokiaLCD::BacklightOn()
{
    BL1
}

void RA_NokiaLCD::BacklightOff()
{
    BL0
}

#if defined FONT_8x8 || defined FONT_8x16 || defined FONT_12x16 || defined NUMBERS_8x8 || defined NUMBERS_8x16 || defined NUMBERS_12x16
void RA_NokiaLCD::DrawLargeTextLine(byte fcolor, byte bcolor, byte x, byte y, uint16_t c, byte height)
{
	int i;
	byte inc;
	byte xoffset;
	switch ( height )
	{
		default:
		case Num8x8:
		case Font8x8:
		{
			inc = 7;
			xoffset = 10;
		}
		break;
		case Num8x16:
		case Font8x16:
		{
			inc = 15;
			xoffset = 8;
		}
		break;
		case Num12x16:
		case Font12x16:
		{
			inc = 15;
			xoffset = 15;
		}
		break;
	}
	SetBox(x,y,x+xoffset,y);
	SendCMD(RAMWR);
	for(i=inc;i>=0;i--)
	{
		if (1<<i & c)
			if (LCDID==0) SendColor12Bit(fcolor); else SendData(~fcolor);
		else
			if (LCDID==0) SendColor12Bit(bcolor); else SendData(~bcolor);
	}
}
#endif  // FONT_8x8 || FONT_8x16 || FONT_12x16 || NUMBERS_8x8 || NUMBERS_8x16 || NUMBERS_12x16

#if defined FONT_8x8 || defined FONT_8x16 || defined NUMBERS_8x8 || defined NUMBERS_8x16
void RA_NokiaLCD::DrawLargeText(byte fcolor, byte bcolor, byte x, byte y, char* text, byte height /*= Font8x8*/)
{
	uint16_t c;
	int t;
	const prog_uchar *f = NULL;
	byte w;
	byte y_w;
	byte x_offset = 1;
	byte char_offset = 32;
	switch ( height )
	{
		default:
#ifdef FONT_8x8
		case Font8x8:
		{
			w = 8;
			y_w = 8;
			f = font_8x8;
			break;
		}
#endif  // FONT_8x8
#ifdef NUMBERS_8x8
		case Num8x8:
		{
			char_offset = 46;
			w = 8;
			y_w = 8;
			f = num_8x8;
			break;
		}
#endif  // NUM_8x8
#ifdef FONT_8x16
		case Font8x16:
		{
			w = 16;
			y_w = 16;
			x_offset = 0;
			f = font_8x16;
			break;
		}
#endif  // FONT_8x16
#ifdef NUMBERS_8x16
		case Num8x16:
		{
			char_offset = 46;
			w = 16;
			y_w = 16;
			x_offset = 0;
			f = num_8x16;
			break;
		}
#endif  // NUM_8x16
	}  // height

	while(*text != 0)
	{
		t=*text;
		t-=char_offset;
		t*=w;
		for(int j = t; j < t+w; j++)
		{
			c = pgm_read_byte_near(f + j);
			DrawLargeTextLine(fcolor, bcolor, x-x_offset, y++, c, height);
		}
		text++;
		y-=y_w;
		x+=8;
	}
}
#endif  // FONT_8x8 || FONT_8x16

#if defined FONT_12x16 || defined NUMBERS_12x16
void RA_NokiaLCD::DrawHugeText(byte fcolor, byte bcolor, byte x, byte y, char* text, byte height /*= Font12x16*/)
{
	uint16_t c;
	int t;
	byte w = 16;
	byte x_w = 16;
	byte y_w = 16;
	byte char_offset = 32;
	const prog_uint16_t *f = NULL;

#ifdef FONT_12x16
	// set the font to be 12x16 unless overridden later
	f = font_12x16;
#endif  // FONT_12x16

	if ( height == Num12x16 )
	{
#ifdef NUMBERS_12x16
		f = num_12x16;
		char_offset = 46;
#endif  // NUMBERS_12x16
	}

	while(*text != 0)
	{
		t=*text;
		t-=char_offset;
		t*=w;
		for(int j = t; j < t+w; j++)
		{
			c = pgm_read_word_near(f + j);
			DrawLargeTextLine(fcolor, bcolor, x-5, y++, c, height);
		}
		text++;
		y-=y_w;
		x+=x_w;
	}
}
#endif  // FONT_12x16 || NUMBERS_12x16

#ifdef NUMBERS_16x16
void RA_NokiaLCD::DrawHugeNumbersLine(byte fcolor, byte bcolor, byte x, byte y, uint16_t c)
{
	int i;
	SetBox(x,y,x,y+15);
	SendCMD(RAMWR);
	for(i=0;i<16;i++)
	{
		if (1<<i & c)
			if (LCDID==0) SendColor12Bit(fcolor); else SendData(~fcolor);
		else
			if (LCDID==0) SendColor12Bit(bcolor); else SendData(~bcolor);
	}
}

void RA_NokiaLCD::DrawHugeNumbers(byte fcolor, byte bcolor, byte x, byte y, char* text)
{
	uint16_t c;
	int t;
	while(*text != 0)
	{
		t=*text;
		t-=46;
		t*=16;
		for(int j = t; j < t+16; j++)
		{
			c = pgm_read_word_near(num_16x16 + j);
			DrawHugeNumbersLine(fcolor, bcolor, x++, y, c);
		}
		text++;
	}
}
#endif  // NUMBERS_16x16

void RA_NokiaLCD::DrawTextLine(byte fcolor, byte bcolor, byte x, byte y,char c)
{
    byte i;
    SetBox(x,y,x,y+7);
    if (LCDID!=0) SendCMD(RAMWR);
    for(i=0;i<8;i++)
    {
		if (1<<i & c)
			if (LCDID==0) SendColor12Bit(fcolor); else SendData(~fcolor);
		else
			if (LCDID==0) SendColor12Bit(bcolor); else SendData(~bcolor);
    }
}

void RA_NokiaLCD::DrawText(byte fcolor, byte bcolor, byte x, byte y, char *text)
{
    byte c;
    int t;
	int j;
    while(*text != 0)
    {
        t = *text;
        t -= 32;
        t *= 5;
        for(j = t; j < t+5; j++)
        {
            c = pgm_read_byte_near(font + j);
            DrawTextLine(fcolor, bcolor, x++, y, c);
        }
        DrawTextLine(fcolor, bcolor, x++, y, 0);
        text++;
    }
}

void RA_NokiaLCD::DrawText(byte fcolor, byte bcolor, byte x, byte y,int text)
{
	char temp[6];
	itoa(text,temp,10);
	DrawText(fcolor, bcolor, x, y,temp);
}

void RA_NokiaLCD::DrawText(byte fcolor, byte bcolor, byte x, byte y,byte text)
{
	char temp[6];
	itoa(text,temp,10);
	DrawText(fcolor, bcolor, x, y,temp);
}

void RA_NokiaLCD::DrawText(byte fcolor, byte bcolor, byte x, byte y,long text)
{
	char temp[20];
	ltoa(text,temp,10);
	DrawText(fcolor, bcolor, x, y,temp);
}

void RA_NokiaLCD::PutPixel(byte color, byte x, byte y)
{
	SetBox(x,y,x+1,y+1);
	if (LCDID==0)
	{
    	SendColor12Bit(color);
	}
	else
	{
		SendCMD(RAMWR);
		SendData(~color);
	}
}

void RA_NokiaLCD::SetContrast(byte Contrast)
{
    SendCMD(SETCON);
    SendData(Contrast);
}

void RA_NokiaLCD::DrawCircleOutline(byte x, byte y, byte radius, byte bordercolor)
{
	// 3/21/12 - Original code from Jon (jsclownfish)
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	byte x_pos = 0;
	byte y_pos = radius;

	PutPixel(bordercolor, x, y+radius);
	PutPixel(bordercolor, x, y-radius);
	PutPixel(bordercolor, x+radius, y);
	PutPixel(bordercolor, x-radius, y);

	while ( x_pos < y_pos )
	{
		if ( f >= 0 )
		{
			y_pos--;
			ddF_y += 2;
			f += ddF_y;
		}
		x_pos++;
		ddF_x += 2;
		f += ddF_x;

		PutPixel(bordercolor, x + x_pos, y + y_pos);
		PutPixel(bordercolor, x - x_pos, y + y_pos);
		PutPixel(bordercolor, x + x_pos, y - y_pos);
		PutPixel(bordercolor, x - x_pos, y - y_pos);
		PutPixel(bordercolor, x + y_pos, y + x_pos);
		PutPixel(bordercolor, x - y_pos, y + x_pos);
		PutPixel(bordercolor, x + y_pos, y - x_pos);
		PutPixel(bordercolor, x - y_pos, y - x_pos);
	}
}

void RA_NokiaLCD::FillCircle(byte x, byte y, byte radius, byte fillcolor)
{
	// 3/21/12 - Original code from Jon (jsclownfish)
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	byte x_pos = 0;
	byte y_pos = radius;
	byte i;

	for ( i = y-radius; i <= y+radius; i++ )
	{
		PutPixel(fillcolor, x, i);
	}
	while ( x_pos < y_pos )
	{
		if ( f >= 0 )
		{
			y_pos--;
			ddF_y += 2;
			f += ddF_y;
		}
		x_pos++;
		ddF_x += 2;
		f += ddF_x;
		for ( i = y-y_pos; i <= y+y_pos; i++ )
		{
			PutPixel(fillcolor, x+x_pos, i);
			PutPixel(fillcolor, x-x_pos, i);
		}
		for ( i = y-x_pos; i <= y+x_pos; i++ )
		{
			PutPixel(fillcolor, x+y_pos, i);
			PutPixel(fillcolor, x-y_pos, i);
		}
	}
}

void RA_NokiaLCD::DrawCircleOutletBox(byte x, byte y, byte RelayData, bool reverse /*= false*/)
{
	// 3/21/12 - Original code from Jon (jsclownfish)
	byte a = 0;
	byte b = 0;
	byte c = 0;
	for (a=0;a<2;a++)
	{
		// 0 & 1
		if ( reverse )
			c = 1 - a;
		else
			c = a;
		DrawCircleOutline((a*10)+x,y,5,OutletBorderColor);
		if ((RelayData&(1<<c))==1<<c)
		{
			FillCircle((a*10)+x,y,3,OutletOnBGColor);
		}
		else
		{
			FillCircle((a*10)+x,y,3,OutletOffBGColor);
		}
	}
	for (a=2;a<4;a++)
	{
		// 2 & 3
		if ( reverse )
			c = 3 - (a-2);
		else
			c = a;
		b=(a-2)*10;
		DrawCircleOutline(b+x,y+10,5,OutletBorderColor);
		if ((RelayData&(1<<c))==1<<c)
		{
			FillCircle(b+x,y+10,3,OutletOnBGColor);
		}
		else
		{
			FillCircle(b+x,y+10,3,OutletOffBGColor);
		}
	}
	for (a=4;a<6;a++)
	{
		// 4 & 5
		if ( reverse )
			c = 5 - (a-4);
		else
			c = a;
		b=(a-4)*10;
		DrawCircleOutline(b+x,y+20,5,OutletBorderColor);
		if ((RelayData&(1<<c))==1<<c)
		{
			FillCircle(b+x,y+20,3,OutletOnBGColor);
		}
		else
		{
			FillCircle(b+x,y+20,3,OutletOffBGColor);
		}
	}
	for (a=6;a<8;a++)
	{
		// 6 & 7
		if ( reverse )
			c = 7 - (a-6);
		else
			c = a;
		b=(a-6)*10;
		DrawCircleOutline(b+x,y+30,5,OutletBorderColor);
		if ((RelayData&(1<<c))==1<<c)
		{
			FillCircle(b+x,y+30,3,OutletOnBGColor);
		}
		else
		{
			FillCircle(b+x,y+30,3,OutletOffBGColor);
		}
	}
}

void RA_NokiaLCD::DrawCircleOutletBoxHorizontal(byte x, byte y, byte RelayData)
{
   // altered version of DrawCircleOutletBox by JerkyJunky
   // designed for a layout where the plugs are aligned like:
   // 8 6 4 2
   // 7 5 3 1
   
   //Variables:
   byte offset = 5;      //distance between given x,y location and the center of first circle
   byte a = 0;            //our counter of relay port
   byte b = 3;            //horizontal column counter
   byte c = 1;            //vertical row counter
   
   //Main Loop - Starting at port 1 work our way up and left
   for (a=0;a<8;a++)
   {
      DrawCircleOutline((b*10) + x + offset, (c*10) + y + offset, 5, OutletBorderColor);
      //check if relay is active and color appropriately
      if ((RelayData&(1<<a))==1<<a)
      {
         FillCircle((b*10) + x + offset, (c*10) + y + offset, 3, OutletOnBGColor);
      }
      else
      {
         FillCircle((b*10) + x + offset, (c*10) + y + offset, 3, OutletOffBGColor);
      }
      //adjust counters for next circle (up and left)
      if (c == 0)
      {
         c = 1;
         b = b - 1;
      }
      else
      {
         c = c - 1;
      }
   }
}

void RA_NokiaLCD::DrawDate(byte x, byte y)
{
    //byte iTimeHourOffset=0;
    char text[21];
    char temp[]="  ";
    strcpy(text,"");
    itoa(month(),temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    strcat(text,"/");
    itoa(day(),temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    strcat(text,"/");
    itoa(year()-2000,temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    strcat(text," ");
    //if (iTimeHour>12) iTimeHourOffset=12;
    itoa(hourFormat12(),temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    strcat(text,":");
    itoa(minute(),temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    strcat(text,":");
    itoa(second(),temp,10);
    if (temp[1]==0) strcat(text,"0");
    strcat(text,temp);
    if (isAM())
    {
        strcat(text," AM");
    }
    else
    {
        strcat(text," PM");
    }
    DrawText(DateTextColor, DefaultBGColor, x, y, text);
}

#if defined(DATETIME24)
// Draw date and time according to ISO 8601
// Example: 2012-05-25 21:48:43
void RA_NokiaLCD::DrawDateTimeISO8601(byte x, byte y)
{
    char text[20];
    char temp2[]="  ";
    char temp4[]="    ";
    strcpy(text,"");
	
	// Date
	itoa(year(),temp4,10);
    strcat(text,temp4);
    strcat(text,"-");
	
    itoa(month(),temp2,10);
    if (temp2[1]==0) strcat(text,"0");
    strcat(text,temp2);
    strcat(text,"-");
	
    itoa(day(),temp2,10);
    if (temp2[1]==0) strcat(text,"0");
    strcat(text,temp2);
    strcat(text," ");
	
	// Time
    itoa(hour(),temp2,10);
    if (temp2[1]==0) strcat(text,"0");
    strcat(text,temp2);
    strcat(text,":");
	
    itoa(minute(),temp2,10);
    if (temp2[1]==0) strcat(text,"0");
    strcat(text,temp2);
    strcat(text,":");
	
    itoa(second(),temp2,10);
    if (temp2[1]==0) strcat(text,"0");
    strcat(text,temp2);
    
    DrawText(DateTextColor, DefaultBGColor, x, y, text);
}
#endif // DATETIME24

void RA_NokiaLCD::DrawOutletBox(byte x, byte y,byte RelayData)
{
    Clear(OutletBorderColor,x,y,x+104,y);  //94
    Clear(OutletBorderColor,x,y+12,x+104,y+12);
    for (byte a=0;a<8;a++)
    {
        byte bcolor = OutletOffBGColor;
        byte fcolor = OutletOffFGColor;
        char temp[]="  ";
        if ((RelayData&(1<<a))==1<<a)
        {
            bcolor = OutletOnBGColor;
            fcolor = OutletOnFGColor;
        }
        Clear(bcolor,x+(a*13),y+1,x+13+(a*13),y+11);
        itoa(a+1,temp,10);
        DrawText(fcolor,bcolor,x+4+(a*13),y+3,temp);
    }
}

void RA_NokiaLCD::DrawSingleMonitor(int Temp, byte fcolor, byte x, byte y, byte decimal)
{
	char text[7];
	if ( Temp == 0xFFFF ) Temp = 0;
	if (( Temp == 0 ) && ( decimal > 1 ))
	{
		strcpy(text, "Error");
	}
	else
	{
		ConvertNumToString(text, Temp, decimal);
	}
    Clear(DefaultBGColor,x,y,x+30,y+8);
    DrawText(fcolor,DefaultBGColor,x,y,text);
}
void RA_NokiaLCD::DrawSingleMonitorAlarm(int Temp, byte fcolor, byte x, byte y, byte decimal, int high, int low, byte warn_color)
{
  int mod=second()%2;
  if (Temp > high || Temp < low) 
  {
    if (mod==0) 
    {
      Clear(DefaultBGColor,x,y,x+40,y+10);
    }
    else 
    {
      DrawSingleMonitor(Temp, warn_color, x, y, decimal);
    }
  }
  else 
  {
    DrawSingleMonitor(Temp, fcolor, x, y, decimal);
  }
}

#if defined DisplayLEDPWM && ! defined RemoveAllLights
void RA_NokiaLCD::DrawMonitor(byte x, byte y, ParamsStruct Params, byte DaylightPWMValue, byte ActinicPWMValue)
#else  // defined DisplayLEDPWM && ! defined RemoveAllLights
void RA_NokiaLCD::DrawMonitor(byte x, byte y, ParamsStruct Params)
#endif  // defined DisplayLEDPWM && ! defined RemoveAllLights
{
    DrawText(T1TempColor,DefaultBGColor,x,y,"T1:");
    DrawSingleMonitor(Params.Temp[T1_PROBE], T1TempColor, x+18, y,10);
    DrawText(T2TempColor,DefaultBGColor,x,y+10,"T2:");
    DrawSingleMonitor(Params.Temp[T2_PROBE], T2TempColor, x+18, y+10,10);
    DrawText(T3TempColor,DefaultBGColor,x,y+20,"T3:");
    DrawSingleMonitor(Params.Temp[T3_PROBE], T3TempColor, x+18, y+20,10);
    DrawText(PHColor,DefaultBGColor,x+60,y,"PH:");
    DrawSingleMonitor(Params.PH, PHColor, x+78, y,100);
#if defined DisplayLEDPWM && ! defined RemoveAllLights
    DrawText(DPColor,DefaultBGColor,x+60,y+10,"DP:");
    DrawSingleMonitor(DaylightPWMValue, DPColor, x+78, y+10,1);
    DrawText(APColor,DefaultBGColor,x+60,y+20,"AP:");
    DrawSingleMonitor(ActinicPWMValue, APColor, x+78, y+20,1);
#endif  // defined DisplayLEDPWM && ! defined RemoveAllLights
}

void RA_NokiaLCD::DrawSingleGraph(byte color, byte x, byte y, int EEaddr)
{
	int start;
#if defined WDT || defined WDT_FORCE
	wdt_reset();
#endif  // defined WDT || defined WDT_FORCE
	for (byte a=0;a<120;a++)
	{
		start=EEaddr+a;
		if (start > (int(EEaddr/120)+1)*120) start=start-120;
//		Wire.beginTransmission(I2CAddr);
//		Wire.print((int)(start >> 8));   // MSB
//		Wire.print((int)(start & 0xFF)); // LSB
//		Wire.endTransmission();
//		Wire.requestFrom(I2CAddr,1);
//		if (Wire.available()) PutPixel(color,x+a,y+50-Wire.read());
        PutPixel(color, x+a, y+50-Memory.Read(start));
	}

}

void RA_NokiaLCD::DrawEEPromImage(int swidth, int sheight, byte x, byte y, int I2CAddr, int EEaddr)
{
    int count = 0;
    SetBox(x,y,swidth-1+x,sheight-1+y);
    SendCMD(0x2c);

    do
    {
        Wire.beginTransmission(I2CAddr);
        Wire.write((int)(EEaddr+count >> 8));   // MSB
        Wire.write((int)(EEaddr+count & 0xFF)); // LSB
        Wire.endTransmission();
        Wire.requestFrom(I2CAddr,30);
        for (byte j = 0; j < 30; j++)
        {
            count+=1;
            if ((count<=swidth*sheight) && Wire.available()) SendData(~Wire.read());
        }
    }
    while (count < swidth*sheight);
}

void RA_NokiaLCD::DrawImage(int swidth, int sheight, byte x, byte y, const prog_uchar *iPtr)
{
    int count = 0;
    SetBox(x,y,swidth-1+x,sheight-1+y);
    SendCMD(0x2c);

    do
    {
        for (byte j = 0; j < 30; j++)
        {
            count+=1;
            if (count<=swidth*sheight) SendData(~pgm_read_byte_near(iPtr++));
        }
    }
    while (count < swidth*sheight);
}

void RA_NokiaLCD::DrawGraph(byte x, byte y)
{
    // Draws the main screen graph
    Clear(DefaultBGColor,0,y,131,y+50);
    Clear(DefaultFGColor,x,y,x,y+50);
    for (byte i=6; i<=131; i+=3)
    {
        PutPixel(GraphDotLineColor, i, y+25);
    }
    DrawSingleGraph(T1TempColor,x,y,InternalMemory.T1Pointer_read());
    DrawSingleGraph(T2TempColor,x,y,InternalMemory.T1Pointer_read()+120);
    DrawSingleGraph(T3TempColor,x,y,InternalMemory.T1Pointer_read()+240);
    DrawSingleGraph(PHColor,x,y,InternalMemory.T1Pointer_read()+360);
}

void RA_NokiaLCD::DrawOption(int Option, byte Selected, byte x, byte y, char *unit, char *subunit, byte maxdigits)
{
    byte x1,x2=0,x3=0;
    char text[10]="";
    char temp[10]="";
    byte bcolor, fcolor, width;
    byte offset[6] = {0, 6, 6, 12, 18, 24};

    itoa(Option,text,10);
    if (Option >= 10000)
    {
        x1 = x+24;
    }
    else if (Option > 999)
    {
        x1 = x+18;
    }
    else if (Option > 99)
    {
        x1 = x+12;
    }
    else if (Option < 100 && Option > 9)
    {
        x1 = x+6;
    }
    else if (Option < 10)
    {
        x1 = x;
        if ( (strcmp(unit,"")==0) && (strcmp(subunit,"")==0) )
        {
            text[0]=0x30;
            itoa(Option,temp,10);
            strcat(text,temp);
            x1 = x+6;
            //      Option=10; //Just to offset the prefix "0" when # is < 10
        }
    }
    x2 = x1;
    width = offset[maxdigits]+5;
    if ( strcmp(unit,"") !=0 )
    {
        x2 += 8;
        width += 8;
    }
    if ( strcmp(subunit,"") !=0 )
    {
        x3 = x2;
        // if there is no unit, move the subunit over
        if ( strcmp(unit,"") == 0 )
        {
            x3 += 5;
        }
        x2 += 5;
        width += 5;
    }
    bcolor = DefaultBGColor;
    fcolor = DefaultFGColor;
    Clear(DefaultBGColor, x-1, y-8, x+width, y+15);
    if (Selected)
    {
        bcolor = SelectionBGColor;
        fcolor = SelectionFGColor;
        DrawText(DefaultFGColor,DefaultBGColor, x+((x2-x-12)/2), y-8, " ^ ");
        DrawText(DefaultFGColor,DefaultBGColor, x+((x2-x-12)/2), y+8, " ` ");
    }

    // should always print text at X and clear to X1
    Clear(bcolor,x-1,y-2,x2+5,y+8);
    DrawText(fcolor,bcolor,x,y,text);
    DrawText(fcolor,bcolor,x3,y-5,subunit);
    DrawText(fcolor,bcolor,x2,y,unit);
    if ( strcmp(subunit,"") != 0 )
    {
        Clear(DefaultBGColor, x1-2, y-5, x2+6, y-3);
    }
}

void RA_NokiaLCD::DrawCancel(bool Selected)
{
    byte bcolor;
    Clear(BtnBorderColor,14,109,59,126);
    if ( Selected )
    {
        bcolor = BtnActiveColor;
    }
    else
    {
        //bcolor = COLOR_LIGHTSTEELBLUE;
        bcolor = BtnInactiveColor;
    }
    Clear(bcolor,15,110,58,125);
    DrawText(DefaultFGColor,bcolor,20,115,"Cancel");
}

void RA_NokiaLCD::DrawOK(bool Selected)
{
    byte bcolor;
    Clear(BtnBorderColor,74,109,119,126);
    if ( Selected )
    {
        bcolor = BtnActiveColor;
    }
    else
    {
        //bcolor = COLOR_LIGHTSTEELBLUE;
        bcolor = BtnInactiveColor;
    }
    Clear(bcolor,75,110,118,125);
    DrawText(DefaultFGColor,bcolor,92,115,"Ok");
}

void RA_NokiaLCD::DrawCalibrate(int i, byte x, byte y)
{
  char text[10] = {0};
  Clear(DefaultBGColor, x, y, x+20, y+10);
  itoa(i,text,10);
  strcat(text , "   ");
  DrawText(CalibrateColor, DefaultBGColor, x, y, text);
}
