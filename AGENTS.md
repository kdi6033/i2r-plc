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

## 0-1. 제작 콘셉트 (모든 프로그램에 적용)

i2r PLC는 **Wi-Fi로 원격 제어하는 것이 기본**이고, **Wi-Fi가 연결되지 않아도 혼자(stand alone) PLC로 동작**해야 합니다. 프로그램은 두 부분으로 나눕니다.

| 부분 | 하는 일 | 규칙 |
|---|---|---|
| PLC 제어 (항상) | 입력 읽기, 타이머, 자기유지, 온도 제어, 출력 | Wi-Fi, MQTT 상태와 상관없이 `loop()`에서 매번 실행한다 |
| 원격 제어 (있으면 더함) | Wi-Fi 연결, MQTT 명령 받기, 상태 보내기 | 연결을 기다리며 멈추지 않는다. 끊기면 몇 초마다 다시 시도한다 |

- `setup()`에서 Wi-Fi 연결을 `while (WiFi.status() != WL_CONNECTED)`처럼 **기다리지 않습니다.** `WiFi.begin()`만 하고 바로 넘어갑니다.
- 다시 연결은 `millis()`로 5~10초마다 시도합니다. `delay()`로 기다리지 않습니다.
- 출력은 **하나의 함수 `plc.output(n, on)`** 으로만 바꿉니다. 입력 단자에서 온 제어와 MQTT 명령이 모두 이 함수를 부릅니다. 출력이 바뀐 것을 서버에 알릴 때는 `plc.onOutputChange(함수)`를 씁니다.
- Wi-Fi가 다시 연결되면 지금 입력과 출력 상태를 서버에 다시 보냅니다.
- Wi-Fi가 끊겼다고 출력을 바꾸지 않습니다. 사용자가 "끊기면 꺼 줘"처럼 따로 말한 경우에만 그렇게 만듭니다.
- 사용자가 Wi-Fi 없이 PLC 동작만 원하면 원격 제어 부분은 빼도 됩니다. 원격 제어를 더할 때도 PLC 제어 부분은 그대로 둡니다.

```cpp
void loop() {
  plc.update();     // 0) 입력 읽기, 예약된 출력 처리 (I2R_PLC 라이브러리)
  plcControl();     // 1) PLC 제어: 항상 먼저, Wi-Fi와 상관없이
  remoteControl();  // 2) 원격 제어: 연결돼 있을 때만 명령 처리, 끊겼으면 가끔 다시 연결
}
```

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

> 프로그램은 **I2R_PLC 라이브러리**(5장)로 만들기 때문에 보통은 이 핀 번호를 쓸 일이 없습니다. 라이브러리가 보드 이름(`I2R_02`, `I2R_03`, `I2R_04`)에 맞춰 핀을 정합니다. 이 표는 배선 안내, 문제 해결, 라이브러리에 없는 기능을 만들 때 봅니다.

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
| **I2R_PLC** | `I2R_PLC` | **항상** (입력, 출력, 타이머, 카운터, i2r-03 온습도) |
| PubSubClient | `PubSubClient` | MQTT 통신 |
| ArduinoJson | `ArduinoJson` | MQTT 메시지(JSON) |

- i2r-03 온습도 센서(AHT21)는 I2R_PLC가 직접 읽습니다. **Adafruit AHTX0은 설치하지 않아도 됩니다.**

- 시리얼 속도: **115200**

## 4. 꼭 지킬 규칙

1. **I2R_PLC 라이브러리로 만듭니다.** `I2R_PLC plc(I2R_03);`처럼 사용자의 보드 이름을 넣고, 입력과 출력은 `plc.input(1)`, `plc.output(1, ON)`처럼 **보드에 인쇄된 번호(1부터)** 로 씁니다. 핀 번호, `pinMode`, `digitalRead`, `digitalWrite`로 입출력을 직접 다루지 않습니다.
2. `setup()`에서 **`plc.begin()`**, `loop()` 맨 앞에서 **매번 `plc.update()`** 를 부릅니다. `begin()`이 모든 출력을 꺼진 상태로 시작하고, `update()`가 입력 떨림 제거(30ms), 자동 끄기, 깜빡임, 센서 읽기를 합니다.
3. 입력이 **들어오는 순간**은 `plc.rising(n)`, **꺼지는 순간**은 `plc.falling(n)`으로 봅니다. 떨림 제거는 라이브러리가 하므로 따로 만들지 않습니다. 이 두 함수는 `update()` 한 번 동안만 `true`라서, 한 `loop()` 안에서 같은 번호를 여러 번 불러도 같은 값입니다.
4. **`delay()`로 오래 멈추지 않습니다.** PLC는 입력을 계속 보고 있어야 합니다. 정해진 시간 켜기는 `plc.output(n, ON, 초)`, TON/TOF는 `plc.onDelay` / `plc.offDelay`, 깜빡임은 `plc.blink`로 만듭니다. 라이브러리에 없는 시간 동작만 `millis()`로 만듭니다.
5. 같은 출력을 짧은 간격으로 계속 켜고 끄지 않습니다. 온도 제어처럼 기준값 근처에서 흔들리는 제어는 **히스테리시스**(예: 30도에서 켜고 28도에서 끄기)를 둡니다. 릴레이가 상하고 연결 장치에도 무리가 갑니다.
6. 온습도는 `plc.temperature()`, `plc.humidity()`로 읽습니다. (라이브러리가 2초마다 읽어 둠) 센서를 찾지 못하면 `plc.sensorOk()`가 `false`이고 값은 `NAN`입니다. `while(1)`로 멈추지 말고, 시리얼에 알린 뒤 나머지 PLC 동작은 계속합니다. 온습도 센서는 **i2r-03에만** 있습니다.
7. **Wi-Fi 비밀번호, 이메일, 서버 주소는 `secrets.h`에 따로 두고** 스케치에서 `#include "secrets.h"`로 불러옵니다. `secrets.h`는 `.gitignore`에 들어 있어 GitHub에 올라가지 않습니다. 비밀 값을 코드에 직접 쓰거나 시리얼로 출력하지 않습니다.
8. **NVS(보드 저장 공간)를 통째로 지우지 않습니다.** `nvs_flash_erase()`를 쓰지 않습니다. 값을 저장하려면 `Preferences`로 `my`로 시작하는 새 이름을 씁니다. 예: `prefs.begin("myplc", false);`
9. MQTT를 쓸 때 메시지는 **JSON**으로 하고, 명령 형식은 [i2r 프로토콜](https://github.com/kdi6033/i2r/blob/main/README.md)을 따릅니다. 새 형식을 마음대로 만들지 않습니다.
10. 새 프로그램은 `my_sketches/스케치이름/스케치이름.ino`로 만듭니다. (폴더 이름과 .ino 파일 이름이 같아야 합니다) `examples/`와 `boards/`는 사용자가 요청할 때만 고칩니다.

## 5. 프로그램 모양 (이 틀에서 시작)

[I2R_PLC 라이브러리](https://github.com/kdi6033/I2R_PLC)를 씁니다. Arduino IDE 라이브러리 매니저에서 `I2R_PLC`로 찾아 설치합니다. (arduino-cli: `arduino-cli lib install I2R_PLC`)
보드 이름만 사용자의 보드로 바꾸면 세 보드 모두 같은 코드입니다.

```cpp
// 기본 틀: 보드 이름만 내 보드로 (I2R_02, I2R_03, I2R_04)
#include <I2R_PLC.h>

I2R_PLC plc(I2R_03);

void setup() {
  Serial.begin(115200);
  plc.begin();                        // 핀 설정, 출력 모두 끄기, 센서 찾기
  Serial.println("i2r-03 PLC 시작");
}

void loop() {
  plc.update();                       // loop 맨 앞에서 매번
  plc.output(1, plc.input(1));        // 예: IN1이 들어오면 OUT1 켜기
}
```

**I2R_PLC 함수** (번호는 보드에 인쇄된 대로 1부터)

| 함수 | 설명 |
|---|---|
| `plc.begin()` / `plc.update()` | `setup()`에서 한 번 / `loop()` 맨 앞에서 매번 |
| `plc.type()`, `plc.board()` | 보드 이름 `"i2r-03"` / 보드 종류 `I2R_03` |
| `plc.inputCount()`, `plc.outputCount()` | 입력, 출력 개수 (4 또는 8) |
| `plc.output(n, ON)` / `plc.output(n, OFF)` | 출력 n 켜기 / 끄기. 없는 번호면 아무것도 안 하고 `false` |
| `plc.output(n, ON, 초)` | 켜고 정한 시간(초) 뒤 자동으로 끄기. 그 전에 다시 부르면 처음부터 |
| `plc.output(n, ON, 기다림초, 동작초)` | 기다린 뒤 켜고, 동작 시간 뒤 끄기 (동작초 0 = 켜 둠) |
| `plc.toggle(n)`, `plc.allOff()` | 출력 반대로 / 모든 출력 끄기 |
| `plc.outputState(n)` | 출력 n이 켜져 있으면 `true` |
| `plc.blink(n, 켜짐ms, 꺼짐ms)` | 계속 깜빡이기. `plc.output(n, OFF)`로 멈춤 |
| `plc.remain(n)` | 자동 끄기/늦게 켜기까지 남은 초 |
| `plc.input(n)` | 입력 n이 들어와 있으면 `true` (떨림 제거됨) |
| `plc.rising(n)` / `plc.falling(n)` | 입력 n이 막 들어온 / 막 꺼진 순간에만 `true` |
| `plc.setDebounce(ms)` | 떨림 제거 시간 바꾸기 (기본 30ms) |
| `plc.onDelay(n, ms)` | TON: 입력 n이 ms 동안 계속 들어와 있으면 `true` |
| `plc.offDelay(n, ms)` | TOF: 입력 n이 들어와 있는 동안, 그리고 꺼진 뒤 ms 동안 `true` |
| `plc.count(n)` / `plc.resetCount(n)` | CTU: 입력 n이 들어온 횟수 / 0으로 |
| `plc.onOutputChange(함수)` | 출력이 실제로 바뀔 때마다 `함수(n, on)` 호출 (원격 상태 알림용) |
| `plc.inputBits()`, `plc.outputBits()`, `plc.outputBits(값)` | 모든 입력/출력을 숫자 하나로 (bit0 = 1번) / 출력 한꺼번에 바꾸기 |
| `plc.inputText()`, `plc.outputText()` | `"0101"` 문자 (맨 왼쪽이 1번) |
| `plc.led(ON)`, `plc.button()` | 내장 LED / 공장 초기화 버튼이 눌려 있으면 `true` |
| `plc.temperature()`, `plc.humidity()`, `plc.sensorOk()` | i2r-03 온도(℃), 습도(%), 센서를 찾았는지 |

- 위 표에 없는 함수를 지어내지 않습니다. 필요한 기능이 없으면 이 함수들과 `millis()`로 만듭니다.
- **번호 규칙**: 스케치에서는 늘 1부터(`plc.output(1)` = 보드의 OUT1) 씁니다. MQTT JSON(i2r 프로토콜)은 0부터라서 `"n":0`이 OUT1입니다. 원격 명령을 받을 때는 `plc.output(n + 1, ...)`처럼 바꿉니다.

PLC에서 자주 쓰는 동작을 I2R_PLC로 옮기는 방법입니다.

| PLC 동작 | I2R_PLC에서 |
|---|---|
| 접점 (A접점 / B접점) | `plc.input(n)` / `!plc.input(n)` |
| 코일 | `plc.output(n, 상태)` |
| 자기유지 회로 | `if (plc.rising(START)) plc.output(n, ON); if (plc.rising(STOP)) plc.output(n, OFF);` |
| 상승 에지 / 하강 에지 | `plc.rising(n)` / `plc.falling(n)` |
| TON (ON 딜레이 타이머) | `plc.onDelay(n, ms)` |
| TOF (OFF 딜레이 타이머) | `plc.offDelay(n, ms)` |
| 정해진 시간 켜기 (원숏) | `if (plc.rising(n)) plc.output(m, ON, 초);` |
| 카운터 (CTU) | `plc.count(n) >= 목표` 이면 출력, `plc.resetCount(n)`으로 초기화 |
| 플리커 (깜빡임) | `plc.blink(n, 켜짐ms, 꺼짐ms)` |

## 6. 참고할 예제

새 프로그램은 가장 비슷한 예제를 먼저 읽고 그 방식대로 만듭니다. ([examples/](examples/))

| 예제 | 보드 | 내용 |
|---|---|---|
| i2r02_output_blink | i2r-02 | 출력 1~4를 차례로 1초씩 켜기 |
| i2r02_input_to_output | i2r-02 | IN1~4가 들어오면 같은 번호의 OUT1~4 켜기 |
| i2r03_output_blink | i2r-03 | 출력 1~4를 차례로 1초씩 켜기 |
| i2r03_input_to_output | i2r-03 | IN1~4가 들어오면 같은 번호의 OUT1~4 켜기 |
| i2r03_input_timer | i2r-03 | IN1을 누르면 OUT1을 5초 켜기 (`plc.rising`, `plc.output(n, ON, 초)`) |
| i2r03_temp_relay | i2r-03 | 온도 30도 이상이면 OUT1 켜고 28도 이하면 끄기 (히스테리시스) |
| i2r04_output_blink | i2r-04 | 출력 1~8을 차례로 1초씩 켜기 |
| i2r04_input_to_output | i2r-04 | IN1~8이 들어오면 같은 번호의 OUT1~8 켜기 |

웹에서 읽을 때는 raw 주소를 씁니다. 예: `https://raw.githubusercontent.com/kdi6033/i2r-plc/main/examples/i2r03_temp_relay/i2r03_temp_relay.ino`

모든 예제는 I2R_PLC 라이브러리를 씁니다. 다른 보드에서 쓰려면 `I2R_PLC plc(I2R_03);`의 보드 이름만 바꿉니다.
자기유지(ToggleSwitch), TON(OnDelay), TOF(StairLight), 카운터(Counter), 깜빡임(BlinkOutput), 상태 알림(StateReport) 예제는 [I2R_PLC 라이브러리 예제](https://github.com/kdi6033/I2R_PLC/tree/main/examples)에 있습니다. 예: `https://raw.githubusercontent.com/kdi6033/I2R_PLC/main/examples/Counter/Counter.ino`

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
arduino-cli lib update-index
arduino-cli lib install I2R_PLC PubSubClient ArduinoJson
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
| 릴레이가 안 켜짐 | `I2R_PLC plc(...)`의 보드 이름이 실제 보드와 같은지, `plc.begin()`과 `plc.update()`를 불렀는지 확인. 시리얼에 `[I2R_PLC] OUT5은(는) i2r-03에 없습니다` 같은 알림이 나오면 그 보드에 없는 출력 번호를 쓴 것. USB 전원만으로 릴레이 여러 개를 동시에 켜면 전원이 부족할 수 있으니 DC 전원을 연결 |
| 자동 끄기, 깜빡임, `rising`이 동작하지 않음 | `loop()` 맨 앞에서 `plc.update()`를 매번 부르는지, `loop()` 안에 긴 `delay()`가 없는지 확인 |
| 입력이 반대로 읽힘 | i2r-03은 COM 단자 배선(0V 또는 입력 전압)을, i2r-02/04는 접점 배선을 확인. 시리얼에 `plc.inputText()`를 찍어 사용자와 함께 확인. **i2r-02/04 입력에 전압을 넣어 시험하지 않음** (고장) |
| `I2R_PLC.h: No such file` | 3장 표의 라이브러리를 설치 (`arduino-cli lib install I2R_PLC`, 또는 Arduino IDE 라이브러리 매니저에서 `I2R_PLC`) |
| 온습도 센서를 못 찾음 (i2r-03) | 보드 이름이 `I2R_03`인지, 다른 코드가 GPIO 21, 22(I2C)를 쓰고 있지 않은지 확인. `plc.sensorOk()`로 확인 |
| Wi-Fi 연결이 안 됨 | 2.4GHz 공유기인지 확인 (5GHz는 안 됨). `secrets.h`의 이름과 비밀번호 확인 |

## 9. 공식 펌웨어로 되돌리기

내 스케치를 올리면 공식 i2r 펌웨어(스마트폰 앱 연결, MQTT 원격 제어)가 덮어써집니다.
다시 쓰려면 i2r.link에서 그 보드의 펌웨어를 다시 설치합니다. 펌웨어 파일은 [download](https://github.com/kdi6033/download) 저장소에도 있습니다.

## 10. 문서

- 보드별 설명: [boards/i2r-02](boards/i2r-02/README.md), [boards/i2r-03](boards/i2r-03/README.md), [boards/i2r-04](boards/i2r-04/README.md)
- 통신 프로토콜(MQTT, JSON 명령): https://github.com/kdi6033/i2r/blob/main/README.md
- 제품 설명과 공식 펌웨어 소스: https://github.com/kdi6033/i2r-02, https://github.com/kdi6033/i2r-03, https://github.com/kdi6033/i2r-04
