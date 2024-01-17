#include "film_gui/gui.h"
#include <SA/SA.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

#define MOVIE_COUNT 500

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
void draw_movie_list_from_percentage_offset(SA_GraphicsWindow *window, double percentage, int* pixel_offset, ElevatorProperties* elevator_properties)
{
    if (percentage < 0)
    {
        percentage = 0;
    }
    if (percentage > 1)
    {
        percentage = 1;
    }

    int movie_count = MOVIE_COUNT;
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT;
    *pixel_offset = round(list_height * percentage);

    int element = *pixel_offset / LIST_ENTRY_HEIGHT;
    printf("First (at least partially) visible element = %d\n", element);

    SA_graphics_vram_draw_vertical_line(window, 0, HEADER_HEIGHT + 1, WINDOW_HEIGHT, WINDOW_BACKGROUND, LIST_WIDTH * 2);

    char text[50] = "Numero #"; 
    int text_length = strlen(text);

    for (int i = 0; i < (WINDOW_HEIGHT - HEADER_HEIGHT) / LIST_ENTRY_HEIGHT + 1; i++)
    {
        snprintf(&text[text_length], 10, "%d", element + i);
        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT, WINDOW_FOREGROUND, 1);
        if (HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 10 <= HEADER_HEIGHT)
        {
            continue;
        }
        SA_graphics_vram_draw_text(window, 10, HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 20, text, WINDOW_FOREGROUND);
    }

    elevator_properties->position_y = percentage * (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT) + HEADER_HEIGHT;
    redraw_elevator(window, elevator_properties);
}

/// @brief Draw the movie list from a pixel offset of scrolling
/// @param window Which window to draw the list in
/// @param direction Number of pixels to scroll (negative to go up)
/// @param pixel_offset Pixel offset in the list to store current scroll position
/// @param elevator_properties Pointer to properties of the scrollbar
void draw_movie_list_from_relative_pixel_offset(SA_GraphicsWindow* window, int direction, int* pixel_offset, ElevatorProperties* elevator_properties)
{
    int movie_count = MOVIE_COUNT;
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT;
    if (*pixel_offset + direction < 0)
    {
        *pixel_offset = 0;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties);
        return;
    }
    if (*pixel_offset + direction > list_height)
    {
        *pixel_offset = list_height;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties);
        return;
    }
    *pixel_offset += direction;
    double percentage = (double) *pixel_offset / list_height;
    draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties);
}

/// @brief This function receives all the events linked to a window
/// @param window The window that produced the event
void draw_callback(SA_GraphicsWindow *window)
{
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2, WINDOW_BACKGROUND, WINDOW_HEIGHT);
    SA_graphics_vram_draw_text(window, 10, 25, "Statistiques des films", WINDOW_FOREGROUND);
    int pixel_offset = 0;
    ElevatorProperties elevator_properties = {.color = ELEVATOR_COLOR_DEFAULT, .position_y = 0};
    SA_EventMouse cursor_properties = {.x = 0, .y = 0};
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, HEADER_HEIGHT, WINDOW_FOREGROUND, 2);
    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_HEIGHT, WINDOW_FOREGROUND, 2);

    draw_movie_list_from_percentage_offset(window,  0.0, &pixel_offset, &elevator_properties);

    SA_graphics_vram_flush(window);

    SA_bool event_read;
    SA_GraphicsEvent event;

    SA_bool elevator_mouse_down = SA_FALSE;

    do {
        if ((event_read = SA_graphics_wait_next_event(window, &event)))
        {
            switch(event.event_type)
            {
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_DOWN:
                    if (event.events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && event.events.mouse.x < LIST_WIDTH)
                    {
                        elevator_properties.color = ELEVATOR_COLOR_CLICKED;
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT), &pixel_offset, &elevator_properties);
                        SA_graphics_vram_flush(window);
                        elevator_mouse_down = SA_TRUE;
                    }
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_UP:
                    elevator_properties.color = ELEVATOR_COLOR_DEFAULT;
                    draw_movie_list_from_relative_pixel_offset(window, 0, &pixel_offset, &elevator_properties);
                    SA_graphics_vram_flush(window);
                    elevator_mouse_down = SA_FALSE;
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_MOVE:
                    cursor_properties = event.events.mouse;
                    if (elevator_mouse_down == SA_TRUE)
                    {
                        elevator_properties.color = ELEVATOR_COLOR_CLICKED;
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT), &pixel_offset, &elevator_properties);
                    }
                    else if ((int) event.events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && (int) event.events.mouse.x <= LIST_WIDTH && (int) event.events.mouse.y >= elevator_properties.position_y && (int) event.events.mouse.y <= elevator_properties.position_y + ELEVATOR_HEIGHT)
                    {
                        elevator_properties.color = ELEVATOR_COLOR_HOVER;
                        redraw_elevator(window, &elevator_properties);
                    }
                    else
                    {
                        elevator_properties.color = ELEVATOR_COLOR_DEFAULT;
                        redraw_elevator(window, &elevator_properties);
                    }
                    SA_graphics_vram_flush(window);
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_UP:
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT)
                    {
                        draw_movie_list_from_relative_pixel_offset(window, -SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_DOWN:
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT)
                    {
                        draw_movie_list_from_relative_pixel_offset(window, SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_CLOSE_WINDOW:
                    printf("Window close\n");
                    break;
                default:
                    printf("Other event, %d\n", event.event_type);
                    break;
            }
        }
    } while (!event_read || event.event_type != SA_GRAPHICS_EVENT_CLOSE_WINDOW);
}

/// @brief Start the graphical window
void start_gui()
{
    SA_graphics_create_window("Statistiques", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, draw_callback, SA_GRAPHICS_QUEUE_EVERYTHING, NULL);
}