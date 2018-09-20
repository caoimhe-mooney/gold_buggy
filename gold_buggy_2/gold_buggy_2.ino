//hardware interface
#define CTRL_PIN 3
#define IR_PIN 2
//read control messages
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
//Gantry
volatile boolean irInterrupt = false;
int pulse; //signal-in from the gantry 
//Ultrasonic
const int pingPin =  A4;      // the pin number for the signal 
const int powerPin =  A3;    // the pin number for the power 
const int gndPin =  A2;       // the pin number for the gnd
boolean ultraSonics = false;   //flag to turn on/off the ultrasonics
unsigned long previousMillis = 0;        // will store last time ultrasonics checked was updated
const long interval = 1000;       
//gantry counter
int counter = 0;

void setup() {
  //set up Nano board pin
  pinMode(CTRL_PIN, OUTPUT);
  digitalWrite(CTRL_PIN, LOW);
  pinMode(IR_PIN, INPUT);
  // set the digital pin as output:
  pinMode(pingPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  pinMode(gndPin, OUTPUT);
  //Power the module
  digitalWrite(powerPin, HIGH);
  digitalWrite(gndPin, LOW);
  
  //attach irInterrput tp setup()
  attachInterrupt(digitalPinToInterrupt(IR_PIN), ISRinfrared, RISING); 

  //set up serial comms
  Serial.begin(9600);
  Serial.print("+++");
  delay(1500);
  Serial.println("ATID 3111, CH C, CN");
  delay(1100);
  while (Serial.read() != -1) {};
}


void loop() {

  //ultrasonic control
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you checked the ultrasonics
    previousMillis = currentMillis;

   if(ultraSonics) //ultrasonics turned on (true/false)
  {
   
   if(ultraSonicDet()) {  //if object detected
   writeXbee("detected (Buggy2)");
   stopBuggy();
   }
 
   else{
       startBuggy();
      }
    }
  }
//read messages

//read command    
   if (stringComplete) {

 //control start
    if(inputString == "run2"){
      writeXbee("RUN BUGGY 2");
      startBuggy();
      ultraSonics = true;  //turns on ultrasonics module
      }
      
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
//gantry detect and gantry number report
  if (irInterrupt){
  
    stopBuggy();
    int gantry = readGantry();// read in the gantry number according to frequency
    if (gantry == 4){  //abnormal condition 
      writeXbee("ERROR");
    }
    else{
      writeXbee("B2G" + String(gantry));  //lets supervisory pc know which gantry buggy 2 is at
      ultraSonics = false; //stops ultrasonics running the buggy again instead of the go command 
    }
    delay(1000);
    irInterrupt = false; 
    counter++;  //adds to gantry counter

    if(counter == 2){  //parks buggy after 2 gantries as buggy 2 always parks after 2 gantries
      writeXbee("PARKING BUGGY 2");
      parking(); 
      counter = 0; //resets gantry counter to 0 each time the buggy parks
    }
  }
}

//Functions start here
void writeXbee(String msg) {  //sends a message as a string to the serial port to communicate with the supervisory pc
  Serial.println(msg);
  Serial.flush();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      
    }
    else {
      inputString += inChar;
      stringComplete=false;
      }
  }
}

//Gantry recognition
int readGantry(){
  while(digitalRead(IR_PIN)!=LOW);
  //read the average frenquency for precision
  //prevents excess readings of "gantry 4"
   int pulse0 = pulseIn(IR_PIN, HIGH);
   delay(2);
   int pulse1 = pulseIn(IR_PIN, HIGH);
   delay(2);
   int pulse2 = pulseIn(IR_PIN, HIGH);
   delay(2);
   int pulse3 = pulseIn(IR_PIN, HIGH);
   pulse = (pulse0 + pulse1 + pulse2 + pulse3)/4;

   //labelling of gantries depending on infrared pulse they send
   if(pulse >= 500 && pulse <= 1500){
     return 1;
   }
   else if (pulse >= 1500 && pulse <=2500){
    return 2;
   }
   else if (pulse >=2500 && pulse <=3500){
    return 3;
   }
   else {
    return 4;
   }
}

void ISRinfrared(){  //void function necessary for the attach interrupt function
   irInterrupt = true;
}
//choose modes on Buggy Controller Chip 
void stopBuggy(){
      delay(2);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
      digitalWrite(CTRL_PIN,HIGH);
      delay(2);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
}

void startBuggy(){
      delay(20);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
      digitalWrite(CTRL_PIN,HIGH);
      delay(4);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
}

void rightOverride(){
      delay(20);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
      digitalWrite(CTRL_PIN,HIGH);
      delay(8);
      digitalWrite(CTRL_PIN,LOW);
      delay(2);
}

//ultrasonic module
boolean ultraSonicDet()
{
  pinMode(pingPin, OUTPUT);  //allows ultrasonic soundwave to be sent as an output
  digitalWrite(pingPin,LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin,HIGH);  //sends 10 microsecond signal to be measured
  delayMicroseconds(10);
  digitalWrite(pingPin,LOW);

  pinMode(pingPin, INPUT);  //allows sent soundwave to be taken back in
   
  float ultrasonicPulse = pulseIn(pingPin,HIGH);
  int distance = (ultrasonicPulse/20000) * (343.2);

   if(distance < 10)  //10cm
   return true;
   else
   return false;
}


void parking(){
  rightOverride();  //buggy 2 must go anticlockwise for correct park function to work
  delay(3000);
  startBuggy();  //returns buggy to normal mode to allow it to continue into parking lane correctly
  delay(3500);  //delay allows buggy to park properly in parking lane
  stopBuggy();
  ultraSonics = false;  //stops buggy restarting after parking
  delay(3000);
  }


