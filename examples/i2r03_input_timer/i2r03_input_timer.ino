// i2r-03 입력 타이머 (계단 조명처럼)
// IN1에 입력이 들어오는 순간 OUT1을 켜고, 5초 뒤에 자동으로 끈다.
// 켜져 있는 동안 IN1이 다시 들어오면 5초를 처음부터 다시 센다.
// 자동 끄기는 plc.update()가 millis()로 처리하므로 기다리는 동안에도 입력을 계속 본다.
// 필요한 라이브러리: I2R_PLC (라이브러리 매니저에서 설치)

#include <I2R_PLC.h>

I2R_PLC plc(I2R_03);  // i2r-03 보드

const uint32_t ON_TIME_SEC = 5;  // 출력을 켜 두는 시간 (5초)

void setup() {
  Serial.begin(115200);
  plc.begin();                   // 출력은 모두 꺼진 상태에서 시작
  Serial.println("i2r-03 입력 타이머 시작: IN1을 넣으면 OUT1이 5초 켜집니다");
}

void loop() {
  plc.update();                  // loop 맨 앞에서 매번

  // 입력이 꺼져 있다가 들어오는 순간(상승 에지)에만 타이머 시작
  if (plc.rising(1)) {
    plc.output(1, ON, ON_TIME_SEC);  // 켜고 5초 뒤 자동으로 끄기 (다시 부르면 처음부터)
    Serial.println("IN1 들어옴 → OUT1 켜짐 (5초)");
  }
}
