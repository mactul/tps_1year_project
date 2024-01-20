#include <math.h>
#include "film_gui/gui.h"
#include "src/data_structs.h"

#define MIN(a, b) (a > b ? b : a)

/// @brief Color a list item's background inside a window
/// @param window The window to draw in
/// @param i Index of the list item
/// @param pixel_offset First pixel of the list that is visible on the screen
/// @param color Background color of the list item
static void color_row(SA_GraphicsWindow* window, int i, uint32_t pixel_offset, uint32_t color)
{
    if (i == 0) // it's the first entry, it may be partially cropped
    {
        SA_graphics_vram_draw_rectangle(window, 0, HEADER_HEIGHT + SEARCH_BAR_HEIGHT,  LIST_WIDTH - ELEVATOR_WIDTH, LIST_ENTRY_HEIGHT - pixel_offset % LIST_ENTRY_HEIGHT, color);
    }
    else
    {
        SA_graphics_vram_draw_rectangle(window, 0, 1 + HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - pixel_offset % LIST_ENTRY_HEIGHT, LIST_WIDTH - ELEVATOR_WIDTH, LIST_ENTRY_HEIGHT, color);
    }
}

/// @brief Redraws the scrollbar in a graphical window
/// @param window Which window to draw the scrollbar in
/// @param elevator_properties Pointer to properties of the scrollbar
void redraw_elevator(SA_GraphicsWindow* window, ElevatorProperties* elevator_properties)
{
    SA_graphics_vram_draw_rectangle(window, LIST_WIDTH - ELEVATOR_WIDTH, elevator_properties->position_y, ELEVATOR_WIDTH, ELEVATOR_HEIGHT, elevator_properties->color);
}

/// @brief Draw the movie list from a percentage of scrolling
/// @param function_arguments Structure of various arguments
/// @param percentage Percentage of scroll in the list
void draw_movie_list_from_percentage_offset(FunctionArguments* function_arguments, double percentage)
{
    SA_DynamicArray* film_stats_to_use = (*(function_arguments->display_query) == SA_TRUE) ? *(function_arguments->film_stats_filtered) : function_arguments->films_stats;

    int movie_count = SA_dynarray_size(film_stats_to_use);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;

    if (*(function_arguments->selected_index) >= movie_count)
    {
        return; // Can't click on an empty element
    }

    if (percentage < 0.0)
    {
        percentage = 0.0;
    }
    if (percentage > 1.0)
    {
        percentage = 1.0;
    }

    *(function_arguments->pixel_offset) = round(list_height * percentage); // Where we are in the list

    if (list_height <= 0)
    {
        *(function_arguments->pixel_offset) = 0;
    }

    int element = *(function_arguments->pixel_offset) / LIST_ENTRY_HEIGHT; // Which element is the first partially visible

    // Clear elevator track
    SA_graphics_vram_draw_rectangle(function_arguments->window, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT, ELEVATOR_WIDTH, WINDOW_HEIGHT, ELEVATOR_TRAIL_COLOR);
    // Clear list
    SA_graphics_vram_draw_rectangle(function_arguments->window, 0, HEADER_HEIGHT + SEARCH_BAR_HEIGHT, LIST_WIDTH - ELEVATOR_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT, WINDOW_BACKGROUND);
    
    char text_limited[(LIST_WIDTH - ELEVATOR_WIDTH - TITLE_PADDING_X) / FONT_WIDTH] = {0};
    char year[5] = {0};

    int max_i_value = MIN(movie_count, (WINDOW_HEIGHT - HEADER_HEIGHT - HEADER_HEIGHT) / LIST_ENTRY_HEIGHT + 1); // If there are less elements than what can be displayed, limit the for loop

    for (int i = 0; i < max_i_value; i++)
    {
        if(((element+i) & 0x1) == 0)
        {
            color_row(function_arguments->window, i, *(function_arguments->pixel_offset), WINDOW_BACKGROUND_ALTERNATE);
        }

        uint32_t fg_color_alt = WINDOW_FOREGROUND_ALTERNATE;
        uint32_t fg_color = WINDOW_FOREGROUND;

        if (element + i == *(function_arguments->selected_index))
        {
            color_row(function_arguments->window, i, *(function_arguments->pixel_offset), WINDOW_BACKGROUND_SELECTED);
            fg_color = fg_color_alt = WINDOW_FOREGROUND_SELECTED;
        }

        // List separator
        SA_graphics_vram_draw_horizontal_line(function_arguments->window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*(function_arguments->pixel_offset) + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT, WINDOW_FOREGROUND, 1);

        if (i * LIST_ENTRY_HEIGHT - (*(function_arguments->pixel_offset) + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 25 <= 0) // Don't display text if it will be hidden by the header
        {
            continue;
        }

        // Get film text and year and display them
        FilmStats* fstats = _SA_dynarray_get_element_ptr(film_stats_to_use, element + i);
        FilmInfo* info = _SA_dynarray_get_element_ptr(function_arguments->films_infos, fstats->film_id);

        if(SA_strncpy(text_limited, info->name, sizeof(text_limited)-3) == sizeof(text_limited)-4)
        {
            SA_strcat(text_limited, "...");
        }

        SA_uint64_to_str(year, info->year);
        SA_graphics_vram_draw_text(function_arguments->window, TITLE_PADDING_X, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*(function_arguments->pixel_offset) + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 35, text_limited, fg_color);
        SA_graphics_vram_draw_text(function_arguments->window, DATE_PADDING_X, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*(function_arguments->pixel_offset) + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 60, year, fg_color_alt);
    }

    function_arguments->elevator_properties->position_y = percentage * (WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT) + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;
    redraw_elevator(function_arguments->window, function_arguments->elevator_properties);
}

/// @brief Draw the movie list from a pixel offset of scrolling
/// @param function_arguments Structure of various arguments
/// @param direction Number of pixels to scroll (negative to go up)
void draw_movie_list_from_relative_pixel_offset(FunctionArguments* function_arguments, int direction)
{
    SA_DynamicArray* film_stats_to_use = *(function_arguments->display_query) == SA_TRUE ? *(function_arguments->film_stats_filtered) : function_arguments->films_stats;
    int movie_count = SA_dynarray_size(film_stats_to_use);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;

    if (list_height <= 0)
    {
        *(function_arguments->pixel_offset) = 0;
    }

    // Compute percentage based on the position in the list
    if (*(function_arguments->pixel_offset) + direction < 0)
    {
        *(function_arguments->pixel_offset) = 0;
        double percentage = (double) *(function_arguments->pixel_offset) / list_height;
        draw_movie_list_from_percentage_offset(function_arguments, percentage);
        return;
    }
    if (*(function_arguments->pixel_offset) + direction > list_height)
    {
        *(function_arguments->pixel_offset) = list_height;
        double percentage = (double) *(function_arguments->pixel_offset) / list_height;
        draw_movie_list_from_percentage_offset(function_arguments, percentage);
        return;
    }
    *(function_arguments->pixel_offset) += direction;
    double percentage = (double) *(function_arguments->pixel_offset) / list_height;
    draw_movie_list_from_percentage_offset(function_arguments, percentage);
}