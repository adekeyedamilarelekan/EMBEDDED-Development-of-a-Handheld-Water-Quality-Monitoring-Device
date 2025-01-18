//This is for the Video Explanation account 
// Blynk info = email: richardlennon@gmail.com password: richardlennon01
//Email info =  email: richardlennon@gmail.com password: richardlennon01 
//Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL2OiLhWN7P"
#define BLYNK_TEMPLATE_NAME "Handheld Drinking Water Quality Tester"
#define BLYNK_AUTH_TOKEN "8Bf7V84Ntu6KhYf0wEK-vgY7KZ8scByM"



//This is for the Video Explanation account 
// Blynk info = Email: handheldwaterqualitytester@gmail.com password: handheld@12345 
//Email info =  Email: handheldwaterqualitytester@gmail.com password: handheld01
// This is the newer version. Edit these three (3) below credentials with your BLYNK credentials.
#define BLYNK_TEMPLATE_ID "TMPL2HSZxx4Ra"
#define BLYNK_TEMPLATE_NAME "Handheld Drinking Water Quality Tester"
#define BLYNK_AUTH_TOKEN "5cwXPazbKQLPPVgxmYFJX0jxnlFo-q-v"


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define WiFiLed 26

BlynkTimer timer;

const unsigned long wifiTimeout = 10000;  // WiFi connection timeout in milliseconds
unsigned long wifiStartTime;
bool notificationlogOnceStatus = true;


//LCD2004
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


//DS18B20 Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 27;            // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);          // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature sensor
float temperature;


//TDS sensor variabls.
#define TdsSensorPin 34
#define VREF 3.3           // analog reference voltage(Volt) of the ADC
#define SCOUNT 30          // sum of sample point
int analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;


// Turbidity Sensor variables.
int sensorPin = 36, Vclear = 2.00;
float volt, sensVal;
float ntu;
int samples = 800;

//pH variables.
float calibration_value = 21.34 - 0.3;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
float ph_act;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Deewansonic_Engineering";
char pass[] = "lkjhgfdsa";


void myTimer() {
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  Blynk.virtualWrite(V0, ph_act);
  Blynk.virtualWrite(V1, tdsValue);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, ntu);
}


BLYNK_CONNECTED() {
  // update the latest state to the server
  // Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13);  // will cause BLYNK_WRITE(V0) to be executed
  // or
  Blynk.syncAll();  //ensure to have enabled the "sync" in each datastream in blynk platform/dashboard as without enabling the "sync", the   Blynk.syncAll(); will not work and it will only work for only the datastream's 'sync' that is enabled.
  // or
  // Blynk.virtualWrite(V0, LIGHT);  // Blynk.virtualWrite(V1, FAN);  // Blynk.virtualWrite(V2, SOCKET1);  // Blynk.virtualWrite(V3, SOCKET2);
}

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(TdsSensorPin, INPUT);
  pinMode(WiFiLed, OUTPUT);


  //for the lcd
  lcd.init();           // initialize the lcd
  lcd.backlight();      //lcd backlight
  lcd.setCursor(0, 0);  //setting the lcd cursor
  lcd.print(".......WELCOME......");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(".......WELCOME......");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print(".......WELCOME......");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print(".......WELCOME......");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("         A          ");
  lcd.setCursor(0, 1);
  lcd.print(" HANDHELD DRINKING  ");
  lcd.setCursor(0, 2);
  lcd.print("    WATER QUALITY   ");
  lcd.setCursor(0, 3);
  lcd.print("       TESTER       ");
  delay(3000);
  connectWiFi();
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
    timer.run();
  }


  //Temperature sensor evaluation codes
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  //TDS sensor evaluation codes
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;                                                                                                   // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                                //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                             //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;  //convert voltage value to tds value

    Serial.print("TDS Value:");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");

    Serial.print("Temperature:");
    Serial.print(temperature);
    Serial.println("ºC");
  }

  //Turbidity Sensor evaluation code (function call to execute itself)
  turbidity();


  //pH sensor evaluation codes
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(39);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 3.3 / 4095 / 6;
  ph_act = -5.70 * volt + calibration_value;
  Serial.print("pH Val:");
  Serial.println(ph_act);


  //LCD2004 to display the parameters code
  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(ph_act);
  lcd.setCursor(0, 1);
  lcd.print("TDS: ");
  lcd.print(tdsValue);
  lcd.print("ppm");
  lcd.setCursor(0, 2);
  lcd.print("Temperature: ");
  lcd.print(temperature);
  lcd.print((char)223);  //THis is to print the symbol º degree on LCD.
  lcd.print("C");
  lcd.setCursor(0, 3);
  lcd.print("Turbidity: ");
  lcd.print(ntu);
  lcd.print("%");
  delay(100);
}


/**********************************************************************/
//FUNTIONS
/**********************************************************************/
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}


//For turbidity sensor
void turbidity() {
  volt = 0;
  for (int i = 0; i < samples; i++) {
    //        volt += ((float)analogRead(sensorPin)/1023)*5;
    volt += ((float)analogRead(sensorPin) / 4095) * 3.3;
  }
  volt = volt / samples;

  volt = round_to_dp(volt, 2);
  if (volt < 0.605) {  //when it is half of the cleaniest value which is 2.42. half is 1.21. then say that it is very dirty.
    //    ntu = 3000;  //very dirty water
    ntu = 100;  //very dirty water 100% dirty// mapped into percentages.

  } else {
    //    ntu = -1120.4 * sq(volt) + 5742.3 * volt - 4353.8; //in NTU.
    sensVal = constrain((float)analogRead(sensorPin), 0, 3011);  //3011 is the highest analog value we have gotten from the sensor
    ntu = map(sensVal, 0, 3011, 100, 0);                         //in percentege. // Map this value from 0 - 3011 to 100 and 0 in percentage so that it becomes percentage of cleanliness.
    //    ntu = 100 - (volt / Vclear) * 100; //in percentege.
    if (ntu < 0.00) {
      ntu = 0.00;
    }
  }

  Serial.print("Voltage:");
  Serial.print(volt);
  Serial.print("      ");
  Serial.print("sensorPin:");
  Serial.print(analogRead(sensorPin));
  Serial.print("      ");
  Serial.print("Turbidity NTU:");
  Serial.println(ntu);
}
float round_to_dp(float in_value, int decimal_place) {
  float multiplier = powf(10.0f, decimal_place);
  in_value = roundf(in_value * multiplier) / multiplier;
  return in_value;
}



//**************************************************************************************************
// Function to connect to the internet and after 10secs, timeout and continue with the microcontroller functions.
// If you want offline functionality then you’ll need to attempt a WiFi connection manually and if that is successful use Blynk.config and Blynk.connect instead.
void connectWiFi() {  //This enables the device to work without internet. When the WiFi tries get connected but not connected in 10secs, it skip the blynk.begin() to execute the void loop().
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  wifiStartTime = millis();
  lcd.clear();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartTime < wifiTimeout) {
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Trying to connect to");
    lcd.setCursor(0, 1);
    lcd.print("the internet.     ");
    lcd.setCursor(0, 3);
    lcd.print("Please wait...      ");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WiFiLed, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Internet Connected. ");
    lcd.setCursor(0, 2);
    lcd.print("Starting system with");
    lcd.setCursor(0, 3);
    lcd.print("internet.");
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    timer.setInterval(500L, checkBlynkStatus);  //when the wifi is connected, set the timer interval
    timer.setInterval(500L, myTimer);           //when the wifi is connected, set the timer interval
    Blynk.syncAll();
    lcd.clear();
    delay(2000);
  } else {
    digitalWrite(WiFiLed, LOW);
    Serial.println("");
    Serial.println("Failed to connect to WiFi within timeout period");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to connect to");
    lcd.setCursor(0, 1);
    lcd.print("the internet.     ");
    lcd.setCursor(0, 2);
    lcd.print("System start without");
    lcd.setCursor(0, 3);
    lcd.print("internet.");
    delay(2000);
    lcd.clear();
  }
}


//for checking the status of the internet
void checkBlynkStatus() {  // called every 2 seconds by SimpleTimer
  if (Blynk.connected() == true) {
    //    Serial.println("Blynk Not Connected");
    digitalWrite(WiFiLed, HIGH);
    if (notificationlogOnceStatus == true) {
      Blynk.logEvent("alert", "DEVICE AND BLYNK IS CONNECTED");  //(event_code, any statements)
      notificationlogOnceStatus = false;
    }
  } else {
    //    Serial.println("Blynk Connected");
    digitalWrite(WiFiLed, LOW);
    notificationlogOnceStatus = true;
  }
}