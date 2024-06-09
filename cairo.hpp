#pragma once

#include <stdexcept>

#include "X11.hpp"

#include <cairo.h>
#include <cairo-xlib.h>

namespace Cairo {

class Surface {
public:
    Surface(Display* display, X11::Overlay& overlay) {
        width  = overlay.width;
        height = overlay.height;
        cairo_surface_t* raw = cairo_xlib_surface_create(
            display, overlay.window, overlay.vinfo.visual, width, height);

        if (cairo_surface_status(raw) != CAIRO_STATUS_SUCCESS) {
            throw std::runtime_error("Failed to create cairo surface");
        }
        surface = { raw, [](cairo_surface_t* s){ cairo_surface_destroy(s); } };

        cairo_t* raw_ctx = cairo_create(surface.get());
        if (cairo_status(raw_ctx) != CAIRO_STATUS_SUCCESS) {
            throw std::runtime_error("Failed to create cairo context");
        }
        ctx = { raw_ctx, [](cairo_t* c){ cairo_destroy(c); } };
    }

    void clear() {
        cairo_set_source_rgba(ctx.get(), 0.0, 0.0, 0.0, 0.0);
        cairo_set_operator(ctx.get(), CAIRO_OPERATOR_CLEAR);
        cairo_rectangle(ctx.get(), 0, 0, width, height);
        cairo_fill(ctx.get());
    }

    void draw_line(int x1, int y1, int x2, int y2, int width, cv::Vec4f color, cv::Vec4f color_end) {
        cairo_set_operator(ctx.get(), CAIRO_OPERATOR_OVER);
        // cairo linear gradient

        cairo_pattern_t* pattern = cairo_pattern_create_linear(x1, y1, x2, y2);
        cairo_pattern_add_color_stop_rgba(pattern, 0, color[0], color[1], color[2], color[3]);
        cairo_pattern_add_color_stop_rgba(pattern, 1, color_end[0], color_end[1], color_end[2], color_end[3]);

        cairo_set_source(ctx.get(), pattern);
        cairo_set_line_width(ctx.get(), width);

        cairo_move_to(ctx.get(), x1, y1);
        cairo_line_to(ctx.get(), x2, y2);
        cairo_stroke(ctx.get());
    }

    void draw_text(int x, int y, const std::string& text, int size, cv::Vec4f color) {
        cairo_select_font_face(ctx.get(), "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(ctx.get(), size);
        cairo_set_source_rgba(ctx.get(), color[0], color[1], color[2], color[3]);
        cairo_move_to(ctx.get(), x, y);
        cairo_show_text(ctx.get(), text.c_str());
        cairo_stroke(ctx.get());
    }

private:
    int width, height;
    std::unique_ptr<cairo_t, void(*)(cairo_t*)> ctx = { nullptr, [](cairo_t*){} };
    std::unique_ptr<cairo_surface_t, void(*)(cairo_surface_t*)> surface = { nullptr, [](cairo_surface_t*){} };
}; // class Surface

}  // namespace Cairo

