#include "MKE_I2C_AHT20.h"

MKE_I2C_AHT20 mySensor;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mySensor.begin();
  delay(1000);
}

void loop()
{
  if (millis() - mySensor.lastRead() >= 1000)
  {
    mySensor.read();
    Serial.print(F("Temperature: "));
    Serial.print(mySensor.getTemperature());
    Serial.print(F(" C\t"));
    Serial.print(F("Humidity: "));
    Serial.print(mySensor.getHumidity());
    Serial.println(F(" %"));
  }
}
