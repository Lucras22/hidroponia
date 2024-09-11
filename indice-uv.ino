void setup() 
{
  Serial.begin(9600);
}
 
void loop() 
{
  float sensorVoltage; 
  float sensorValue;
 
  sensorValue = analogRead(36);
  sensorVoltage = sensorValue/4095*3.3;
  Serial.print("Leitura do Sensor = ");
  Serial.print(sensorValue);
  Serial.println("");
  Serial.print("Voltagem do Sensor = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  delay(1000);
}