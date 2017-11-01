/*
  SD card datalogger
    This logs data from sensors
    to an SD card using the SD library.

  The circuit:
    * analog sensors on analog ins 0, 1, and 2
    * SD card attached to SPI bus as follows:
      ** MOSI - pin 11
      ** MISO - pin 12
      ** CLK - pin 13
      ** CS - pin 10 (arduino nano uses pin 10)

  Arduino Datalogger Code used with permission from Tom Igoe
*/

#include <SPI.h>                                            // allow communication on SPI Buses
#include <SD.h>                                             // Library for writing TXT files to SD Cards

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

//define global variables
const int chipSelect = 10;
char FileName[] = "DTALOG00.CSV";
unsigned long cur_time;
File dataFile;

void setup() {

  Serial.begin(9600);                                       // Start Serial Comms
  while (!Serial) {
    ;                                                       // wait for serial port to connect.
  }                                                         // Needed for native USB port only

  if (!SD.begin(chipSelect)) {                              // test open the SD Module
    Serial.println("initialization failed!");
    return;
  }

  Serial.print("Initializing SD card...");                  // see if the card is present
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {             // and can be initialized:
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  }

  if (!volume.init(card)) {                                 // See if we can open 'volume'/'partition'
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }
  
  Serial.println("card initialized.");

  /* Test for existing files and set current filename */
  for (byte i = 0; i < 100; i++) {
    FileName[6] = i/10 + '0';  
    FileName[7] = i%10 + '0';
                                /*Serial.println(FileName); //DEBUGONLY */ 
    if (SD.exists(FileName)) {
      Serial.print("File ");
      Serial.print(FileName);
      Serial.println(" exists, checking next file");
      continue;                                             //Check next value if file exists
      } 

      dataFile = SD.open(FileName, FILE_WRITE);             // Open the new file for writing
      dataFile.println("Sensor 1,Sensor 2, Sensor 3, Time from Launch"); //when file found set headers
      dataFile.close();                                     // Close the File
      break;
  }
}



void loop() {

  dataFile = SD.open(FileName, FILE_WRITE);                 // open the file for writing
    
  String dataString = "";                                   // reset the data string to blank

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {     // do for pin values between 1 and 3
    int sensor = analogRead(analogPin);                     // read pin and set to variable sensor
    dataString += String(sensor);                           // append the sensor value to the datastring
    dataString += ",";                                      // append a comma to the string
  }
  
  dataString += millis();                                   // append the time in MS to the datastring

  if (dataFile) {                                           // Attempt to open datafile
    dataFile.println(dataString);                           // Append the datastring to the file
    dataFile.close();                                       // close out the file
    Serial.println(dataString);                             // print to the serial port as well
  }

  else {
    Serial.print("error opening ");                         // if the file isn't open, pop up an error:
    Serial.println(FileName);
  }
}
