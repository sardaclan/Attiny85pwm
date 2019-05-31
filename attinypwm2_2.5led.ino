// Original code was in Korean by unknown contributor, most of the sketch patched together by sortofsmarter

#define Thermistor_R 100000   //The resistance value when the thermistor is at room temperature (25 degrees)
#define Temp_Norm 25          //Room temperature standard
#define Num_Samples 3         //Number of sampling iterations to average
#define B_Coeffecient 2468    //Beta coefficient of thermistor(usually 3000-4000)
#define S_Resitor 100000      //Resistance value of resistor connected to thermistor

//Attiny 85 Set as the standard.
//                  ┍━━━=━━┑
//            RESET ┥1    8┝ 5V
//            A3/D3 ┥2    7┝ D2/A1/SCK/SCL
//       PWM4/A2/D4 ┥3    6┝ D1/PWM1/MISO
//              GND ┥4    5┝ D0/PWM0/MOSI/SDA/Aref
//                  ┕━━━=━━┙
//                  ATTINY85

//Pinout for Attiny85
#define Thermistor_Pin A1       //A1(P2) Set pin to thermistor measuring pin
#define Fan_Control_Pin 1       //D1(6) Set the pin as a pin for fan control
#define Setting_Pin A2          //A2(D4:3) Set pin as setting pin
#define LED_PIN 3               //A3 (P3) Set pin for output 


#define Min_Temp 20             //The minimum value within the temperature range. In this case, only the set PWM value is output.
#define Max_Temp 100             //Maximum value within the temperature range. 
#define Min_PWM 30              //PWM minimum value. This needs to be set depending on the min power needed to keep fan running
#define Max_PWM 254            //PWM maximum value. 
#define WARN_TEMP 80


int samples[Num_Samples];
float Fan_PWM;                  //Variable for PWM output

float Celsius;                  //Variable for storing Celsius temperature.
float Ratio;                    //Variable for adjusting the output level.
int i;

void setup(void) {
  //analogReference(EXTERNAL);          //AREF Input (P0)
  pinMode(Fan_Control_Pin, OUTPUT);
  analogWrite(Fan_Control_Pin, 254);    //Maximum fan speed for the first 3 seconds of operation
  delay(300);
}

void loop(void) {


  //To analog input N_Samples Thermistor measurement as many times
  for (i = 0; i < Num_Samples; i++) {
    samples[i] = analogRead(Thermistor_Pin);
    delay(5);
  }

  float average = 0;
  for (i = 0; i < Num_Samples; i++) {
    average += samples[i];
  }
  average /= Num_Samples;

  //Convert measured values ​​to resistance values
  average = 1023 / average - 1;
  average = S_Resitor / average;


  Celsius = average / Thermistor_R;     // (R/Ro)
  Celsius = log(Celsius);                  // ln(R/Ro)
  Celsius /= B_Coeffecient;                   // 1/B * ln(R/Ro)
  Celsius += 1.0 / (Temp_Norm + 273.15); // + (1/To)
  Celsius = 1.0 / Celsius;                 // Invert
  Celsius -= 273.15;                         // convert to C

  Ratio = analogRead(Setting_Pin);
  Ratio = map(Ratio, 0, 1023, -10, 10);
  //  Celsius = Celsius + Ratio;
  if (Celsius >= WARN_TEMP)


    digitalWrite(LED_PIN, HIGH); // led on as threshold exceeded

  else

    digitalWrite(LED_PIN, LOW); // led off as threshold not reached



  Fan_PWM = map(Celsius, Min_Temp, Max_Temp, Min_PWM, Max_PWM);  //Map the temperature value to the PWM in the range of 0 to 255.
  Fan_PWM = constrain(Fan_PWM, Min_PWM, Max_PWM);
  analogWrite(Fan_Control_Pin, Fan_PWM);      //Output PWM.

}
