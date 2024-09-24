#ifndef BOARD_DEF_H
#define BOARD_DEF_H

/*
 * Picopad Pro - Video Card
 *
 * This code is for the video card extension (ExtDisp) for the Picopad Pro, utilizing the RP2040 microcontroller.
 * The video card connects to the PicopadBus and outputs to HDMI/DVI or VGA but does not include additional peripherals
 * like LEDs, buzzers, or buttons. Therefore, it uses the buzzer and LED from the Picopad itself.
 *
 * Connection Between Picopad and ExtDisp:
 *
 * The video card connects to the Picopad using only four pins, which are used to capture display data from the Picopad.
 * These pins are utilized for UART communication. However, since it's not possible to use the standard UART pin layout
 * (because GPIO29 is used as the CS pin for the display), UART over PIO (Programmable Input/Output) is used.
 *
 * DVI Output GPIO Pins:      VGA Output GPIO Pins:
 *
 * +-------+-------------+    +---------+-------------+
 * | GPIO  | Function    |    | GPIO    | Function    |
 * +-------+-------------+    +---------+-------------+
 * | 0     | DVI_CLK -   |    |  8 - 12 | B0 - B4     | // Blue color bits
 * | 1     | DVI_CLK +   |    | 13 - 18 | G0 - G5     | // Green color bits
 * | 2     | DVI_D0 -    |    | 19 - 23 | R0 - R4     | // Red color bits
 * | 3     | DVI_D0 +    |    |      24 | HSYNC       |
 * | 4     | DVI_D1 -    |    |      25 | VSYNC       |
 * | 5     | DVI_D1 +    |    +---------+-------------+
 * | 6     | DVI_D2 -    |
 * | 7     | DVI_D2 +    |
 * +-------+-------------+
 *
 * Control of LED and Buzzer:
 *
 * The LED and buzzer on the Picopad are controlled via serial communication using VT100 control characters.
 *
 * Relevant Control Characters:
 *
 * +------+-------+---------+------+---------------------------------------------------------------+
 * | Char | Octal | Decimal | Hex  | Action Taken                                                  |
 * +------+-------+---------+------+---------------------------------------------------------------+
 * | BEL  | 007   | 7       | 0x07 | Sounds the bell (activates buzzer).                           |
 * | DC1  | 021   | 17      | 0x11 | Device Control 1 (XON) - Resumes transmission.                |
 * | DC2  | 022   | 18      | 0x12 | Device Control 2 - Custom: LED ON                             |
 * | DC3  | 023   | 19      | 0x13 | Device Control 3 (XOFF) - Pauses transmission.                |
 * | DC4  | 024   | 20      | 0x14 | Device Control 4 - Custom: LED OFF                            |
 * +------+-------+---------+------+---------------------------------------------------------------+
 *
 * Mapping Between Picopad, ExtDisp, and VersaTerm Ports:
 *
 * +----------------+-------+-------+----------------+-------+--------------+
 * | PicopadBus     | GPIO  | UART  | ExtDisp        | GPIO  | VersaTerm    |
 * +----------------+-------+-------+----------------+-------+--------------+
 * | DISP_DC_PIN    | 17    |       | LCD_DC         | 27    | LCD_OFFLINE  |
 * | DISP_SCK_PIN   | 18    | RX    | LCD_SCK        | 29    | TX           |
 * | DISP_MOSI_PIN  | 19    | TX    | LCD_MOSI       | 26    | RX           |
 * | DISP_RES_PIN   | 20    |       | LCD_RES        | RUN   |              |
 * | DISP_CS_PIN    | 21    |       | LCD_CS         | 28    | LCD_CS       |
 * +----------------+-------+-------+----------------+-------+--------------+
 *
 * Startup Behavior:
 *
 * Upon startup, the state of the GPIO port `LCD_DC` is checked. If this GPIO is in a LOW state for at least 50 ms,
 * VersaTerm is launched. Otherwise, the original program for ExtDisp is loaded into memory and executed.
 *
 * Notes:
 *
 * - **UART over PIO**: The use of UART over PIO is necessary due to pin conflicts with the standard UART pin assignment.
 *   This allows UART functionality to be assigned to arbitrary GPIO pins.
 * - **LED and Buzzer Control**: Controlling the LED and buzzer via VT100 control characters enables standard terminal
 *   behavior to manage hardware features.
 *
 * References:
 *
 * - Picopad Pro Video Card: https://pajenicko.cz/picopad-pro-videokarta
 * - VT100 Control Characters: https://vt100.net/docs/vt100-ug/chapter3.html
 */

#define USE_VGA							0
#define USE_HDMI						1
#define USE_DEVICE_INIT     1
#define USE_BUZZER          0
#define USE_HDMI_DETECT     0
#define USE_PS2_KEYBOARD    0
#define USE_UART            0
#define USE_LED             0
#define USE_KEY             0

#define DVI_DEFAULT_SERIAL_CONFIG picodvi_picopad_extdisp_cfg

#define SPLASH_CONTENT \
{ \
    "\016lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\n", \
    "x\017   PicoPad external display module v1 - VersaTerm 1.0  \016x", \
    "x\017                 (C) 2022 David Hansel                 \016x", \
    "x\017          https://github.com/dhansel/VersaTerm         \016x", \
    "tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu", \
  	"x\017  F1 ... F10 Load Config                               \016x", \
    "x\017  F11        Keyboard Macro START/STOP                 \016x", \
    "x\017  F12        Config menu                               \016x", \
    "mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\017" \
}

#define UART_PIO  				  pio1
#define PIO_UART_TX         29     // PIO - original uart0: 0, 12, 16, 28  uart1: 4,  8, 20, 24
#define PIO_UART_RX         26     // PIO - original uart0: 1, 13, 17, 29  uart1: 5,  9, 21, 25
#define LCD_OFFLINE         27
#define LCD_CS						  28

#define DC_PIN_LOW_MS       50
#define DC_PIN_MAX_WAIT_MS  200

#endif
