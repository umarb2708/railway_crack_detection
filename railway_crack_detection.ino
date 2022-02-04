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
char call;
long duration, inches, cm;
float lat = 28.5458,lon = 77.1703;
String phone[]={"+916379293599","+917034221248"};
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
  String usr_msg=ReceiveMessage();
  if (usr_msg=="Start"|usr_msg=="start") 
  {
    start_robo();
    SendMessage(0);
  }
  else if (usr_msg.startsWith("number")|usr_msg.startsWith("Number"))
  {
    if(ChangePhone(usr_msg))
      SendMessage(3);
    else
      SendMessage(4);
  }
  if (object_detection())
  {
    SendMessage(2);
    stop_robo();
  }
  if(crack_detection())
  {
    SendMessage(1);
    stop_robo();
  }
  delay(2000);
  startmsg();
}

void startmsg()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(100);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(100);
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
  else
  {
    sms="Invalid Command";
  }
  Serial.println("Sending SMS:"+sms);
  
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
    
  }while(i<2);
}
String ReceiveMessage()
{
  String rcvd_msg="nomsg"
  if (mySerial.available()>0)
  {
    rcvd_msg=mySerial.readString();
    Serial.println("Received MSG:"+rcvd_msg);
  }
  rcvd_msg.replace("\n","");
  rcvd_msg.replace("\s","");
  return rcvd_msg.toLowerCase();
}

void start_robo()
{
  Serial.println("Robo Moving Forward");
  digitalWrite(motor_pin, HIGH);
}

void stop_robo()
{
  Serial.println("Robo stopped moving");
  digitalWrite(motor_pin, LOW);
  msg="";
}

int ChangePhone(String usr_msg)
{
  usr_msg.replace("number","");
  if(usr_msg.length()==10)
  {
    phone[0]="+91"+usr_msg;
    return 1;
  }
  else return 0;
}

int object_detection()
{
  int obj_detect=digitalRead(ir_pin);
  if(obj_detect)
  {
    Serial.println("Object detected infron of track");
    return 1;
  }
  else return 0;
  
}


int crack_detection()
{
  float dist=calc_distance();
  Serial.print("Calculated distance=");
  Serial.println(cm);
  if(dist >9.0) 
  {
    Serial.println("Crack detected in track");
    return 1;
  }
  else return 0;
}

float calc_distance()
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
   delay(100);
   return cm;
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
