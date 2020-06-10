/*
   Author: Safa Awadalla
   Project: Shade Controller WiFi
   Date: 2020 04 20
   Summary:
   The sketch configures the Arduino to function as a shade controller by
   accomplishing the following tasks in order.
   1. Using any device with WiFi capability the user searches for an unsecured network that is advertised by the Arduino. For now I have named it “Always Sunny”, but we can change it if anyone objects to that network name.
   2. Once the user’s device has successfully obtained access to the network they will navigate to 192.168.4.1.  Then will then be presented with a web Page that allows them to enter the credentials for their home network (SSID and password).
      a. The Arduino will store these values in its on board non-volatile flash memory.
   3. Once the user has satisfactorily entered their network credentials they will then click a reset button the web page.  This will send a command to the Arduino to perform a soft reset.
   4. When the Arduino boots up this time it will realize that network credentials have been setup and attempt to connect to the user's network.
   5. Once logged into a network with Internet access, the Arduino also reaches out to 4 different servers on the world wide web for different services
      a. freegeoip.app provides IP-based location information that is later used to obtain weather forecasts
      b. api.openweathermap.org provides 3 weather forecasts evenly spaced at 3 hour intervals as well as sunrise and sunset times and timezone information
      c. time.nist.gov NTP server to syncronize the Arduino's system time
      d. Our webserver at http://72.14.190.151 to get current .css and .js files when our webpage is rendered in the user's browser.
   6. The Arduino is always monitoring wind, temperature, and light. These measurements are used to make decisions about when to raise/lower the shade
   7. If a valid sunrise/sunset time has been obtained from the weather service, the Arduino can automate control based on sunrise and sunset times   
   8. The user will be able to navigate to http://asacontroller and they will be presented a new web page that contains features to control the shade

    !!!!   NOTE: This shade controller operates as an open loop system.       !!!!
    !!!!   It does no know if the shade is up or down. This means it will     !!!!
    !!!!   send raise and lower commands even if the shade is already in the  !!!!
    !!!!   position to which it is being commanded.                           !!!!
*/


// include necessary libraries

#include <SPI.h>          //required to maintain wifi, server, and client services
#include <WiFiNINA.h>     //required to maintain wifi, server, and client services
#include <FlashStorage.h> //required to utilize the built-in flash as long term storage
#include <WDTZero.h>      //required to implement the watchdog
#include <ArduinoJson.h>  //required to parse weather and location data
#include <time.h>         //required to manipulate times associated with weather data
#include <WiFiUdp.h>      //required to implement to handle UDP data associated with the NTP service


//struct used to store network details
typedef struct {
  char valid[100];
  char ssid[100];
  char password[100];
} Network;

//struct used to store the windsensor data
typedef struct {
  float tempC;
  float windMPH;
} WindSensor;

//struct used to store weatherRecord data
typedef struct {
  int timezone;
  struct tm epoch;
  String timeDescription;
  float wind;
  String condition;
  float temp;
  unsigned long sunrise;
  unsigned long sunset;
} Weather;

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

char locationServer[] = "freegeoip.app";    // name address for freegeoip site

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

String apiKey = "2903bfdfa29043b14ac72de4f61986a7"; //open weather map api key

String location = "76522"; //the zipcode you want the weather for

char weatherServer[] = "api.openweathermap.org";

Weather weatherArray[3]; //used to store the next 3 weather records in the forecast

struct tm sunrise;  //stores the sunrise time

struct tm sunset;   //stores the sunset time

int timezone = -14400; //EST timezone (-4 hours)

WDTZero MyWatchDog; //create watchdog object

FlashStorage(storedNetwork, Network); //setup the flash storage for use in the program

Network thisNetwork; //network instance named "thisNetwork"

WindSensor thisWindSensor; //create an instane of the windsensor structure.

//time keeping variables
unsigned long target_time_one = 0L;
unsigned long target_time_two = 0L;
unsigned long target_time_three = 0UL;
unsigned long target_time_four = 0UL;
#define FIVEMINUTES (5*60*1000L)
#define THREEHOURS (120*60*1000L)
#define ONESECONDS (1000UL)
#define ONEMINUTE (60*1000UL)

unsigned long secsSince1900 = 0UL; //seconds since 1900 provided by NTP service
unsigned long epochNTP = 0UL;
unsigned long millisec = 0UL;


int status = WL_IDLE_STATUS; //wifi connection status variable

WiFiServer server(80); //server used to serve web pages to clients

WiFiClient weatherClient; //client used to request weather data

WiFiSSLClient locationClient; //SSL client used to request location data

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// variables used to keep track of the client responses
String responseArray[10] = {"", "", "", "", "", "", "", "", "", ""};
int responseCount = 0;

// pin definitions
int led = LED_BUILTIN;    //digital output pin used to control the builtin LED
int downPin = 7;          //digital output pin used to control the down movement of the shade
int upPin = 8;            //digital output pin used to control the up movement of the shade
const int lightPin = A2;  //analog pin for lux measurements

// setting variables
//int controlSetting = 0; //manual
//int controlSetting = 1; //temp
//int controlSetting = 2; //light
int controlSetting = 3; //sun
int safetyResetCounter = 0;
bool setupRequired = true;

//wind sensor variables
float windToRaise = 5.0;
float windToLower = 3.0;
int quickWindRaiseCounter = 0;  //used to perform wind checks every minute.
int windRaiseCounter = 0;       //used to perform wind checks every 5 miuntes
int windLowerCounter = 0;       //used to perform wind checks every 5 minutes
int quickWindAnomolyLimit = 5;  // used to perform wind checks every minute.  if the wind is elevated for 5 seconds during any one minute the shade will raise
int windAnomolyLimit = 10;      // results in the wind being anomolous for 10 seconds in a 5 minute period
//String WIND = "\"\"";
const int SAFETYDELAY = 1800; // number of seconds the the safety delay will remain on.
int safetyTimer = 0;

//temp sensor variables
float tempToLower = 20.0;
float tempToRaise = 15.0;
int tempLowerCounter = 0;
int tempRaiseCounter = 0;
int tempAnomolyLimit = 60;  //  results in the temp being anomolous for 1 minute in a 5 minute period
//String TEMP = "\"\"";

//light sensor variables
float lightToLower = 300.0;
float lightToRaise = 50.0;
int lightRaiseCounter = 0;
int lightLowerCounter = 0;
int lightAnomolyLimit = 60; // results in the light being anomolous for 1 minute in a 5 minute period

int lightMeasurementCount = 0;
float lightArray[50];
float lux = 0.0;

/*-----------------------------------------------------------------*/

/*
   Tasks performed in the setup method

   1. Set the analog resolution to 12 bits.
   2. Set the serial connection up to allow for easier troubleshooting
   3. Define the pin modes for the various pins
   4. Read the memory to get the stored network settings.  This is used later to determine if this
   device is going to become an access point or become a member of the network
   5. Initialize the wifi chip
   6. either start up in access point mode or start up in network member mode\
*/
void setup() {

  //set the analog resolution to 12 bits
  analogReadResolution(12);

  //setup a serial communications port for testing purposes
  Serial.begin(9600);
//  while (!Serial) {
//    ; //halt setup until a serial connection is made.
//  }

  //set digital pins modes, set default values
  pinMode(downPin, OUTPUT);   //pin applying gate voltage to NMOS transistor to send down signal to the shade
  digitalWrite(downPin, LOW);

  pinMode(upPin, OUTPUT);     //pin applying gate voltage to NMOS transistor to send up signal to the shade
  digitalWrite(upPin, LOW);

  pinMode(led, OUTPUT);      // set the LED pin mode
  digitalWrite(led, LOW);

  printLine(); //print a dashed line to the serial output to help with readablity

  //read in any stored network settings.
  thisNetwork = storedNetwork.read();
  Serial.println("   Valid Network: " + String(thisNetwork.valid));
  Serial.println("    Network SSID: " + String(thisNetwork.ssid));
  Serial.println("Network Password: " + String(thisNetwork.password));

  //check that the wifi module is working
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Communication with WiFi module failed!");
    while (true); //halt here if the module is not installed or not working
  }

  // obtain and display the firmware version
  String firmware = WiFi.firmwareVersion();
  Serial.println("        Firmware: " + firmware);

  // obtain and display the MAC
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.println("             MAC: " + printMacAddress(mac));


  /* Check to see if there is are network configuration parameters.  If there are
      not any valid configurations then we will create an open network.  Otherwise,
      we will attempt to connect to the defined network.
  */
  if (String(thisNetwork.valid) != "valid") { // create an open network if no settings are present
    char ssid[] = "Always Sunny";
    printLine();
    Serial.println("Creating an open network");
    // Create open network WPA network
    status = WiFi.beginAP(ssid);
    if (status != WL_AP_LISTENING) {
      Serial.println("Creating access point failed");
      // don't continue
      while (true);
    }
    // wait 5 seconds for connection:
    delay(5000);
    server.begin();
    printLine();
  } else { //attempt to connect if network settings are present
    setupRequired = false;
    String password = thisNetwork.password;
    String chosenNetwork = thisNetwork.ssid;
    printLine();
    while (status != WL_CONNECTED) {
      char pass[password.length() + 1];
      password.toCharArray(pass, password.length() + 1);
      char ssid[chosenNetwork.length() + 1];
      chosenNetwork.toCharArray(ssid, chosenNetwork.length() + 1);
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network:
      WiFi.setHostname("asaController");
      status = WiFi.begin(ssid, pass);
      // wait 10 seconds for connection:
      delay(10000);
      server.begin();
    }
    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
    printCurrentNet();
    printWifiData();
    printLine();
    getLocation();
    delay(2000); //allow time for the location data to be obtained.
    getWeather();
    printLine();
    Udp.begin(localPort);
    timeSync();
    
  }

}


/***************************Main Loop below this line***************************/

/*
   Tasks performed in the main method:

   1. read the wind sensor
   2. Check to see if the wifi status is connected
   3. either start a networked server or isolated access point
   4. check to see if it is safe to automate
        Conditions for automation:
        1. Wind levels are safe
          a. wind levels are safe when the wind has not broken 5 mph for 30 minutes
        2. user has allowed automation by either temperature or light

*/


void loop() {

  //perform wireless access checks
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }


  if (setupRequired == false) {
    networkedServer();
  } else {
    isolatedServer();
  }

  // take required measurements to make decisions later
  thisWindSensor = getWindSensorData(); //measure wind and temperature
  lux = measureLight(); // measure light levels

  // perform actions that need to be executed at a specified interval
  if ((millis() - target_time_two) >= THREEHOURS) {
    target_time_two += THREEHOURS; //maintains time period
    getWeather();
  }
  if ((millis() - target_time_three) >= ONESECONDS) {
    target_time_three += ONESECONDS; //maintains time period
    updateTime();

    /* countdown for safety timer.  The safety timer is used to prevent automation if the wind is
       detected to be >= the wind limit for more than 30 seconds of a 5 minute period or
       5 seconds in during any one minute period
    */
    //reduce the safety timer by 1 if statement is required to prevent the overflow that would occur every 9.1 days
    if (safetyTimer > 0 ) {
      safetyTimer = safetyTimer - 1;
    }

    //check wind conditions
    greaterThanConditionCheck(windToRaise, thisWindSensor.windMPH, quickWindRaiseCounter, quickWindAnomolyLimit); //one minute check
    greaterThanConditionCheck(windToRaise, thisWindSensor.windMPH, windRaiseCounter, windAnomolyLimit);
    lessThanConditionCheck(   windToLower, thisWindSensor.windMPH, windLowerCounter, windAnomolyLimit);

    //check temp condition
    greaterThanConditionCheck(tempToLower, thisWindSensor.tempC, tempLowerCounter, tempAnomolyLimit);
    lessThanConditionCheck(   tempToRaise, thisWindSensor.tempC, tempRaiseCounter, tempAnomolyLimit);

    //check light condition
    greaterThanConditionCheck(lightToLower, lux, lightLowerCounter, lightAnomolyLimit);
    lessThanConditionCheck(   lightToRaise, lux, lightRaiseCounter, lightAnomolyLimit);

    //lessThanConditionCheck(   tempToRaise, thisWindSensor.tempC, tempRaiseCounter, tempAnomolyLimit);
  }


  if ((millis() - target_time_four) >= ONEMINUTE) {
    target_time_four += ONEMINUTE; //maintains time period
    if (setupRequired == false) {
    timeSync();
    }
    if ((greaterThanConditionCheck(windToRaise, thisWindSensor.windMPH, quickWindRaiseCounter, quickWindAnomolyLimit)) && (safetyTimer < 1)) {
      raiseShade();
      safetyTimer = SAFETYDELAY;
    }
    quickWindRaiseCounter = 0; //reset the quickWindRaiseCounter back to zero in preparation for the next minute
    if (setupRequired == false) {
    printReport();
    }
  }


  // decide what the sunshade needs to do by checking on the historical anomolies for the last 5 minutes
  // perform condition checks every 5 minutes
  if ((millis() - target_time_one) >= FIVEMINUTES) {
    target_time_one += FIVEMINUTES; //maintains time period
    if (safetyTimer < 1) {
      //wind check to ensure the wind is at a safe level
      if (greaterThanConditionCheck(windToRaise, thisWindSensor.windMPH, windRaiseCounter, windAnomolyLimit)) {
        Serial.println(epochToString(epochNTP) + "Elevated wind levels detected...Raising...");
        raiseShade();
        safetyTimer =  SAFETYDELAY;
      } else { //check the condition that corresponds with the selected control setting
        switch (controlSetting) {
          case 1:
            //temp check
            if (lessThanConditionCheck( tempToRaise, thisWindSensor.tempC, tempRaiseCounter, tempAnomolyLimit)) {
              Serial.println(epochToString(epochNTP) + "Low temperature levels detected...Raising...");
              raiseShade();
            } else if (greaterThanConditionCheck(tempToLower, thisWindSensor.tempC, tempLowerCounter, tempAnomolyLimit)) {
              Serial.println(epochToString(epochNTP) + "High temperature levels detected...Lowering...");
              lowerShade();
            }
            break;
          case  2:
            //light check
            if (lessThanConditionCheck( lightToRaise, lux, lightRaiseCounter, lightAnomolyLimit)) {
              Serial.println(epochToString(epochNTP) + "Low light levels detected...Raising...");
              raiseShade();
            } else  if (greaterThanConditionCheck( lightToLower, lux, lightLowerCounter, lightAnomolyLimit)) {
              Serial.println(epochToString(epochNTP) + "High light levels detected...Lowering...");
              lowerShade();
            }
            break;
          case 3:
            //time sun
             //this code only runs if we have successfully obtained the sunrise/set time from the weather service at least once.
            if (weatherArray[0].sunset != NULL) {
              unsigned long test = 0UL;
              int futureTime = epochNTP + 300;
              Serial.println("epoch: " + String(epochNTP));//5 minutes into the future
              Serial.println("sunrise: " + String(weatherArray[0].sunrise));
              Serial.println("time until sunrise: " + String(futureTime - int(weatherArray[0].sunrise)));
              Serial.println("time until sunset: " + String(futureTime - int(weatherArray[0].sunset)));
              if (abs(futureTime - int(weatherArray[0].sunrise)) < 300) {
                Serial.println(epochToString(epochNTP) + "Sunrise lower the shade");
                lowerShade();
              } else if (abs(futureTime - int(weatherArray[0].sunset)) < 300) {
                Serial.println(epochToString(epochNTP) + "Sunset raise the shade");
                raiseShade();
              }
            }
            break;
        }


      }
    }
    resetAllCounters();  //reset all counters after making decisions during this 5 minute increment
  }
}
/***************************Other Methods below this line***************************/

// Method used to print a line to the serial
void printLine() {
  Serial.println("--------------------------------------");
}

// Method used to print the MAC Address
String printMacAddress(byte mac[]) {
  String macAdd = "";
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      //Serial.print("0");
      macAdd = macAdd + "0";
    }
    //Serial.print(mac[i], HEX);
    macAdd = macAdd + String(mac[i], HEX);
    if (i > 0) {
      //Serial.print(":");
      macAdd = macAdd + ":";
    }
  }
  //Serial.println();
  return macAdd;
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
/******************************getWindSensorData()****************************************/
/*
   returns a WindSensor Object
   gets wind and temperature data from the Modern Device sensor
   wind is measured in MPH and temperature is measured in degrees C

*/
WindSensor getWindSensorData() {
  WindSensor sensor;
  const int OutPin  = A0;
  const int TempPin = A1;


  float windArray[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float tempArray[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float tempC = 0.0;
  float windMPH = 0.0;
  for (int i = 0; i < 5; i++) {
    // read wind
    int windADunits = analogRead(OutPin);
    // Serial.print("RW ");   // print raw A/D for debug
    // Serial.print(windADunits);
    // Serial.print("\t");

    // temp routine and print raw and temp C
    int tempRawAD = analogRead(TempPin);
    // Serial.print("RT ");    // print raw A/D for debug
    // Serial.print(tempRawAD);
    // Serial.print("\t");

    // convert to volts then use formula from datatsheet
    // Vout = ( TempC * .0195 ) + .400
    // tempC = (Vout - V0c) / TC   see the MCP9701 datasheet for V0c and TC

    //float tempC = ((((float)tempRawAD * 5.0) / 1024.0) - 0.400) / .0195;
    tempC = ((((float)tempRawAD * 3.3) / 4096.0) - 0.500) / .0195;
    //Serial.print(tempC);
    //Serial.println(" C");

    // wind formula derived from a wind tunnel data, annemometer and some fancy Excel regressions
    // this scalin doesn't have any temperature correction in it yet
    //float windMPH =  pow((((float)windADunits - 264.0) / 85.6814), 3.36814);
    windMPH =  pow((((float)windADunits - 1600.0) / 341.74), 3.36814);
    //float windMPH = pow(((3.3-1.371)/(3.038517*(pow(tempC,0.115157)))/0.08288),3.009364);
    //Serial.print(windMPH);
    //Serial.print(" MPH\t");
    //delay(10);
    tempArray[i] = tempC;
    windArray[i] = windMPH;
  }

  sensor.tempC = average(tempArray, 5);
  sensor.windMPH = average(windArray, 5);
  return sensor;
}

/***************average(float array[]) method******************/
/*
   This is a simple method that takes an array of 5 values and
   returns the average of the values

*/
float average(float anArray[], int count) {
  float sum = 0;
  float average = 0;
  for (int i = 0; i < count; i++) {
    sum += anArray[i];
  }
  average = sum / count;
  return average;
}
/*******************raiseShade() Method*************************/
/*
   This method sends the command to raise the shade and illuminate
   the default LED for 1.5 seconds.  The raise command and LED are
   then turned off

*/
void raiseShade() {
  digitalWrite(led, HIGH);
  digitalWrite(upPin, HIGH);
  delay(1500);
  digitalWrite(led, LOW);
  digitalWrite(upPin, LOW);
}
/*******************lowerShade() Method*************************/
/*
   This method sends the command to lower the shade and illuminate
   the default LED for 1.5 seconds.  The lower command and LED are
   then turned off

*/
void lowerShade() {
  digitalWrite(led, HIGH);
  digitalWrite(downPin, HIGH);
  delay(1500);
  digitalWrite(led, LOW);
  digitalWrite(downPin, LOW);
}
bool greaterThanConditionCheck(float threshold, float currentReading, int & conditionCounter, int activationCount) {
  bool raiseNow = false;
  //Serial.println(String(threshold) + "   :   " + String(currentReading) + "   :   " + String(conditionCounter) + "   :   " + String(activationCount));
  if (currentReading >= threshold) {
    conditionCounter ++;
  }
  if (conditionCounter >= activationCount) {
    raiseNow = true;
  }
  return raiseNow;
}
bool lessThanConditionCheck(float threshold, float currentReading, int & conditionCounter, int activationCount) {
  bool lowerNow = false;
  if (currentReading < threshold) {
    conditionCounter ++;
  }
  if (conditionCounter >= activationCount) {
    lowerNow = true;
  }
  return lowerNow;
}
float measureLight() {
  float luxMeasurement = lux;
  if (lightMeasurementCount < 50) {  //take 50 readings and store them so that they can be averaged
    lightArray[lightMeasurementCount] = analogRead(lightPin);
    lightMeasurementCount ++;
  }

  if (lightMeasurementCount == 50) { //average the measurements and return the result in Lux
    float raw = average(lightArray, lightMeasurementCount);
    luxMeasurement = .5395 * exp(0.0017 * raw);
    //String result = "raw: " + String(raw) + " lux: " + String(luxMeasurement);
    //String result = String(raw);
    //Serial.println(result);
    lightMeasurementCount = 0;

  }
  return luxMeasurement;
}
/*
   this method is used to print generate and print response lines that are returned from the client
   The array of responses are erased to prepare for the next client response.
*/
void printResponses() {
  for (int i = 0; i < 10; i = i + 1) {
    Serial.println(responseArray[i]);
    responseArray[i] = "";
    responseCount = 0;
  }
}
void printNetworkSettings() {
  Serial.println("   Valid Network: " + String(thisNetwork.valid));
  Serial.println("    Network SSID: " + String(thisNetwork.ssid));
  Serial.println("Network Password: " + String(thisNetwork.password));
}
void getWeather() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (weatherClient.connect(weatherServer, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    weatherClient.print("GET /data/2.5/forecast?");
    weatherClient.print("zip=" + location);
    weatherClient.print("&appid=" + apiKey);
    weatherClient.print("&cnt=3");
    weatherClient.println("&units=metric");
    weatherClient.println("Host: api.openweathermap.org");
    weatherClient.println("Connection: close");
    weatherClient.println();
  } else {
    Serial.println("unable to connect");
  }
  delay(1000);
  String line = "";
  while (weatherClient.connected()) {
    line = weatherClient.readStringUntil('\n');
    //Serial.println(line);
    Serial.println("parsingValues");
    //create a json buffer where to store the json data
    StaticJsonDocument<5000> doc;
    deserializeJson(doc, line);
    //get the data from the json tree

    timezone = doc["city"]["timezone"];
    sunrise = getCustomTime(timezone , doc["city"]["sunrise"]);
    sunset = getCustomTime(timezone , doc["city"]["sunset"]);

    for (int i = 0; i < 3; i++) {
      Weather weatherRecord;
      weatherRecord.timezone = timezone;
      weatherRecord.epoch = getCustomTime(timezone, doc["list"][i]["dt"]);
      String nextWeatherTime = stringDateStamp(weatherRecord.epoch);
      weatherRecord.timeDescription = nextWeatherTime;
      weatherRecord.wind = float(doc["list"][i]["wind"]["speed"]) / 1609.34 * 3600;
      String nextWeather = doc["list"][i]["weather"][0]["description"];
      weatherRecord.condition = nextWeather;
      weatherRecord.temp = doc["list"][i]["main"]["temp"];
      weatherRecord.sunrise = int(doc["city"]["sunrise"]);
      weatherRecord.sunset = int(doc["city"]["sunset"]);
      weatherArray[i] = weatherRecord;

    }

    // Print values.
    //Serial.println(timezone/3600);
    //Serial.print("Sunrise:");
    //Serial.println(stringTime(sunrise));
    //Serial.print("Sunset:");
    //Serial.println(stringTime(sunset));
    for (int i = 0; i < 3; i++) {
      String conditions = " At " + weatherArray[i].timeDescription + " it will be " + weatherArray[i].condition + " and " + weatherArray[i].temp + " degrees C and winds up to " + weatherArray[i].wind + " MPH";
      // Serial.println(conditions);
    }

  }
}
String stringTime(struct tm tma) {
  String minutes = String(tma.tm_min);
  if (tma.tm_min < 10) {
    minutes = "0" + String(tma.tm_min);
  }
  String seconds = String(tma.tm_sec);
  if (tma.tm_sec < 10) {
    seconds = "0" + String(tma.tm_sec);
  }
  return String(tma.tm_hour) + ":" + minutes + ":" + seconds;
}
String stringDateStamp(struct tm tma) {
  String minutes = String(tma.tm_min);
  if (tma.tm_min < 10) {
    minutes = "0" + String(tma.tm_min);
  }
  String seconds = String(tma.tm_sec);
  if (tma.tm_sec < 10) {
    seconds = "0" + String(tma.tm_sec);
  }
  return String(tma.tm_mon + 1) + "/" + String(tma.tm_mday) + "/" + String(tma.tm_year + 1900) + ", " + String(tma.tm_hour) + ":" + minutes + ":" + seconds;
}
struct tm getCustomTime(int atimezone, int epochTimeGMT) {
  time_t offset = epochTimeGMT + atimezone;
  // aTime = *gmtime(&offset);
  return *gmtime(&offset);
}
void processNTPData() {
  Serial.println("packet received");
  // We've received a packet, read the data from it
  Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  secsSince1900 = highWord << 16 | lowWord;

  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  epochNTP = secsSince1900 - seventyYears; // subtract seventy years:
}
unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}
void timeSync() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);// wait to see if a reply is available
  if (Udp.parsePacket()) {
    processNTPData();
  }
  millisec = millis();
}
String epochToString(unsigned long ep) {
  // print the hour, minute and second:
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  String minutes = "";
  String seconds = "";
  String hours = String(((ep  % 86400L) / 3600)); // print the hour (86400 equals secs per day)
  if (((ep % 3600) / 60) < 10) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    minutes = "0" + String((ep  % 3600) / 60);
  } else {
    minutes = (ep  % 3600) / 60;
  }
  if ((ep % 60) < 10) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    seconds = "0" + String(ep % 60);
  } else {
    seconds = (ep % 60); // print the second
  }
  return String(hours + ":" + minutes + ":" + seconds);

}
void updateTime() {
  epochNTP = epochNTP + 1;
  //Serial.println(epochToString(epochNTP));
}
String printReport() {
  printLine();
  String timeString = epochToString(epochNTP) + "\n";
  String controlString = "Control Method: " + controlToString() + "\n";
  String windString = "Current wind level : " + String(thisWindSensor.windMPH) + " MPH,      Wind thresholds: Raise = " + String(windToRaise) + " MPH, High Wind Anomolies: " + windRaiseCounter + " of " + windAnomolyLimit + " allowed.\n";
  String tempString = "Current temperature: " + String(thisWindSensor.tempC) + " C, Temperature threshold: " + String(tempToRaise) + " C, Low Temp Anomolies: " + tempRaiseCounter + " of " + tempAnomolyLimit + " allowed.\n";
  String lightString = "Current light level: " + String(lux) + " lux,  Low Light threshold: " + String(lightToRaise) + " Lux, Low Light Anomolies: " + lightRaiseCounter + " of " + lightAnomolyLimit + " allowed.\n";
  String reportString =  timeString + controlString + windString + tempString + lightString;

  String HTMLTableHeader = "<table class=\"rwd-table\"><tr><th>Safety Timer: " + String(safetyTimer) + " seconds</th><th></th><th></th><th>Time: " + epochToString(epochNTP + timezone) + "</th><th></th><th>Control: " + controlToString() +
                           "</tr><tr><th>Description</th><th>Current Value</th><th>Lower Shade Threshold</th><th>Lower Anomolies / Anomoly Limit</th><th>Raise Shade Threshold</th><th>Raise Anomolies / Anomoly Limit</th></tr>";
  String HTMLWindRow0 = "<tr><td>Wind (One minute)</td><td>" + String(thisWindSensor.windMPH) + "</td><td>" +
                        String(windToLower) + "</td><td>" +  "N/A </td><td>" +
                        String(windToRaise) + "</td><td>" + quickWindRaiseCounter + "/" + quickWindAnomolyLimit + "</tr>";
  String HTMLWindRow1 = "<tr><td>Wind (five minutes)</td><td>" + String(thisWindSensor.windMPH) + "</td><td>" +
                        String(windToLower) + "</td><td>" +  windLowerCounter + "/" + windAnomolyLimit + "</td><td>" +
                        String(windToRaise) + "</td><td>" + windRaiseCounter + "/" + windAnomolyLimit + "</tr>";
  String HTMLTempRow = "<tr><td>Temperature</td><td>" + String(thisWindSensor.tempC) + "</td><td>" +
                       String(tempToLower) + "</td><td>" +  tempLowerCounter + "/" + tempAnomolyLimit + "</td><td>" +
                       String(tempToRaise) + "</td><td>" + tempRaiseCounter + "/" + tempAnomolyLimit + "</tr>";
  String HTMLLightRow = "<tr><td>Lux</td><td>" + String(lux) + "</td><td>" +
                        String(lightToLower) + "</td><td>" +  lightLowerCounter + "/" + lightAnomolyLimit + "</td><td>" +
                        String(lightToRaise) + "</td><td>" + lightRaiseCounter + "/" + lightAnomolyLimit + "</tr>";
  String endTable = "</tbody></table>";
  String HTMLReportString = HTMLTableHeader + HTMLWindRow0 + HTMLWindRow1 + HTMLTempRow + HTMLLightRow + endTable;
  Serial.println(reportString);
  printLine();
  return HTMLReportString;

}

String printWeatherForecast(){
  printLine();
  String HTMLReportString = "<table class=\"rwd-table\"><tr><th></th><th></th><th>Sunrise: " + stringTime(sunrise) ="</th><th>SunSet: " + stringTime(sunset) +"</th></tr><tr><th>Time</th>    <th>Condition</th>    <th>Temperature (C)</th>    <th>Wind (MPH)</th>  </tr></tbody></table>";
  
  if (weatherArray[0].sunset != NULL) {
  String HTMLTableHeader = "<table class=\"rwd-table\" style=\"margin-left:auto;margin-right:auto;\"><tr><th>Last Updated: " + epochToString(epochNTP +
  timezone) + "</th><th></th><th>Sunrise: " + stringTime(sunrise) + "</th><th>SunSet: " + stringTime(sunset) +"</th></tr>";
  String HTMLColHeader = "<tr><th>Time</th>    <th>Condition</th>    <th>Temperature (C)</th>    <th>Wind (MPH)</th>  </tr>";
  String HTMLRow0 = "<tr><td>" + weatherArray[0].timeDescription + "</td><td>" + weatherArray[0].condition + "</td><td>" + weatherArray[0].temp + "</td><td>" + weatherArray[0].wind +"</td></tr>";
  String HTMLRow1 = "<tr><td>" + weatherArray[1].timeDescription + "</td><td>" + weatherArray[1].condition + "</td><td>" + weatherArray[1].temp + "</td><td>" + weatherArray[1].wind +"</td></tr>";
  String HTMLRow2 = "<tr><td>" + weatherArray[2].timeDescription + "</td><td>" + weatherArray[2].condition + "</td><td>" + weatherArray[2].temp + "</td><td>" + weatherArray[2].wind +"</td></tr>";
  String endTable = "</tbody></table>";

  HTMLReportString = HTMLTableHeader + HTMLColHeader + HTMLRow0 + HTMLRow1 + HTMLRow2 + endTable;
  }
  Serial.println(HTMLReportString);
  return HTMLReportString;
}

/*
 * typedef struct {
  int timezone;
  struct tm epoch;
  String timeDescription;
  float wind;
  String condition;
  float temp;
  unsigned long sunrise;
  unsigned long sunset;
} Weather;
 */

String controlToString() {
  switch (controlSetting) {
    case 1:
      //temp check
      return "Temperature";
      break;
    case  2:
      //light check
      return "Light level";
      break;
    case 3:
      //time sun
      return "Sunset/Sunrise";
      break;
    default:
      return "Manual";

  }
}

void resetAllCounters() {
  windRaiseCounter = 0;
  windLowerCounter = 0;
  tempLowerCounter = 0;
  tempRaiseCounter = 0;
  lightRaiseCounter = 0;
  lightLowerCounter = 0;
}

void getLocation() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (locationClient.connect(locationServer, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    locationClient.println("GET /json/ HTTP/1.1");
    locationClient.println("Host: freegeoip.app");
    locationClient.println("Connection: close");
    locationClient.println();
  } else {
    Serial.println("unable to connect location service");
  }
  delay(1000);
  String line = "";
  while (locationClient.connected()) {
    line = locationClient.readStringUntil('\n');
    //Serial.println(line);
    Serial.println("parsingValues");
    //create a json buffer where to store the json data
    StaticJsonDocument<5000> doc;
    deserializeJson(doc, line);
    //get the data from the json tree
    
      String parsedZipcode = doc["zip_code"];
      if(parsedZipcode != "null"){
      Serial.println("Parsed zipcode: " + parsedZipcode);
      Serial.println("Previous zipcode: " + location);
      location = parsedZipcode;
      Serial.println("New zipcode: " + location);
      Serial.println("disconnecting from server.");
      locationClient.stop();
    }
  }

}
