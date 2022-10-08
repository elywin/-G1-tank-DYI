#include "Wire.h"
#include "Adafruit_MotorShield.h"
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "IRremote.h"
#include "Servo.h"

int speed1 = 200; //Speed Ranges 0 (Stop) - 255 (fullSpeed)

int RECV_PIN = 11;  //InPut Pin IReciver 
long distancia;    //Espacio para calcular el tiempo de rebote
long tiempo;       //del pulso ultrasonico emitido
int pos =0;

Servo myservo;
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(4);
IRrecv irrecv(RECV_PIN);
decode_results results;
//----( SETUP: RUNS ONCE )----
void setup() {
  Serial.begin(9600);
  AFMS.begin();
  pinMode(9, OUTPUT);
  pinMode(8, INPUT);
  myservo.attach(12);
  
  myMotor3->setSpeed(speed1);
  myMotor4->setSpeed(speed1);

  irrecv.enableIRIn(); // Start the receiver

}
//--(end setup )---

void loop() {

  if (irrecv.decode(&results)) {
    translateIR(); 
    irrecv.resume(); // receive the next value
  } 
  digitalWrite(9,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(9, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo=pulseIn(8, HIGH); 
  distancia= int(0.017*tiempo); 
  /*Monitorización en centímetros por el monitor serial*/
  Serial.println("Distancia ");
  Serial.println(distancia);
  Serial.println(" cm");
  delay(1000);

  uint8_t i;
  if(distancia > 7){
  myMotor3->run(FORWARD);
  myMotor4->run(FORWARD);
  
  } else {
  myMotor3->run(BACKWARD);
  myMotor4->run(BACKWARD);
  for (i=0; i<111; i++) {
    myMotor3->setSpeed(i);
    myMotor4->setSpeed(i);   
    myservo.write(180); 
  }
  myMotor3->run(BACKWARD);
  myMotor4->run(FORWARD);
  for (i=0; i<55; i++) {
    myMotor3->setSpeed(i);
    myMotor4->setSpeed(i);  
  }
  }

}

 
/*-----( Function )-----*/
void translateIR() 
{
  uint8_t i;
  switch(results.value) 
  {
  case 0xFF9867: Serial.println(" FORDWARD");    
    myMotor3->run(FORWARD);
    myMotor4->run(FORWARD);
    delay(10);
    break;
  case 0xFF30CF: Serial.println(" LEFT");    
    myMotor3->run(FORWARD);
    myMotor4->run(BACKWARD);
    delay(10);
    break;
  case 0xFF18E7: Serial.println(" -OK-");
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
    break;
  case 0xFF7A85: Serial.println(" RIGHT");    
    for (i=0; i<111; i++) {
    myMotor3->setSpeed(i);
    myMotor4->setSpeed(i);   
    delay(10);
    }
    break;
  case 0xFF38C7: Serial.println(" BACKWARD");
    myMotor3->run(BACKWARD);
    myMotor4->run(BACKWARD);
    break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");
    break;  
  default: 
    Serial.println(" other button   ");
 
  }
 
} //END translateIR
