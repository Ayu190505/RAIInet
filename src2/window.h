#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <X11/Xlib.h>
#include <iostream>
#include <string>
#include <map>

class Xwindow {
  Display *d;
  Window w;
  int s, width, height;
  GC gc;
  unsigned long colours[12];
  std::map<std::string, Font> fonts;
  std::map<std::string, GC> fontGCs;

 public:
  Xwindow(int width=500, int height=500);  // Constructor; displays the window.
  ~Xwindow();                              // Destructor; destroys the window.

  enum {White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown, CornFlowerBlue, DarkGold, Gray}; // Available colours.

  int getWidth() const;
  int getHeight() const;

  // Draws a rectangle
  void fillRectangle(int x, int y, int width, int height, int colour=Black);

    void drawRectangle(int x, int y, int width, int height, int colour=Black);

    // Draws a string using default font
    void drawString(int x, int y, std::string msg, int colour=Black);

    // Draws a string using specified font
    void drawStringFont(int x, int y, std::string msg, std::string fontName, int colour=Black);
    void drawLine(int x1, int y1, int x2, int y2, int colour=Black);
    // Loads a font
    void loadFont(std::string fontName);
};

#endif
