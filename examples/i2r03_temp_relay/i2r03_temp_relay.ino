// i2r-03 온도로 릴레이 제어 (히스테리시스)
// 온도가 30도 이상이 되면 OUT1(예: 환풍기)을 켜고, 28도 이하로 내려가면 끈다.
// 켜는 온도와 끄는 온도를 2도 떨어뜨려 두어 기준값 근처에서 릴레이가 계속 딸깍거리지 않게 한다.
// 필요한 라이브러리: Adafruit AHTX0

#include <Wire.h>
#include <Adafruit_AHTX0.h>

const int OUT1 = 26;          // OUT1 (HIGH = 릴레이 켜짐)
const int SDA_PIN = 21;       // AHT21 온습도 센서 I2C
const int SCL_PIN = 22;

const float ON_TEMP  = 30.0;  // 이 온도 이상이면 켠다
const float OFF_TEMP = 28.0;  // 이 온도 이하면 끈다

const unsigned long READ_INTERVAL_MS = 2000;  // 센서는 2초에 한 번 읽는다

Adafruit_AHTX0 aht;
bool sensorOk = false;
bool outOn = false;
unsigned long lastRead = 0;

void setup() {
  Serial.begin(115200);
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1, LOW);    // 꺼진 상태에서 시작

  Wire.begin(SDA_PIN, SCL_PIN);
  sensorOk = aht.begin(&Wire);
  if (sensorOk) {
    Serial.println("AHT21 온습도 센서 찾음");
  } else {
    // 센서가 없어도 멈추지 않고 계속 돈다 (출력은 꺼 둔 채로)
    Serial.println("AHT21 센서를 찾지 못했습니다. 배선(SDA 21, SCL 22)을 확인하세요.");
  }
  Serial.println("i2r-03 온도 제어 시작: 30도 이상 켜짐, 28도 이하 꺼짐");
}

void loop() {
  if (millis() - lastRead < READ_INTERVAL_MS) return;
  lastRead = millis();

  if (!sensorOk) {
    sensorOk = aht.begin(&Wire);  // 2초마다 다시 찾아본다
    return;
  }

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  float t = temp.temperature;
  float h = humidity.relative_humidity;

  // 히스테리시스: 켜져 있을 때는 28도 이하가 되어야 끄고, 꺼져 있을 때는 30도 이상이 되어야 켠다
  if (!outOn && t >= ON_TEMP) {
    outOn = true;
    digitalWrite(OUT1, HIGH);
    Serial.println("30도 이상 → OUT1 켜짐");
  } else if (outOn && t <= OFF_TEMP) {
    outOn = false;
    digitalWrite(OUT1, LOW);
    Serial.println("28도 이하 → OUT1 꺼짐");
  }

  Serial.print("온도 ");
  Serial.print(t, 1);
  Serial.print("°C  습도 ");
  Serial.print(h, 1);
  Serial.print("%  OUT1 ");
  Serial.println(outOn ? "켜짐" : "꺼짐");
}
