# 基于 STM32F103C8T6 的智能垃圾桶 (Smart Trash Can)

这是一个基于 STM32F103C8T6 微控制器、适配 Keil MDK-ARM uVision5 嵌入式开发环境的完整智能垃圾桶控制系统工程。

## 🌟 功能特性

1. **超声波自动感应开盖**：使用第一个 HC-SR04 超声波测距模块检测人体靠近（距离 < 20cm），触发 SG90 舵机自动开盖，并在无人靠近后延迟自动关盖。
2. **桶内满溢容量检测**：使用第二个 HC-SR04 超声波测距模块安装于垃圾桶顶部，实时检测桶内垃圾剩余空间并计算容量百分比（0%~100%）。
3. **SG90 舵机 PWM 角度控制**：基于 STM32 TIM2 定时器输出 50Hz PWM 信号，实现桶盖 0°（关闭）与 90°~120°（打开）的平滑切换。
4. **红外防夹检测**：在关盖过程中，红外传感器持续检测垃圾桶口是否有异物/手部阻挡，若检测到阻挡则保持或重新开盖，防止夹伤。
5. **0.96寸 OLED 状态与容量显示**：基于 SSD1306 驱动（I2C 接口），实时显示系统运行模式（AUTO / MANUAL）、桶盖状态（OPEN / CLOSED）、当前容量百分比及满溢告警提示。
6. **蜂鸣器与双色 LED 声光报警**：容量超过 85% 满溢阈值时，红灯亮起并触发蜂鸣器报警，绿灯熄灭；正常状态下绿灯常亮。
7. **按键模式切换与手动控制**：配备按键实现自动模式与手动模式切换，以及手动模式下的开盖/关盖按键控制。

---

## 🛠️ 硬件清单与接线表

### 硬件清单
- **主控板**：STM32F103C8T6 核心板（Blue Pill）
- **超声波模块**：HC-SR04 × 2（人体感应模块 + 桶内容量检测模块）
- **舵机**：SG90 9g 微型舵机 × 1
- **红外传感器**：红外对射/光电避障防夹模块 × 1
- **显示屏**：0.96寸 OLED 显示屏（SSD1306，4Pin I2C）× 1
- **声光报警**：有源蜂鸣器模块 × 1，双色指示灯（红/绿 LED）× 1
- **按键**：轻触按键 × 2

### STM32F103C8T6 引脚接线表

| 硬件模块 | 模块引脚 | STM32F103C8T6 引脚 | 备注说明 |
| :--- | :--- | :--- | :--- |
| **超声波 #1 (人体感应)** | Trig | PB6 | GPIO Push-Pull |
| | Echo | PB7 | GPIO Floating Input |
| **超声波 #2 (容量检测)** | Trig | PB8 | GPIO Push-Pull |
| | Echo | PB9 | GPIO Floating Input |
| **SG90 舵机** | PWM (黄/橙) | PA1 | TIM2_CH2 PWM 输出 |
| | VCC / GND | 5V / GND | 外部5V供电 |
| **红外防夹传感器** | OUT | PA2 | GPIO Pull-Up Input |
| **0.96寸 OLED** | SCL | PB10 | I2C SCL (GPIO) |
| | SDA | PB11 | I2C SDA (GPIO) |
| **蜂鸣器** | VCC/GND/I/O | PA3 | 低电平/高电平触发蜂鸣器 |
| **双色 LED** | Green LED | PA4 | 正常状态指示灯 |
| | Red LED | PA5 | 满溢告警指示灯 |
| **按键模块** | Key1 (Mode) | PA6 | 模式切换键 (AUTO / MANUAL) |
| | Key2 (Control) | PA7 | 手动模式下开/关盖控制键 |

---

## 📂 工程目录结构说明

```
SmartTrashCan/
├── Project/
│   └── SmartTrashCan.uvprojx  # Keil uVision5 工程文件
├── User/
│   ├── main.c                 # 主程序逻辑与状态机
│   ├── stm32f10x_conf.h       # 外设库配置文件
│   ├── stm32f10x_it.c         # 中断服务函数定义
│   └── stm32f10x_it.h         # 中断服务函数头文件
├── Hardware/
│   ├── HC_SR04.c / .h         # 双路超声波测距驱动
│   ├── Servo.c / .h           # SG90舵机 PWM 角度控制驱动
│   ├── OLED.c / .h            # 0.96寸 OLED I2C 驱动
│   ├── OLED_Font.h            # OLED 字符库
│   ├── Buzzer_LED.c / .h      # 蜂鸣器与双色 LED 驱动
│   └── Key.c / .h             # 按键消抖扫描驱动
├── System/
│   ├── Delay.c / .h           # SysTick 微秒/毫秒精确延时
└── .gitignore                 # Keil 编译过程文件忽略配置
```

---

## 🚀 Keil uVision5 编译与烧录指南

1. **环境准备**：
   - 安装 **Keil MDK-ARM uVision5** (v5.20 及以上)。
   - 安装 **Keil::STM32F1xx_DFP** 芯片支持包。
   - 准备 **ST-Link V2** 或 **DAP-Link** 仿真烧录器。

2. **打开工程**：
   - 双击打开 `Project/SmartTrashCan.uvprojx`。

3. **编译工程**：
   - 点击菜单栏 `Project` -> `Build Target` (快捷键 F7)。
   - 确认 Build 输出窗口提示 `0 Error(s), 0 Warning(s)`。

4. **烧录代码**：
   - 使用杜邦线连接烧录器与 STM32 核心板（SWDIO -> PA13, SWCLK -> PA14, 3.3V, GND）。
   - 点击菜单栏 `Flash` -> `Download` (快捷键 F8) 将程序下载至 STM32F103C8T6。

5. **运行验证**：
   - 复位核心板，OLED 屏幕亮起并显示初始状态，伸手靠近超声波感应区，舵机自动平滑开盖。
