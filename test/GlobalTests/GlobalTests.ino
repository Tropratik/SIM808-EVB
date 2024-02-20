#include <AltSoftSerial.h>
#include <Sim808_EVB.h>
#include <util/eu_dst.h>

#define  GNSS_RUN_DELAY 10 * 1000UL
#define  GNSS_FIX_DELAY 7 * 60 * 1000UL

// Prototype Declaration
void testBeginAndConnected(void);
void testIdentification(void);
void testGnssPower(void);
void testCoordinates(void);
void testSatellitesInfo(void);
void testNmeaSentence(void);
void testGnssTime(void);

// Global declarations
AltSoftSerial   altSoftSerial_G;
sim808          sim808EVB_G;

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println(F("*** Echo OFF"));
  testBeginAndConnected();
  testIdentification();
  testGnssPower();
  testCoordinates();
  testSatellitesInfo();
  testGnssTime();
  altSoftSerial_G.end();
  sim808EVB_G.echoOn();
  Serial.println(F("*** Echo ON"));
  testBeginAndConnected();
  testIdentification();
  testGnssPower();
  testCoordinates();
  testSatellitesInfo();
  testGnssTime();
  sim808EVB_G.echoOff();
  Serial.println(F("-----------------"));
  Serial.println(F("|    Test OK    |"));
  Serial.println(F("-----------------"));
}

// Fonction principale du programme, s'exécute en boucle:
void loop()
{
}

void testBeginAndConnected(void)
{
  Serial.print(F("testBeginAndConnected"));
  if(sim808EVB_G.connected())
  {
    errorDetected(F("connected()"));
  }
  altSoftSerial_G.begin(38400);
  if((!sim808EVB_G.begin(altSoftSerial_G)) || (!sim808EVB_G.connected()))
  {
    errorDetected(F("connected() or begin()"));
  }
  altSoftSerial_G.end();
  if(sim808EVB_G.connected())
  {
    errorDetected(F("connected()"));
  }
  altSoftSerial_G.begin(38400);
  Serial.println(F(" ==> OK"));
}

void testGnssPower(void)
{
  elapsedMillis chronometre_L;
  unsigned long chronometreMemory_L;

  Serial.print(F("testGnssPower"));
  if((!sim808EVB_G.gnssPowerOn()) || (!sim808EVB_G.getGnssPower()))
  {
    errorDetected(F("getGnssPower() or gnssPowerOn()"));
  }
  if((!sim808EVB_G.gnssPowerOff()) || (sim808EVB_G.getGnssPower()))
  {
    errorDetected(F("getGnssPower() or gnssPowerOff()"));
  }
  if(!sim808EVB_G.gnssPowerOn())
  {
    errorDetected(F("gnssPowerOn()"));
  }
  chronometre_L=0;
  while((!sim808EVB_G.getGnssRunStatus())&&(chronometre_L<=GNSS_RUN_DELAY))
  {
    delay(1000);
  }
  if(chronometre_L>GNSS_RUN_DELAY)
  {
    errorDetected(F("getGnssRunStatus() or [module error]"));
  }
  else
  {
    chronometreMemory_L = chronometre_L;
    Serial.print(F(" / Run status in "));
    Serial.print(chronometreMemory_L/(1000UL*60));
    Serial.print(F(" min "));
    Serial.print((chronometreMemory_L%(1000UL*60))/1000);
    Serial.print(F(" sec. "));
  }
  chronometre_L=0;
  while((!sim808EVB_G.getGnssFixStatus())&&(chronometre_L<=GNSS_FIX_DELAY))
  {
    delay(1000);
  }
  if(chronometre_L>GNSS_FIX_DELAY)
  {
    errorDetected(F("getGnssFixStatus() or [module error]"));
  }
  else
  {
    chronometreMemory_L = chronometre_L;
    Serial.print(F("/ Fix status in "));
    Serial.print(chronometreMemory_L/(1000UL*60));
    Serial.print(F(" min "));
    Serial.print((chronometreMemory_L%(1000UL*60))/1000);
    Serial.print(F(" sec. /"));
  }
  Serial.println(F(" ==> OK"));
}

void testIdentification(void)
{
  identification_t identification_L;

  Serial.print(F("testIdentification"));
  identification_L = sim808EVB_G.getIdentification();
  if(strcmp_P(identification_L.manufacturerId, (const char*)F("SIMCOM_Ltd"))!=0)
  {
    errorDetected(F("Bad Manufacturer Id"));
  }
  if(strcmp_P(identification_L.productId, (const char*)F("SIMCOM_SIM808"))!=0)
  {
    errorDetected(F("Bad Product Id"));
  }
  if(strcmp_P(identification_L.softwareVersion, (const char*)F("1418B01SIM808M32"))!=0)
  {
    Serial.println(identification_L.softwareVersion);
    errorDetected(F("Bad Software version"));
  }
  Serial.println(F(" ==> OK"));
}

void testCoordinates(void)
{
  gnssCoordinates_t gnssCoordinates_L;

  Serial.print(F("testCoordinates"));
  gnssCoordinates_L = sim808EVB_G.getGnssCoordinates();
  if(
    (!gnssCoordinates_L.status)||
    (gnssCoordinates_L.speed<0)||(gnssCoordinates_L.speed>20)||
    (gnssCoordinates_L.longitude.degree==0)
    )
  {
    errorDetected(F("getGnssCoordinates()"));
  }
  Serial.println(F(" ==> OK"));
}

void testSatellitesInfo(void)
{
  satellitesInfo_t satellitesInfo_L;

  Serial.print(F("testSatellitesInfo"));
  satellitesInfo_L = sim808EVB_G.getSatellitesInfo();
  if(
    (satellitesInfo_L.gnssInView<2)||
    (satellitesInfo_L.gnssInView<satellitesInfo_L.gnssUsed)
    )
  {
    errorDetected(F("getSatellitesInfo()"));
  }
  Serial.println(F(" ==> OK"));
}

void testNmeaSentence(void)
{
  Serial.print(F("testNmeaSentence"));
  if(sim808EVB_G.getNmeaSentence() != RMC)
  {
    errorDetected(F("Default value for NMEA sentence"));
  }
  if((sim808EVB_G.setNmeaSentence(GGA)) || (sim808EVB_G.getNmeaSentence()!=GGA))
  {
    errorDetected(F("setNmeaSentence(GGA) or getNmeaSentence() for GGA"));
  }
  if((sim808EVB_G.setNmeaSentence(GSA)) || (sim808EVB_G.getNmeaSentence()!=GSA))
  {
    errorDetected(F("setNmeaSentence() or getNmeaSentence() for GSA"));
  }
  if((sim808EVB_G.setNmeaSentence(GSV)) || (sim808EVB_G.getNmeaSentence()!=GSV))
  {
    errorDetected(F("setNmeaSentence() or getNmeaSentence() for GSV"));
  }
  if((sim808EVB_G.setNmeaSentence(RMC)) || (sim808EVB_G.getNmeaSentence()!=RMC))
  {
    errorDetected(F("setNmeaSentence() or getNmeaSentence() for RMC"));
  }
  Serial.println(F(" ==> OK"));
}

void testGnssTime(void)
{
  time_t gnssTime_L;
  tm gnssTimeSplitted_L;
  
  set_zone(1 * ONE_HOUR);
  set_dst(eu_dst); 
  Serial.print(F("testGnssTime"));
  gnssTime_L = sim808EVB_G.getGnssTime();
  localtime_r(&gnssTime_L, &gnssTimeSplitted_L);
  if(((gnssTimeSplitted_L.tm_year+1900)<2024)||((gnssTimeSplitted_L.tm_year+1900)>2030))
  {
    errorDetected(F("getGnssTime"));
  }
  Serial.println(F(" ==> OK"));
}

void errorDetected(const __FlashStringHelper* message_P)
{
  Serial.print(F("\n!!! Error: "));
  Serial.println(message_P);
  sim808EVB_G.gnssPowerOff();
  while(true);
}