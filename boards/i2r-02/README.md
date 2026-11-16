# i2r-02 IoT PLC (4채널)

Wi-Fi, Bluetooth를 쓰는 ESP32 PLC입니다. 입력 4개, 릴레이 출력 4개가 있습니다. KC 인증을 받았습니다.

<img src="https://github.com/kdi6033/i2r-02/raw/main/images/i2r-02-pin.png" width="700" alt="i2r-02 단자와 핀">

## 사양

| 항목 | 값 |
|---|---|
| MCU | ESP32 (플래시 8MB) |
| 전원 | DC 7~26V (보드 안에서 5V로 바꿈) |
| 입력 | 4채널, **무전압 접점**. 스위치처럼 연결만 하면 동작합니다. **입력 단자에 따로 전압을 넣으면 고장 납니다** |
| 출력 | 릴레이 4채널. 1개는 30A 250VAC/30VDC, 3개는 10A 125VAC/28VDC |
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
| LED | 2 | 보드 내장 LED |
| 공장 초기화 버튼 | 34 | 입력 전용, 누르면 LOW |

## Arduino IDE 설정

| 메뉴 | 값 |
|---|---|
| 보드 | ESP32 Dev Module |
| Flash Size | 8MB (64Mb) |
| Partition Scheme | Huge APP (3MB No OTA/1MB SPIFFS) |

```
FQBN = esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app
```

## 예제

- [i2r02_output_blink](../../examples/i2r02_output_blink/): 출력 1~4를 차례로 켜기
- [i2r02_input_to_output](../../examples/i2r02_input_to_output/): 입력이 들어오면 같은 번호의 출력 켜기

## 더 보기

- 제품 설명, 케이스 STL, 공식 펌웨어 소스: https://github.com/kdi6033/i2r-02
- 공식 펌웨어 파일: https://github.com/kdi6033/download (`i2r-02.ino.merged.bin`)
