/**
 * IDEA LAB (Makerspace)
 * Sacred Heart University
 * 
 * Morse code threshold demo
 * 
 * Author: Cedric Bleimling
 * Licence: CC NC SA
 */

// This demo showcases the inner workings of a threshold in electronics
// You can change the threshold by changing the "threshold" variable below.
// If the sensor value is above the threshold, the output will be set to "1" otherwise it is "0".

int threshold = 400; // Values must be between 0 and 1024
int photoResistorPin = A1; // Pin where we connect the photoresistor

void setup() {
  pinMode(photoResistorPin,INPUT); // Initailizing the photoresistor pin as an input
  Serial.begin(9600);// Start a Serial Connection
}

void loop() {
  // we store the values of the photoresistor (y1), threshold (y2) and output (y3) and then send them over serial to be displayed
  int y1 = analogRead(photoResistorPin);
  int y2 = threshold;
  int y3 = 0; // by default we assume the output is 0
  if(y1>y2){y3=1024;} // if photoresistor value > threshold, we switch output to 1

  Serial.print(y1);
  Serial.print(" "); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.print(y2);
  Serial.print(" "); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.println(y3);

  delay(100);
}
