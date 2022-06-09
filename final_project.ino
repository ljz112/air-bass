//accelerometer
int scale = 3;
int accelX = A0;
int accelY = A1;
int accelZ = A2;

//ultrasonic sensor
int filterArray[20];
const int trigPin = 9;
const int echoPin = 10;

//finger sensors, from nicole
const int LH1 = 2;
const int LH2 = 3;
const int LH3 = 4;
const int LH4 = 5;
const int RH1 = 6;
const int RH2 = 7;
const int RH3 = 8;
const int RH4 = 11;
const float ratios[] = {1, 1.067, 1.125, 1.2, 1.25};
const float strings[] = {0, 98, 73.4, 55, 41.2};

//string tuners
const int tuners[] = {A3, A4};

void setup() {
  //ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //finger stuff
  for (int i = 2; i < 3; i++){
    pinMode(i, INPUT);
  }
  
  Serial.begin(9600);
}

void loop() {
  //accelerometer
  int tries = 1;
  int rawX = takeAvgAccel(accelX, tries);
  int rawY = takeAvgAccel(accelY, tries);
  int rawZ = takeAvgAccel(accelZ, tries);
  float scaledX, scaledY, scaledZ; // Scaled values for each axis
  scaledX = mapf(rawX, 0, 1023, -scale, scale);
  scaledY = mapf(rawY, 0, 1023, -scale, scale);
  scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
  float total = getTotal(scaledX, scaledY, scaledZ);
  Serial.print(total > 2.25);

  //ultrasonic sensor
  //Source: https://arduinogetstarted.com/faq/how-to-filter-noise-from-sensor
  /* right now 20 measurements, avg of middle 10 */
  /* 1. TAKING MULTIPLE MEASUREMENTS AND STORE IN AN ARRAY*/
  int trials = 20;
  for (int sample = 0; sample < trials; sample++) {
    filterArray[sample] = getUltraMeasurement();
  }
  /* 2. SORTING THE ARRAY IN ASCENDING ORDER */
  for (int i = 0; i < trials - 1; i++) {
    for (int j = i + 1; j < trials; j++) {
      if (filterArray[i] > filterArray[j]) {
        int swap = filterArray[i];
        filterArray[i] = filterArray[j];
        filterArray[j] = swap;
      }
    }
  }
  /* 3. FILTERING NOISE
  // + the five smallest samples are considered as noise -> ignore it
  // + the five biggest  samples are considered as noise -> ignore it
  // ----------------------------------------------------------------
  // => get average of the 10 middle samples (from 5th to 14th) */
  long sum = 0;
  int len = ((int) 3 * trials / 4) - ((int) trials / 4);
  for (int sample = (int) trials / 4; sample < (int) 3 * trials / 4; sample++) {
    sum += filterArray[sample];
  }
  
  float avg_value = sum / len;
  float avg_dist = avg_value * 0.034 / 2;
  /*if you want ultrasonic at top*/
  avg_dist = (avg_dist > 50) ? 100 : (100 - avg_dist);
  Serial.print(" "); Serial.print(avg_dist);

  //finger stuff
  //Source: https://github.com/HesslerN/Digit-al_Violin
  int chosenLH = 0;
  int chosenRH = 0;
  for (int i = 0; i < 100; i++) {
    if (!digitalRead(LH1)){
    chosenLH = 1;
    } 
    if (!digitalRead(LH2)){
      chosenLH = 2;
    } 
    if (!digitalRead(LH3)){
      chosenLH = 3;
    } 
    if (!digitalRead(LH4)){
      chosenLH = 4;
    } 
    /*Serial.print(" "); Serial.print(chosenLH);*/
    if (!digitalRead(RH1)){
      chosenRH = 1;
    } 
    if (!digitalRead(RH2)){
      chosenRH = 2;
    } 
    if (!digitalRead(RH3)){
      chosenRH = 3;
    } 
    if (!digitalRead(RH4)){
      chosenRH = 4;
    } 
    if (chosenRH) {
      break;
    }
  }
  Serial.print(" "); Serial.print(ratios[chosenLH]);
  Serial.print(" "); Serial.print(strings[chosenRH]);
  
  Serial.println();
}




float mapf (float x, float in_min, float in_max, float out_min, float out_max)
{
  //map function but returns float type
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float getTotal (float x, float y, float z) {
  //magnitude of acceleration
  return sqrt(x*x + y*y + z*z);
}

int takeAvgAccel(int analog, int tries) {
  //take average acceleration with number of trials for given axis
  int sum = 0;
  for (int i = 0; i < tries; i++) {
    int raw = analogRead(analog);
    sum += raw;
  }
  sum /= tries;
  return sum;
}

int getUltraMeasurement() {
  //get measurement of ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echoPin, HIGH);
  if (duration >= 5883) {
    /*limit of 100 cm*/
    duration = 5882;
  }
  return duration;
}
