#include <Keypad.h>
#include <Servo.h>
#include <Password.h>

Servo myServo;
Password myPassword = Password("1234");
int pos = 0;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4,3,2,1}; //connect to the column pinouts of the keypad

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int soundSensor = A4;
int clap = 0;
int ledPin = 13;
long detectionRangeStart = 0;
long detectionRange = 0;
boolean lightStatus = false;

int cekMasuk = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  myKeypad.addEventListener(keypadEvent);
  myServo.attach(9);
  pinMode(soundSensor, INPUT);
}

void loop() {
    char inputKey = myKeypad.getKey();
    if (cekMasuk == 1) turnLightOn();
}

void keypadEvent(KeypadEvent eKey) {
    switch (myKeypad.getState()) {
        case PRESSED: {
            Serial.print("Pressed: ");
            Serial.println(eKey);
            switch (eKey) {
                case '*': checkPassword(); break;
                case '#': myPassword.reset(), cekMasuk = 0; break;
                case '0': closeDoor(); break;
                default: myPassword.append(eKey); break;
            }
        }
    }
}

void checkPassword() {
    if (myPassword.evaluate()) {
        Serial.println("Password is valid");
        openDoor();
        cekMasuk = 1;
    } else {
        Serial.println("Password is invalid");
    }
}

void openDoor() {
    Serial.println("Opening the door");
    for (pos = 0; pos <= 90; pos ++) {
        myServo.write(pos);
        delay(15);
        break;
    }
}

void closeDoor() {
    Serial.println("Closing door");
    for (pos = 90; pos >= 0; pos --) {
        myServo.write(pos);
        delay(15);
        break;
    }
}

void turnLightOn() {
    int sensorStatus = digitalRead(soundSensor);
    if (!sensorStatus) {
        if (!clap) {
            detectionRangeStart = detectionRange = millis();
            clap ++;
        } else if (clap && millis() - detectionRange >= 50) {
            detectionRange = millis();
            clap ++;
        }
    }

    if (millis() - detectionRangeStart >= 400) {
        if (clap == 2) {
            if (!lightStatus) {
                Serial.println("Light is on");
                digitalWrite(ledPin, HIGH);
                lightStatus = true;
            } else {
                Serial.println("Light is off");
                digitalWrite(ledPin, LOW);
                lightStatus = false;
            }
        }
    }
    clap = 0;
}
