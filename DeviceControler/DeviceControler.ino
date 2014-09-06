// Possible commands are listed here:
//
// "digital/13"     -> digitalRead(13)
// "digital/13/1"   -> digitalWrite(13, HIGH)
// "analog/2/123"   -> analogWrite(2, 123)
// "analog/2"       -> analogRead(2)
// "mode/13/input"  -> pinMode(13, INPUT)
// "mode/13/output" -> pinMode(13, OUTPUT)

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Console.h>
#include <SD.h>
#include <SPI.h>


#define rfTransmitPin 7  //RF Transmitter pin = digital pin 4
#define ledPin 13        //Onboard LED = digital pin 13

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

const int chipSelect = 4;
const int codeSize = 25;
int timeDelay=120;

void setup() {
  Serial.begin(9600);

  // Bridge startup
  Bridge.begin();
  /*if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }*/
  loadDevices();
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.noListenOnLocalhost();
  server.begin();
  pinMode(rfTransmitPin, OUTPUT);   //Transmit pin is an output  
  pinMode(ledPin, OUTPUT);  
  
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();
  
  // There is a new client?
  if (client) {
    
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  
  int logRequest = client.parseInt();
  int device = client.parseInt();
  int statusDevice = client.parseInt();
  int reply = processCommand(device, statusDevice);
  sendReply(client, logRequest, reply, statusDevice);
  
  
}

void loadDevices() {
  //Device 1. TODO: read from SD
  Bridge.put("1_type", "RF");
  Bridge.put("1_status", "Off");
  Bridge.put("1_ON_SEQ", "2442244242224442422444443");
  Bridge.put("1_OFF_SEQ","2442244242224442422424443");
  //Device 2. TODO: read from SD
  Bridge.put("2_type", "RF");
  Bridge.put("2_status", "Off");
  Bridge.put("2_ON_SEQ", "2442244242224442422442443");
  Bridge.put("2_OFF_SEQ","2442244242224442422422443");
}

int processCommand(int device, int statusDevice) {
  int reply = 1;
  char deviceType[2];
  
  
  String deviceStringType = String(device)+"_type";
  char key [deviceStringType.length()+1];
  deviceStringType.toCharArray(key, strlen(key));
  
  //Todo check transmision type
  Bridge.get(key, deviceType, 2);
   
  if(compareCharSeqString(deviceType,"RF")){
    return processRF(device, statusDevice);
  }
  
  return reply;
}

int processRF(int device, int statusDevice) {
  int reply=1;
  char seq[codeSize];
  String seqKey = String(device)+"_ON_SEQ";
  
  char key [seqKey.length()+1];
  seqKey.toCharArray(key, seqKey.length()+1);
 
  Bridge.get(key, seq, codeSize);
   
  if(statusDevice==0){
   String seqKeyOff = String(device)+"_OFF_SEQ";
   
   char keyOff [seqKeyOff.length()+1];
   seqKeyOff.toCharArray(keyOff, seqKeyOff.length()+1);
   
   Bridge.get(keyOff, seq, codeSize);
  }
  
  reply = transmitCodeRF(seq);
  return reply;
}


int transmitCodeRF(char seq[]){
    // The LED will be turned on to create a visual signal transmission indicator.
    digitalWrite(ledPin, HIGH);
   
   //initialise the variables 
    int highLength = 0;
    int lowLength = 0;
    int numTimes = 4;
    
    //The signal is transmitted 6 times in succession - this may vary with your remote.       
    for(int j = 0; j<numTimes; j++){
      for(int i = 0; i<codeSize; i++){ 
        switch(seq[i]){
          case '1': // SH + SL
            highLength=3;
            lowLength=3;
          break;
          case '2': // SH + LL
            highLength=3;
            lowLength=7;
          break;
          case '3': // SH + VLL
            highLength=3;
            lowLength=92;
          break;
          case '4': // LH + SL
            highLength=7;
            lowLength=3;
          break;
          case '5': // LH + LL
            highLength=7;
            lowLength=7;
          break;
          case '6': // LH + VLL
            highLength=7;
            lowLength=92;
          break;
        }
           
         /* Transmit a HIGH signal - the duration of transmission will be determined 
            by the highLength and timeDelay variables */
         digitalWrite(rfTransmitPin, HIGH);     
         delayMicroseconds(highLength*timeDelay); 
         
         /* Transmit a LOW signal - the duration of transmission will be determined 
            by the lowLength and timeDelay variables */
         digitalWrite(rfTransmitPin,LOW);     
         delayMicroseconds(lowLength*timeDelay);  
      }
      
    }
    //Turn the LED off after the code has been transmitted.
    digitalWrite(ledPin, LOW); 
    return 1;
 }
 
 int compareCharSeqString(char seq[], String string){
   if(string.length()!=(strlen(seq)-1)){return 0;}
   for(int j = 0; j<strlen(seq)-1; j++){
     if(seq[j]!=string.charAt(j)){return 0;}
   }
   return 1;
 }
 
 void sendReply(YunClient client, int logRequest, int reply, int statusDevice){
     client.print("OK");
 }

