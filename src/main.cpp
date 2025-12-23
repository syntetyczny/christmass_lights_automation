#include <Arduino.h>
#include <Dusk2Dawn.h>
#include <WiFi.h>
#include <time.h>
#include <arduino-timer.h>
#include "../../../../PlatformIO/Projects/password_wifi.h"    

/* Configuration of NTP */
const char MY_NTP_SERVER[] = "pl.pool.ntp.org";
// https://remotemonitoringsystems.ca/time-zone-abbreviations.php           
const char MY_TZ[] =  "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  

/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;
auto timer = timer_create_default(); // create a timer with default settings
uint32_t what_time_is_it;
uint32_t rise_time_is;
uint32_t set_time_is;
int ElSunrise;
int ElSunset;

char set_time[] = "00:00";
char rise_time[] = "00:00";

bool christmass_lights_on(void *)
{
 tm.tm_min++;
 if (tm.tm_min > 59)
 {
  tm.tm_hour++;
  tm.tm_min = 0;
  if(tm.tm_hour > 23) 
  {
        // start network
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print ( "." );
    }
    Serial.println("\nWiFi connected");
    // Set the timezone environment variable
    setenv("TZ", MY_TZ, 1);
    tzset(); // Apply the timezone setting

  // Configure NTP (UTC offset is set to 0 as TZ handles it)
    configTime( 0, 0, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time
    Dusk2Dawn::min2str(rise_time, ElSunrise);
    Dusk2Dawn::min2str(set_time, ElSunset);

    rise_time_is = (((uint32_t)rise_time[0] - (uint32_t)'0')*10)*60+
                (((uint32_t)rise_time[1] - (uint32_t)'0'))*60+
                (((uint32_t)rise_time[3] - (uint32_t)'0'))*10+
                (((uint32_t)rise_time[4] - (uint32_t)'0'));
  
    set_time_is  = (((uint32_t)set_time[0] - (uint32_t)'0')*10)*60+
                  (((uint32_t)set_time[1] - (uint32_t)'0'))*60+
                  (((uint32_t)set_time[3] - (uint32_t)'0'))*10+
                  (((uint32_t)set_time[4] - (uint32_t)'0'));

    what_time_is_it = (uint32_t)tm.tm_hour*60+(uint32_t)tm.tm_min;

  }
 }
  
  what_time_is_it = (uint32_t)tm.tm_hour*60+(uint32_t)tm.tm_min;
#ifdef DEBUG
  Serial.printf("|=======================================|\n\r");
  Serial.printf("What time is it %d\n\r", what_time_is_it);
  Serial.printf("Rise time is    %d\n\r", rise_time_is);
  Serial.printf("Set  time is    %d\n\r", set_time_is);
#endif
  return true;
}

void setup() {
  pinMode(D5, OUTPUT);
  Serial.begin (115200);
  
  // Set the timezone environment variable
  setenv("TZ", MY_TZ, 1);
  tzset(); // Apply the timezone setting

  // Configure NTP (UTC offset is set to 0 as TZ handles it)
  configTime( 0, 0, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ( "." );
  }
  Serial.println("\nWiFi connected");
  // by default, the NTP will be started after 60 secs

  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
  Serial.printf("\nTime is %u", now);
  // Multiple instances can be created. Arguments are longitude, latitude, and
  // time zone offset in hours from UTC.
  //
  // The first two must be in decimal degrees (DD), e.g. 10.001, versus the more
  // common degrees, minutes, and seconds format (DMS), e.g. 10° 00′ 3.6″. The
  // time zone offset can be expressed in decimal fractions, e.g. "5.75" for
  // Nepal Standard Time, in the few cases where the zones are offset by 30 or
  // 45 minutes.
  //
  // HINT: An easy way to find the longitude and latitude for any arbitrary
  // location is to find the spot in Google Maps, right click the place on the
  // map, and select "What's here?". At the bottom, you’ll see a card with the
  // coordinates.

  // https://latitudelongitude.org/pl/elblag/
  Dusk2Dawn Elblag(54.152, 19.408, 1);


  // Available methods are sunrise() and sunset(). Arguments are year, month,
  // day, and if Daylight Saving Time is in effect.

  ElSunrise  = Elblag.sunrise(tm.tm_year + 1900, //year since 1900
                                      tm.tm_mon + 1, // January = 0 (!)
                                         tm.tm_mday, // day of month
                                              false);

  ElSunset  = Elblag.sunset(tm.tm_year + 1900, //year since 1900
                                    tm.tm_mon + 1, // January = 0 (!)
                                       tm.tm_mday, // day of month
                                            false);

  // A static method converts the returned time to a 24-hour clock format.
  // Arguments are a character array and time in minutes.
  Dusk2Dawn::min2str(rise_time, ElSunrise);
  Dusk2Dawn::min2str(set_time, ElSunset);

  what_time_is_it = (uint32_t)tm.tm_hour*60+(uint32_t)tm.tm_min;

  rise_time_is = (((uint32_t)rise_time[0] - (uint32_t)'0')*10)*60+
                 (((uint32_t)rise_time[1] - (uint32_t)'0'))*60+
                 (((uint32_t)rise_time[3] - (uint32_t)'0'))*10+
                 (((uint32_t)rise_time[4] - (uint32_t)'0'));
  
  set_time_is  = (((uint32_t)set_time[0] - (uint32_t)'0')*10)*60+
                 (((uint32_t)set_time[1] - (uint32_t)'0'))*60+
                 (((uint32_t)set_time[3] - (uint32_t)'0'))*10+
                 (((uint32_t)set_time[4] - (uint32_t)'0'));

  timer.every(60*1000, christmass_lights_on);
}


void loop()
{
  timer.tick(); // tick the timer
 
  if((what_time_is_it>=rise_time_is)&&(what_time_is_it<=set_time_is))
  {
    digitalWrite(D5,false);
  }else
  {
    digitalWrite(D5,true);
  }
}
