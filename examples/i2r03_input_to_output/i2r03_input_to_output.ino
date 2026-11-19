// i2r-03 입력을 출력으로
// IN1~IN4에 입력이 들어오면 같은 번호의 OUT1~OUT4를 켠다.
// 입력이 바뀔 때마다 시리얼에 상태를 보여 준다.
// 입력 떨림(채터링)은 라이브러리가 30ms 동안 걸러 준다.
// 필요한 라이브러리: I2R_PLC (라이브러리 매니저에서 설치)

#include <I2R_PLC.h>

I2R_PLC plc(I2R_03);  // i2r-03 보드

void setup() {
  Serial.begin(115200);
  plc.begin();                       // 출력은 모두 꺼진 상태에서 시작
  Serial.println("i2r-03 입력 → 출력 시작");
}

void loop() {
  plc.update();                      // loop 맨 앞에서 매번

  for (int n = 1; n <= plc.inputCount(); n++) {
    if (plc.rising(n) || plc.falling(n)) {   // 입력이 바뀐 순간에만
      bool on = plc.input(n);
      plc.output(n, on);                     // 입력과 같은 번호의 출력

      Serial.print("IN");
      Serial.print(n);
      Serial.print(on ? " 들어옴 → OUT" : " 꺼짐 → OUT");
      Serial.print(n);
      Serial.println(on ? " 켜짐" : " 꺼짐");
    }
  }
}
