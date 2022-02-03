/*
 * GSM connection 
 * GSM ----> Arduino
 * 5VT ----> D9
 * 5VR ----> D10
 * 
 */

//Header Files
#include <SoftwareSerial.h>
#include <TinyGPS.h>

//Objects
SoftwareSerial mySerial(9, 10);
SoftwareSerial gpsSerial(3,4);//rx,tx 
TinyGPS gps; 

//Constant Variables
const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
const int ir_pin = 2; // digital Pin of IR sensor
const int motor_pin = 5; // digital Pin of IR sensor

//global Variables
char msg;
String message="initialize";
String rcvd="";
char call;
int obj_detected=0;
int start=0;
long duration, inches, cm;
float lat = 28.5458,lon = 77.1703;
String phone[]={"+917034221248","+917034221248"};
void setup() 
{
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(115200);    // Setting the baud rate of Serial Monitor (Arduino)
  //gpsSerial.begin(9600); // connect gps sensor
  pinMode(ir_pin,INPUT);
  pinMode(motor_pin,OUTPUT);
  Serial.println("Railway Crack Detection System");
  delay(100);
  startmsg();
}

void loop() 
{
  ReceiveMessage();
  
  
  
  
  
  
  
  
  
  if(rcvd.indexOf("Start")>0)
{
  start=2;
}
  if(start>0)
  {
    Serial.println("Start Message recieved starting robo");
    start_robo();
      calc_distance();//ULTRA SONIC sensor
  object_detection();//IR detection
  if(obj_detected==0 | cm >9)
  {
    Serial.print("Fault detected"); 
    Serial.print("obj_detected:");
    Serial.print(obj_detected);
    Serial.print("cm");
    Serial.println(cm);
    stop_robo();
    start=0;
    rcvd="";
    find_location();
    SendMessage();
    delay(2000);
    startmsg();
  }
  }

}

void startmsg()
{
    mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(100);  // Delay of 1000 milli seconds or 1 second
  
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(100);
}

void start_robo()
{
  Serial.println("Moving");
  digitalWrite(motor_pin, HIGH);
}

void stop_robo()
{
  Serial.println("stopped");
  digitalWrite(motor_pin, LOW);
  msg="";
}
void SendMessage(int msg_type)
{
  int i=0;
  String latitude = String(lat,6); 
  String longitude = String(lon,6);
  String sms="robo started"
  //Type=0:robo start message
  //Type=1:crack message
  //Type=2:object detect message
  //Type=3:number change message
  if (msg_type==0)
  {
    sms="Robo started. Please check your number for message";
  }
  else if (msg_type==1)
  {
    sms="Alert!  Crack detected at LAT:"+latitude+"  LON:"+longitude;
  }
  else if (msg_type==2)
  {
    sms="Alert!  Object detected at LAT:"+latitude+"  LON:"+longitude;
  }  
  else if (msg_type==3)
  {
    sms="Phone changed to "+phone[0];
  }
  
  
  do
  {
    mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    mySerial.println("AT+CMGS=\""+phone[i]+"\"\r");
    delay(1000);
    mySerial.println(sms);
    delay(100);
    mySerial.println((char)26);// ASCII code of CTRL+Z
    msg="";
    delay(1000);
    
    i++;
    if(i==2) msg_type=0;
    
  }while(msg_type==3)
}

void ReceiveMessage()
{
  if (mySerial.available()>0)
  {
    rcvd=mySerial.readString();
    Serial.print(rcvd);
  }
}
void object_detection()
{
  obj_detected=digitalRead(ir_pin);
     Serial.print("Object");
   Serial.println(obj_detected);
}
void calc_distance()
{
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);
}
long microsecondsToInches(long microseconds) 
{
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) 
{
   return microseconds / 29 / 2;
}
void find_location()
{
   while(gpsSerial.available()){ // check for gps data 
  if(gps.encode(gpsSerial.read()))// encode gps data 
  {  
  gps.f_get_position(&lat,&lon); // get latitude and longitude 
  // display position 

  Serial.println("Location Identified"); 
  Serial.print("Latitude:"); 
  Serial.print(lat,6); 
  Serial.print(";"); 
  Serial.print("Longitude:"); 
  Serial.println(lon,6);  

 } 
}  
}
