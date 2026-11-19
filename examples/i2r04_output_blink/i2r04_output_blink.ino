// i2r-04 출력 차례로 켜기
// OUT1 → OUT2 → ... → OUT8 순서로 1초씩 켜고 끈다.
// 릴레이가 딸깍거리는 소리와 보드의 출력 LED로 확인한다.
// 필요한 라이브러리: I2R_PLC (라이브러리 매니저에서 설치)

#include <I2R_PLC.h>

I2R_PLC plc(I2R_04);  // i2r-04 보드

const unsigned long STEP_MS = 1000;  // 출력 하나를 켜 두는 시간 (1초)

int current = 1;                     // 지금 켜져 있는 출력 번호 (1 = OUT1)
unsigned long lastChange = 0;        // 마지막으로 바꾼 시각
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  plc.begin();                       // 출력은 모두 꺼진 상태에서 시작

  plc.output(current, ON);
  lastChange = millis();
  Serial.println("i2r-04 출력 차례로 켜기 시작");
  Serial.println("OUT1 켜짐");
}

void loop() {
  plc.update();                      // loop 맨 앞에서 매번

  // delay() 대신 millis()로 시간을 재서 PLC가 멈추지 않게 한다
  if (millis() - lastChange >= STEP_MS) {
    lastChange = millis();
    plc.output(current, OFF);                        // 지금 출력 끄기
    current = current % plc.outputCount() + 1;       // 다음 출력으로 (8 다음은 1)
    plc.output(current, ON);                         // 다음 출력 켜기
    ledOn = !ledOn;
    plc.led(ledOn);                                  // 내장 LED 깜빡이기

    Serial.print("OUT");
    Serial.print(current);
    Serial.println(" 켜짐");
  }
}
