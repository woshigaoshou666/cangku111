// 定义LED引脚
const int ledPin = 2;
#define LED_NIP 26

// 设置PWM属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 用于时间跟踪的变量
unsigned long previousMillis = 0;  // 上次状态切换的时间（毫秒）
int dutyCycle = 0;                 // 当前PWM占空比（0-255）
int step = 5;                      // 占空比变化步长（数值越小，呼吸越平滑）
bool isFadingIn = true;            // 标记是否在“变亮”阶段

void setup() {
  Serial.begin(115200);

  // 配置PWM通道（引脚、频率、分辨率）
  ledcAttach(ledPin, freq, resolution);
  ledcAttach(LED_NIP, freq, resolution);

  // 初始化PWM占空比为0（灯灭）
  ledcWrite(ledPin, 0);
  ledcWrite(LED_NIP, 0);
}

void loop() {
  unsigned long currentMillis = millis();  // 获取当前时间（毫秒）

  // 每500ms切换一次“变亮/变暗”状态（1Hz周期的一半）
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;  // 更新上次切换时间

    // 切换方向（变亮→变暗，或变暗→变亮）
    isFadingIn = !isFadingIn;

    // 方向切换时，重置占空比（避免连续变化）
    if (isFadingIn) {
      dutyCycle = 0;   // 变亮时从0开始
    } else {
      dutyCycle = 255; // 变暗时从255开始
    }
  }

  // 根据当前方向，微调占空比
  if (isFadingIn) {
    // 变亮：占空比从0→255（每次加step）
    dutyCycle += step;
    if (dutyCycle > 255) dutyCycle = 255;  // 限制最大值
  } else {
    // 变暗：占空比从255→0（每次减step）
    dutyCycle -= step;
    if (dutyCycle < 0) dutyCycle = 0;     // 限制最小值
  }

  // 写入PWM占空比（控制两个LED同步呼吸）
  ledcWrite(ledPin, dutyCycle);
  ledcWrite(LED_NIP, dutyCycle);

  // 打印调试信息（可选，观察占空比变化）
  Serial.println(dutyCycle);
}