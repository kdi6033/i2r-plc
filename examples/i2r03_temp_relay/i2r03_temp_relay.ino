// i2r-03 온도로 릴레이 제어 (히스테리시스)
// 온도가 30도 이상이 되면 OUT1(예: 환풍기)을 켜고, 28도 이하로 내려가면 끈다.
// 켜는 온도와 끄는 온도를 2도 떨어뜨려 두어 기준값 근처에서 릴레이가 계속 딸깍거리지 않게 한다.
// 온습도 센서(AHT21)는 라이브러리가 2초마다 읽는다. 따로 설치할 센서 라이브러리는 없다.
// 필요한 라이브러리: I2R_PLC (라이브러리 매니저에서 설치)

#include <I2R_PLC.h>

I2R_PLC plc(I2R_03);  // 온습도 센서는 i2r-03에만 있습니다

const float ON_TEMP  = 30.0;  // 이 온도 이상이면 켠다
const float OFF_TEMP = 28.0;  // 이 온도 이하면 끈다

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  plc.begin();                // 출력은 모두 꺼진 상태에서 시작
  if (!plc.sensorOk()) {
    // 센서가 없어도 멈추지 않고 계속 돈다 (라이브러리가 2초마다 다시 찾는다)
    Serial.println("AHT21 센서를 찾지 못했습니다. 2초마다 다시 찾습니다.");
  }
  Serial.println("i2r-03 온도 제어 시작: 30도 이상 켜짐, 28도 이하 꺼짐");
}

void loop() {
  plc.update();               // loop 맨 앞에서 매번
  if (!plc.sensorOk()) {
    plc.output(1, OFF);       // 센서가 없으면 안전하게 꺼 둔다
    return;
  }

  // 히스테리시스: 켜져 있을 때는 28도 이하가 되어야 끄고, 꺼져 있을 때는 30도 이상이 되어야 켠다
  float t = plc.temperature();
  if (!plc.outputState(1) && t >= ON_TEMP) {
    plc.output(1, ON);
    Serial.println("30도 이상 → OUT1 켜짐");
  } else if (plc.outputState(1) && t <= OFF_TEMP) {
    plc.output(1, OFF);
    Serial.println("28도 이하 → OUT1 꺼짐");
  }

  if (millis() - lastPrint >= 2000) {  // 2초마다 보여 준다
    lastPrint = millis();
    Serial.print("온도 ");
    Serial.print(t, 1);
    Serial.print("°C  습도 ");
    Serial.print(plc.humidity(), 1);
    Serial.print("%  OUT1 ");
    Serial.println(plc.outputState(1) ? "켜짐" : "꺼짐");
  }
}
