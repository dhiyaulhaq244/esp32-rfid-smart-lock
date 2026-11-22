#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include "SinricPro.h"
#include "SinricProSwitch.h" // Menggunakan pustaka Switch kembali

// ============================================================================
// 1. KREDENSIAL WI-FI & SINRIC PRO
// ============================================================================
#include "secrets.h"

// ============================================================================
// 2. KONFIGURASI PIN HARDWARE
// ============================================================================
#define PIN_LED_MERAH     17
#define PIN_LED_HIJAU     16
#define PIN_BUZZER        25
#define PIN_RELAY         4

#define PIN_RFID_SS       5
#define PIN_RFID_RST      22

MFRC522 mfrc522(PIN_RFID_SS, PIN_RFID_RST);

// ============================================================================
// 3. DAFTAR KARTU & VARIABEL STATUS
// ============================================================================
String KARTU_TERDAFTAR[] = {"0X72DFF006", "0X34812907", "0X08EF285A"};
const int JUMLAH_KARTU = sizeof(KARTU_TERDAFTAR) / sizeof(KARTU_TERDAFTAR[0]);

bool pintuTerbuka = false;
unsigned long waktuBuka = 0;
const unsigned long TIMEOUT_KUNCI = 15000; // 15 detik

// ============================================================================
// 4. FUNGSI KONTROL PINTU
// ============================================================================
void kunciPintu() {
  digitalWrite(PIN_RELAY, LOW);
  digitalWrite(PIN_LED_HIJAU, LOW);
  digitalWrite(PIN_LED_MERAH, HIGH);
  pintuTerbuka = false;
  Serial.println("Pintu TERKUNCI.\n");

  // Kirim status balik ke server: false artinya Saklar posisi OFF
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  mySwitch.sendPowerStateEvent(false); 
}

void bukaPintu() {
  digitalWrite(PIN_RELAY, HIGH);
  digitalWrite(PIN_LED_MERAH, LOW);
  digitalWrite(PIN_LED_HIJAU, HIGH);
  pintuTerbuka = true;
  waktuBuka = millis();
  Serial.println("Pintu TERBUKA.");

  // Kirim status balik ke server: true artinya Saklar posisi ON
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  mySwitch.sendPowerStateEvent(true); 
}

// ============================================================================
// 5. CALLBACK SINRIC PRO (Memproses Perintah ON/OFF dari Web/App)
// ============================================================================
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("\n[Sinric Pro] Menerima Perintah Saklar -> %s\n", state ? "ON" : "OFF");
  
  if (state && !pintuTerbuka) { // Perintah saklar diubah ke ON
    Serial.println("[Sinric Pro] Menjalankan Aksi: Membuka Pintu...");
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
    bukaPintu();
  } else if (!state && pintuTerbuka) { // Perintah saklar diubah ke OFF
    Serial.println("[Sinric Pro] Menjalankan Aksi: Mengunci Pintu...");
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
    kunciPintu();
  }
  return true;
}

// ============================================================================
// 6. SETUP AWAL
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  
  kunciPintu();

  SPI.begin();
  mfrc522.PCD_Init();

  Serial.print("Menghubungkan ke Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Terhubung! IP: " + WiFi.localIP().toString());

  // Setup Sinric Pro dengan interface Switch
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  mySwitch.onPowerState(onPowerState); // Menangani event klik sakelar

  SinricPro.begin(APP_KEY, APP_SECRET);
  Serial.println("Sistem Sinric Pro Online & Siap...");
}

// ============================================================================
// 7. LOOP UTAMA
// ============================================================================
void loop() {
  SinricPro.handle();

  // A. LOGIKA RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uidStr = "0x";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
      uidStr += String(mfrc522.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();
    Serial.println("Membaca kartu: " + uidStr);

    bool kartuValid = false;
    for (int i = 0; i < JUMLAH_KARTU; i++) {
      if (uidStr == KARTU_TERDAFTAR[i]) {
        kartuValid = true;
        break;
      }
    }

    if (kartuValid) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(100);
      digitalWrite(PIN_BUZZER, LOW);

      if (!pintuTerbuka) {
        Serial.println("Akses Kartu Diterima: Membuka...");
        bukaPintu();
      } else {
        Serial.println("Akses Kartu Diterima: Mengunci manual...");
        kunciPintu();
      }
      delay(1500);
    } else {
      Serial.println("AKSES DITOLAK! Kartu tidak dikenal.");
      digitalWrite(PIN_LED_HIJAU, LOW);
      for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_LED_MERAH, LOW);
        digitalWrite(PIN_BUZZER, HIGH);
        delay(100);
        digitalWrite(PIN_LED_MERAH, HIGH);
        digitalWrite(PIN_BUZZER, LOW);
        delay(100);
      }
    }
    mfrc522.PICC_HaltA();
  }

  // B. LOGIKA AUTO-TIMEOUT 10 DETIK
  if (pintuTerbuka) {
    if (millis() - waktuBuka >= TIMEOUT_KUNCI) {
      Serial.println("Waktu habis! Mengunci otomatis...");
      kunciPintu();
    }
  }

  delay(50);
}