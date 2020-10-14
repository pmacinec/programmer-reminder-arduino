#include "pitches.h"
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

void print_greetings(void) {
  u8g.setFont(u8g_font_profont15);
  u8g.setPrintPos(0, 10);
  u8g.print("Hello Peter!");
  u8g.setFont(u8g_font_profont12);
  u8g.setPrintPos(0, 40);
  u8g.print("Hopefully, you will");
  u8g.setPrintPos(0, 55);
  u8g.print("enjoy today at work!");
}

void draw2(void) {
  u8g.setFont(u8g_font_profont12);
  u8g.setPrintPos(0, 10);
  u8g.print("Here we go");
  u8g.setPrintPos(0, 25);
  u8g.print("UNLUCKYYYYYY");
}


void setup() {
  u8g.firstPage();
  do {
    print_greetings();
  } while (u8g.nextPage());
  delay(3000);
}
void loop() {
  u8g.firstPage();
  do {
    draw2();
  } while (u8g.nextPage() );
  delay(3500);
}
