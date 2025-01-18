// //Note: Although, the ESP32 is a 12bit ADC resolution device which makes out analog readings from 0 to 4095 as 0 to 3.3 V.
// //Yet, you must physically tune/calibrate the pH sensor potentiometer until it brings 2.5 V (the same tuning for Arduino Uno, Nano, Mega which are 10 bit resulution devices).
// //This way, you will get the pH sensor to perfectly work with the ESP32.

// // For ESP32
// int samples = 100;
// float adc_resolution = 4095.0;  //ESP 32 12 bit ADC Resolution
// int measurings = 0;
// int pHSensorPin = 39;  //p0 terminal in pH sensor.

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
// }

// void loop() {
//   //Find the average of all the 100ttnvj
//   int measurings = 0;
//   for (int i = 0; i < samples; i++) {
//     measurings += analogRead(pHSensorPin);
//     delay(1);
//   }
//   float Avg_ph_value = measurings / samples;
//   float voltage_12bit = (3.3 / adc_resolution) * Avg_ph_value;  //Avg_ph_value is for data consistency and to minimize error.

//   // put your main code here, to run repeatedly:
//   Serial.print("pHSensorPin: ");
//   Serial.println(analogRead(pHSensorPin));
//   Serial.print("Avg_ph_value: ");
//   Serial.println(Avg_ph_value);
//   Serial.print("VOLTAGE 12Bit: ");
//   Serial.println(voltage_12bit);
//   Serial.println();
//   delay(500);
// }




// //-------------------------------------------------------------------------------------------------------------------------------------------------------
// // For Arduino
// int samples = 100;
// float adc_resolution = 1023.0;  //ESP 32 12 bit ADC Resolution
// int measurings = 0;
// int pHSensorPin = A0;  //p0 terminal in pH sensor.

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
// }

// void loop() {
//   //Find the average of all the 100ttnvj
//   int measurings = 0;
//   for (int i = 0; i < samples; i++) {
//     measurings += analogRead(pHSensorPin);
//     delay(1);
//   }
//   float Avg_ph_value = measurings / samples;
//   float voltage_10bit = (5.0 / adc_resolution) * Avg_ph_value;  //Avg_ph_value is for data consistency and to minimize error.

//   // put your main code here, to run repeatedly:
//   Serial.print("pHSensorPin: ");
//   Serial.println(analogRead(pHSensorPin));
//   Serial.print("Avg_ph_value: ");
//   Serial.println(Avg_ph_value);
//   Serial.print("VOLTAGE 10bit: ");
//   Serial.println(voltage_10bit);
//   Serial.println();
//   delay(500);
// }







//-------------------------------------------------------------------------------------------------------------------------------------------------------




//Get the pH values... The code for testing the pH sensor.

// float calibration_value = 21.34;
// float calibration_value = 20.24 - 0.7;
float calibration_value = 21.34 - 0.3;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
void setup() {
  Serial.begin(9600);
}
void loop() {
  //pH sensor codes
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
  float ph_act = -5.70 * volt + calibration_value;
  Serial.print("pH Val:");
  Serial.println(ph_act);

  delay(1000);
}