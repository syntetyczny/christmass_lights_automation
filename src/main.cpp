#include <Arduino.h>
#include <Dusk2Dawn.h>
#include <ESP8266WiFi.h>            // we need wifi to get internet access
#include <time.h>
#include "../../../../PlatformIO/Projects/password_wifi.h"    

/* Configuration of NTP */
#define MY_NTP_SERVER "pl.pool.ntp.org"
// https://remotemonitoringsystems.ca/time-zone-abbreviations.php           
#define MY_TZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"   

/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;

void setup() {
  Serial.begin (115200);
  
  Serial.println("\nNTP TZ DST - bare minimum");

  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

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

  int ElSunrise  = Elblag.sunrise(tm.tm_year + 1900, //year since 1900
                                      tm.tm_mon + 1, // January = 0 (!)
                                         tm.tm_mday, // day of month
                                              false);

  int ElSunset  = Elblag.sunset(tm.tm_year + 1900, //year since 1900
                                    tm.tm_mon + 1, // January = 0 (!)
                                       tm.tm_mday, // day of month
                                            false);

  Serial.print("\thour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min); 

  // A static method converts the returned time to a 24-hour clock format.
  // Arguments are a character array and time in minutes.
  char time[] = "00:00";
  Dusk2Dawn::min2str(time, ElSunrise);
  Serial.println("Sun Rise: ");
  Serial.println(time); // 07:47
  
  Dusk2Dawn::min2str(time, ElSunset);
  Serial.println("Sun Set: ");
  Serial.println(time); // 15:19
}


void loop() {
}