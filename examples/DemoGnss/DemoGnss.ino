#include <AltSoftSerial.h>
#include <Sim808_EVB.h>

char* geographicalToText(sexagesimale_t latitude, sexagesimale_t longitude);
char* timeToText(time_t time_P);

// Global declarations
AltSoftSerial   altSoftSerial_G;
sim808          sim808EVB_G;

void setup()
{
  Serial.begin(115200);

  altSoftSerial_G.begin(38400);
  Serial.println(F("\nConnecting to the SIM808 EVB board"));
  while(!sim808EVB_G.begin(altSoftSerial_G))
  {
    Serial.println(F("Communication failed with SIM808 EVB board. New attempt..."));
    delay(1000);
  }
  sim808EVB_G.gnssPowerOn(); 
  Serial.println(F("Initialization of the GNSS."));
  while(!sim808EVB_G.getGnssRunStatus())
  {
    Serial.print(F("."));
    delay(1000);
  }
  Serial.println(F("GNSS is searching satellites."));
  while(!sim808EVB_G.getGnssFixStatus())
  {
    Serial.print(F("."));
    delay(1000);
  }
  Serial.println(F("GNSS initialized."));
}

void loop()
{
  gnssCoordinates_t gnssCoordinates_L;
  time_t gnssTime_L; 
  satellitesInfo_t satellitesInfo_L;

  gnssCoordinates_L = sim808EVB_G.getGnssCoordinates();
  if(gnssCoordinates_L.status)
  {
    Serial.print(F("\nGeographical coordinates: "));    
    Serial.print(geographicalToText(gnssCoordinates_L.latitude, gnssCoordinates_L.longitude));
    Serial.print(F(" - Altitude: "));
    Serial.print(gnssCoordinates_L.altitude);    
    Serial.print(F("m - Speed: "));    
    Serial.print(gnssCoordinates_L.speed);    
    Serial.println(F("km/h"));    
  }
  else
  {
    Serial.println(F("GNSS reading error."));  
  }
  gnssTime_L = sim808EVB_G.getGnssTime();
  if(gnssTime_L != 0)
  {
    Serial.print(F("GMT date and time : "));
    Serial.println(timeToText(gnssTime_L));
  }
  satellitesInfo_L = sim808EVB_G.getSatellitesInfo();
  Serial.print(F("Satellites info: "));
  Serial.print(satellitesInfo_L.gnssInView);
  Serial.print(F(" GNSS in view - "));
  Serial.print(satellitesInfo_L.gnssUsed);
  Serial.print(F(" GNSS used - "));
  Serial.print(satellitesInfo_L.glonassUsed);
  Serial.println(F(" Glonass used"));
  delay(5000);
}

char* timeToText(time_t time_P)
{
  static char textTime_L[25];
  tm TimeSplitted_L;

  set_zone(0 * ONE_HOUR);
  set_dst(0); 
  localtime_r(&time_P, &TimeSplitted_L);
  sprintf(textTime_L, "%d/%d/%d %.2dh%.2d:%.2d", TimeSplitted_L.tm_mday, TimeSplitted_L.tm_mon+1, TimeSplitted_L.tm_year+1900, TimeSplitted_L.tm_hour, TimeSplitted_L.tm_min, TimeSplitted_L.tm_sec); 
  return(textTime_L);
}

char* geographicalToText(sexagesimale_t latitude_P, sexagesimale_t longitude_P)
{
  static char textCoordinates_L[35];
  char latitude_sign_L;
  char longitude_sign_L;
  float partie_entiere;

  if(latitude_P.positive)
  {
    latitude_sign_L = 'N';
  }
  else
  {
    latitude_sign_L = 'S';
  }
  if(longitude_P.positive)
  {
    longitude_sign_L = 'E';
  }
  else
  {
    longitude_sign_L = 'W';
  }
  sprintf(textCoordinates_L, "%d°%d'%02d.%02d\"%c, %d°%d'%02d.%02d\"%c",  latitude_P.degree, latitude_P.minute , int(latitude_P.second),  int(modff(latitude_P.second, &partie_entiere)*100), latitude_sign_L,
                                                                          longitude_P.degree, longitude_P.minute , int(longitude_P.second),  int(modff(longitude_P.second, &partie_entiere)*100), longitude_sign_L);    
  return(textCoordinates_L);
}