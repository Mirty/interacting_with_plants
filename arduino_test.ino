// https://www.arduino.cc/reference/it/language/functions/analog-io/analogread/ 

int analogPin = A0; // pin da cui prendere l'input
int sensorValue = 0; // variabile in cui memorizzo il valore letto di volta in volta
int threshold = 200; // è più alto quanto più il led si accende facilmente a causa del rumore

void setup() {
  Serial.begin (9600); // initializzo la comunicazione seriale a 9600 bps
  pinMode(13, OUTPUT);
}

void loop() {
  
  sensorValue = analogRead (analogPin); // leggo 10 bit => 2^10 = 1024. quindi leggo valori da 0 a 1023
  Serial.println (sensorValue); // passo il valore del pin in output
  if (sensorValue > threshold) { 
      digitalWrite(13, HIGH); // se il valore attuale è maggiore della soglia allora faccio accendere il led
    }
  else {
    digitalWrite(13, LOW); // altrimenti lo spengo
    }
}
