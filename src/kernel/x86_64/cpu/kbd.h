#pragma once
#include <stdint.h>

#define KEY_NULL         0
#define KEY_ESC          1
#define KEY_1            2
#define KEY_2            3
#define KEY_3            4
#define KEY_4            5
#define KEY_5            6
#define KEY_6            7
#define KEY_7            8
#define KEY_8            9
#define KEY_9            10
#define KEY_0            11
#define KEY_MINUS        12
#define KEY_EQUAL        13
#define KEY_BACKSPACE    14
#define KEY_TAB          15
#define KEY_Q            16
#define KEY_W            17
#define KEY_E            18
#define KEY_R            19
#define KEY_T            20
#define KEY_Y            21
#define KEY_U            22
#define KEY_I            23
#define KEY_O            24
#define KEY_P            25
#define KEY_LBRACE       26
#define KEY_RBRACE       27
#define KEY_ENTER        28
#define KEY_LCTRL        29
#define KEY_A            30
#define KEY_S            31
#define KEY_D            32
#define KEY_F            33
#define KEY_G            34
#define KEY_H            35
#define KEY_J            36
#define KEY_K            37
#define KEY_L            38
#define KEY_SEMICOLON    39
#define KEY_APOSTROPHE   40
#define KEY_GRAVE        41
#define KEY_LSHIFT       42
#define KEY_BACKSLASH    43
#define KEY_Z            44
#define KEY_X            45
#define KEY_C            46
#define KEY_V            47
#define KEY_B            48
#define KEY_N            49
#define KEY_M            50
#define KEY_COMMA        51
#define KEY_PERIOD       52
#define KEY_SLASH        53
#define KEY_RSHIFT       54
#define KEY_NUM_ASTERISK 55
#define KEY_LALT         56
#define KEY_SPACE        57
#define KEY_CAPSLOCK     58
#define KEY_F1           59
#define KEY_F2           60
#define KEY_F3           61
#define KEY_F4           62
#define KEY_F5           63
#define KEY_F6           64
#define KEY_F7           65
#define KEY_F8           66
#define KEY_F9           67
#define KEY_F10          68


#define PACKETS_IN_PIPE 1024
#define PS2_DATA        0x60
#define PS2_COMMAND     0x64
#define PS2_TIMEOUT     1000


#define PS2_SCANCODE    0xF0
#define PS2_ENABLE_P1   0xAE
#define PS2_ENABLE_P2   0xA8
#define PS2_DISABLE_P1  0xAD
#define PS2_DISABLE_P2  0xA7


void ps2_setup(void);
void ps2_keyboard_handler(void);
