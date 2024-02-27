<h1> SIM808-EVB </h1>

[![SIM808 EVB](https://tropratik.fr/wp-content/uploads/2023/06/Face_Antenne2.png "SIM808 EVB")](https://tropratik.fr/wp-content/uploads/2023/06/Face_Antenne2.png "SIM808 EVB")

<h2> Summary: </h2>

An Arduino library for the [SIM808 EVB card](https://tropratik.fr/produit/kit-de-developpement-sim808-evb-v3-2-gsm-gprs-gps "SIM808 EVB card").

<h2> Installation: </h2>
To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

<h2> Limitations: </h2>
Today only GPS features are implemented.

<h2> How to use: </h2>

**1) library dependency**
This library requires a serial TTL communication interface to rely on. It is recommended to use:
- either a physical interface if available.
- either the Arduino AltSoftSerial library of "Paul Stoffregen”which allows to replace a hardware serial interface.

**2) Declaration**

```cpp
sim808          sim808EVB_G;

```
When using the AltSoftSerial library, you must also declare it.
Example for Arduino UNO:
```cpp
AltSoftSerial   altSoftSerial_G;
```
**3) Initialization**
You must pass the serial interface object as a parameter (hardware or software according to your choice).
Example for Arduino UNO:
```cpp
altSoftSerial_G.begin(38400);
sim808EVB_G.begin(altSoftSerial_G);
```

<h2>Methods: </h2>

- `bool begin(Stream &serial_P);`  	Initialization of the library object
- `identification_t    getIdentification(void);`	Returns the different identifications of the SIM808
- `bool                echoOn(void);`	Turn on send echo.
- `bool                echoOff(void);`	Turn off send echo.
- `bool                gnssPowerOn(void);`	Power on the GNSS module.
- `bool                gnssPowerOff(void);`	Power off the GNSS module.
- `bool                connected(void);` 	Return the current status the connexion between the Arduino and SIM808 boards
- `bool                getGnssPower(void);`	Returns the current status of the GNSS Power supply
- `bool                getGnssRunStatus(void);`	Return the running status of the GNSS module
- `bool                getGnssFixStatus(void);`	Return the fixing status of the GNSS module
- `gnssCoordinates_t   getGnssCoordinates(void);`	Return the GNSS coordinates obtained
- `satellitesInfo_t    getSatellitesInfo(void);`	Get informations about the availabilty of satellites
- `bool                setNmeaSentence(nmeaSentence_t sentence_P);`	Define the last NMEA sentence that parsed
- `nmeaSentence_t      getNmeaSentence(void);`	Returns the current setting of the last NMEA sentence parsed
- `time_t              getGnssTime(void);`	Get the time given by the GNSS module. If not available return 0.

<h2>Tutorial: </h2>

For more detailed explanations, please consult the following article: [Coordonnées GPS avec Arduino et la SIM808arduino-et-la-sim808](https://tropratik.fr/coordonnees-gps-avec-arduino-et-la-sim808 "Coordonnées GPS avec Arduino et la SIM808arduino-et-la-sim808")
