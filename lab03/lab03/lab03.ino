// 定义LED引脚
const int ledPin = 2;     // 板载LED通常是GPIO 2
const int LED_NIP = 26;   // 外部LED引脚

// --- 时间参数定义 (毫秒) ---
const unsigned long SHORT_DUR = 200;   // 短闪亮灯时间
const unsigned long LONG_DUR = 600;    // 长闪亮灯时间
const unsigned long GAP_DUR = 200;     // 闪灯之间的灭灯间隔
const unsigned long LETTER_GAP = 500;  // 字母之间的间隔 (S和O之间)
const unsigned long WORD_GAP = 2000;   // 单词之间的间隔 (SOS结束后)

// --- 状态机变量 ---
enum State {
  STATE_S1, // 第一个 S
  STATE_O,  // 中间的 O
  STATE_S2, // 最后的 S
  STATE_PAUSE // 长停顿
};

State currentState = STATE_S1; // 初始状态

unsigned long previousMillis = 0; // 记录上一次动作的时间
int flashCount = 0;               // 记录当前字母闪了多少次 (0-3)
bool isLightOn = false;           // 记录灯当前是亮还是灭

void setup() {
  // 设置引脚模式
  pinMode(ledPin, OUTPUT);
  pinMode(LED_NIP, OUTPUT);
  
  // 初始化灯为灭
  digitalWrite(ledPin, LOW);
  digitalWrite(LED_NIP, LOW);
  
  Serial.begin(115200);
  Serial.println("SOS Signal Started");
}

void loop() {
  unsigned long currentMillis = millis();

  // 计算距离上一次动作过去了多久
  unsigned long interval = currentMillis - previousMillis;

  switch (currentState) {
    
    // --- 状态 1: 短闪 S ---
    case STATE_S1:
      handleFlashLogic(currentMillis, interval, SHORT_DUR, 3, STATE_O);
      break;

    // --- 状态 2: 长闪 O ---
    case STATE_O:
      handleFlashLogic(currentMillis, interval, LONG_DUR, 3, STATE_S2);
      break;

    // --- 状态 3: 短闪 S ---
    case STATE_S2:
      handleFlashLogic(currentMillis, interval, SHORT_DUR, 3, STATE_PAUSE);
      break;

    // --- 状态 4: 长停顿 ---
    case STATE_PAUSE:
      if (interval >= WORD_GAP) {
        // 停顿时间结束，重置状态，准备下一轮 SOS
        currentState = STATE_S1;
        flashCount = 0;
        previousMillis = currentMillis;
        Serial.println("Restarting SOS...");
      }
      break;
  }
}

/**
 * 通用闪烁处理函数
 * @param currentMillis 当前时间
 * @param interval 距离上次动作的时间差
 * @param onDuration 亮灯持续时间 (短闪200或长闪600)
 * @param targetCount 目标闪烁次数 (固定为3)
 * @param nextState 当前字母闪完后跳转的下一个状态
 */
void handleFlashLogic(unsigned long currentMillis, unsigned long interval, unsigned long onDuration, int targetCount, State nextState) {
  
  // 如果当前字母的3次闪烁已经完成
  if (flashCount >= targetCount) {
    // 等待字母间隔时间 (LETTER_GAP)
    if (interval >= LETTER_GAP) {
      currentState = nextState;      // 切换到下一个状态 (S->O 或 O->S)
      flashCount = 0;                // 重置计数器
      previousMillis = currentMillis; // 重置时间基准
      setLED(LOW);                   // 确保灯是灭的
    }
    return;
  }

  // --- 执行闪烁逻辑 ---
  
  // 1. 如果灯是灭的，且间隔时间到了 -> 开灯
  if (!isLightOn && interval >= GAP_DUR) {
    setLED(HIGH);
    isLightOn = true;
    previousMillis = currentMillis; // 重置时间，开始计算亮灯时间
  } 
  // 2. 如果灯是亮的，且亮灯时间到了 -> 关灯，并增加计数
  else if (isLightOn && interval >= onDuration) {
    setLED(LOW);
    isLightOn = false;
    flashCount++; // 完成一次闪烁
    previousMillis = currentMillis; // 重置时间，开始计算灭灯间隔
  }
}

// 辅助函数：同时控制两个LED
void setLED(int state) {
  digitalWrite(ledPin, state);
  digitalWrite(LED_NIP, state);
}