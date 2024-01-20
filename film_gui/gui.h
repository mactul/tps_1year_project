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

#define MARGIN_DESC_TEXT 20

#define GRAPH_PAD 50
#define GRAPH_HEIGHT 300
#define GRAPH_PLOT_COLOR WINDOW_BACKGROUND_SELECTED

#define SEARCH_BAR_HEIGHT 40
#define SEARCH_BG_COLOR 0x31363b
#define SEARCH_BAR_PLACEHOLDER_COLOR 0xB0B0B0

#define TITLE_PADDING_X 15
#define DATE_PADDING_X 20

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

typedef struct _function_arguments {
    SA_GraphicsWindow* window;
    SA_GraphicsEvent* event;
    SA_EventMouse* cursor_properties;
    SA_GraphicsTextInput* text_input;
    SA_bool* query_has_results;
    SA_bool* search_bar_highlight;
    SA_bool* elevator_mouse_down;
    SA_bool* display_query;
    ElevatorProperties* elevator_properties;
    int* pixel_offset;
    int* selected_index;
    const char* text_input_string;
    SA_DynamicArray* films_stats;
    SA_DynamicArray* films_infos;
    SA_DynamicArray** film_stats_filtered;
} FunctionArguments;


void draw_star(SA_GraphicsWindow* window, double x, double y, float percent_filling);

void draw_movie_info(FunctionArguments* function_arguments, uint32_t mouse_y);

void redraw_elevator(SA_GraphicsWindow* window, ElevatorProperties* elevator_properties);
void draw_movie_list_from_percentage_offset(FunctionArguments* function_arguments, double percentage);
void draw_movie_list_from_relative_pixel_offset(FunctionArguments* function_arguments, int direction);

#endif