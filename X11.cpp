#include "X11.hpp"

#include <functional>

namespace X11 {

static std::unique_ptr<Display, void(*)(Display*)> display = { nullptr, [](Display*){} };
static Window root;
static int default_screen;
static int screen_width, screen_height;

void init() {
    Display* raw_disp = XOpenDisplay(nullptr);
    if (!raw_disp) {
        throw std::runtime_error("Failed to open X display");
    }
    display = std::unique_ptr<Display, void(*)(Display*)> { raw_disp, [](Display* d){ XCloseDisplay(d); } };

    default_screen = DefaultScreen(display.get());
    root = RootWindow(display.get(), default_screen);

    screen_width = DisplayWidth(display.get(), default_screen);
    screen_height = DisplayHeight(display.get(), default_screen);
}

Display* get_display() {
    return display.get();
}

Window get_root() {
    return root;
}

int get_default_screen() {
    return default_screen;
}

int get_screen_width() {
    return screen_width;
}

int get_screen_height() {
    return screen_height;
}

void Flush() {
    XFlush(display.get());
}

std::unique_ptr<XImage, void(*)(XImage*)> capture() {
    XImage* raw = XGetImage(display.get(), root, 0, 0, screen_width, screen_height, AllPlanes, ZPixmap);
    if (!raw) {
        throw std::runtime_error("Failed to capture screen");
    }
    return std::unique_ptr<XImage, void(*)(XImage*)>(raw, [](XImage* img){ XDestroyImage(img); });
}

cv::Mat1b capture_edges(int low_thresh, int high_thresh) {
    auto x11_img = capture();
    cv::Mat3b img(screen_height, screen_width);
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            unsigned long pixel = XGetPixel(x11_img.get(), x, y);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(pixel & 0xFF, (pixel >> 8) & 0xFF, (pixel >> 16) & 0xFF);
        }
    }

    cv::Mat1b gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Mat1b edges;
    cv::Canny(gray, edges, low_thresh, high_thresh);
    return edges;
}

cv::Vec2i get_cursor_pos() {
    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask_return;
    XQueryPointer(display.get(), root, &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask_return);
    return { root_x, root_y };
}

Overlay::Overlay() : width(screen_width), height(screen_height) {
    // all we need for *overlays*
    XSetWindowAttributes attrs;
    attrs.override_redirect = true;

    // ensure we have 32bit support
    if (!XMatchVisualInfo(display.get(), default_screen, 32, TrueColor, &vinfo)) {
        throw std::runtime_error("No 32bit visual found");
    }

    // give it 32bit depth
    attrs.colormap = XCreateColormap(display.get(), root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    // create the overlay window
    window = XCreateWindow(
        display.get(), root,
        0, 0, screen_width, screen_height, 0,
        vinfo.depth, InputOutput,
        vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );

    // make it noninteractive by giving it a shape rect of 0 size
    XRectangle rect;
    XserverRegion region = XFixesCreateRegion(display.get(), &rect, 1);
    XFixesSetWindowShapeRegion(display.get(), window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display.get(), region);

    // map the window
    XMapWindow(display.get(), window);
}

Overlay::~Overlay() {
    XDestroyWindow(display.get(), window);
}

}  // namespace X11
