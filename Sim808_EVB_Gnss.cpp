/*
 * sim808_EVB_Gnss.cpp - Library to manage the SIM808 Development Kit: https://tropratik.fr/produit/kit-de-developpement-sim808-evb-v3-2-gsm-gprs-gps.
 * Created by Tropratik (https://tropratik.fr).
 * Version 1.0.
 * license GNU General Public License Version 3 (GPL-3.0)
 */

/* Library header */
#include "Sim808_EVB.h"

/* Définition des constantes GNSS */

bool sim808::gnssPowerOn(void)
{
  return(simpleAtRequest(AT_CGNSPWR, true));
}

bool sim808::gnssPowerOff(void)
{
  return(simpleAtRequest(AT_CGNSPWR, false));
}

/*bool sim808::gnssDebugOn(void)
{
  return(simpleAtRequest(AT_CGNSTST, true));
}*/

/*bool sim808::gnssDebugOff(void)
{
  return(simpleAtRequest(AT_CGNSTST, false));
}*/

bool sim808::setNmeaSentence(nmeaSentence_t sentence_P)
{
  bool status_L;
  
  status_L = false;
  switch(sentence_P)
  {
    case GGA:
      status_L = simpleAtRequest(AT_CGNSSEQ, SENTENCE_GGA);
      break;
    case GSA:
      status_L = simpleAtRequest( AT_CGNSSEQ, SENTENCE_GSA);
      break;
    case GSV:
      status_L = simpleAtRequest(AT_CGNSSEQ, SENTENCE_GSV);
      break;
    case RMC:
      status_L = simpleAtRequest(AT_CGNSSEQ, SENTENCE_RMC);
      break;
    case UNKNOWN:
      traceln(F("'UNKNOWN' is an incorrect value for the NMEA sentence"));
      break;
  }
  return(status_L);
}

bool  sim808::getNmeaSentence(nmeaSentence_t *pSentence_P)
{
  bool status_L;
  char NmeaSentence_L[4];

  // Valeurs par defaut
  status_L = false;
  if (get(AT_CGNSSEQ_ASK, NmeaSentence_L))
  {
    if(strncmp_P(NmeaSentence_L, SENTENCE_GGA,3)==0)
    {
      *pSentence_P = GGA;  
      status_L = true;
    }
    else if(strncmp_P(NmeaSentence_L, SENTENCE_GSA,3)==0)
    {
      *pSentence_P = GSA;  
      status_L = true;
    }
    else if(strncmp_P(NmeaSentence_L, SENTENCE_GSV,3)==0)
    {
      *pSentence_P = GSV;  
      status_L = true;
    }
    else if(strncmp_P(NmeaSentence_L, SENTENCE_RMC,3)==0)
    {
      *pSentence_P = RMC;  
      status_L = true;
    }
    else
    {
      traceln(F("Incorrect return value ='%s' in 'getNmeaSentence'"), NmeaSentence_L);
    }
  }
  return(status_L);
}

nmeaSentence_t  sim808::getNmeaSentence(void)
{
  nmeaSentence_t nmeaSentence_L;

  if(!getNmeaSentence(&nmeaSentence_L))
  {
    // Valeur retournée en cas d'erreur
    nmeaSentence_L=UNKNOWN;  
  }
  return(nmeaSentence_L);
}

bool sim808::getGnssCoordinates(sexagesimale_t *pLatitude_P, sexagesimale_t *pLongitude_P, float *pAltitude_P, float *pSpeed_P)
{
  bool status_L;
  gnssData_t pRmcGnssData_L;

  // Valeurs par defaut
  status_L = false;

  if(get(AT_CGNSINF_ASK, &pRmcGnssData_L))
  {
    if(pRmcGnssData_L.latitude.present && pRmcGnssData_L.longitude.present && pRmcGnssData_L.altitude.present && pRmcGnssData_L.speed.present)
    {
      *pLatitude_P = pRmcGnssData_L.latitude.value;
      *pLongitude_P = pRmcGnssData_L.longitude.value;
      *pAltitude_P = pRmcGnssData_L.altitude.value;
      *pSpeed_P = pRmcGnssData_L.speed.value;
      status_L = true; 
    }
  }
  return(status_L);
}

gnssCoordinates_t sim808::getGnssCoordinates(void)
{
  gnssCoordinates_t gnssCoordinates_L;

  gnssCoordinates_L.status = getGnssCoordinates(&gnssCoordinates_L.latitude, &gnssCoordinates_L.longitude, &gnssCoordinates_L.altitude, &gnssCoordinates_L.speed);
  return(gnssCoordinates_L);
}

bool sim808::getSatellitesInfo(uint8_t *pGnssSatellitesInView_P, uint8_t *pGnssSatellitesUsed, uint8_t *pGlonassSatellitesUsed)
{
  bool status_L;
  gnssData_t pRmcGnssData_L;

  // Valeurs par defaut
  status_L = false;

  if(get(AT_CGNSINF_ASK, &pRmcGnssData_L))
  {
    if(pRmcGnssData_L.gnssSatellitesInView.present)
    {
      *pGnssSatellitesInView_P = pRmcGnssData_L.gnssSatellitesInView.value;
    }
    if(pRmcGnssData_L.gnssSatellitesUsed.present)
    {
      *pGnssSatellitesUsed = pRmcGnssData_L.gnssSatellitesUsed.value;
    }
    if(pRmcGnssData_L.glonassSatellitesUsed.present)
    {
      *pGlonassSatellitesUsed = pRmcGnssData_L.glonassSatellitesUsed.value;
    }
    status_L = true;
  }
  return(status_L);
}

satellitesInfo_t sim808::getSatellitesInfo(void)
{
  satellitesInfo_t satellitesInfo_L;
  
  satellitesInfo_L.gnssInView = 0; 
  satellitesInfo_L.gnssUsed = 0;
  satellitesInfo_L.glonassUsed = 0;
  getSatellitesInfo(&satellitesInfo_L.gnssInView, &satellitesInfo_L.gnssUsed, &satellitesInfo_L.glonassUsed);
  return(satellitesInfo_L);
}

bool sim808::getGnssTime(time_t *pTime_P)
{
  bool status_L;
  gnssData_t pRmcGnssData_L;

  // Valeurs par defaut
  status_L = false;

  if(get(AT_CGNSINF_ASK, &pRmcGnssData_L))
  {
    if(pRmcGnssData_L.time.present)
    {
      *pTime_P = pRmcGnssData_L.time.value;
      status_L = true; 
    }
  }
  return(status_L);
}

time_t sim808::getGnssTime(void)
{
  time_t gnssTime_L;

  if(!getGnssTime(&gnssTime_L))
  {
    // Valeur retournée en cas d'erreur
    gnssTime_L=0;  
  }
  return(gnssTime_L);
}

bool sim808::getGnssRunStatus(bool *pRunStatus_P)
{
  bool status_L;
  gnssData_t pRmcGnssData_L;

  // Valeurs par defaut
  status_L = false;

  if(get(AT_CGNSINF_ASK, &pRmcGnssData_L))
  {
    if(pRmcGnssData_L.runStatus.present)
    {
      *pRunStatus_P = pRmcGnssData_L.runStatus.value;
      status_L = true; 
    }
  }
  return(status_L);
}

bool  sim808::getGnssPower(void)
{
  bool powerState_L;

  if(!getGnssPower(&powerState_L))
  {
    // Valeur retournée en cas d'erreur
    powerState_L=false;  
  }
  return(powerState_L);
}

bool sim808::getGnssPower(bool *pPowerState_P)
{
  return(get(AT_CGNSPWR_ASK, pPowerState_P));
}

bool sim808::getGnssRunStatus(void)
{
  bool runStatus_L;

  if(!getGnssRunStatus(&runStatus_L))
  {
    // Valeur retournée en cas d'erreur
    runStatus_L=false;  
  }
  return(runStatus_L);
}

bool sim808::getGnssFixStatus(bool *pFixStatus_P)
{
  bool status_L;
  gnssData_t pRmcGnssData_L;

  // Valeurs par defaut
  status_L = false;

  if(get(AT_CGNSINF_ASK, &pRmcGnssData_L))
  {
    if(
		(pRmcGnssData_L.fixStatus.present)&&
		(pRmcGnssData_L.latitude.present)&&
		(pRmcGnssData_L.longitude.present)&&
		(pRmcGnssData_L.altitude.present)&&
		(pRmcGnssData_L.speed.present)
	  )
    {
      *pFixStatus_P = pRmcGnssData_L.fixStatus.value;
      status_L = true; 
    }
  }
  return(status_L);
}

bool sim808::getGnssFixStatus(void)
{
  bool fixStatus_L;

  if(!getGnssFixStatus(&fixStatus_L))
  {
    // Valeur retournée en cas d'erreur
    fixStatus_L=false;  
  }
  return(fixStatus_L);
}

/*bool sim808::sendPmtkCommand(char *pPmtkCommand)
{
  return(simpleAtRequest(AT_CGNSCMD, pPmtkCommand));
}*/

/*bool sim808::gnssUrcReportOn(void)
{
  bool status_L;

  status_L = simpleAtRequest(AT_CCGNSURC, gnssUrcReportFrequency);
  if(status_L)
  {
    gnssUrcReportActivated = true;
  }
  return(status_L);
}*/

/*bool sim808::gnssUrcReportOff(void)
{
  bool status_L;

  status_L = simpleAtRequest(AT_CCGNSURC, 0);
  if(status_L)
  {
    gnssUrcReportActivated = false;
  }
  return(status_L);
}*/

/*bool sim808::setGnssUrcReportFrequency(uint8_t frequency_P)
{
  bool status_L;

  if(gnssUrcReportActivated)
  {
    status_L = simpleAtRequest(AT_CCGNSURC, frequency_P);
    if(status_L)
    {
      gnssUrcReportFrequency = frequency_P;
    }
  }
  else
  {
    gnssUrcReportFrequency = frequency_P;
    status_L = true;
  }
  return(status_L);
}*/

bool sim808::extractFromField(char* field_P, sexagesimale_t *pSexagesimaleData_P)
{
  bool    status_L;
  double  rawValue_L;
  double  integralPart_L;

  // Valeurs par defaut
  status_L = false;

  status_L = extractFromField(field_P, &rawValue_L);
  if(status_L)
  {
      // Détermination du signe +/- de la coordonnée
      pSexagesimaleData_P->positive = (rawValue_L>=0);
      rawValue_L = fabs(rawValue_L);
      
      // Calcul des degrés
      modf(rawValue_L, &integralPart_L);
      pSexagesimaleData_P->degree = (uint8_t)integralPart_L;
      
      // Calcul des minutes
      rawValue_L = rawValue_L - pSexagesimaleData_P->degree;   // On retire la partie entière
      rawValue_L = rawValue_L * 60;
      modf(rawValue_L, &integralPart_L);
      pSexagesimaleData_P->minute = (uint8_t)integralPart_L;
      
      // Calcul des secondes
      rawValue_L = rawValue_L - pSexagesimaleData_P->minute;   // On retire la partie entière
      rawValue_L = rawValue_L * 60;
      pSexagesimaleData_P->second = (float)rawValue_L;
  }
  return(status_L);
}

bool sim808::extractFromField(char* field_P, time_t *pTimeData_P)
{
  bool  status_L;
  char  *pIndexPosition_L;
  char  *pEndConversion_L;
  unsigned long   conversionResult_L;
  tm              tmUtc_L;

  // Valeurs par defaut
  status_L = true;
  pIndexPosition_L = field_P;

  // On vérifie que le champ n'est pas vide
  if (*pIndexPosition_L == '\0')
  {
    status_L = false;  // Pas de valeur trouvée
  }
  // Récupération de l'année
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 4);
    if(conversionResult_L==1980)
    {
      status_L = false;  // Valeur par défaut renseignée
    }
    else if((pEndConversion_L==pIndexPosition_L)||(conversionResult_L<1980) || (conversionResult_L>2100))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect year format"));
    }
    else
    {
      tmUtc_L.tm_year = conversionResult_L - 1900;
      pIndexPosition_L = pIndexPosition_L + 4;
    }
  }
  // Récupération du mois
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 2);
    if((pEndConversion_L==pIndexPosition_L)||(conversionResult_L<1) || (conversionResult_L>12))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect month format"));
    }
    else
    {
      tmUtc_L.tm_mon = conversionResult_L - 1;
      pIndexPosition_L = pIndexPosition_L + 2;
    }
  }
  // Récupération du jour du mois
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 2);
    if((pEndConversion_L==pIndexPosition_L)||(conversionResult_L<1) || (conversionResult_L>31))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect day of month format"));
    }
    else
    {
      tmUtc_L.tm_mday = conversionResult_L;
      pIndexPosition_L = pIndexPosition_L + 2;
    }
  }
  // Récupération de l'heure
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 2);
    if((pEndConversion_L==pIndexPosition_L) || (conversionResult_L>23))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect hour format"));
    }
    else
    {
      tmUtc_L.tm_hour = conversionResult_L;
      pIndexPosition_L = pIndexPosition_L + 2;
    }
  }
  // Récupération des minutes
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 2);
    if((pEndConversion_L==pIndexPosition_L) || (conversionResult_L>59))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect minute format"));
    }
    else
    {
      tmUtc_L.tm_min = conversionResult_L;
      pIndexPosition_L = pIndexPosition_L + 2;
    }
  }
  // Récupération des secondes
  if(status_L)
  {
    conversionResult_L = strntoul(pIndexPosition_L, &pEndConversion_L, 10, 2);
    if((pEndConversion_L==pIndexPosition_L) || (conversionResult_L>59))
    {
      status_L = false;  // Pas de valeur trouvée
      traceln(F("Incorrect second format"));
    }
    else
    {
      tmUtc_L.tm_sec = conversionResult_L;
      pIndexPosition_L = pIndexPosition_L + 6;
    }
  }
  if(status_L)
  {
    *pTimeData_P = mk_gmtime(&tmUtc_L);
  }
  return(status_L);
}

bool  sim808::get(const char* command_P, gnssData_t *pGnssData_P, ...)
{
  bool status_L;
  char *dataField_L[21];
  unsigned int ii_L;
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  // Valeurs par defaut
  status_L = false;

  va_start(pArgument_L, pGnssData_P);
  if(atRequestForDataFields(sizeof(dataField_L)/sizeof(char*), dataField_L, command_P, pArgument_L))
  {
    pGnssData_P->runStatus.present  = extractFromField(dataField_L[0], &pGnssData_P->runStatus.value);
    pGnssData_P->fixStatus.present  = extractFromField(dataField_L[1], &pGnssData_P->fixStatus.value);
    pGnssData_P->time.present  = extractFromField(dataField_L[2], &pGnssData_P->time.value);
    pGnssData_P->latitude.present   = extractFromField(dataField_L[3], &pGnssData_P->latitude.value);
    pGnssData_P->longitude.present   = extractFromField(dataField_L[4], &pGnssData_P->longitude.value);
    pGnssData_P->altitude.present   = extractFromField(dataField_L[5], &pGnssData_P->altitude.value);
    pGnssData_P->speed.present   = extractFromField(dataField_L[6], &pGnssData_P->speed.value);
    pGnssData_P->course.present   = extractFromField(dataField_L[7], &pGnssData_P->course.value);
    pGnssData_P->fixMode.present   = extractFromField(dataField_L[8], &pGnssData_P->fixMode.value);
    pGnssData_P->hDop.present   = extractFromField(dataField_L[10], &pGnssData_P->hDop.value);
    pGnssData_P->pDop.present   = extractFromField(dataField_L[11], &pGnssData_P->pDop.value);
    pGnssData_P->vDop.present   = extractFromField(dataField_L[12], &pGnssData_P->vDop.value);
    pGnssData_P->gnssSatellitesInView.present   = extractFromField(dataField_L[14], &pGnssData_P->gnssSatellitesInView.value);
    pGnssData_P->gnssSatellitesUsed.present   = extractFromField(dataField_L[15], &pGnssData_P->gnssSatellitesUsed.value);
    pGnssData_P->glonassSatellitesUsed.present   = extractFromField(dataField_L[16], &pGnssData_P->glonassSatellitesUsed.value);
    pGnssData_P->cNoMAx.present   = extractFromField(dataField_L[18], &pGnssData_P->cNoMAx.value);
    for(ii_L=0; ii_L<sizeof(dataField_L)/sizeof(char*); ii_L++)
    {
      free(dataField_L[ii_L]);
    }
    traceln(*pGnssData_P);
    status_L = true;
  }
  va_end(pArgument_L);
  return(status_L);
}

void sim808::traceln(gnssData_t gnssData_P)
{
#if DEBUG_SIM808 == TRUE
  if(gnssData_P.runStatus.present) trace(F("RunStatus.value=%d, "), gnssData_P.runStatus.value);
  if(gnssData_P.fixStatus.present) trace(F("FixStatus.value=%d, "), gnssData_P.fixStatus.value);
  if(gnssData_P.time.present)
  {
    tm TimeSplitted_L;
    
    localtime_r(&gnssData_P.time.value, &TimeSplitted_L);
    trace(F("Date='%d/%d/%d %.2dh%.2d:%.2d', "), TimeSplitted_L.tm_mday, TimeSplitted_L.tm_mon+1, TimeSplitted_L.tm_year+1900, TimeSplitted_L.tm_hour, TimeSplitted_L.tm_min, TimeSplitted_L.tm_sec); 
  }
  if(gnssData_P.latitude.present)
  {
    char floatBuffer_L[7];
    
    trace(F("gnssData_P.latitude.value="));
    if(gnssData_P.latitude.value.positive)
    {
      trace(F("+"));
    }
    else
    {
      trace(F("-"));
    }
    dtostrf(gnssData_P.latitude.value.second, 4, 3, floatBuffer_L);
    trace(F("%d°%d'%s'', "), gnssData_P.latitude.value.degree, gnssData_P.latitude.value.minute, floatBuffer_L);
  }
  if(gnssData_P.longitude.present)
  {
    char floatBuffer_L[8];

    trace(F("gnssData_P.longitude.value="));
    if(gnssData_P.longitude.value.positive)
    {
      trace(F("+"));
    }
    else
    {
      trace(F("-"));
    }
    dtostrf(gnssData_P.longitude.value.second, 5, 3, floatBuffer_L);
    trace(F("%d°%d'%s'', "), gnssData_P.longitude.value.degree, gnssData_P.longitude.value.minute, floatBuffer_L);
  }
  if(gnssData_P.altitude.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.altitude.value, 4, 3, floatBuffer_L);
    trace(F("gnssData_P.altitude.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.speed.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.speed.value, 4, 3, floatBuffer_L);
    trace(F("gnssData_P.speed.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.course.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.course.value, 4, 3, floatBuffer_L);
    trace(F("gnssData_P.course.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.fixMode.present) trace(F("gnssData_P.fixMode.value=%d, "), gnssData_P.fixMode.value);
  if(gnssData_P.hDop.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.hDop.value, 4, 3, floatBuffer_L);
    trace(F("gnssData_P.hDop.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.pDop.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.pDop.value, 4, 3, floatBuffer_L);
    trace(F("gnssData_P.pDop.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.vDop.present)
  {
    char floatBuffer_L[9];
    
    dtostrf(gnssData_P.vDop.value, 4, 3, floatBuffer_L);
    traceln(F("gnssData_P.vDop.value=%s, "), floatBuffer_L);
  }
  if(gnssData_P.gnssSatellitesInView.present) trace(F("gnssData_P.gnssSatellitesInView.value=%d, "), gnssData_P.gnssSatellitesInView.value);
  if(gnssData_P.gnssSatellitesUsed.present) trace(F("gnssData_P.gnssSatellitesUsed.value=%d, "), gnssData_P.gnssSatellitesUsed.value);
  if(gnssData_P.glonassSatellitesUsed.present) trace(F("gnssData_P.glonassSatellitesUsed.value=%d, "), gnssData_P.glonassSatellitesUsed.value);
  if(gnssData_P.cNoMAx.present) traceln(F("gnssData_P.cNoMAx.value=%d"), gnssData_P.cNoMAx.value);
#else
  gnssData_P=gnssData_P;
#endif
}
