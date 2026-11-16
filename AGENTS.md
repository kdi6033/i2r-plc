# AGENTS.md — i2r IoT PLC AI 코딩 안내

이 파일은 **AI 코딩 도구(Claude, ChatGPT, Gemini, Cursor 등)가 읽는 안내서**입니다.
i2r IoT PLC 보드(**i2r-02, i2r-03, i2r-04**)의 아두이노 프로그램을 만들 때 여기 적힌 하드웨어 정보와 규칙을 그대로 따르세요.
보드별 자세한 설명은 [boards/](boards/)에 있습니다.

---

## 0. 사용자와 대화 방식

- 사용자는 대부분 **학생과 PLC 초보자**입니다. **한국어**로, 쉬운 말로 설명합니다.
- **어떤 보드인지 먼저 확인합니다.** 사용자가 말하지 않았으면 "i2r-02, i2r-03, i2r-04 중 어느 보드인가요?"라고 묻습니다. 보드마다 핀이 달라서 추측하면 안 됩니다.
- 코드는 **처음부터 끝까지 전체**를 줍니다. "...생략..." 같은 부분 코드를 주지 않습니다.
- 한 번에 한 가지 동작만 만들고, 올려서 확인한 뒤 다음 기능을 덧붙입니다.
- 코드에는 초보자가 이해할 수 있는 한국어 주석을 붙입니다.
- 릴레이에는 실제 전기 장치(모터, 전등, 히터)가 연결될 수 있습니다. 출력을 켜는 코드를 올리기 전에 **출력에 무엇이 연결되어 있는지** 확인합니다.

## 1. 보드 한눈에 보기

| 보드 | 입력 | 출력(릴레이) | 센서 | 전원 |
|---|---|---|---|---|
| **i2r-02** | 4채널, **무전압 접점** | 4채널 (1개 30A, 3개 10A) | 없음 | DC 7~26V |
| **i2r-03** | 4채널, **전압 입력** (COM 사용) | 4채널 (1개 30A, 3개 10A) | AHT21 온습도 (I2C) | DC 7~30V |
| **i2r-04** | 8채널, **무전압 접점** | 8채널 (1개 30A, 7개 10A) | 없음 | DC 7~26V |

- **무전압 접점 입력(i2r-02, i2r-04)**: 스위치나 릴레이 접점처럼 **연결만 하면** 동작합니다. 입력 단자에 따로 전압을 넣으면 **보드가 고장 납니다.** 배선을 안내할 때 꼭 알려 줍니다.
- **전압 입력(i2r-03)**: COM 단자에 0V 또는 입력 전압을 연결하고, 입력 단자에 전압이 들어오면 동작합니다.

**세 보드 공통**

| 항목 | 값 |
|---|---|
| MCU | ESP32 (ESP32-WROOM-32E, 플래시 8MB) |
| USB | USB-시리얼 칩 CH340C (세 보드 공통) |
| 전원 | 위 표의 DC 전압을 보드 안에서 5V로 바꿈. USB로도 프로그램 업로드와 시험 가능 |
| 통신 | Wi-Fi 2.4GHz (5GHz 안 됨), Bluetooth 4.2 / BLE |
| 내장 LED | GPIO 2 |

## 2. 핀 맵

출력은 모두 **HIGH = 릴레이 켜짐(ON)**, 입력은 **HIGH = 입력 들어옴(ON)** 입니다. (공식 펌웨어 기준)

### i2r-02

| 기능 | GPIO |
|---|---|
| OUT1, OUT2, OUT3, OUT4 | 26, 27, 32, 33 |
| IN1, IN2, IN3, IN4 | 16, 17, 18, 19 |
| LED | 2 |
| 공장 초기화 버튼 | 34 (누르면 LOW) |

### i2r-03

| 기능 | GPIO |
|---|---|
| OUT1, OUT2, OUT3, OUT4 | 26, 27, 32, 33 |
| IN1, IN2, IN3, IN4 | 16, 17, 18, 19 |
| AHT21 온습도 센서 (I2C) | SDA 21, SCL 22 (주소 0x38) |
| LED | 2 |
| 공장 초기화 버튼 | 34 (누르면 LOW) |

### i2r-04

| 기능 | GPIO |
|---|---|
| OUT1 ~ OUT8 | 26, 27, 32, 33, 21, 22, 23, 25 |
| IN1 ~ IN8 | 18, 19, 4, 5, 12, 13, 14, 15 |
| LED | 2 |
| 공장 초기화 버튼 | 36 (누르면 LOW) |

**핀 주의 사항**

- GPIO 34, 36은 **입력 전용**이고 내부 풀업이 없습니다. 출력으로 쓰지 않습니다.
- i2r-04의 GPIO 21, 22는 **릴레이 출력(OUT5, OUT6)** 입니다. i2r-04에서 I2C 센서를 쓰려면 사용자와 다른 핀을 먼저 정합니다.
- i2r-04의 **IN5(GPIO 12)** 는 ESP32가 켜질 때 플래시 전압을 정하는 핀입니다. 전원을 켜는 순간 IN5에 입력이 들어와 있으면 부팅이 안 될 수 있습니다. 부팅이 안 된다고 하면 이것부터 확인합니다.

## 3. 개발 환경

- ESP32 보드 패키지: **esp32 by Espressif Systems 3.0 이상** (3.3.x에서 확인함)
- 보드 설정 (세 보드 모두 같음)

| Arduino IDE 메뉴 | 값 |
|---|---|
| 보드 | **ESP32 Dev Module** |
| Flash Size | 8MB (64Mb) |
| Partition Scheme | **Huge APP (3MB No OTA/1MB SPIFFS)** |
| Erase All Flash Before Sketch Upload | Disabled |

```
FQBN = esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app
```

- 업데이트는 USB(시리얼)로만 합니다. OTA(무선 업데이트)는 쓰지 않습니다. 그래서 프로그램 공간이 큰 Huge APP을 씁니다. Wi-Fi와 BLE를 함께 써도 공간이 충분합니다.
- `partitions.csv` 파일은 만들지 않습니다. (공식 펌웨어 소스에 있는 partitions.csv는 OTA용이라 내 스케치에는 필요 없습니다)
- 필요한 라이브러리

| 라이브러리 | arduino-cli 설치 이름 | 언제 |
|---|---|---|
| Adafruit AHTX0 | `"Adafruit AHTX0"` | i2r-03 온습도 센서 |
| PubSubClient | `PubSubClient` | MQTT 통신 |
| ArduinoJson | `ArduinoJson` | MQTT 메시지(JSON) |

- 시리얼 속도: **115200**

## 4. 꼭 지킬 규칙

1. **핀 번호는 2장 표에서 사용자의 보드에 맞는 것만** 씁니다. i2r-02/03과 i2r-04는 입력 핀이 다릅니다.
2. 출력 핀은 `setup()`에서 `pinMode(핀, OUTPUT)` 다음에 **바로 `digitalWrite(핀, LOW)`** 로 꺼진 상태에서 시작합니다. 전원을 켜자마자 릴레이가 켜지면 연결된 장치가 갑자기 움직일 수 있습니다.
3. 입력은 `pinMode(핀, INPUT)`으로 둡니다. 버튼 신호는 떨림(채터링)이 있으므로 **30~50ms 디바운스**를 넣습니다.
4. **`delay()`로 오래 멈추지 않습니다.** PLC는 입력을 계속 보고 있어야 합니다. 타이머(TON, TOF), 깜빡임, 센서 주기는 `millis()`로 만듭니다. (예제 [examples/i2r03_input_timer](examples/i2r03_input_timer/) 참고)
5. 같은 출력을 짧은 간격으로 계속 켜고 끄지 않습니다. 온도 제어처럼 기준값 근처에서 흔들리는 제어는 **히스테리시스**(예: 30도에서 켜고 28도에서 끄기)를 둡니다. 릴레이가 상하고 연결 장치에도 무리가 갑니다.
6. 온습도 센서는 **2초에 한 번** 정도만 읽습니다. 센서를 찾지 못해도 `while(1)`로 멈추지 말고, 시리얼에 알린 뒤 나머지 PLC 동작은 계속합니다.
7. **Wi-Fi 비밀번호, 이메일, 서버 주소는 `secrets.h`에 따로 두고** 스케치에서 `#include "secrets.h"`로 불러옵니다. `secrets.h`는 `.gitignore`에 들어 있어 GitHub에 올라가지 않습니다. 비밀 값을 코드에 직접 쓰거나 시리얼로 출력하지 않습니다.
8. **NVS(보드 저장 공간)를 통째로 지우지 않습니다.** `nvs_flash_erase()`를 쓰지 않습니다. 값을 저장하려면 `Preferences`로 `my`로 시작하는 새 이름을 씁니다. 예: `prefs.begin("myplc", false);`
9. MQTT를 쓸 때 메시지는 **JSON**으로 하고, 명령 형식은 [i2r 프로토콜](https://github.com/kdi6033/i2r/blob/main/README.md)을 따릅니다. 새 형식을 마음대로 만들지 않습니다.
10. 새 프로그램은 `my_sketches/스케치이름/스케치이름.ino`로 만듭니다. (폴더 이름과 .ino 파일 이름이 같아야 합니다) `examples/`와 `boards/`는 사용자가 요청할 때만 고칩니다.

## 5. 프로그램 모양 (이 틀에서 시작)

i2r-03 예시입니다. 다른 보드는 2장 표에 맞춰 핀 배열만 바꿉니다.

```cpp
// i2r-03 기본 틀: 출력 4개, 입력 4개
const int OUT_PINS[4] = {26, 27, 32, 33};   // OUT1~OUT4 (HIGH = 릴레이 켜짐)
const int IN_PINS[4]  = {16, 17, 18, 19};   // IN1~IN4  (HIGH = 입력 들어옴)
const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(OUT_PINS[i], OUTPUT);
    digitalWrite(OUT_PINS[i], LOW);         // 꺼진 상태에서 시작 (규칙 2)
    pinMode(IN_PINS[i], INPUT);
  }
  pinMode(LED_PIN, OUTPUT);
  Serial.println("i2r-03 PLC 시작");
}

void loop() {
  // 예: IN1이 들어오면 OUT1을 켠다
  bool in1 = digitalRead(IN_PINS[0]) == HIGH;
  digitalWrite(OUT_PINS[0], in1 ? HIGH : LOW);
}
```

PLC에서 자주 쓰는 동작을 아두이노로 옮기는 방법입니다.

| PLC 동작 | 아두이노에서 |
|---|---|
| 접점 (A접점 / B접점) | `digitalRead(핀) == HIGH` / `== LOW` |
| 코일 | `digitalWrite(핀, 상태)` |
| 자기유지 회로 | `bool` 변수에 상태를 기억 (START로 켜고 STOP으로 끔) |
| 상승 에지 (입력이 들어오는 순간) | 이전 상태를 기억했다가 `이전 == LOW && 지금 == HIGH` |
| TON (ON 딜레이 타이머) | 입력이 들어온 시각을 `millis()`로 기억, 정한 시간이 지나면 출력 |
| TOF (OFF 딜레이 타이머) | 입력이 꺼진 시각을 기억, 정한 시간 동안 출력 유지 |
| 카운터 | 상승 에지마다 변수 +1 |

## 6. 참고할 예제

새 프로그램은 가장 비슷한 예제를 먼저 읽고 그 방식대로 만듭니다. ([examples/](examples/))

| 예제 | 보드 | 내용 |
|---|---|---|
| i2r02_output_blink | i2r-02 | 출력 1~4를 차례로 1초씩 켜기 |
| i2r02_input_to_output | i2r-02 | IN1~4가 들어오면 같은 번호의 OUT1~4 켜기 |
| i2r03_output_blink | i2r-03 | 출력 1~4를 차례로 1초씩 켜기 |
| i2r03_input_to_output | i2r-03 | IN1~4가 들어오면 같은 번호의 OUT1~4 켜기 |
| i2r03_input_timer | i2r-03 | IN1을 누르면 OUT1을 5초 켜기 (`millis()` 타이머, 디바운스) |
| i2r03_temp_relay | i2r-03 | 온도 30도 이상이면 OUT1 켜고 28도 이하면 끄기 (히스테리시스) |
| i2r04_output_blink | i2r-04 | 출력 1~8을 차례로 1초씩 켜기 |
| i2r04_input_to_output | i2r-04 | IN1~8이 들어오면 같은 번호의 OUT1~8 켜기 |

웹에서 읽을 때는 raw 주소를 씁니다. 예: `https://raw.githubusercontent.com/kdi6033/i2r-plc/main/examples/i2r03_temp_relay/i2r03_temp_relay.ino`

> I2R_PLC 라이브러리(보드 이름만 고르면 핀을 알아서 맞추는 라이브러리)를 준비하고 있습니다. 나오면 이 장과 예제를 라이브러리 방식으로 바꿉니다.

## 7. Claude Code / 터미널에서 직접 컴파일하고 올리기

AI 코딩 도구가 터미널을 쓸 수 있으면 아래 순서로 **직접 컴파일, 업로드, 결과 확인**까지 합니다.

**① 준비 확인 (처음 한 번)**

```bash
arduino-cli version
```

- 없으면 Arduino IDE에 들어 있는 것을 씁니다.
  Windows: `"/c/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe"`
- 그것도 없으면 설치합니다. Arduino IDE는 없어도 됩니다.
  - Windows: `winget install --id ArduinoSA.CLI -e --source winget --accept-source-agreements --accept-package-agreements`
  - macOS: `brew install arduino-cli`
  - 설치 직후에는 지금 터미널의 PATH에 반영되지 않아 `arduino-cli`를 못 찾을 수 있습니다. 그때는 `%LOCALAPPDATA%\Microsoft\WinGet\Links\arduino-cli.exe` 또는 `%LOCALAPPDATA%\Microsoft\WinGet\Packages\ArduinoSA.CLI*\arduino-cli.exe`를 찾아 **전체 경로로** 실행합니다.
- 보드 패키지와 라이브러리 설치 (이미 있으면 건너뜀). esp32 보드 패키지는 내려받는 양이 많아 **5~15분** 걸릴 수 있으니 사용자에게 미리 알립니다.

```bash
arduino-cli core update-index --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core install esp32:esp32 --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli lib install "Adafruit AHTX0" PubSubClient ArduinoJson
```

**② 컴파일**

```bash
arduino-cli compile --fqbn esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app my_sketches/my_plc
```

오류가 나면 오류 메시지를 읽고 고친 뒤 다시 컴파일합니다. 성공할 때까지 반복합니다.

**③ 포트 찾기**

```bash
arduino-cli board list
```

i2r PLC는 USB-시리얼 칩(CH340C)이라 보통 `USB-SERIAL CH340` 또는 VID `1A86`으로 보입니다. (Windows `COM3` 같은 이름, macOS `/dev/cu.wchusbserial...` 또는 `/dev/cu.usbserial-...`)
포트가 여러 개면 사용자에게 USB를 뺐다 꽂게 해서 새로 생긴 포트를 고릅니다. 포트가 아예 안 보이면 CH340 드라이버가 필요할 수 있습니다. (8장)

**④ 업로드**

```bash
arduino-cli upload -p COM5 --fqbn esp32:esp32:esp32:FlashSize=8M,PartitionScheme=huge_app my_sketches/my_plc
```

**⑤ 시리얼 출력 확인** (정해진 시간만 읽고 끝남)

- Windows: `powershell -ExecutionPolicy Bypass -File tools/serial-read.ps1 -Port COM5 -Seconds 10`
- macOS/Linux: `timeout 10 arduino-cli monitor -p /dev/cu.wchusbserial1410 --config baudrate=115200`

**⑥ 사용자에게 확인 요청**: 릴레이 딸깍 소리, 보드의 출력 LED, 연결한 장치가 실제로 어떻게 동작했는지 묻고, 그 말을 바탕으로 고칩니다.
입력은 시리얼에 상태를 찍어 두고 사용자와 함께 확인합니다. i2r-03은 "IN1 단자에 전압을 넣어 보세요", i2r-02/04는 "IN1 접점을 스위치로 연결해 보세요(배선은 boards 문서의 단자 그림 참고)"처럼 보드에 맞게 부탁합니다. (i2r-02/04 입력에 전압을 넣으면 고장)

## 8. 문제 해결

| 증상 | 원인과 해결 |
|---|---|
| 포트가 안 보임 | ① **데이터 전송이 되는** USB-C 케이블인지 확인 (충전 전용 케이블은 포트가 안 보임) ② CH340 드라이버 설치 (Windows는 대부분 자동, 안 되면 WCH 공식 사이트의 CH341SER 드라이버) ③ 다른 USB 포트나 케이블로 바꿔 보기 |
| 업로드 실패 (`Failed to connect`, `Timed out waiting for packet header`) | ① Arduino IDE 시리얼 모니터 등 다른 프로그램이 포트를 쓰고 있으면 닫기 ② 업로드 속도를 낮춰 다시 시도 (`--board-options UploadSpeed=115200`) ③ 그래도 안 되면 보드의 **BOOT 버튼을 누른 채** 업로드를 시작하고 `Connecting...`이 나오면 손을 뗌 |
| i2r-04가 전원을 켜도 시작하지 않음 | IN5(GPIO 12)에 입력이 들어와 있는지 확인. 입력을 빼고 다시 켬 (2장 핀 주의 사항) |
| 릴레이가 안 켜짐 | 핀 번호가 그 보드 것인지, `pinMode(핀, OUTPUT)`을 했는지 확인. USB 전원만으로 릴레이 여러 개를 동시에 켜면 전원이 부족할 수 있으니 DC 전원을 연결 |
| 입력이 반대로 읽힘 | i2r-03은 COM 단자 배선(0V 또는 입력 전압)을, i2r-02/04는 접점 배선을 확인. 시리얼에 `digitalRead` 값을 찍어 사용자와 함께 확인한 뒤 코드의 비교를 맞춤. **i2r-02/04 입력에 전압을 넣어 시험하지 않음** (고장) |
| `Adafruit_AHTX0.h: No such file` | 3장 표의 라이브러리를 설치 |
| 온습도 센서를 못 찾음 (i2r-03) | `Wire.begin(21, 22)`를 불렀는지, 다른 코드가 GPIO 21, 22를 쓰고 있지 않은지 확인 |
| Wi-Fi 연결이 안 됨 | 2.4GHz 공유기인지 확인 (5GHz는 안 됨). `secrets.h`의 이름과 비밀번호 확인 |

## 9. 공식 펌웨어로 되돌리기

내 스케치를 올리면 공식 i2r 펌웨어(스마트폰 앱 연결, MQTT 원격 제어)가 덮어써집니다.
다시 쓰려면 i2r.link에서 그 보드의 펌웨어를 다시 설치합니다. 펌웨어 파일은 [download](https://github.com/kdi6033/download) 저장소에도 있습니다.

## 10. 문서

- 보드별 설명: [boards/i2r-02](boards/i2r-02/README.md), [boards/i2r-03](boards/i2r-03/README.md), [boards/i2r-04](boards/i2r-04/README.md)
- 통신 프로토콜(MQTT, JSON 명령): https://github.com/kdi6033/i2r/blob/main/README.md
- 제품 설명과 공식 펌웨어 소스: https://github.com/kdi6033/i2r-02, https://github.com/kdi6033/i2r-03, https://github.com/kdi6033/i2r-04
