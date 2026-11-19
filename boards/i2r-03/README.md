# i2r-03 IoT PLC (4채널 + 온습도 센서)

Wi-Fi, Bluetooth를 쓰는 ESP32 PLC입니다. 입력 4개, 릴레이 출력 4개, AHT21 온습도 센서가 있습니다. KC 인증을 받았습니다.

<img src="https://github.com/kdi6033/i2r-03/raw/main/images/i2r-03-port.jpg" width="700" alt="i2r-03 단자와 핀">

## 사양

| 항목 | 값 |
|---|---|
| MCU | ESP32-WROOM-32E-N8 (플래시 8MB) |
| USB | USB-C, USB-시리얼 칩 CH340C |
| 전원 | DC 7~30V (보드 안에서 5V로 바꿈) |
| 입력 | 4채널, **전압 입력**. COM에 0V 또는 입력 전압을 연결하고, 입력 단자에 전압이 들어오면 동작 |
| 출력 | 릴레이 4채널. 1개는 30A 250VAC/30VDC, 3개는 10A 125VAC/28VDC |
| 센서 | AHT21 온습도 (I2C, 주소 0x38). 온도 -40~120℃, 습도 0~100%RH |
| 통신 | Wi-Fi 2.4GHz (5GHz 안 됨), Bluetooth 4.2 / BLE, TTL 시리얼(RX, TX 단자) |
| 작동 온도 | -40 ~ 85℃ |

## 핀 맵

| 기능 | GPIO | 비고 |
|---|---|---|
| OUT1 | 26 | HIGH = 릴레이 켜짐 |
| OUT2 | 27 | |
| OUT3 | 32 | |
| OUT4 | 33 | |
| IN1 | 16 | HIGH = 입력 들어옴 |
| IN2 | 17 | |
| IN3 | 18 | |
| IN4 | 19 | |
| SDA | 21 | AHT21 온습도 센서 |
| SCL | 22 | AHT21 온습도 센서 |
| LED | 2 | 보드 내장 LED |
| 공장 초기화 버튼 | 34 | 입력 전용, 누르면 LOW |

## 입력 배선

<img src="https://github.com/kdi6033/i2r-03/blob/main/%EC%9E%90%EB%A3%8C/i2r-03%20input%20%EB%B0%B0%EC%84%A0.png?raw=true" width="600" alt="i2r-03 입력 배선">

## Arduino IDE 설정

| 메뉴 | 값 |
|---|---|
| 보드 | ESP32 Dev Module |
| Flash Size | 8MB (64Mb) |
| Partition Scheme | Huge APP (3MB No OTA/1MB SPIFFS) |

```
FQBN = esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app
```

라이브러리 매니저에서 **I2R_PLC**를 설치하고 `I2R_PLC plc(I2R_03);`로 씁니다. 온습도 센서도 `plc.temperature()`, `plc.humidity()`로 읽습니다. (Adafruit AHTX0은 필요 없음)

## 예제

- [i2r03_output_blink](../../examples/i2r03_output_blink/): 출력 1~4를 차례로 켜기
- [i2r03_input_to_output](../../examples/i2r03_input_to_output/): 입력이 들어오면 같은 번호의 출력 켜기
- [i2r03_input_timer](../../examples/i2r03_input_timer/): IN1을 넣으면 OUT1을 5초 켜기
- [i2r03_temp_relay](../../examples/i2r03_temp_relay/): 30도 이상이면 OUT1 켜고 28도 이하면 끄기

## 더 보기

- 제품 설명, 공식 펌웨어 소스: https://github.com/kdi6033/i2r-03
- 공식 펌웨어 파일: https://github.com/kdi6033/download (`i2r-03.ino.merged.bin`)
