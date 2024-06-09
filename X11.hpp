#pragma once

#include <stdexcept>
#include <memory>

#include <opencv2/opencv.hpp>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

namespace X11 {

void init();

Display* get_display();
Window get_root();
int get_default_screen();
int get_screen_width();
int get_screen_height();

void Flush();

std::unique_ptr<XImage, void(*)(XImage*)> capture();
cv::Mat1b capture_edges(int low_thresh=60, int high_thresh=120);

cv::Vec2i get_cursor_pos();

class Overlay {
public:
    Overlay();
    ~Overlay();

    const int width, height;
    Window window;
    XVisualInfo vinfo;
}; // class Overlay

}  // namespace X11
