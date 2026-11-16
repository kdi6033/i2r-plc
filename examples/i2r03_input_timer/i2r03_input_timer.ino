// i2r-03 입력 타이머 (계단 조명처럼)
// IN1에 입력이 들어오는 순간 OUT1을 켜고, 5초 뒤에 자동으로 끈다.
// 켜져 있는 동안 IN1이 다시 들어오면 5초를 처음부터 다시 센다.
// delay() 없이 millis()로 시간을 재기 때문에 기다리는 동안에도 입력을 계속 본다.

const int OUT1 = 26;  // OUT1 (HIGH = 릴레이 켜짐)
const int IN1  = 16;  // IN1  (HIGH = 입력 들어옴)

const unsigned long ON_TIME_MS  = 5000;  // 출력을 켜 두는 시간 (5초)
const unsigned long DEBOUNCE_MS = 30;    // 입력 떨림(채터링)을 무시하는 시간

bool inStable = false;          // 떨림을 걸러 낸 IN1 상태
bool inLastRead = false;        // 바로 전에 읽은 IN1 값
unsigned long inChangeTime = 0; // IN1 값이 마지막으로 바뀐 시각

bool outOn = false;             // OUT1이 켜져 있는지
unsigned long outOnTime = 0;    // OUT1을 켠 시각

void setup() {
  Serial.begin(115200);
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1, LOW);      // 꺼진 상태에서 시작
  pinMode(IN1, INPUT);
  Serial.println("i2r-03 입력 타이머 시작: IN1을 넣으면 OUT1이 5초 켜집니다");
}

void loop() {
  // 1) IN1 읽기 (디바운스)
  bool now = digitalRead(IN1) == HIGH;
  if (now != inLastRead) {
    inLastRead = now;
    inChangeTime = millis();
  }
  if (millis() - inChangeTime >= DEBOUNCE_MS && now != inStable) {
    inStable = now;

    // 2) 입력이 꺼져 있다가 들어오는 순간(상승 에지)에만 타이머 시작
    if (inStable) {
      outOn = true;
      outOnTime = millis();
      digitalWrite(OUT1, HIGH);
      Serial.println("IN1 들어옴 → OUT1 켜짐 (5초)");
    }
  }

  // 3) 5초가 지나면 끄기
  if (outOn && millis() - outOnTime >= ON_TIME_MS) {
    outOn = false;
    digitalWrite(OUT1, LOW);
    Serial.println("5초 지남 → OUT1 꺼짐");
  }
}
