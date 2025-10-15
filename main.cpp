#include <ps5Controller.h>
#include <Arduino.h>
/// ######################################################################################
/// Variable
/// ######################################################################################
#define PWML_F 2
#define PWML_B 15
#define PWMR_F 4
#define PWMR_B 16
unsigned long lastTimeStamp = 0;
int analog_ly = 0;
int analog_rx = 0;
/// ######################################################################################
/// Function
/// ######################################################################################
void notify()
{
  char messageString[200];
  sprintf(messageString, "%4d,%4d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
  ps5.LStickX(),
  ps5.LStickY(),
  ps5.RStickX(),
  ps5.RStickY(),
  ps5.Left(),
  ps5.Down(),
  ps5.Right(),
  ps5.Up(),
  ps5.Square(),
  ps5.Cross(),
  ps5.Circle(),
  ps5.Triangle(),
  ps5.L1(),
  ps5.R1(),
  ps5.L2(),
  ps5.R2(),  
  ps5.Share(),
  ps5.Options(),
  ps5.PSButton(),
  ps5.Touchpad(),
  ps5.Charging(),
  ps5.Audio(),
  ps5.Mic(),
  ps5.Battery());

  //Only needed to print the message properly on serial monitor. Else we dont need it.
  if (millis() - lastTimeStamp > 50)
  {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

void onConnect()
{
  Serial.println("Connected!.");
}

void onDisConnect()
{
  Serial.println("Disconnected!.");    
}

int deadzone(int value) {
  const int DEAD_ZONE_THRESHOLD = 8; 
  if (abs(value) < DEAD_ZONE_THRESHOLD) {
    return 0; // Returns 0 if centered
  } else {
    return value; // Returns the actual value
  }
}

void pwm_command(int pwml1, int pwml2, int pwmr1, int pwmr2){
  analogWrite(PWML_F,pwml1);
  analogWrite(PWML_B,pwml2);
  analogWrite(PWMR_F,pwmr1);
  analogWrite(PWMR_B,pwmr2);
}

void foward(int pwm){
  pwm_command(0,pwm,pwm,0);
  Serial.print("Moving Foward : ");
  Serial.print(map(pwm,8,127,1,100));
  Serial.println("%");
}

void backward(int pwm){
  pwm_command(pwm,0,0,pwm);
  Serial.print("Moving Backward : ");
  Serial.print(map(pwm,8,127,1,100));
  Serial.println("%");
}

void rotate_cw(int pwm){
  pwm_command(0,pwm,0,pwm);
  Serial.print("Rotating Clockwise : ");
  Serial.print(map(pwm,8,127,1,100));
  Serial.println("%");
}

void rotate_ccw(int pwm){
  pwm_command(pwm,0,pwm,0);
  Serial.print("Rotating Counter Clockwise : ");
  Serial.print(map(pwm,8,127,1,100));
  Serial.println("%");
}

void stop(){
  pwm_command(0,0,0,0);
  Serial.println("Stoping");
}

void control()
{
  int pwm_ly = deadzone(analog_ly);
  int pwm_rx = deadzone(analog_rx);
  if ((pwm_ly > 0) && (pwm_rx == 0)){
    foward(pwm_ly);
  }else if((pwm_ly < 0) && (pwm_rx == 0)){
    backward(abs(pwm_ly));
  }else if ((pwm_ly == 0) && (pwm_rx > 0)){
    rotate_cw(pwm_rx);
  }else if ((pwm_ly == 0) && (pwm_rx < 0)){
    rotate_ccw(abs(pwm_rx));
  }else if((pwm_ly == 0) && (pwm_rx == 0)){
    stop();
  }else{
    pwm_command(0,0,0,0);
    Serial.println("Value Error!");
  }
}

void debug(){
  Serial.print(analog_ly);
  Serial.print(" | ");
  Serial.print(analog_rx);
  Serial.print(" | ");
  Serial.print(deadzone(analog_ly));
  Serial.print(" | ");
  Serial.println(deadzone(analog_rx));
}

/// ######################################################################################
/// Main
/// ######################################################################################
void setup() 
{
  Serial.begin(115200);
  //ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisConnect);
  ps5.begin("90:B6:85:3C:00:79");  //your PS5 controller mac address 
  while (ps5.isConnected() == false) 
  { 
    Serial.println("PS5 controller not found");
    delay(300);
  }
  Serial.println("Ready.");
  pinMode(PWML_F,OUTPUT);
  pinMode(PWML_B,OUTPUT);
  pinMode(PWMR_F,OUTPUT);
  pinMode(PWMR_B,OUTPUT);
}

void loop() 
{
  analog_ly = ps5.LStickY();
  analog_rx = ps5.RStickX();
  // debug();
  control();
}