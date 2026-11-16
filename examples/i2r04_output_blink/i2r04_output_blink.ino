// i2r-04 출력 차례로 켜기
// OUT1 → OUT2 → ... → OUT8 순서로 1초씩 켜고 끈다.
// 릴레이가 딸깍거리는 소리와 보드의 출력 LED로 확인한다.

const int NUM_OUT = 8;
const int OUT_PINS[NUM_OUT] = {26, 27, 32, 33, 21, 22, 23, 25};  // OUT1~OUT8 (HIGH = 릴레이 켜짐)
const int LED_PIN = 2;                                            // 보드 내장 LED

const unsigned long STEP_MS = 1000;  // 출력 하나를 켜 두는 시간 (1초)

int current = 0;                     // 지금 켜져 있는 출력 번호 (0 = OUT1)
unsigned long lastChange = 0;        // 마지막으로 바꾼 시각

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_OUT; i++) {
    pinMode(OUT_PINS[i], OUTPUT);
    digitalWrite(OUT_PINS[i], LOW);  // 꺼진 상태에서 시작
  }
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(OUT_PINS[current], HIGH);
  lastChange = millis();
  Serial.println("i2r-04 출력 차례로 켜기 시작");
  Serial.println("OUT1 켜짐");
}

void loop() {
  // delay() 대신 millis()로 시간을 재서 PLC가 멈추지 않게 한다
  if (millis() - lastChange >= STEP_MS) {
    lastChange = millis();
    digitalWrite(OUT_PINS[current], LOW);       // 지금 출력 끄기
    current = (current + 1) % NUM_OUT;          // 다음 출력으로 (8 다음은 1)
    digitalWrite(OUT_PINS[current], HIGH);      // 다음 출력 켜기
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // 내장 LED 깜빡이기

    Serial.print("OUT");
    Serial.print(current + 1);
    Serial.println(" 켜짐");
  }
}
