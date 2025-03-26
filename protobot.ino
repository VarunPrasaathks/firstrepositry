#include <ps5Controller.h>
#include<ESP32Servo.h>
#define shootingbldcpin 4
#define flapperPin 26
#define lpwm 23
#define ldir 22

Servo shootingbldc;
Servo flapper;

int pwm[3] = { 12, 33, 27 };
int dir[3] = { 14, 25, 26 };

void setup() {

  Serial.begin(115200);
  for (int j = 0; j < 3; j++) {
    pinMode(dir[j], OUTPUT);
    pinMode(pwm[j], OUTPUT);
  }
  ps5.begin("e8:47:3a:0f:65:ee");  //e8:47:3a:0f:65:ee - pink ps5     e8:47:3a:36:ed:ca  white
  pinMode(lpwm,OUTPUT);
  pinMode(ldir,OUTPUT);
  shootingbldc.attach(shootingbldcpin);
  flapper.attach(flapperPin);
  flapper.write(90);
}


//locomotion - done
//dribbling - done
//flapper - done
//shooting - 
float vx, vy, g;
float m[3], p[3];
String direction;

int Lbuttonold = 1;
int Lbuttonnew;
int Lbuttonstate = 0;
int Rbuttonold = 1;
int Rbuttonnew;
int Rbuttonstate = 0;

void loop() {

  if (ps5.isConnected()) {

    Serial.println("PS5 Controller Connected!");
    float x = ps5.RStickX();
    float y = ps5.RStickY();

    vx = map(x, -128, 128, -90, 90);
    vy = map(y, -128, 128, -90, 90);

    button_rotate();
    print_all();
    ductadjust();
    shooting();
  }
}


void calculations() {
  if (abs(vx) < 20 && abs(vy) < 20) {

    m[0] = 0;
    m[1] = 0;
    m[2] = 0;
    direction = "STOP";
  } else {

    m[0] = (-0.5 * vx) - (0.866 * vy);
    m[1] = (-0.5 * vx) + (0.866 * vy);
    m[2] = vx;
  }
  for (int i = 0; i <= 3; i++) {
    if (m[i] > 90) { m[i] = 90; }
    if (m[i] < -90) { m[i] = -90; }
  }
  g = abs(m[0]);
  for (int i = 1; i < 3; i++) {
    if (g < abs(m[i])) {
      g = abs(m[i]);
    }
  }
  if (g > 90) { g = 90; }
  if (g < -90) { g = -90; }
  g = map(abs(g), 0, 90, 0, 255);
}


void locomotion() {
  if (ps5.L1() == 1) {
    direction = " RL ";
    rotate(g);
  } else if (ps5.R1() == 1) {
    direction = " RR ";
    rotate(-g);
  } else {
    direction = "FBLR";
    motormovement(m[0], m[1], m[2]);
  }
}


void motormovement(float m0, float m1, float m2) {

  float m[3] = { m0, m1, m2 };
  for (int i = 0; i < 3; i++) {
    p[i] = map(abs(m[i]), 0, 90, 0, 255);
    (m[i] < 0) ? digitalWrite(dir[i], LOW) : digitalWrite(dir[i], HIGH);
    analogWrite(pwm[i], p[i]);
  }
}


void rotate(float g1) {
  if (g1 < 0) {
    directions(HIGH, HIGH, HIGH);
  } else {
    directions(LOW, LOW, LOW);
  }
  for (int i = 0; i < 3; i++) {
    analogWrite(pwm[i], abs(g1));
  }
}


void directions(bool dir1, bool dir2, bool dir3) {
  digitalWrite(dir[0], dir1);
  digitalWrite(dir[1], dir2);
  digitalWrite(dir[2], dir3);
}


void print_all() {
  Serial.print("Right : X=" + String(vx) + "\tY=" + String(vy) + "\t");
  Serial.print("\tBefore\tM0: ");
  Serial.print(m[0]);
  Serial.print("\tM1: ");
  Serial.print(m[1]);
  Serial.print("\tM2: ");
  Serial.print(m[2]);
  Serial.print("\tp0:");
  Serial.print(p[0]);
  Serial.print("\tp1:");
  Serial.print(p[1]);
  Serial.print("\tp2:");
  Serial.print(p[2]);
  Serial.print("\tg : ");
  Serial.print(g);
  Serial.print("\t Directions : " + String(digitalRead(dir[0])) + String(digitalRead(dir[1])) + String(digitalRead(dir[2])) + "\t");
  Serial.print(direction);
  Serial.println("\tBattery : " + ps5.Battery());
}

void button_rotate() {

  Lbuttonnew = ps5.Square();
  Rbuttonnew = ps5.Circle();
  if (Lbuttonold == 0 && Lbuttonnew == 1) {
    if (!Rbuttonstate) {
      Lbuttonstate = !Lbuttonstate;

      Serial.print("\t Left button - ");
      Serial.println(Lbuttonstate);
    }
  }
  Lbuttonold = Lbuttonnew;
  if (Rbuttonold == 0 && Rbuttonnew == 1) {
    if (!Lbuttonstate) {
      Rbuttonstate = !Rbuttonstate;
      Serial.print("\t Right button - ");
      Serial.println(Rbuttonstate);
    }
  }
  if (Lbuttonstate) {
    Serial.println("\t rotate - 90");
    rotate(90);
    direction = "BL";
  } else if (Rbuttonstate) {
    Serial.println("\t rotate- -90");
    rotate(-90);
    direction = "BR";
  } else {
    Serial.println("\t calculations");
    calculations();
    locomotion();
  }
  Rbuttonold = Rbuttonnew;
  delay(10);
}

void shooting(){
  if(ps5.Right()){
    shootingbldc.write(1750);
    delay(500);
    flapper.write(90);
    delay(2000);
    flapper.write(0);
    shootingbldc.write(1500);
    delay(500);
  }
}
void ductadjust(){
   if(ps5.Up())
    {
      digitalWrite(ldir,LOW);
      analogWrite(lpwm,255);
      Serial.print("Moving Up");
    }
    else if(ps5.Down())
    {
      digitalWrite(ldir,HIGH);
      analogWrite(lpwm,255);
      Serial.print("Moving down");
    }
    else
    {
      analogWrite(lpwm,0);
      Serial.print("no movement");
    }
}