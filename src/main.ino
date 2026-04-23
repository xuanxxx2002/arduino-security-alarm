#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

#define PIR_PIN 2
#define BUZZER_PIN 3
#define LED_GREEN 4
#define LED_RED 5

bool systemArmed = true;

// 允許的 RFID UID
byte validUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.println("System Ready...");
}

void loop() {
  checkRFID();
  checkMotion();
}

// RFID 驗證
void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (isValidCard(rfid.uid.uidByte)) {
    Serial.println("Access Granted");
    disarmSystem();
  } else {
    Serial.println("Access Denied");
    triggerAlarm();
  }

  rfid.PICC_HaltA();
}

// 驗證卡片
bool isValidCard(byte *uid) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] != validUID[i]) return false;
  }
  return true;
}

// 紅外線偵測
void checkMotion() {
  if (systemArmed && digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Motion Detected!");
    triggerAlarm();
  }
}

// 觸發警報
void triggerAlarm() {
  digitalWrite(LED_RED, HIGH);

  for (int i = 0; i < 5; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }

  digitalWrite(LED_RED, LOW);
}

// 解除警報
void disarmSystem() {
  systemArmed = false;
  digitalWrite(LED_GREEN, HIGH);
  delay(3000);
  digitalWrite(LED_GREEN, LOW);
  systemArmed = true;
}