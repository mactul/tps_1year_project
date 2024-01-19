#include <math.h>
#include "film_gui/gui.h"
#include "src/data_structs.h"

#define MIN(a, b) (a > b ? b : a)


static void color_row(SA_GraphicsWindow* window, int i, int element, int pixel_offset)
{
    if(((element+i) & 0x1) == 0x1)
    {
        return;
    }

    if (i == 0) // Hack to color the first partially visible entry's background
    {
        int bottom = HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT;
        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, (bottom + HEADER_HEIGHT + SEARCH_BAR_HEIGHT) / 2, WINDOW_BACKGROUND_ALTERNATE, bottom - HEADER_HEIGHT - SEARCH_BAR_HEIGHT);
    }
    else
    {
        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_ALTERNATE, LIST_ENTRY_HEIGHT);
    }
}

/// @brief Redraws the scrollbar in a graphical window
/// @param window Which window to draw the scrollbar in
/// @param elevator_properties Pointer to properties of the scrollbar
void redraw_elevator(SA_GraphicsWindow* window, ElevatorProperties* elevator_properties)
{
    SA_graphics_vram_draw_horizontal_line(window, LIST_WIDTH - ELEVATOR_WIDTH, LIST_WIDTH, elevator_properties->position_y + ELEVATOR_HEIGHT / 2, elevator_properties->color, ELEVATOR_HEIGHT);
}

/// @brief Draw the movie list from a percentage of scrolling
/// @param window Which window to draw the list in
/// @param percentage Percentage of scroll in the list
/// @param pixel_offset Pixel offset in the list to store current scroll position
/// @param elevator_properties Pointer to properties of the scrollbar
/// @param films_infos Array of structures containing the title and release year for every movie
/// @param films_stats Array of structures containing stats about every movie
void draw_movie_list_from_percentage_offset(SA_GraphicsWindow *window, double percentage, int* pixel_offset, ElevatorProperties* elevator_properties, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index, SA_bool* display_query, SA_DynamicArray* film_stats_filtered)
{
    SA_DynamicArray* film_stats_to_use = (*display_query == SA_TRUE) ? film_stats_filtered : films_stats;

    int movie_count = SA_dynarray_size(film_stats_to_use);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;

    if (*selected_index >= movie_count)
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

    *pixel_offset = round(list_height * percentage); // Where we are in the list

    if (list_height <= 0)
    {
        *pixel_offset = 0;
    }

    int element = *pixel_offset / LIST_ENTRY_HEIGHT; // Which element is the first partially visible

    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH - (ELEVATOR_WIDTH / 2) - 1, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + 1, WINDOW_HEIGHT, ELEVATOR_TRAIL_COLOR, ELEVATOR_WIDTH); // Clear elevator track
    SA_graphics_vram_draw_vertical_line(window, (LIST_WIDTH - ELEVATOR_WIDTH) / 2, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + 1, WINDOW_HEIGHT, WINDOW_BACKGROUND, (LIST_WIDTH - ELEVATOR_WIDTH)); // Clear list

    char text_limited[(LIST_WIDTH - ELEVATOR_WIDTH - TITLE_PADDING_X) / FONT_WIDTH] = {0};
    char year[5] = {0};

    int max_i_value = MIN(movie_count, (WINDOW_HEIGHT - HEADER_HEIGHT - HEADER_HEIGHT) / LIST_ENTRY_HEIGHT + 1); // If there are less elements than what can be displayed, limit the for loop

    for (int i = 0; i < max_i_value; i++)
    {
        color_row(window, i, element, *pixel_offset);

        uint32_t fg_color_alt = WINDOW_FOREGROUND_ALTERNATE;
        uint32_t fg_color = WINDOW_FOREGROUND;

        if (element + i == *selected_index)
        {
            if (i == 0)
            {
                int bottom = HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT;
                SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, (bottom + HEADER_HEIGHT + SEARCH_BAR_HEIGHT) / 2, WINDOW_BACKGROUND_SELECTED, bottom - HEADER_HEIGHT - SEARCH_BAR_HEIGHT);
            }
            else
            {
                SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_SELECTED, LIST_ENTRY_HEIGHT);
            }
            fg_color = fg_color_alt = WINDOW_FOREGROUND_SELECTED;
        }
        else if (element + i + 1 == *selected_index)
        {
            SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_SELECTED, LIST_ENTRY_HEIGHT);
        }

        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT, WINDOW_FOREGROUND, 1); // List separator

        if (i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 25 <= 0) // Don't display text if it will be hidden by the header
        {
            continue;
        }

        // Get film text and year and display them
        FilmStats* fstats = _SA_dynarray_get_element_ptr(film_stats_to_use, element + i);
        FilmInfo* info = _SA_dynarray_get_element_ptr(films_infos, fstats->film_id);

        if(SA_strncpy(text_limited, info->name, sizeof(text_limited)-3) == sizeof(text_limited)-4)
        {
            SA_strcat(text_limited, "...");
        }

        SA_uint64_to_str(year, info->year);
        SA_graphics_vram_draw_text(window, TITLE_PADDING_X, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 35, text_limited, fg_color);
        SA_graphics_vram_draw_text(window, DATE_PADDING_X, HEADER_HEIGHT + SEARCH_BAR_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 60, year, fg_color_alt);
    }

    elevator_properties->position_y = percentage * (WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT) + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;
    redraw_elevator(window, elevator_properties);
}

/// @brief Draw the movie list from a pixel offset of scrolling
/// @param window Which window to draw the list in
/// @param direction Number of pixels to scroll (negative to go up)
/// @param pixel_offset Pixel offset in the list to store current scroll position
/// @param elevator_properties Pointer to properties of the scrollbar
/// @param films_infos Array of structures containing the title and release year for every movie
/// @param films_stats Array of structures containing stats about every movie
void draw_movie_list_from_relative_pixel_offset(SA_GraphicsWindow* window, int direction, int* pixel_offset, ElevatorProperties* elevator_properties, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index, SA_bool* display_query, SA_DynamicArray* film_stats_filtered)
{
    SA_DynamicArray* film_stats_to_use = *display_query == SA_TRUE ? film_stats_filtered : films_stats;
    int movie_count = SA_dynarray_size(film_stats_to_use);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT + SEARCH_BAR_HEIGHT;

    if (list_height <= 0)
    {
        *pixel_offset = 0;
    }

    // Compute percentage based on the position in the list
    if (*pixel_offset + direction < 0)
    {
        *pixel_offset = 0;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
        return;
    }
    if (*pixel_offset + direction > list_height)
    {
        *pixel_offset = list_height;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
        return;
    }
    *pixel_offset += direction;
    double percentage = (double) *pixel_offset / list_height;
    draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
}