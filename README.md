# IoT-Based Smart Lock System Using ESP32 and RFID

## 1. Project Overview

An IoT-based smart door lock system developed using the ESP32 microcontroller, RFID authentication, and Sinric Pro cloud integration.

The system provides two access mechanisms: local authentication through RFID cards and remote door control through a Wi-Fi-connected application.

It combines embedded systems, wireless communication, and cloud-based device control to implement a flexible electronic access control system.

## 2. Key Features

- **RFID Authentication:** Uses an MFRC522 RFID reader to identify authorized RFID cards.
- **Remote Access Control:** Enables remote door locking and unlocking through Sinric Pro.
- **Automatic Door Locking:** Automatically locks the door after 15 seconds of being unlocked.
- **Access Feedback:** Uses red and green LEDs and a buzzer to indicate access status.
- **Real-Time Synchronization:** Sends door status updates to the Sinric Pro platform.
- **Access Validation:** Rejects unregistered RFID cards.

## 3. Hardware Components

| Component | Function |
|---|---|
| ESP32 | Main microcontroller and Wi-Fi connectivity |
| MFRC522 RFID Reader | Reads RFID card identification data |
| RFID Cards/Tags | User authentication credentials |
| Relay Module | Controls the electronic locking mechanism |
| Red LED | Indicates locked or denied access |
| Green LED | Indicates unlocked or accepted access |
| Buzzer | Provides audible feedback |
| Electronic Door Lock | Physical locking mechanism |

## 4. Software and Technologies

- Arduino IDE
- ESP32 Arduino Core
- C/C++ (Arduino Framework)
- MFRC522 Library
- SPI Library
- WiFi Library
- Sinric Pro Library
- Sinric Pro Cloud Platform

## 5. Hardware Pin Configuration

| Component | ESP32 GPIO |
|---|---|
| Red LED | GPIO 17 |
| Green LED | GPIO 16 |
| Buzzer | GPIO 25 |
| Relay | GPIO 4 |
| RFID SDA / SS | GPIO 5 |
| RFID RST | GPIO 22 |
| RFID SCK | SPI SCK |
| RFID MOSI | SPI MOSI |
| RFID MISO | SPI MISO |

*Note: SPI pin assignments use the ESP32 default SPI configuration initialized through `SPI.begin()`.*

## 6. System Architecture

The system consists of three main functional layers:

1. **Input Layer:** RFID reader receives card identification data.
2. **Processing Layer:** ESP32 validates the RFID card and manages the door state.
3. **Output and Communication Layer:** Relay, LEDs, and buzzer provide physical responses, while Wi-Fi and Sinric Pro enable remote control and status synchronization.

## 7. System Operation

### A. RFID-Based Access Control

1. An RFID card is presented to the MFRC522 reader.
2. The reader retrieves the card UID.
3. ESP32 compares the UID with the registered card list.
4. If the card is authorized, the door changes its current state:
   - Locked → Unlocked.
   - Unlocked → Locked.
5. If the card is unauthorized, access is denied and the buzzer and red LED provide warning signals.

### B. Remote Access Control

1. ESP32 connects to a Wi-Fi network.
2. The device establishes communication with Sinric Pro.
3. Users can send ON/OFF commands through the connected Sinric Pro interface.
4. The ESP32 processes the command and controls the relay.
5. Door status changes are synchronized with the cloud platform.

### C. Automatic Locking

When the door is unlocked, the ESP32 records the opening time using `millis()`.

If the unlocked duration reaches 15 seconds, the system automatically activates the locking mechanism.

## 8. Getting Started

### Requirements

- ESP32 development board
- MFRC522 RFID module
- Arduino IDE
- Required Arduino libraries
- Sinric Pro account and configured switch device

### Installation

1. Install Arduino IDE.
2. Install the ESP32 board package.
3. Install the required libraries:
   - MFRC522
   - SinricPro
4. Configure the Wi-Fi credentials and Sinric Pro credentials securely.
5. Configure the hardware connections according to the pin configuration.
6. Open `esp32-rfid-smart-lock.ino`.
7. Select the appropriate ESP32 board and COM port.
8. Upload the program to the ESP32.

## 9. Project Status

**Status:** Implemented Prototype

The current implementation supports RFID-based authentication, remote control through Sinric Pro, visual and audible feedback, and automatic door locking.

## 10. Future Development

Potential improvements include:

- Web-based access management.
- Dynamic RFID user registration and deletion.
- Access logs and timestamp recording.
- Improved authentication and credential security.
- Non-blocking control logic for more responsive operation.
- Integration with additional IoT monitoring features.

## 11. Author

**Dhiya'ul Haq**  
Electrical Engineering Student  
Universitas Muhammadiyah Yogyakarta

---

*This project was developed as an embedded systems and Internet of Things (IoT) learning project.*