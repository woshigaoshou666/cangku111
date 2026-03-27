#define TOUCH_PIN  4    // 触摸引脚
#define LED_PIN    2    // LED引脚（支持PWM）
#define THRESHOLD  600  // 触摸阈值，可根据实际调整

// 呼吸灯核心变量
int brightness = 0;     // 当前亮度(0-255)
int fadeStep = 3;       // 亮度变化步长（由档位控制）

// 档位控制变量（1慢/2中/3快）
volatile int speedMode = 1;  // 中断中修改，需加volatile
const int MAX_MODE = 3;

// 触摸防抖变量
volatile bool touchTriggered = false;
unsigned long lastTouchTime = 0;
const unsigned long DEBOUNCE_MS = 200;  // 防抖200ms

// 触摸中断服务函数
void IRAM_ATTR onTouch() {
  if (millis() - lastTouchTime > DEBOUNCE_MS) {
    touchTriggered = true;
    lastTouchTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);  // 初始熄灭
  
  // 绑定触摸中断
  touchAttachInterrupt(TOUCH_PIN, onTouch, THRESHOLD);
}

void loop() {
  // 处理触摸事件：切换档位
  if (touchTriggered) {
    touchTriggered = false;
    speedMode++;
    if (speedMode > MAX_MODE) speedMode = 1;
    
    // 根据档位更新步长（步长越大，呼吸越快）
    switch(speedMode) {
      case 1: fadeStep = 3;  break;  // 慢呼吸
      case 2: fadeStep = 8;  break;  // 中速呼吸
      case 3: fadeStep = 15; break;  // 快速呼吸
      default: fadeStep = 5;
    }
    Serial.print("当前档位: ");
    Serial.println(speedMode);
  }

  // 呼吸灯核心逻辑
  analogWrite(LED_PIN, brightness);
  brightness += fadeStep;

  // 边界反转（实现渐亮→渐灭循环）
  if (brightness <= 0 || brightness >= 255) {
    fadeStep = -fadeStep;
  }

  delay(15);  // 基础延时，保证PWM平滑
}