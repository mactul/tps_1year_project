#ifndef GUI_H
#define GUI_H

#include <SA/SA.h>
#include <stdint.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define HEADER_HEIGHT 40

#define LIST_ENTRY_HEIGHT 80
#define LIST_WIDTH 500

#define ELEVATOR_WIDTH 15
#define ELEVATOR_HEIGHT 100

#define ELEVATOR_TRAIL_COLOR 0xF3F3FF

#define SCROLL_PIXEL_COUNT 15

#define WINDOW_BACKGROUND 0xFFFFFF
#define WINDOW_FOREGROUND 0x000000

#define WINDOW_BACKGROUND_ALTERNATE 0xE0E0EC
#define WINDOW_FOREGROUND_ALTERNATE 0x606060

#define WINDOW_BACKGROUND_SELECTED 0x3DAEE9
#define WINDOW_FOREGROUND_SELECTED 0xFFFFFF

#define GRAPH_PAD 50
#define GRAPH_HEIGHT 300
#define GRAPH_PLOT_COLOR WINDOW_BACKGROUND_SELECTED

#define FONT_WIDTH 6

enum ELEVATOR_COLOR {
    ELEVATOR_COLOR_DEFAULT = 0x606060,
    ELEVATOR_COLOR_HOVER = 0x808080,
    ELEVATOR_COLOR_CLICKED = 0xa0a0a0,
};

typedef struct _elevator_properties {
    enum ELEVATOR_COLOR color;
    int position_y;
} ElevatorProperties;


void draw_star(SA_GraphicsWindow* window, double x, double y, float percent_filling);

#endif