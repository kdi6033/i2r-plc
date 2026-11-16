// i2r-02 입력을 출력으로
// IN1~IN4에 입력이 들어오면 같은 번호의 OUT1~OUT4를 켠다.
// 입력이 바뀔 때마다 시리얼에 상태를 보여 준다.

const int NUM_CH = 4;
const int OUT_PINS[NUM_CH] = {26, 27, 32, 33};  // OUT1~OUT4 (HIGH = 릴레이 켜짐)
const int IN_PINS[NUM_CH]  = {16, 17, 18, 19};  // IN1~IN4  (HIGH = 입력 들어옴)

const unsigned long DEBOUNCE_MS = 30;  // 입력 떨림(채터링)을 무시하는 시간

bool stableState[NUM_CH];              // 떨림을 걸러 낸 입력 상태
bool lastRead[NUM_CH];                 // 바로 전에 읽은 값
unsigned long lastChangeTime[NUM_CH];  // 읽은 값이 마지막으로 바뀐 시각

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_CH; i++) {
    pinMode(OUT_PINS[i], OUTPUT);
    digitalWrite(OUT_PINS[i], LOW);    // 꺼진 상태에서 시작
    pinMode(IN_PINS[i], INPUT);
    stableState[i] = false;
    lastRead[i] = false;
    lastChangeTime[i] = 0;
  }
  Serial.println("i2r-02 입력 → 출력 시작");
}

void loop() {
  for (int i = 0; i < NUM_CH; i++) {
    bool now = digitalRead(IN_PINS[i]) == HIGH;

    // 값이 바뀌면 시각을 기억하고, 30ms 동안 그대로면 진짜로 바뀐 것으로 본다
    if (now != lastRead[i]) {
      lastRead[i] = now;
      lastChangeTime[i] = millis();
    }
    if (millis() - lastChangeTime[i] >= DEBOUNCE_MS && now != stableState[i]) {
      stableState[i] = now;
      digitalWrite(OUT_PINS[i], now ? HIGH : LOW);  // 입력과 같은 번호의 출력

      Serial.print("IN");
      Serial.print(i + 1);
      Serial.print(now ? " 들어옴 → OUT" : " 꺼짐 → OUT");
      Serial.print(i + 1);
      Serial.println(now ? " 켜짐" : " 꺼짐");
    }
  }
}
