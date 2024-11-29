#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <X11/Xlib.h>
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Xwindow {
  Display *d;
  Window w;
  int s, width, height;
  GC gc;
  unsigned long colours[13];
  map<string, Font> fonts;
  map<string, GC> fontGCs;

 public:
  Xwindow(int width=500, int height=500);  // Constructor; displays the window.
  ~Xwindow();                              // Destructor; destroys the window.

  enum {White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown, CornFlowerBlue, DarkGold, Gray}; // Available colours.

    // Draws a rectangle
    void fillRectangle(int x, int y, int width, int height, int colour=Black);

    void drawRectangle(int x, int y, int width, int height, int colour=Black);

    // Draws a string using default font
    void drawString(int x, int y, string msg, int colour=Black);

    // Draws a string using specified font
    void drawStringFont(int x, int y, string msg, string fontName, int colour=Black);
    void drawLine(int x1, int y1, int x2, int y2, int colour=Black);

    // Loads a font
    void loadFont(string fontName);
    
};

#endif

