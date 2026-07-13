# 🚀 Getting Started

## Prerequisites

Before building the project, ensure you have the following installed:

* Visual Studio Code
* PlatformIO Extension
* ESP32 Development Board
* USB Cable
* Wi-Fi Connection
* ThingsBoard Cloud Account

---

## 📥 Installation

1. Clone the repository.

```bash
git clone https://github.com/kabusufiyan2006/IoT-Based-Smart-Safety-Helmet-for-Mine-Workers-Using-ESP32.git
```

2. Open the project using **Visual Studio Code**.

3. Install the required PlatformIO libraries.

4. Update the following credentials in `main.cpp`:

* Wi-Fi SSID
* Wi-Fi Password
* ThingsBoard Device Token

5. Build the project.

6. Upload the firmware to the ESP32 board.

---

# ▶️ Usage

1. Power the ESP32 development board.
2. Connect the device to the configured Wi-Fi network.
3. Wait until the ESP32 connects to ThingsBoard Cloud.
4. Touch the ESP32 capacitive touch sensor once to activate the monitoring system.
5. The ESP32 continuously monitors:

   * MQ-2 Gas Sensor
   * Pulse Sensor
   * DHT11 Temperature & Humidity Sensor
   * IR Sensor (Helmet Detection)
6. Live sensor readings are displayed on the Serial Monitor.
7. Hold the touch sensor for approximately **3 seconds** to activate the SOS alert.
8. Sensor telemetry is automatically published to the configured ThingsBoard dashboard.

---

# 📁 Repository Structure

```text
IoT-Based-Smart-Safety-Helmet-for-Mine-Workers-Using-ESP32/
│
├── platformio.ini        # PlatformIO project configuration
├── README.md             # Project documentation
├── .gitignore            # Git ignore rules
└── src/
    └── main.cpp          # Main firmware source code
```

---

# 🔗 Project Showcase

This project was showcased during a hackathon.

📌 **LinkedIn Project Post**

https://www.linkedin.com/posts/abu-sufiyan-k-3419a1328_vellore-tamilnadu-hackathon-activity-7423031070297710592-Lr-s?utm_source=share&utm_medium=member_desktop&rcm=ACoAAFK6XqEB5FC1LAbWbZURtxTmLcfmIWD-DSo

---

# 🔮 Future Improvements

* GPS-based worker location tracking
* GSM/SMS emergency notifications
* Battery health monitoring
* Mobile application integration
* Additional environmental sensors
* AI-based hazard prediction
* Low-power optimization for extended battery life

---

# 📄 License

This project is licensed under the **MIT License**.

---

# 👨‍💻 Author

**Abu Sufiyan K**

**Electronics and Communication Engineering (ECE) Student**

### Connect with Me

* **GitHub:** https://github.com/kabusufiyan2006
* **LinkedIn:** https://www.linkedin.com/in/abu-sufiyan-k-3419a1328

---

## ⭐ Support

If you found this project helpful, consider giving this repository a **⭐ Star** on GitHub. Your support is greatly appreciated!
