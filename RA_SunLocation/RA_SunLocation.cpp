// functions adapted and incorporated into a Class by Curt Binder for Reef Angel

//By Matthew Hockin 2012.
//SWFLTEK library functions, #includes, and #define were copied directly from the Epherma library
//Epherma (SWFLTEK.com) was written by Michael Rice- thanks Michael it works great.
//If you copy from this (its all open source) please
//acknowledge Michael for SWFLTEK library code (obviously labeled) or Matthew Hockin (for the rest).

#include "RA_SunLocation.h"

RA_SunLocation::RA_SunLocation()
{
  m_Latitude = ddToSeconds(-18.285833); // Great Barrier Reef, Australia GMT +10
  m_Longitude = ddToSeconds(147.699722); // Great Barrier Reef, Australia GMT +10

  m_UserRiseOffset = 0;
  m_UserSetOffset = 0;

  m_DayOfMonth = 0;

}

void RA_SunLocation::Init(long latitude, long longitude)
{
  m_Latitude = ddToSeconds(latitude);
  m_Longitude = ddToSeconds(longitude);
}

void RA_SunLocation::SetOffset(long risehour, long risesec, long sethour, long setsec)
{
  m_UserRiseOffset = (risehour * 3600) + risesec;
  m_UserSetOffset = (sethour * 3600) + setsec;
}

void RA_SunLocation::CheckAndUpdate()
{
  // check if we are in a new day and recalculate sunrise and sunset as necessary
  if (m_DayOfMonth != day())
  {
    CalSun();
    m_DayOfMonth = day();

//    Serial.print("Sunrise: ");
//    Serial.print(GetRiseHour());
//    Serial.print(":");
//    Serial.println(GetRiseMinute());
//    Serial.print("Sunset: ");
//    Serial.print(GetSetHour());
//    Serial.print(":");
//    Serial.println(GetSetMinute());
//    Serial.println("");
  }
}

// CalcSun function created by Matthew Hockin
void RA_SunLocation::CalSun()
{
  //Start of sunrise, sunset and cloud calculations- runs on reset and once a day thereafter.
  long newDay = now();

  newDay = newDay - (newDay % SECS_PER_DAY); // Go to midnight of today
  newDay -= SECS_YR_2000; //convert GMT unix Epoch to seconds elasped since 2000

  m_rise = newDay;
  m_set = newDay;

  //Calculate rise time and set time using Epherma Library functions (see end of code)
  SunRise (&m_rise); //call to Epherma function
  SunSet (&m_set); //Call to Epherma functionunsigned long newDay;

  m_rise += SECS_YR_2000 + m_UserRiseOffset;
  m_set += SECS_YR_2000 + m_UserSetOffset;

  // Convert rise/set to normal hours/minutes
  m_SunriseHour = hour(m_rise);
  m_SunriseMin = minute(m_rise);
  m_SunsetHour = hour(m_set);
  m_SunsetMin = minute(m_set);
}

char RA_SunLocation::CalcSunRise(unsigned long* when)
{
  *when -= SECS_YR_2000;
  SunRise(when);
  *when += SECS_YR_2000;

  return 0;
}
char RA_SunLocation::CalcSunSet(unsigned long* when)
{
  *when -= SECS_YR_2000;
  SunSet(when);
  *when += SECS_YR_2000;

  return 0;
}

long RA_SunLocation::ddToSeconds(long dd)
{
  // decimal degress
  return dd * 3600.0;
}

int RA_SunLocation::equation_of_time(unsigned long dt)
{
  /* ------------------------------------------------------------------------------------------------
   'Equation of Time'
   We use the 'short form equation, which has a theoretical accuracy of about 40 seconds.
   The returned value is in seconds.
   */
  double t;

  dt -= 192540UL; // refer to Jan 3 2000 05:29 (first periapsis)
  dt %= _tropical_year;
  t = dt;
  t /= _tropical_year;
  t *= 6.283185307179586;
  t = -459.27672 * sin(t) + 575.333472 * sin(2.0 * t + 3.588414);
  return t;
}

double RA_SunLocation::SolarDeclination(unsigned long dt)
{
  /* -----------------------------------------------------------------------------------------------
   'Solar Declination'
   Returns declination in radians
   Accurate to within 50 arc-seconds
   */
  double y;

  dt %= _tropical_year;
  y = dt;
  y /= _tropical_year; // fractional year
  y *= 6.283185307179586;
  y = 0.006918 - 0.399912 * cos(y) + 0.070257 * sin(y) - 0.006758 * cos(y * 2) + 0.000907 * sin(y * 2) - 0.002697 * cos(y * 3) + 0.00148 * sin(y * 3);
  return y;
}

unsigned long RA_SunLocation::daylightseconds(unsigned long dt)
{
  /* ------------------------------------------------------------------------------------------------
   Return the period between sunrise and sunset, in seconds.
   At high latitudes around the time of the solstices, this could be zero, or all day.
   */
  float l, d, e;
  long n;

  d = -SolarDeclination(dt); // will be positive in Northern winter
  l = m_Latitude / _sec_rad; // latitude in radians

  e += 60.0 * l * tan(l + d); // latitudinal error
  d = tan(l) * tan(d); //

  if (d > 1.0) return 86400UL;
  if (d < -1.0) return 0UL;

  d = acos(d);
  d /= _zenith;

  n = 86400UL * d;
  n += e;
  return n;
}

char RA_SunLocation::SunRiseSet(unsigned long * dt, char set)
{
  /* ------------------------------------------------------------------------------------------------
   Modify the passed time stamp to the time of sunrise (or sunset if 'set' is non-zero).
   Returns 0 to signal 'normal' completion. If the position is in a polar circle, 1 will be
   returned if the sun is above the horizon all day, and -1 if the sun is below the horizon
   all day.

   */
  unsigned long daylen;

  daylen = daylightseconds(*dt);
  if (daylen == 86400UL) return 1;   // there is no 'night' today (midnight sun)
  if (daylen == 0UL) return -1; // there is no 'day' today

  *dt /= 86400UL;
  *dt *= 86400UL;
  *dt += 43200UL; // set the time stamp to 12:00:00 GMT

  *dt -= daylen / 2; //        sunrise at the prime meridian
  if (set) *dt += daylen; //     sunset at the prime meridian

  *dt -= equation_of_time(*dt);

  *dt -= m_Longitude / 15.0; // rotate to our own meridian

  return 0;
}

char RA_SunLocation::SunRise(unsigned long* when)
{
  return SunRiseSet(when, 0);
}

char RA_SunLocation::SunSet(unsigned long* when)
{
  return SunRiseSet(when, 1);
}