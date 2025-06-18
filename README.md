# 💧 Water Leakage Detection System (ESP32 + Blynk)

This project detects water leakage using flow sensors and notifies the user via Blynk IoT. It also displays flow rates on an LCD and triggers a buzzer when leakage is detected.

## 🧰 Hardware Used
- ESP32 Dev Module
- Flow Sensors (Inlet & Outlet)
- 16x2 I2C LCD
- Buzzer
- Blynk IoT Platform

## 🔌 Circuit Diagram
*(Include or link an image here if possible)*

## 📱 Features
- Detects leakage based on flow rate difference
- Sends alerts via Blynk
- Activates buzzer on leakage
- LCD shows inlet/outlet flow

## 🧠 How It Works
The code compares the inlet and outlet flow. If the difference is above a threshold, it assumes leakage and alerts via Blynk + buzzer.

## 📜 License
MIT License

## 📸 Project Photo

![Water Leakage Detector](https://github.com/Shounakjaipurkar/Water-leakage-Detection/blob/main/Leakage.jpg)

