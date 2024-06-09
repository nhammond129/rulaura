#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

#include <chrono>
#include <thread>
#include <iostream>

#include "X11.hpp"
#include "cairo.hpp"

enum class INCDEC { INC, DEC };
inline int lookx(const cv::Mat1b& img, int x, int y, const int limit, const INCDEC dir) {
	if (dir == INCDEC::INC) {
		while (++x < limit) { if (img.at<uchar>(y, x) == 255) return x; }
	} else {
		while (--x >= 0) { if (img.at<uchar>(y, x) == 255) return x; }
	}
	return x;
}
inline int looky(const cv::Mat1b& img, int x, int y, const int limit, const INCDEC dir) {
	if (dir == INCDEC::INC) {
		while (++y < limit) { if (img.at<uchar>(y, x) == 255) return y; }
	} else {
		while (--y >= 0) { if (img.at<uchar>(y, x) == 255) return y; }
	}
	return x;
}

cv::Vec4i get_point_span(const cv::Mat1b& img, int x, int y) {
	int x_min = lookx(img, x, y,        0, INCDEC::DEC);
	int x_max = lookx(img, x, y, img.cols, INCDEC::INC);
	int y_min = looky(img, x, y,        0, INCDEC::DEC);
	int y_max = looky(img, x, y, img.rows, INCDEC::INC);

	return { x_min, y_min, x_max, y_max };
}

// single channel 8bit pixel
bool test_px(uchar px) {
	return (px == 255);
}

int main() {

	X11::init();
	XSynchronize(X11::get_display(), true);  // Synchronize with X server for debugging
	X11::Overlay overlay;
	Cairo::Surface surface(X11::get_display(), overlay);

	cv::Mat1b edges = X11::capture_edges(50, 100);

	constexpr float FOOT_SIZE = 8.0f;
	while (true) {
		cv::Vec2i cursor = X11::get_cursor_pos();
		const int x = cursor[0];
		const int y = cursor[1];

		cv::Vec4i span = get_point_span(edges, cursor[0], cursor[1]);
		std::string text1 = "x: " + std::to_string(x) + "\ty: " + std::to_string(y);
		std::string text2 = "w: " + std::to_string(span[2]-span[0]) + "\th: " + std::to_string(span[3]-span[1]);

		surface.clear();
		{	/* vertical */
			// legs
			surface.draw_line(x, span[1], x, y, 1, {1, 0, 0, 1}, {1, 0, 0, 0.1});
			surface.draw_line(x, span[3], x, y, 1, {1, 0, 0, 1}, {1, 0, 0, 0.1});

			// feet
			surface.draw_line(
					x-FOOT_SIZE/2., span[1],
					x+FOOT_SIZE/2., span[1],
					1, {1, 0, 0, 1}, {1, 0, 0, 1}
				);
			surface.draw_line(
					x-FOOT_SIZE/2., span[1],
					x+FOOT_SIZE/2., span[1],
					1, {1, 0, 0, 1}, {1, 0, 0, 1}
				);
		}
		{	/* horizontal */
			// legs
			surface.draw_line(span[0], y, x, y, 1, {1, 0, 0, 1}, {1, 0, 0, 0.1});
			surface.draw_line(span[2], y, x, y, 1, {1, 0, 0, 1}, {1, 0, 0, 0.1});

			// feet
			surface.draw_line(
					span[0], y-FOOT_SIZE/2.,
					span[0], y+FOOT_SIZE/2.,
					1, {1, 0, 0, 1}, {1, 0, 0, 1}
				);
			surface.draw_line(
					span[2], y-FOOT_SIZE/2.,
					span[2], y+FOOT_SIZE/2.,
					1, {1, 0, 0, 1}, {1, 0, 0, 1}
				);
		}


		constexpr int SHADOW_OFFSET = 2;
		{
			surface.draw_text(x+20+SHADOW_OFFSET, y-20+SHADOW_OFFSET, text1, 18, { 0, 0, 0, 1 });
			surface.draw_text(x+20, y-20, text1, 18, { 1, 0, 0, 1 });

			surface.draw_text(x+20+SHADOW_OFFSET, y+20+SHADOW_OFFSET, text2, 18, { 0, 0, 0, 1 });
			surface.draw_text(x+20, y+20, text2, 18, { 1, 0, 0, 1 });
		}

		X11::Flush();

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

    return 0;
}
