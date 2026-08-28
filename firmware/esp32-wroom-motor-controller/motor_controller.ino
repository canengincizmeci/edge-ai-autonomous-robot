
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;


#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 13
#define IN3 14
#define IN4 12


#define CAM_RX 16
#define CAM_TX 17

HardwareSerial CamSerial(2);

bool robotEnabled = false;

unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 1000;

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void executeCommand(char cmd) {
  if (!robotEnabled) {
    stopMotors();
    return;
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'L':
      turnLeft();
      break;

    case 'R':
      turnRight();
      break;

    case 'S':
      stopMotors();
      break;

    case 'B':
   
      break;
  }

  Serial.print("CMD: ");
  Serial.println(cmd);
}

void setup() {
  Serial.begin(115200);

  SerialBT.begin("GreenBalloonRobot");

  CamSerial.begin(
    115200,
    SERIAL_8N1,
    CAM_RX,
    CAM_TX
  );

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  stopMotors();

  Serial.println("WROOM MOTOR CONTROLLER READY");
  Serial.println("Bluetooth: START / STOP");
}

void loop() {
  
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command == "START") {
      robotEnabled = true;
      stopMotors();

      Serial.println("ROBOT ENABLED");
      SerialBT.println("ROBOT ENABLED");
    }

    else if (command == "STOP") {
      robotEnabled = false;
      stopMotors();

      Serial.println("ROBOT DISABLED");
      SerialBT.println("ROBOT DISABLED");
    }
  }

 
  while (CamSerial.available()) {
    char cmd = CamSerial.read();

    if (
      cmd == 'F' ||
      cmd == 'L' ||
      cmd == 'R' ||
      cmd == 'S' ||
      cmd == 'B'
    ) {
      lastCommandTime = millis();
      executeCommand(cmd);
    }
  }


  if (
    robotEnabled &&
    millis() - lastCommandTime > COMMAND_TIMEOUT
  ) {
    stopMotors();
  }
}
