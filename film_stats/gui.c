#include "film_stats/gui.h"
#include <SA/SA.h>
#include <stdio.h>

#include <math.h>

#define MOVIE_COUNT 200

void draw_movie_list_from_percentage_offset(SA_GraphicsWindow *window, double percentage)
{
    if (percentage < 0)
    {
        percentage = 0;
    }
    if (percentage > 1)
    {
        percentage = 1;
    }

    uint32_t movie_count = MOVIE_COUNT;
    uint32_t list_height = ((movie_count + 1) * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT - HEADER_HEIGHT;
    uint32_t pixel_offset = round(list_height * percentage);

    uint32_t element = (pixel_offset) / LIST_ENTRY_HEIGHT;
    printf("First (at least partially) visible element = %d\n", element);

    SA_graphics_vram_draw_vertical_line(window, 0, HEADER_HEIGHT + 1, WINDOW_HEIGHT, 0, LIST_WIDTH * 2);

    for (int i = 0; i < (WINDOW_HEIGHT - HEADER_HEIGHT) / LIST_ENTRY_HEIGHT + 1; i++)
    {
        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT, 0x0000FF, 2);
    }

    SA_graphics_vram_draw_horizontal_line(window, LIST_WIDTH - ELEVATOR_WIDTH, LIST_WIDTH, percentage * (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT) + HEADER_HEIGHT + ELEVATOR_HEIGHT / 2, 0x808080, ELEVATOR_HEIGHT);
}

void draw_callback(SA_GraphicsWindow *window)
{
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, HEADER_HEIGHT, 0x0000FF, 2);
    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_HEIGHT, 0x0000FF, 2);

    draw_movie_list_from_percentage_offset(window,  0.0);

    SA_graphics_vram_flush(window);

    SA_bool event_read;
    SA_GraphicsEvent event;

    SA_bool mouse_down = SA_FALSE;

    do {
        if ((event_read = SA_graphics_wait_next_event(window, &event)))
        {
            switch(event.event_type)
            {
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_DOWN:
                    if (event.events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && event.events.mouse.x < LIST_WIDTH)
                    {
                        // printf("ELEVATOR MOVE AT HEIGHT %d\n", event.events.mouse.y - HEADER_HEIGHT);
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT));
                        SA_graphics_vram_flush(window);
                        mouse_down = SA_TRUE;
                    }
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_UP:
                    printf("RELEASE %d %d\n", event.events.mouse.x, event.events.mouse.y);
                    mouse_down = SA_FALSE;
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_MOVE:
                    if (mouse_down)
                    {
                        // printf("ELEVATOR MOVE AT HEIGHT %d\n", event.events.mouse.y - HEADER_HEIGHT);
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT));
                        SA_graphics_vram_flush(window);
                        mouse_down = SA_TRUE;
                    }
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_UP:
                    printf("SCROLL UP\n");
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_DOWN:
                    printf("SCROLL DOWN\n");
                    break;
                case SA_GRAPHICS_EVENT_CLOSE_WINDOW:
                    printf("Window close\n");
                    break;
                default:
                    printf("Other, %d\n", event.event_type);
                    break;
            }
        }
    } while (!event_read || event.event_type != SA_GRAPHICS_EVENT_CLOSE_WINDOW);
}

void start_gui()
{
    SA_graphics_create_window("Statistiques", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, draw_callback, (uint32_t) -1, NULL);
}