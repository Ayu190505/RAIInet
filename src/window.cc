#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "window.h"

Xwindow::Xwindow(int width, int height) : width(width), height(height) {
    d = XOpenDisplay(NULL);
    if (d == NULL) {
        std::cerr << "Cannot open display" << std::endl;
        exit(1);
    }
    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapRaised(d, w);

    gc = XCreateGC(d, w, 0, (XGCValues *)0);

    XFlush(d);
    XFlush(d);

    // Set up colours.
    XColor xcolour;
    Colormap cmap;
    char color_vals[13][20] = {"white", "black", "red", "mediumseagreen", "blue",
                               "cyan", "yellow", "magenta", "orange", "brown", "cornflowerblue", "darkgoldenrod1", "gray"};

    cmap = DefaultColormap(d, DefaultScreen(d));
    for(int i = 0; i < 13; ++i) {
        if(!XParseColor(d, cmap, color_vals[i], &xcolour)) {
            std::cerr << "Bad colour: " << color_vals[i] << std::endl;
        }
        if(!XAllocColor(d, cmap, &xcolour)) {
            std::cerr << "Bad colour: " << color_vals[i] << std::endl;
        }
        colours[i] = xcolour.pixel;
    }

    XSetForeground(d, gc, colours[Black]);

    // Make window non-resizable.
    XSizeHints hints;
    hints.flags = (USPosition | PSize | PMinSize | PMaxSize );
    hints.height = hints.base_height = hints.min_height = hints.max_height = height;
    hints.width = hints.base_width = hints.min_width = hints.max_width = width;
    XSetNormalHints(d, w, &hints);

    XSynchronize(d, True);

    usleep(1000);
}

Xwindow::~Xwindow() {
    // Free loaded fonts and GCs
    for(auto &pair : fontGCs) {
        XFreeGC(d, pair.second);
    }
    // Close the display
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XFillRectangle(d, w, gc, x, y, width, height);
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::drawRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XDrawRectangle(d, w, gc, x, y, width, height);
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::drawString(int x, int y, std::string msg, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XDrawString(d, w, gc, x, y, msg.c_str(), msg.length());
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::loadFont(std::string fontName) {
    if(fonts.find(fontName) == fonts.end()) {
        XFontStruct *font_info = XLoadQueryFont(d, fontName.c_str());
        if(!font_info) {
            std::cerr << "Error loading font: " << fontName << std::endl;
            return;
        }
        fonts[fontName] = font_info->fid;

        XGCValues values;
        values.font = font_info->fid;
        values.foreground = BlackPixel(d, s);
        GC fontGC = XCreateGC(d, w, GCFont | GCForeground, &values);
        fontGCs[fontName] = fontGC;

        XFreeFont(d, font_info);
    }
}

void Xwindow::drawStringFont(int x, int y, std::string msg, std::string fontName, int colour) {
    if(fontGCs.find(fontName) == fontGCs.end()) {
        loadFont(fontName);
    }
    if(fontGCs.find(fontName) != fontGCs.end()) {
        GC fontGC = fontGCs[fontName];
        XSetForeground(d, fontGC, colours[colour]);
        XDrawString(d, w, fontGC, x, y, msg.c_str(), msg.length());
        XSetForeground(d, fontGC, colours[Black]);
    } else {
        drawString(x, y, msg, colour);
    }
}

void Xwindow::drawLine(int x1, int y1, int x2, int y2, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XDrawLine(d, w, gc, x1, y1, x2, y2);
}
