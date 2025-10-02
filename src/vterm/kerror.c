#include <vterm/kerror.h>

void kerror(const char *msg) {
    vterm_set_fg_color(COLOR_RED);
    vterm_print("Error: ");
    vterm_print(msg);
    vterm_print("\n");
    vterm_set_fg_color(COLOR_WHITE);
}
