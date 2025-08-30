/*
  Authored by
  Moses Jaguar
  2025
  
  Receiver using STM32F407VGTx Discovery board
  using UASRT 2 (tx PA3, rx PA2)
*/

// ------------ Motor Pin -------------- //
#define enableMotor PE7
#define RMotor1 PE13
#define LMotor1 PB10
#define LMotor2 PB6
#define RMotor2 PE6
#define RMotor3 PC8
#define LMotor3 PC9
#define LMotor4 PB7
#define RMotor4 PE5
#define SWITCH PA0

// ---------- Serial ESP32 Setup -------- //
HardwareSerial Serial2(PA3, PA2);

int x_received = 0;
int y_received = 0;
int z_received = 0;
int x_tmp = 0;
int y_tmp = 0;
int z_tmp = 0;

char serialBuffer[64];

// ------------ Motor Setup ---------- //
float kecepatan_motor1, kecepatan_motor2, kecepatan_motor3, kecepatan_motor4;
int speedA = 0, speedB = 0, speedC = 0, speedD = 0;

// -------------- Function -------------- //
void holonomic(float vx, float vy, float vz) {

  kecepatan_motor1 = (-0.35 * vx) + (0.35 * vy) + (0.25 * vz);
  kecepatan_motor2 = (-0.35 * vx) + (-0.35 * vy) + (0.25 * vz);
  kecepatan_motor3 = (0.35 * vx) + (-0.35 * vy) + (0.25 * vz);
  kecepatan_motor4 = (0.35 * vx) + (0.35 * vy) + (0.25 * vz);

  speedA = kecepatan_motor1;
  speedB = kecepatan_motor2;
  speedC = kecepatan_motor3;
  speedD = kecepatan_motor4;

  motorauto();
}

void motorauto() {
  digitalWrite(enableMotor, HIGH);
  if (digitalRead(SWITCH) == LOW) {
    speedA = 0;
    speedB = 0;
    speedC = 0;
    speedD = 0;
  }

  // -------- motor A ------------
  if (speedA > 0) {
    analogWrite(LMotor1, 0);
    analogWrite(RMotor1, speedA);
  } else if (speedA < 0) {
    speedA *= -1;
    analogWrite(LMotor1, speedA);
    analogWrite(RMotor1, 0);
  } else if (speedA == 0) {
    speedA = 0;
    analogWrite(LMotor1, 0);
    analogWrite(RMotor1, 0);
  }


  // -------- motor B ------------
  if (speedB > 0) {
    analogWrite(LMotor2, speedB);
    analogWrite(RMotor2, 0);
  }

  else if (speedB < 0) {
    speedB *= -1;
    analogWrite(LMotor2, 0);
    analogWrite(RMotor2, speedB);
  } else if (speedB == 0) {
    speedB = 0;
    analogWrite(LMotor2, 0);
    analogWrite(RMotor2, 0);
  }


  // -------- motor C ------------
  if (speedC > 0) {
    analogWrite(LMotor3, 0);
    analogWrite(RMotor3, speedC);
  }

  else if (speedC < 0) {
    speedC *= -1;
    analogWrite(LMotor3, speedC);
    analogWrite(RMotor3, 0);
  } else if (speedC == 0) {
    speedC = 0;
    analogWrite(LMotor3, 0);
    analogWrite(RMotor3, 0);
  }


  // -------- motor D ------------
  if (speedD > 0) {
    analogWrite(LMotor4, speedD);
    analogWrite(RMotor4, 0);
  }

  else if (speedD < 0) {
    speedD *= -1;
    analogWrite(LMotor4, 0);
    analogWrite(RMotor4, speedD);
  } else if (speedD == 0) {
    speedD = 0;
    analogWrite(LMotor4, 0);
    analogWrite(RMotor4, 0);
  }
}



void setup() {
  // -------- init Serial ------ //
  Serial.begin(115200);
  Serial2.begin(115200);

  // -------- init Motor ------- //
  pinMode(enableMotor, OUTPUT);

  pinMode(LMotor1, OUTPUT);
  pinMode(RMotor1, OUTPUT);
  pinMode(LMotor2, OUTPUT);
  pinMode(RMotor2, OUTPUT);
  pinMode(LMotor3, OUTPUT);
  pinMode(RMotor3, OUTPUT);
  pinMode(LMotor4, OUTPUT);
  pinMode(RMotor4, OUTPUT);

  pinMode(SWITCH, INPUT);
}

void loop() {
  // ------------ Emergency Button ---------------- //
  if (digitalRead(SWITCH) == HIGH) {
    digitalWrite(enableMotor, HIGH);
  } else if (digitalRead(SWITCH) == LOW) {
    digitalWrite(enableMotor, LOW);
    speedA = 0, speedB = 0, speedC = 0, speedD = 0;
  }

  // ----------- Serial Receive ------------------ //
  if (Serial2.available() > 0) {
    String incomingString = Serial2.readStringUntil('\n');
    incomingString.toCharArray(serialBuffer, 64);

    int parseResult = sscanf(serialBuffer, "M %d %d %d", &x_received, &y_received, &z_received);

    if (parseResult == 3) {
      Serial.print("Data Diterima via Serial2: ");
      Serial.print("X=");
      Serial.print(x_received);
      Serial.print(", Y=");
      Serial.print(y_received);
      Serial.print(", Z=");
      Serial.println(z_received);

    } else {
      Serial.println("Format data di Serial2 tidak sesuai.");
    }
  }

  // ----------- handle motion --------------- //
  x_tmp = x_received;
  y_tmp = y_received;
  z_tmp = z_received;
  holonomic(x_tmp, y_tmp, -z_tmp);
}
