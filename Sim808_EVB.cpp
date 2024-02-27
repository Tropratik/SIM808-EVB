/*
 * sim808_EVB.cpp - Library to manage the SIM808 Development Kit: https://tropratik.fr/produit/kit-de-developpement-sim808-evb-v3-2-gsm-gprs-gps.
 * Created by Tropratik (https://tropratik.fr).
 * Version 1.1
 * license GNU General Public License Version 3 (GPL-3.0)
 */

/* En-tête de la bibliothèque */
#include "Sim808_EVB.h"

// Constructeur
sim808::sim808()
{
  /* Valeurs par defaut: */
  responseDelay = DEFAULT_RESPONSE_TIME;
//  gnssUrcReportActivated = false;
//  gnssUrcReportFrequency = 1;
}

sim808::~sim808(void)
{

}

bool sim808::begin(Stream &serial_P)
{
  sim808Interface = &serial_P;
  // Par defaut, on désactive l'echo
  echoOff();
  return(getSoftwareVersion(softwareVersion));
}

bool sim808::echoOn(void)
{
  return(simpleAtRequest(ATE, true));
}

bool sim808::echoOff(void)
{
  return(simpleAtRequest(ATE, false));
}

bool sim808::connected(void)
{
  return(simpleAtRequest(AT,""));
}

identification_t  sim808::getIdentification(void)
{
  identification_t identification_L;

  if(!getSoftwareVersion(identification_L.softwareVersion))
  {
    // Valeur retournée en cas d'erreur
    identification_L.softwareVersion[0]='\0';  
  }
  if(!getManufacturerId(identification_L.manufacturerId))
  {
    // Valeur retournée en cas d'erreur
    identification_L.manufacturerId[0]='\0';  
  }
  if(!getProductId(identification_L.productId))
  {
    // Valeur retournée en cas d'erreur
    identification_L.productId[0]='\0';  
  }
  if(!getProductSerialNumber(identification_L.productSerialNumber))
  {
    // Valeur retournée en cas d'erreur
    identification_L.productSerialNumber[0]='\0';  
  }
  return(identification_L);
}

bool  sim808::getSoftwareVersion(char *pSoftwareVersion_P)
{
  return(get(AT_CGMR_ASK, pSoftwareVersion_P));
}

bool  sim808::getManufacturerId(char *pManufacturerId_P)
{
  return(get(AT_CGMI_ASK, pManufacturerId_P));
}

bool  sim808::getProductId(char *pProductId_P)
{
  return(get(AT_CGMM_ASK, pProductId_P));
}

bool  sim808::getProductSerialNumber(char *pProductSerialNumber_P)
{
  return(get(AT_CGSN_ASK, pProductSerialNumber_P));
}

bool sim808::simpleAtRequest(const char* command_P, ...)
{
  bool status_L = false;
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres
  
  if(sim808Interface!=NULL)
  {
    va_start(pArgument_L, command_P);
    if(atRequest(NULL, command_P, pArgument_L)==REQ_READ_OK)
    {
      // Commande bien traitée par la SIM808
      status_L = true;
    }
    va_end(pArgument_L);
  }
  return(status_L);
}

bool sim808::extractFromField(char* field_P, bool *pBoolData_P)
{
  bool status_L;

  // Valeurs par defaut
  status_L = false;

  switch(field_P[0])
  {
    case '0':
      *pBoolData_P = false;
      status_L = true;
      break;
    case '1':
      *pBoolData_P = true;
      status_L = true;
      break;
  }
  return(status_L);
}

bool sim808::extractFromField(char* field_P, uint8_t *pUint8Data_P)
{
  bool  status_L;
  char  *endConversion_L;

  // Valeurs par defaut
  status_L = true;

  *pUint8Data_P = (uint8_t)strtol(field_P, &endConversion_L, 10);
  if(endConversion_L==field_P)
  {
    status_L = false;
  }
  return(status_L);
}

bool sim808::extractFromField(char* field_P, float *pFloatData_P)
{
  bool  status_L;
  char  *endConversion_L;

  // Valeurs par defaut
  status_L = true;

  *pFloatData_P = (float)strtod(field_P, &endConversion_L);
  if(endConversion_L==field_P)
  {
    status_L = false;
  }
  return(status_L);
}

bool sim808::extractFromField(char* field_P, double *pDoubleData_P)
{
  bool  status_L;
  char  *endConversion_L;

  // Valeurs par defaut
  status_L = true;

  *pDoubleData_P = strtod(field_P, &endConversion_L);
  if(endConversion_L==field_P)
  {
    status_L = false;
  }
  return(status_L);
}

bool sim808::get(const char* command_P, bool *pBoolData_P, ...)
{
  bool status_L;
  char *dataField_L[1];
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  // Valeurs par defaut
  status_L = false;

  va_start(pArgument_L, pBoolData_P);
  if(atRequestForDataFields(1, dataField_L, command_P, pArgument_L))
  {
    if(extractFromField(dataField_L[0],pBoolData_P))
    {
      status_L = true;
    }
    else
    {
      traceln(F("Incorrect return value ='%c' in 'get[bool]'"), dataField_L[0][0]);
    }
    free(dataField_L[0]);
  }
  va_end(pArgument_L);
  return(status_L);
}

bool  sim808::get(const char* command_P, char *pCharData_P, ...)
{
  bool status_L;
  char *dataField_L[1];
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  // Valeurs par defaut
  status_L = false;

  va_start(pArgument_L, pCharData_P);
  if(atRequestForDataFields(1, dataField_L, command_P, pArgument_L))
  {
    strcpy(pCharData_P,dataField_L[0]);
    status_L= true;
    free(dataField_L[0]);
  }
  va_end(pArgument_L);
  return(status_L);
}

bool sim808::atRequestForDataFields(int fieldCount_P, char *pFieldContent_P[], const char* command_P, va_list pArgument_P)
{
  bool status_L;
  uint8_t ii_L, DataStartPosition_L;;
  int fieldCount_L;
  char  answer_L[20];
  char  *pRawResponse_L;
  size_t prefixLength_L;
 
  // Valeurs par defaut
  status_L = true;
  fieldCount_L = 0;

  // Construction de la réponse:
  answer_L[0]='\r';
  answer_L[1]='\n';
  strcpy_P(answer_L+2, command_P+2); 
  for(ii_L=0;(ii_L<strlen(answer_L))&&(answer_L[ii_L] != '?');ii_L++);
  answer_L[ii_L] = ':';
  answer_L[ii_L+1] = ' ';
  answer_L[ii_L+2] = '\0';
  // Gestion des reponses au format spécifique
  if(strncmp_P(answer_L+2, AT_CGNSSEQ_ASK+2, strlen_P(AT_CGNSSEQ_ASK)-2)==0)
  {
    strcpy_P(answer_L+2, AT_CGNSSEQ_ANS); 
  }
  else if(strncmp_P(answer_L+2, AT_CGMR_ASK+2, strlen_P(AT_CGMR_ASK)-2)==0)
  {
    strcpy_P(answer_L+2, AT_CGMR_ANS); 
  }
  else if(strncmp_P(answer_L+2, AT_CGMI_ASK+2, strlen_P(AT_CGMI_ASK)-2)==0)
  {
    strcpy_P(answer_L+2, AT_CGMI_ANS); 
  }
  else if(strncmp_P(answer_L+2, AT_CGMM_ASK+2, strlen_P(AT_CGMM_ASK)-2)==0)
  {
    strcpy_P(answer_L+2, AT_CGMM_ANS); 
  }
  else if(strncmp_P(answer_L+2, AT_CGSN_ASK+2, strlen_P(AT_CGSN_ASK)-2)==0)
  {
    strcpy_P(answer_L+2, AT_CGSN_ANS); 
  }
  prefixLength_L = strlen(answer_L);
  if(status_L)
  {
    if(atRequest(&pRawResponse_L, command_P, pArgument_P)==REQ_READ_OK)
    {
      if(
        (strlen(pRawResponse_L)>prefixLength_L)&&
        (strncmp(pRawResponse_L, answer_L, prefixLength_L)==0)
        )
      {
        DataStartPosition_L = prefixLength_L;
        ii_L = DataStartPosition_L;
        while((status_L)&&(ii_L<strlen(pRawResponse_L))&&(pRawResponse_L[ii_L-1]!='\r'))
        {
          if((pRawResponse_L[ii_L]==',')||(pRawResponse_L[ii_L]=='\r'))
          {
            if(fieldCount_L<fieldCount_P)
            {
              pFieldContent_P[fieldCount_L]=(char *)malloc(ii_L-DataStartPosition_L+1);
              if(pFieldContent_P[fieldCount_L] == NULL)
              {
                trace(F("\nNo more available memory\n\r"));
                status_L = false;
              }
              else
              {
                strncpy(pFieldContent_P[fieldCount_L], &pRawResponse_L[DataStartPosition_L], ii_L-DataStartPosition_L);
                *(pFieldContent_P[fieldCount_L]+ii_L-DataStartPosition_L)='\0';
                fieldCount_L++;
                DataStartPosition_L = ii_L+1;
              }
            }
            else
            {
              traceln(F("To many data items received !"));
              status_L = false;
            }
          }
          ii_L++;
        }
        if (status_L)
        {
          if(fieldCount_L>fieldCount_P)
          {
            traceln(F("To many fields received in response"));
            status_L = false;
          }
        }
      }
      else
      {
        traceln(F("Answer %s not found in response"),answer_L);
        status_L = false;
      }
      free(pRawResponse_L);
    }
    else
    {
      status_L = false;
    }
  }
  if(status_L)
  {
    if(fieldCount_L<fieldCount_P)
    {
      traceln(F("Data fields are missing in response."));
      status_L = false;
    }
  }
  if(!status_L)
  {
	for(ii_L=0;ii_L<fieldCount_L;ii_L++)
    {
      free(pFieldContent_P[ii_L]);
    }
  }
  return(status_L);
}

sim808::reqResult_t sim808::atRequest(char** ppPayload_P, const char* command_P, va_list pArgument_P)
{
  reqResult_t result_l;
  char  *pAtCommand_L, *pAtCommand2_L, *pReadChars_L, *pBeginPayload_L;
  size_t longueur_L;

  //Valeurs initales:
  result_l = REQ_READ_OK;

  pAtCommand_L = (char*)calloc(strlen_P(command_P)+2, sizeof(char));  // On prévoit de la place pour les caractères '\r' et '\0'  
  if(pAtCommand_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
    result_l = REQ_PROCESSING_ERROR;
  }
  else
  {
    strcpy_P(pAtCommand_L, command_P);
    longueur_L = strlen_P(command_P);
    *(pAtCommand_L+longueur_L)='\r';  // Ajout du <CR> de fin de commande
    *(pAtCommand_L+longueur_L+1)='\0';  // Ajout du caractère de fin de chaine
    pAtCommand2_L = (char*)calloc(strlen_P(command_P)+30+2, sizeof(char));  // On prévoit de la place pour 30 caractères de valeur des paramètres
    if(pAtCommand2_L == NULL)
    {
      trace(F("\nNo more available memory\n\r"));
      free(pAtCommand_L);
      result_l = REQ_PROCESSING_ERROR;
    }
    else
    {
      vsnprintf(pAtCommand2_L, strlen_P(command_P)+30+2, pAtCommand_L, pArgument_P);
      free(pAtCommand_L);
      sim808Interface->print(pAtCommand2_L);
      sim808Interface->flush();
      traceln(F("Sent: %s"), pAtCommand2_L);
      result_l = read(&pReadChars_L);
      if(result_l==REQ_READ_OK)
      {
        // Suppression  du/des écho(s)
        pBeginPayload_L = pReadChars_L;
        while(strncmp (pBeginPayload_L, pAtCommand2_L, strlen(pAtCommand2_L))==0)
        {
          pBeginPayload_L+=strlen(pAtCommand2_L);  
        }
        if(ppPayload_P!=NULL)
        {
          // Un payload est attendu
          *ppPayload_P = (char*)malloc(strlen(pBeginPayload_L+1)); 
          if(*ppPayload_P == NULL)
          {
            trace(F("\nNo more available memory\n\r"));
            result_l = REQ_PROCESSING_ERROR;
          }
          else
          {
            strcpy(*ppPayload_P, pBeginPayload_L);
          }
        }
		    free(pReadChars_L);
      }
      free(pAtCommand2_L);
    }
  }
  return(result_l);
}

sim808::reqResult_t sim808::read(char **ppReadChars_P)
{
  reqResult_t result_L;
  elapsedMillis chronometre_L;
  char *pNextArea_L;
  bool readAnswer_L;
  char AtOk_L[strlen_P(AT_OK)];
  char AtError_L[strlen_P(AT_ERROR)];
  int countMaxChars_L, countChars_L;

  //Valeurs initales:
  chronometre_L = 0;
  countChars_L = 0;
  countMaxChars_L = 30;
  readAnswer_L = true;
  result_L = REQ_READ_OK;

  // Les accès à la mémoire SRAM sont plus rapides que les accès à la FLASH.
  strncpy_P(AtOk_L, AT_OK, sizeof(AtOk_L));
  strncpy_P(AtError_L, AT_ERROR, sizeof(AtError_L));

  *ppReadChars_P=(char *)malloc(countMaxChars_L+1);
  if(*ppReadChars_P == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
    result_L = REQ_PROCESSING_ERROR;
  }
  else
  {
    trace(F("Received: "));
    while (readAnswer_L)
    {
      if(sim808Interface->available() > 0)
      {
        *(*ppReadChars_P+countChars_L) = sim808Interface->read();
        if(  
            (*(*ppReadChars_P+countChars_L)!='\n')&&
            (*(*ppReadChars_P+countChars_L)!='\r')&&
            ((*(*ppReadChars_P+countChars_L)>126)||(*(*ppReadChars_P+countChars_L)<32))
          )
        {
          trace(F("<!>"));
        }
        else
        {
          if(*(*ppReadChars_P+countChars_L)=='\n')
          {
            trace(F("\\n"));
          }
          else if(*(*ppReadChars_P+countChars_L)=='\r')
          {
            trace(F("\\r"));
          }
          else
          {
            trace(F("%c"), *(*ppReadChars_P+countChars_L));
          }
          if(*(*ppReadChars_P+countChars_L) ==0) // Est-ce un caractère de fin de chaine ?
          {
            result_L = REQ_READ_END_OF_STRING;
            trace(F(" - End of string received"));
            readAnswer_L = false;
          }
          if(strncmp((*ppReadChars_P+countChars_L)-sizeof(AtOk_L)+1, AtOk_L, sizeof(AtOk_L))==0)
          {
            result_L = REQ_READ_OK; 
            trace(F(" - OK received"));
            countChars_L = countChars_L - sizeof(AtOk_L)+1; // On supprime le statut du message de reponse
            readAnswer_L = false;
          }
          else if(strncmp((*ppReadChars_P+countChars_L)-sizeof(AtError_L)+1, AtError_L, sizeof(AtError_L))==0)
          {
            result_L = REQ_READ_ERROR; 
            trace(F(" - Error received"));
            countChars_L = countChars_L - sizeof(AtError_L)+1; // On supprime le statut du message de reponse
            readAnswer_L = false;
          }

          countChars_L++;
          if (countChars_L>=countMaxChars_L)
          {
            countMaxChars_L+=30;
            pNextArea_L=(char*)realloc(*ppReadChars_P, countMaxChars_L);
            if(pNextArea_L == NULL)
            {
              trace(F("\nNo more available memory\n\r"));
              result_L = REQ_PROCESSING_ERROR;
              readAnswer_L = false;
            }
            else
            {
              *ppReadChars_P = pNextArea_L;
            }
          }
        }        
      }
      else if (chronometre_L>responseDelay)
      {
        result_L = REQ_TIMEOUT;
        trace(F(" - Timeout, %d chars received"), countChars_L);
        readAnswer_L = false;
      }
    }
    trace(F("\n"));
    if(countChars_L>0)
    {
      *(*ppReadChars_P+countChars_L) ='\0';    // Caractère de fin de chaine
    }
    if(result_L != REQ_READ_OK)
    {
      free(*ppReadChars_P);
    }
  }
  return(result_L); 
}

void  sim808::setResponseDelay(unsigned long responseDelay_P)
{
  responseDelay = responseDelay_P;
}

void sim808::trace(const __FlashStringHelper* message_P, ...)
{
#if DEBUG_SIM808 == TRUE
  char *pBuffer_L;
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  pBuffer_L = (char*)malloc(strlen_P((const char*)message_P)+1); 
  if(pBuffer_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
  }
  else
  {
    strcpy_P(pBuffer_L, (const char*)message_P);
    va_start(pArgument_L, message_P); // initializes the pointer to point to the first unnamed (...) argument
    basicTrace(pBuffer_L, pArgument_L);
    va_end (pArgument_L);
    free(pBuffer_L);
  }
#else
  message_P=message_P;
#endif
}

void sim808::traceln(const __FlashStringHelper* message_P, ...)
{
#if DEBUG_SIM808 == TRUE
  char *pBuffer_L;
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  pBuffer_L = (char*)malloc(strlen_P((const char*)message_P)+2); 
  if(pBuffer_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
  }
  else
  {
    strcpy_P(pBuffer_L, (const char*)message_P);
    *(pBuffer_L+strlen_P((const char*)message_P))='\n';  // Ajout du retour à la ligne
    *(pBuffer_L+strlen_P((const char*)message_P)+1)=0;   // Ajout du caractère de fin de chaîne
    va_start(pArgument_L, message_P); // initializes the pointer to point to the first unnamed (...) argument
    basicTrace(pBuffer_L, pArgument_L);
    va_end (pArgument_L);
    free(pBuffer_L);
  }
#else
  message_P=message_P;
#endif
}

void sim808::trace(char* message_P, ...)
{
#if DEBUG_SIM808 == TRUE
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  va_start(pArgument_L, message_P); // initializes the pointer to point to the first unnamed (...) argument
  basicTrace(message_P, pArgument_L);
  va_end (pArgument_L);
#else
  message_P=message_P;
#endif
}

void sim808::traceln(char* message_P, ...)
{
#if DEBUG_SIM808 == TRUE
  char* pBuffer_L;
  va_list pArgument_L; // Pointeur permettant de parcourir la liste des paramètres

  va_start(pArgument_L, message_P); // initializes the pointer to point to the first unnamed (...) argument
  pBuffer_L = (char*)malloc(strlen(message_P)+2); 
  if(pBuffer_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
  }
  else
  {
    strcpy(pBuffer_L, (const char*)message_P);
    *(pBuffer_L+strlen(message_P))='\n';  // Ajout du retour à la ligne
    *(pBuffer_L+strlen(message_P)+1)=0;   // Ajout du caractère de fin de chaîne
    basicTrace(pBuffer_L, pArgument_L);
    free(pBuffer_L);
  }
  va_end (pArgument_L);
#else
  message_P=message_P;
#endif
}

void sim808::basicTrace(char* message_P, va_list pArgument_P)
{
#if DEBUG_SIM808 == TRUE
  char* pBuffer_L;
  size_t length_L=strlen(message_P)+50;

  pBuffer_L = (char*)malloc(length_L);
  
  if(pBuffer_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
  }
  else
  {
    vsnprintf(pBuffer_L, length_L, message_P, pArgument_P);
    Serial.print(pBuffer_L);
    free(pBuffer_L);
  }
#else
  message_P=message_P;
  pArgument_P = pArgument_P;
#endif
}

unsigned long int sim808::strntoul (char* str, char** endptr, int base, int len)
{
  char              *pBuffer_L;
  unsigned long int result_L;
  char              *endConversion_L;

  result_L = 0;
  pBuffer_L = (char*)calloc(len+1, sizeof(char));
  if(pBuffer_L == NULL)
  {
    trace(F("\nNo more available memory\n\r"));
    *endptr = str;
  }
  else
  {
    strncpy(pBuffer_L, str, len);
    result_L = strtoul (pBuffer_L, &endConversion_L, base);
    *endptr = str + (endConversion_L-pBuffer_L);
    free(pBuffer_L);
  }
  return(result_L);
}