// Turbidity Sensor variable.
int sensorPin = 36, Vclear = 2.00;
float volt, sensVal;
float ntu;
int samples = 800;

void setup() {
  Serial.begin(9600);
}
void loop() {
  turbidity();

  delay(1000);
}


/**********************************************************************/
//FUNTIONS
/**********************************************************************/
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
    sensVal = constrain((float)analogRead(sensorPin), 0, 3011); //3011 is the highest analog value we have gotten from the sensor
    ntu = map(sensVal, 0, 3011, 100, 0);  //in percentege. // Map this value from 0 - 3011 to 100 and 0 in percentage so that it becomes percentage of cleanliness.
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
  Serial.print("NTU:");
  Serial.println(ntu);
}
float round_to_dp(float in_value, int decimal_place) {
  float multiplier = powf(10.0f, decimal_place);
  in_value = roundf(in_value * multiplier) / multiplier;
  return in_value;
}