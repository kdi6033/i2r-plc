# i2r-04 IoT PLC (8채널)

Wi-Fi, Bluetooth를 쓰는 ESP32 PLC입니다. 입력 8개, 릴레이 출력 8개가 있습니다. KC 인증을 받았습니다.

<img src="https://github.com/kdi6033/i2r-04/raw/main/images/i2r-04-pin.jpg" width="700" alt="i2r-04 단자와 핀">

## 사양

| 항목 | 값 |
|---|---|
| MCU | ESP32 (플래시 8MB) |
| USB | USB-시리얼 칩 CH340C |
| 전원 | DC 7~26V (보드 안에서 5V로 바꿈) |
| 입력 | 8채널, **무전압 접점**. 스위치처럼 연결만 하면 동작합니다. **입력 단자에 따로 전압을 넣으면 고장 납니다** |
| 출력 | 릴레이 8채널. 1개는 30A 250VAC/30VDC, 7개는 10A 125VAC/28VDC |
| 통신 | Wi-Fi 2.4GHz (5GHz 안 됨), Bluetooth 4.2 / BLE, TTL 시리얼(RX, TX 단자) |
| 작동 온도 | -40 ~ 85℃ |

## 핀 맵

공식 펌웨어 소스(`board-i2r-04.ino`) 기준입니다.

| 기능 | GPIO | 비고 |
|---|---|---|
| OUT1 | 26 | HIGH = 릴레이 켜짐 |
| OUT2 | 27 | |
| OUT3 | 32 | |
| OUT4 | 33 | |
| OUT5 | 21 | |
| OUT6 | 22 | |
| OUT7 | 23 | |
| OUT8 | 25 | |
| IN1 | 18 | HIGH = 입력 들어옴 |
| IN2 | 19 | |
| IN3 | 4 | |
| IN4 | 5 | |
| IN5 | 12 | **전원을 켜는 순간 입력이 들어와 있으면 부팅이 안 될 수 있음** (ESP32 부팅 설정 핀) |
| IN6 | 13 | |
| IN7 | 14 | |
| IN8 | 15 | |
| LED | 2 | 보드 내장 LED |
| 공장 초기화 버튼 | 36 | 입력 전용, 누르면 LOW |

- GPIO 21, 22가 릴레이 출력(OUT5, OUT6)이라서 I2C 기본 핀을 쓸 수 없습니다. I2C 센서를 달려면 다른 핀을 정해야 합니다.

## Arduino IDE 설정

| 메뉴 | 값 |
|---|---|
| 보드 | ESP32 Dev Module |
| Flash Size | 8MB (64Mb) |
| Partition Scheme | Huge APP (3MB No OTA/1MB SPIFFS) |

```
FQBN = esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app
```

라이브러리 매니저에서 **I2R_PLC**를 설치하고 `I2R_PLC plc(I2R_04);`로 씁니다. 핀 번호 없이 `plc.output(1, ON)`처럼 보드 번호로 씁니다.

## 예제

- [i2r04_output_blink](../../examples/i2r04_output_blink/): 출력 1~8을 차례로 켜기
- [i2r04_input_to_output](../../examples/i2r04_input_to_output/): 입력이 들어오면 같은 번호의 출력 켜기

## 더 보기

- 제품 설명, 회로도, 공식 펌웨어 소스: https://github.com/kdi6033/i2r-04
- 공식 펌웨어 파일: https://github.com/kdi6033/download (`i2r-04.ino.merged.bin`)
