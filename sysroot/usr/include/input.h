//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef INPUT_H
#define INPUT_H

#include <sys/time.h>

struct input_event {
    struct timeval timestamp;
    unsigned long ie_class;
    unsigned long ie_subclass;
    unsigned long ie_type;
    union {
        struct {
            unsigned long axis;
            long x;
            long y;
        } ie_move;
        struct {
            char c;
            char padding[3];
            unsigned long scancode;
            unsigned long flags;
        } ie_key;
        char ie_string[24];
    };
};

struct input_request {
    unsigned long ir_type;
    unsigned long ir_arg;
};

// non device specific
#define IR_INFO_REQUEST 0x01
#define IR_VENDOR_REQUEST 0x02
#define IR_NAME_REQUEST 0x03
#define IR_RESET_REQUEST 0x04

// device specific
#define IR_SET_LED_REQUEST 0x05
#define IR_SET_LAYOUT_REQUEST 0x06

// event types
#define IE_UNKOW_EVENT 0x00
#define IE_ACK_EVENT 0x01
#define IE_MOVE_EVENT 0x02
#define IE_KEY_EVENT 0x03

// class
#define IE_CLASS_UNKNOW 0x00
#define IE_CLASS_KEYBOARD 0x01
#define IE_CLASS_MOUSE 0x02
#define IE_CLASS_JOYSTICK 0x03

// subclass
#define IE_SUBCLASS_UNKNOW 0x00

// keyboard subclass
#define IE_SUBCLASS_USB_KBD 0x01
#define IE_SUBCLASS_PS2_KBD 0x02
#define IE_SUBCALSS_VIRT_KBD 0x03

// mouse subclass
#define IE_SUBCLASS_USB_MOUSE 0x01
#define IE_SUBCLASS_PS2_MOUSE 0x02
#define IE_SUBCLASS_VIRT_MOUSE 0x03
#define IE_SUBCLASS_PS2_TRACKPAD 0x04

// key event flags
#define IE_KEY_PRESS 0x01
#define IE_KEY_RELEASE 0x02
#define IE_KEY_HOLD 0x03
#define IE_KEY_GRAPH 0x04  // printable key
#define IE_KEY_GRPAH IE_KEY_GRAPH  //conserved 

//ioctl
#define I_INPUT_GET_CONTROL  1001
#define I_INPUT_DROP_CONTROL 1002

#endif

_END_C_HEADER
