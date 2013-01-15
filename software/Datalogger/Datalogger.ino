/**********************************************
 * BMP085 csv logger by Leon Rische (@l3kn)   *
 **********************************************/

#include <SD.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

const int chipSelect = 4;
const String header = "time,temperature,pressure";

uint8_t fileNum = 0, writeNum = 0;
Adafruit_BMP085 bmp;

void setup()
{
 Serial.begin(9600);
  // disable chip-select pin
  pinMode(10, OUTPUT);
  
  // is the sd cart present?
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("failed...");
    return;
    // stahp!
  }
  
  if (!bmp.begin()) 
  {
    Serial.println("failed...");
    return;
    // stahp!
  }
  Serial.println("ready to log");
  
  // give the sensor some time to get ready
  delay(1000);
}

void loop()
{
  /**********************************************
   * preparing le file                          *
   **********************************************/
   
  char name[] = "LOG00.CSV";
   
  name[3] = fileNum/10 + '0';
  name[4] = fileNum%10 + '0';

  File dataFile = SD.open(name, FILE_WRITE);
   
  /**********************************************
   * constructing le string                     *
   **********************************************/
  
  String dataString = "";
  
  int time = millis() / 1000;
  int temp = bmp.readTemperature()*10;
  int pres = bmp.readPressure()/100;
  
  dataString += time;
  dataString += ",";
  dataString += temp;
  dataString += ",";
  dataString += pres;
  
  /**********************************************
   * le write-function                          *
   **********************************************/

  // if the file is available, write to it:
  
  if (dataFile) 
  {
    if(writeNum == 0)
     dataFile.println(header);
     
    dataFile.println(dataString);
    dataFile.close();
    // debug
    Serial.println(dataString);
    writeNum ++;
  }  
  
   else 
    Serial.println("error opening file");
  
  if (writeNum >= 100)
  {
    fileNum ++;
    writeNum = 0;
    Serial.println("opening next file");
  }
  delay(1000);
}









