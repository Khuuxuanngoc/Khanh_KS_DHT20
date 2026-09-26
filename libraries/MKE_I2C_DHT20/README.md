# MKE I2C DHT20 Library

*Read this document in: [🇻🇳 Tiếng Việt](#tiếng-việt) | [🇬🇧 English](#english)*

---

<a id="tiếng-việt"></a>
## 🇻🇳 Tiếng Việt

**MKE_I2C_DHT20** là thư viện Arduino được thiết kế để giao tiếp với cảm biến nhiệt độ và độ ẩm DHT20 qua chuẩn I2C.

### 🌟 Tính năng nổi bật
- **Giao tiếp I2C:** Đọc dữ liệu cảm biến dễ dàng qua bus I2C (địa chỉ mặc định `0x38`).
- **Đọc dữ liệu nhanh chóng:** Cung cấp các hàm đọc nhiệt độ và độ ẩm với độ chính xác cao.
- **Dễ dàng tích hợp:** Phù hợp với nhiều nền tảng vi điều khiển khác nhau (Arduino, ESP32, v.v.).

### 🔌 Sơ đồ kết nối (Pinout)
| Chân DHT20 | Chân Arduino / ESP32 | Mô tả |
| :---: | :---: | :--- |
| **GND** | GND | Nối Đất |
| **VDD** | 3.3V hoặc 5V | Nguồn cấp |
| **SDA** | SDA | Dữ liệu I2C |
| **SCL** | SCL | Xung nhịp I2C |

### 🚀 Bắt đầu sử dụng

**Cài đặt thông qua MKE_ONE:**
Thay vì cài đặt thư viện này một cách độc lập, bạn nên cài đặt gói **`MKE_ONE`** thông qua Arduino Library Manager. `MKE_ONE` là hệ sinh thái tổng hợp sẽ tự động cài đặt thư viện này cùng tất cả các thư viện phụ thuộc khác của MakerEdu cho bạn.

Vui lòng tham khảo ví dụ trong thư mục `examples/01_Read_Data` để bắt đầu.

### 📚 Tổng hợp các hàm cơ bản (API)
```cpp
#include "MKE_I2C_DHT20.h"

MKE_I2C_DHT20 mySensor;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    mySensor.begin();
}

void loop() {
    if (millis() - mySensor.lastRead() >= 1000) {
        mySensor.read();
        Serial.print(F("Temperature: "));
        Serial.print(mySensor.getTemperature());
        Serial.println(F(" C"));
    }
}
```

---

<a id="english"></a>
## 🇬🇧 English

The **MKE_I2C_DHT20** is an Arduino library designed for reading temperature and humidity data from the DHT20 sensor via I2C.

### 🌟 Key Features
- **I2C Interface:** Easily read sensor data over the I2C bus (default address `0x38`).
- **Quick Data Reading:** Provides straightforward methods for getting precise temperature and humidity values.
- **Easy Integration:** Works seamlessly across various microcontroller platforms (Arduino, ESP32, etc.).

### 🔌 Wiring (Pinout)
| DHT20 Pin | Arduino / ESP32 Pin | Description |
| :---: | :---: | :--- |
| **GND** | GND | Ground |
| **VDD** | 3.3V or 5V | Power Supply |
| **SDA** | SDA | I2C Data |
| **SCL** | SCL | I2C Clock |

### 🚀 Getting Started

**Installation via MKE_ONE:**
Instead of installing this library directly, we strongly recommend installing the **`MKE_ONE`** package via the Arduino Library Manager. `MKE_ONE` is the central ecosystem that will automatically install this library and all other MakerEdu dependencies for you.

Please check the `examples/01_Read_Data` folder for a basic example to get started.

### 📚 Quick API Reference
```cpp
#include "MKE_I2C_DHT20.h"

MKE_I2C_DHT20 mySensor;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    mySensor.begin();
}

void loop() {
    if (millis() - mySensor.lastRead() >= 1000) {
        mySensor.read();
        Serial.print(F("Temperature: "));
        Serial.print(mySensor.getTemperature());
        Serial.println(F(" C"));
    }
}
```
