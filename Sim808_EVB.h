/*
 * sim808_EVB.h - Library to manage the SIM808 Development Kit: https://tropratik.fr/produit/kit-de-developpement-sim808-evb-v3-2-gsm-gprs-gps.
 * Created by Tropratik (https://tropratik.fr).
 * Version 1.1
 * license GNU General Public License Version 3 (GPL-3.0)
 */

#ifndef Sim808_EVB_h
#define Sim808_EVB_h

// Declaration of Arduino libraries used
#include "Arduino.h"
#include <elapsedMillis.h>
#include <Time.h>

/* Definition of global constants */
// generics
#define TRUE 1
#define FALSE 0
#define DEFAULT_RESPONSE_TIME   1000UL // En millisecondes
// Debug constant
#define DEBUG_SIM808 FALSE

/* standard AT commands */
const char AT[]                       PROGMEM = "AT";                 // Teste la présence de la SIM808
const char AT_OK[]                    PROGMEM = "\r\nOK\r\n";
const char AT_ERROR[]                 PROGMEM = "\r\nERROR\r\n";
const char ATE[]                      PROGMEM = "ATE%d";              // Active/Désactive l'écho des commandes
const char AT_CGMR_ASK[]              PROGMEM = "AT+CGMR";            // Demande de la version logicielle
const char AT_CGMR_ANS[]              PROGMEM = "Revision:";          // Début de la réponse de la  version logicielle
const char AT_CGMI_ASK[]              PROGMEM = "AT+CGMI";            // Demande de l'identifiant du fabricant
const char AT_CGMI_ANS[]              PROGMEM = "";                   // Début de la réponse de l'identifiant du fabricant
const char AT_CGMM_ASK[]              PROGMEM = "AT+CGMM";            // Demande de l'identifiant du produit (carte)
const char AT_CGMM_ANS[]              PROGMEM = "";                   // Début de la réponse de l'identifiant du produit (carte)
const char AT_CGSN_ASK[]              PROGMEM = "AT+CGSN";            // Demande du numéro de série du produit (carte)
const char AT_CGSN_ANS[]              PROGMEM = "";                   // Début de la réponse du numéro de série du produit (carte)
/* Commandes AT GNSS */
const char AT_CGNSTST[]               PROGMEM = "AT+CGNSTST=%d";      // Active/Désactive le mode debug du module GNSS
const char AT_CGNSTST_ASK[]           PROGMEM = "AT+CGNSTST?";        // Demande de l'état du mode debug du module GNSS
const char AT_CGNSPWR[]               PROGMEM = "AT+CGNSPWR=%d";      // Active/Désactive l'alimentation module GNSS
const char AT_CGNSPWR_ASK[]           PROGMEM = "AT+CGNSPWR?";        // Demande l'état d'alimentation du module GNSS
const char AT_CGNSCMD[]               PROGMEM = "AT+CGNSCMD= 0,%s";   // Envoie une commande PMTK au module GNSS
const char AT_CGNSSEQ[]               PROGMEM = "AT+CGNSSEQ=%S";      // Paramétrage du type de trames NMEA à utiliser
static const char AT_CGNSSEQ_ASK[]    PROGMEM = "AT+CGNSSEQ?";        // Demande du type de trames NMEA utilisé
static const char AT_CGNSSEQ_ANS[]    PROGMEM = "+CGNSREQ: ";         // Début de la réponse du type de trames NMEA utilisé
const char SENTENCE_GGA[]             PROGMEM = "GGA";                // Identification du type de trame GGA
const char SENTENCE_GSA[]             PROGMEM = "GSA";                // Identification du type de trame GSA 
const char SENTENCE_GSV[]             PROGMEM = "GSV";                // Identification du type de trame GSV 
const char SENTENCE_RMC[]             PROGMEM = "RMC";                // Identification du type de trame RMC
const char AT_CGNSINF_ASK[]           PROGMEM = "AT+CGNSINF";         // Demande des informations GNSS
const char AT_CCGNSURC[]              PROGMEM = "AT+CGNSURC= %d";     // Definition de la fréquence des rapport URC

typedef enum
{
  UNKNOWN,
  GGA,
  GSA,
  GSV,
  RMC
}nmeaSentence_t;

typedef struct
{
  bool positive;
  uint8_t degree;
  uint8_t minute;
  float second;
} sexagesimale_t;

typedef struct
{
  bool status;
  sexagesimale_t latitude;
  sexagesimale_t longitude;
  float altitude;
  float speed;
} gnssCoordinates_t;

typedef struct
{
  char softwareVersion[20];
  char manufacturerId[20];
  char productId[20];
  char productSerialNumber[20];
} identification_t;

typedef struct
{
  uint8_t         gnssInView;
  uint8_t         gnssUsed;
  uint8_t         glonassUsed;
} satellitesInfo_t;

class sim808
{
  public:
    sim808();
    ~sim808(void);
    bool                begin(Stream &serial_P);
    identification_t    getIdentification(void); //  Returns the different identifications of the SIM808
    bool                echoOn(void);   //  Turn on send echo.
    bool                echoOff(void);  //  Turn off send echo.
    bool                gnssPowerOn(void);  //  Power on the GNSS module.
    bool                gnssPowerOff(void); //  Power off the GNSS module.
    bool                connected(void);    //  Return the current status the connexion between the Arduino and SIM808 boards
    bool                getGnssPower(void); //  Returns the current status of the GNSS Power supply
    bool                getGnssRunStatus(void); //  Return the running status of the GNSS module
    bool                getGnssFixStatus(void); //  Return the fixing status of the GNSS module
    gnssCoordinates_t   getGnssCoordinates(void); //  Return the GNSS coordinates obtained
	satellitesInfo_t    getSatellitesInfo(void); //  Get informations about the availabilty of satellites
    bool                setNmeaSentence(nmeaSentence_t sentence_P); // Define the last NMEA sentence that parsed
    nmeaSentence_t      getNmeaSentence(void);  // Returns the current setting of the last NMEA sentence parsed
    time_t              getGnssTime(void);  //  Get the time given by the GNSS module. If not available return 0.
//    bool                gnssDebugOn(void);  //  Activate the trace of data received from GNSS.
//    bool                gnssDebugOff(void); //  Deactivate the trace of data received from GNSS.
//    bool                sendPmtkCommand(char *pPmtkCommand);
//    bool                gnssUrcReportOn(void);  // Turn on navigation data URC report
//    bool                gnssUrcReportOff(void); // Turn off navigation data URC report
//    bool                setGnssUrcReportFrequency(uint8_t frequency_P); // Report every 'frequency_P' GNSS FIX
private:
    typedef enum
    {
      REQ_READ_OK,            	// Answer with status 'OK' 
      REQ_READ_ERROR,        	// Answer with status 'ERROR'
      REQ_READ_END_OF_STRING,	// End of string of received
      REQ_TIMEOUT,          	// Response timeout
      REQ_PROCESSING_ERROR, 	// Processing error
      REQ_ANSWER_INVALID    	// Inconsistency detected in received answer
    }reqResult_t;
      
    typedef struct
    {
      bool            value;
      bool            present;
    } boolWithFlag_t;
     
    typedef struct
    {
      time_t          value;
      bool            present;
    } timeWithFlag_t;
      
    typedef struct
    {
      sexagesimale_t  value;
      bool            present;
    } sexagesimaleWithFlag_t;
      
    typedef struct
    {
      uint32_t        value;
      bool            present;
    } uint32WithFlag_t;

    typedef struct
    {
      float           value;
      bool            present;
    } floatWithFlag_t;
      
    typedef struct
    {
      uint8_t         value;
      bool            present;
    } uint8WithFlag_t;

    typedef struct
    {
      boolWithFlag_t          runStatus;
      boolWithFlag_t          fixStatus;
      timeWithFlag_t          time;
      sexagesimaleWithFlag_t  latitude;
      sexagesimaleWithFlag_t  longitude;
      floatWithFlag_t         altitude;
      floatWithFlag_t         speed;
      floatWithFlag_t         course;
      uint8WithFlag_t         fixMode;
      floatWithFlag_t         hDop;
      floatWithFlag_t         pDop;
      floatWithFlag_t         vDop;
      uint8WithFlag_t         gnssSatellitesInView;
      uint8WithFlag_t         gnssSatellitesUsed;
      uint8WithFlag_t         glonassSatellitesUsed;
      uint8WithFlag_t         cNoMAx;
    } gnssData_t;

    Stream              *sim808Interface=NULL;
    unsigned long       responseDelay;
//    uint8_t             gnssUrcReportFrequency;
//    bool                gnssUrcReportActivated;
    char                softwareVersion[20];    // Allows to manage behavioral differences between software versions

    bool                getGnssTime(time_t *pTime_P);
    bool                getGnssPower(bool *pPowerState_P);
    bool                getGnssRunStatus(bool *pRunStatus_P);
    bool                getGnssFixStatus(bool *pFixStatus_P);
    bool                getNmeaSentence(nmeaSentence_t *pSentence_P);
    bool                getGnssCoordinates(sexagesimale_t *pLatitude_P, sexagesimale_t *pLongitude_P, float *pAltitude_P, float *pSpeed_P);
	bool 				getSatellitesInfo(uint8_t *pGnssSatellitesInView_P, uint8_t *pGnssSatellitesUsed, uint8_t *pGlonassSatellitesUsed);

    bool                getSoftwareVersion(char *pSoftwareVersion_P);
    bool                getManufacturerId(char *pManufacturerId_P);
    bool                getProductId(char *pProductId_P);
    bool                getProductSerialNumber(char *pProductSerialNumber_P);
    bool                extractFromField(char* field_P, bool *pBoolData_P);
    bool                extractFromField(char* field_P, uint8_t *pUint8Data_P);
    bool                extractFromField(char* field_P, float *pFloatData_P);
    bool                extractFromField(char* field_P, double *pDoubleData_P);
    bool                extractFromField(char* field_P, time_t *pTimeData_P);
    bool                extractFromField(char* field_P, sexagesimale_t *pSexagesimaleData_P);
    bool                get(const char* command_P, bool *pBoolData_P, ...);
    bool                get(const char* command_P, char *pCharData_P, ...);
    bool                get(const char* command_P, gnssData_t *pGnssData_P, ...);
    void                setResponseDelay(unsigned long responseDelay_P);
    reqResult_t         read(char **ppReadChars_P);
    bool                simpleAtRequest(const char* command_P, ...);
    bool                atRequestForDataFields(int FieldCount_P, char *pFieldContent_P[], const char* command_P, va_list pArgument_P);
    reqResult_t         atRequest(char** ppPayload_P, const char* command_P, va_list pArgument_P);
    void                trace(const __FlashStringHelper* message_P, ...);
    void                trace(char* message_P, ...);    
    void                basicTrace(char* message_P, va_list pArgument_P);
    void                traceln(const __FlashStringHelper* message_P, ...);
    void                traceln(char* message_P, ...);     
    void                traceln(gnssData_t gnssData_P);
    unsigned long int   strntoul(char* str, char** endptr, int base, int len);
};

#endif