// hello_plc : 개발 환경 확인용 첫 스케치 (i2r-02, i2r-03, i2r-04 모두 사용 가능)
// 릴레이는 건드리지 않고 보드 내장 LED만 1초마다 깜빡이며 시리얼에 인사한다.
// 컴파일과 업로드가 잘 되는지 확인할 때 쓴다.

const int LED_PIN = 2;  // 세 보드 공통 내장 LED

unsigned long lastBlink = 0;
int count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("안녕하세요, i2r PLC입니다!");
}

void loop() {
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    count++;
    Serial.print("동작 중 ");
    Serial.print(count);
    Serial.println("초");
  }
}
