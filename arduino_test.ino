// https://www.arduino.cc/reference/it/language/functions/analog-io/analogread/ 

int analogPin = A0; // pin da cui prendere l'input
int sensorValue = 0; // variabile in cui memorizzo il valore letto di volta in volta
int threshold = 380; // è più alto quanto più il led si accende facilmente a causa del rumore

void setup() {
  Serial.begin (9600); // initializzo la comunicazione seriale a 9600 bps
  pinMode(13, OUTPUT);
}

void loop() {
  
  sensorValue = analogRead (analogPin); // leggo 10 bit => 2^10 = 1024. quindi leggo valori da 0 a 1023
  Serial.println (sensorValue);
  if (sensorValue > threshold) { // passo il valore del pin in output
      digitalWrite(13, HIGH);
    }
  else {
    digitalWrite(13, LOW);
    }
}
