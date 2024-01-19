#include "film_gui/gui.h"
#include "film_gui/arg-handler.h"
#include "src/data_structs.h"
#include "src/stats_io/parser_stats.h"
#include "src/dataset_io/parser_txt.h"
#include <SA/SA.h>
#include <stdio.h>

#include <math.h>

static GuiArguments _args_structure;
static int _return_code;

/// @brief Draw movie statistics in the info window
/// @param window The window in which to draw
/// @param mouse_y Vertical position of the cursor to find the movie index
/// @param pixel_offset Offset of the list that is shown inside the window
/// @param films_infos Array of structures containing the title and release year for every movie
/// @param films_stats Array of structures containing stats about every movie
void draw_movie_info(SA_GraphicsWindow* window, uint32_t mouse_y, int* pixel_offset, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index)
{
    SA_graphics_vram_draw_horizontal_line(window, LIST_WIDTH + 1, WINDOW_WIDTH, (WINDOW_HEIGHT - HEADER_HEIGHT) / 2 + HEADER_HEIGHT + 1, WINDOW_BACKGROUND, WINDOW_HEIGHT - HEADER_HEIGHT); // Clear main area

    int movie_pixel_offset = mouse_y - HEADER_HEIGHT + *pixel_offset;
    *selected_index = movie_pixel_offset / LIST_ENTRY_HEIGHT;

    // Get film stats and infos
    FilmStats* fstats = _SA_dynarray_get_element_ptr(films_stats, *selected_index);
    FilmInfo* info = _SA_dynarray_get_element_ptr(films_infos, fstats->film_id);

    SA_GraphicsRectangle graphics_rectangle_avg_ratings = {.height = GRAPH_HEIGHT, .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD, .top_left_corner_x = LIST_WIDTH + GRAPH_PAD, .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - 2 * GRAPH_HEIGHT - 2 * GRAPH_PAD};
    SA_GraphicsRectangle graphics_rectangle_ratings_count = {.height = GRAPH_HEIGHT, .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD, .top_left_corner_x = LIST_WIDTH + GRAPH_PAD, .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - GRAPH_HEIGHT};
    
    double years[NUMBER_OF_YEARS_LOGGED_IN_STATS] = {0};
    double ratings[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    double ratings_count[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    int ratings_total_count = 0;
    double ratings_sum = 0;
    double years_of_ratings_count;

    for (int i = 0; i < NUMBER_OF_YEARS_LOGGED_IN_STATS; i++)
    {
        years[i] = fstats->max_year - i;
        ratings[i] = fstats->mean_rating_over_years[i] < 0 ? 0 : fstats->mean_rating_over_years[i];
        ratings_count[i] = fstats->kept_rating_count_over_years[i];
        ratings_total_count += fstats->kept_rating_count_over_years[i];
        if(fstats->mean_rating_over_years[i] != -1)
        {
            ratings_sum += ratings[i];
            years_of_ratings_count++;
        }
    }

    SA_graphics_plot_continuous_graph(window, years, ratings, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_avg_ratings, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);
    SA_graphics_plot_continuous_graph(window, years, ratings_count, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_ratings_count, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);

    const char desc1[] = "Average note over years";
    const char desc2[] = "Number of ratings per year";
    SA_graphics_vram_draw_text(window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(desc1)) / 2 + LIST_WIDTH, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 20, desc1, WINDOW_FOREGROUND); // First graph title
    SA_graphics_vram_draw_text(window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(desc2)) / 2 + LIST_WIDTH, graphics_rectangle_ratings_count.top_left_corner_y + graphics_rectangle_ratings_count.height + 20, desc2, WINDOW_FOREGROUND); // Second graph title
    SA_graphics_vram_draw_text(window, ((WINDOW_WIDTH + LIST_WIDTH) / 2 - SA_strlen(info->name) * FONT_WIDTH) / 2 + LIST_WIDTH, HEADER_HEIGHT + 20, info->name, WINDOW_FOREGROUND); // Main area title (movie name)

    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + 50, "Average note:", WINDOW_FOREGROUND);

    double avg_note = ratings_sum / years_of_ratings_count;
    printf("%f\n", avg_note);
    for(int i = 0; i < 5; i++)
    {
        if(avg_note >= 1.0)
        {
            draw_star(window, LIST_WIDTH + MARGIN_DESC_TEXT + SA_strlen("Average note: ") * FONT_WIDTH + 40 * i, HEADER_HEIGHT + 40, 1.0);
            avg_note -= 1.0;
        }
        else
        {
            draw_star(window, LIST_WIDTH + MARGIN_DESC_TEXT + SA_strlen("Average note: ") * FONT_WIDTH + 40 * i, HEADER_HEIGHT + 40, avg_note);
            avg_note = 0.0;
        }
    }
   
    char rating_count_text[50] = "Number of rating over the last 10 years : ";
    SA_uint64_to_str(rating_count_text + SA_strlen(rating_count_text), ratings_total_count);

    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + 80, rating_count_text, WINDOW_FOREGROUND); // Main area title (movie name)

    SA_graphics_vram_draw_horizontal_line(window, LIST_WIDTH + GRAPH_PAD, WINDOW_WIDTH - GRAPH_PAD, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 30, WINDOW_FOREGROUND_ALTERNATE, 1); // Separator
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
void draw_movie_list_from_percentage_offset(SA_GraphicsWindow *window, double percentage, int* pixel_offset, ElevatorProperties* elevator_properties, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index)
{
    if (percentage < 0)
    {
        percentage = 0;
    }
    if (percentage > 1)
    {
        percentage = 1;
    }

    int movie_count = SA_dynarray_size(films_stats);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT;
    *pixel_offset = round(list_height * percentage); // Where we are in the list

    int element = *pixel_offset / LIST_ENTRY_HEIGHT; // Which element is the first partially visible

    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH - (ELEVATOR_WIDTH / 2) - 1, HEADER_HEIGHT + 1, WINDOW_HEIGHT, ELEVATOR_TRAIL_COLOR, ELEVATOR_WIDTH); // Clear elevator track
    SA_graphics_vram_draw_vertical_line(window, (LIST_WIDTH - ELEVATOR_WIDTH) / 2, HEADER_HEIGHT + 1, WINDOW_HEIGHT, WINDOW_BACKGROUND, (LIST_WIDTH - ELEVATOR_WIDTH)); // Clear list

    char text_limited[WINDOW_WIDTH / FONT_WIDTH - 20] = {0};
    char year[5] = {0};

    for (int i = 0; i < (WINDOW_HEIGHT - HEADER_HEIGHT) / LIST_ENTRY_HEIGHT + 1; i++)
    {
        if (i == 0 && ((element & 1) == 1)) // Hack to color the first partially visible entry's background
        {
            int bottom = HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT;
            SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, (bottom + HEADER_HEIGHT) / 2, WINDOW_BACKGROUND_ALTERNATE, bottom - HEADER_HEIGHT);
        }
        if (((element + i) & 1) == 0) // Alternate two background colors in the list
        {
            SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_ALTERNATE, LIST_ENTRY_HEIGHT);
        }

        uint32_t fg_color_alt = WINDOW_FOREGROUND_ALTERNATE;
        uint32_t fg_color = WINDOW_FOREGROUND;

        if (element + i == *selected_index)
        {
            if (i == 0)
            {
                int bottom = HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT;
                SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, (bottom + HEADER_HEIGHT) / 2, WINDOW_BACKGROUND_SELECTED, bottom - HEADER_HEIGHT);
            }
            else
            {
                SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_SELECTED, LIST_ENTRY_HEIGHT);
            }
            fg_color = fg_color_alt = WINDOW_FOREGROUND_SELECTED;
        }
        else if (element + i + 1 == *selected_index)
        {
            SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + LIST_ENTRY_HEIGHT / 2, WINDOW_BACKGROUND_SELECTED, LIST_ENTRY_HEIGHT);
        }

        SA_graphics_vram_draw_horizontal_line(window, 0, LIST_WIDTH - ELEVATOR_WIDTH, HEADER_HEIGHT + (i + 1) * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT, WINDOW_FOREGROUND, 1); // List separator

        if (HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 25 <= HEADER_HEIGHT) // Don't display text if it will be hidden by the header
        {
            continue;
        }

        // Get film text and year and display them
        FilmStats* fstats = _SA_dynarray_get_element_ptr(films_stats, element + i);
        FilmInfo* info = _SA_dynarray_get_element_ptr(films_infos, fstats->film_id);
        SA_strncpy(text_limited, info->name, sizeof(text_limited));
        snprintf(year, 5, "%hd", info->year);
        SA_graphics_vram_draw_text(window, 15, HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 35, text_limited, fg_color);
        SA_graphics_vram_draw_text(window, 20, HEADER_HEIGHT + i * LIST_ENTRY_HEIGHT - (*pixel_offset + LIST_ENTRY_HEIGHT) % LIST_ENTRY_HEIGHT + 60, year, fg_color_alt);
    }

    elevator_properties->position_y = percentage * (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT) + HEADER_HEIGHT;
    redraw_elevator(window, elevator_properties);
}

/// @brief Draw the movie list from a pixel offset of scrolling
/// @param window Which window to draw the list in
/// @param direction Number of pixels to scroll (negative to go up)
/// @param pixel_offset Pixel offset in the list to store current scroll position
/// @param elevator_properties Pointer to properties of the scrollbar
/// @param films_infos Array of structures containing the title and release year for every movie
/// @param films_stats Array of structures containing stats about every movie
void draw_movie_list_from_relative_pixel_offset(SA_GraphicsWindow* window, int direction, int* pixel_offset, ElevatorProperties* elevator_properties, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index)
{
    int movie_count = SA_dynarray_size(films_stats);
    int list_height = (movie_count * LIST_ENTRY_HEIGHT) - WINDOW_HEIGHT + HEADER_HEIGHT;

    // Compute percentage based on the position in the list
    if (*pixel_offset + direction < 0)
    {
        *pixel_offset = 0;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index);
        return;
    }
    if (*pixel_offset + direction > list_height)
    {
        *pixel_offset = list_height;
        double percentage = (double) *pixel_offset / list_height;
        draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index);
        return;
    }
    *pixel_offset += direction;
    double percentage = (double) *pixel_offset / list_height;
    draw_movie_list_from_percentage_offset(window, percentage, pixel_offset, elevator_properties, films_infos, films_stats, selected_index);
}

/// @brief This function receives all the events linked to a window
/// @param window The window that produced the event
void draw_callback(SA_GraphicsWindow *window)
{
    // Waiting screen, this should not even display as reading stats.bin is almost instantaneous
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2, WINDOW_BACKGROUND, WINDOW_HEIGHT);
    const char wait_text1[] = "Merci de patienter";
    const char wait_text2[] = "Les données sont en train de charger";
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2, (WINDOW_WIDTH - SA_strlen(wait_text1)) / 2, wait_text1, WINDOW_FOREGROUND);
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2 + 20, (WINDOW_WIDTH - SA_strlen(wait_text2)) / 2, wait_text2, WINDOW_FOREGROUND);

    int selected_index = 0;

    SA_DynamicArray* films_infos = get_films_infos("download/movie_titles.txt");

    FILE* films = fopen(_args_structure.stats_bin_file, "rb");

    if (films == NULL)
    {
        _return_code = 2;
        return;
    }
    
    SA_DynamicArray* films_stats = read_stats(films);
    if (films_stats == NULL)
    {
        _return_code = 1;
        return;
    }

    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2, WINDOW_BACKGROUND, WINDOW_HEIGHT); // Clear screen again

    // Initial view, main area is blank
    SA_graphics_vram_draw_text(window, 10, 25, "Movie stats - recommended for you", WINDOW_FOREGROUND);
    int pixel_offset = 0;
    ElevatorProperties elevator_properties = {.color = ELEVATOR_COLOR_DEFAULT, .position_y = 0};
    SA_EventMouse cursor_properties = {.x = 0, .y = 0};
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, HEADER_HEIGHT, WINDOW_FOREGROUND, 2);
    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_HEIGHT, WINDOW_FOREGROUND, 2);
    draw_movie_list_from_percentage_offset(window,  0.0, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
    
    draw_movie_info(window, HEADER_HEIGHT + 1, &pixel_offset, films_infos, films_stats, &selected_index);

    //draw_star(window, 800, 130);

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
                    if (event.events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && event.events.mouse.x < LIST_WIDTH) // Click (or hold) on elevator
                    {
                        elevator_properties.color = ELEVATOR_COLOR_CLICKED;
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT), &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                        SA_graphics_vram_flush(window);
                        elevator_mouse_down = SA_TRUE;
                    }
                    else if (event.events.mouse.x < LIST_WIDTH - ELEVATOR_WIDTH && event.events.mouse.y > HEADER_HEIGHT) // Click on a movie
                    {
                        draw_movie_info(window, event.events.mouse.y, &pixel_offset, films_infos, films_stats, &selected_index);
                        draw_movie_list_from_relative_pixel_offset(window, 0, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_UP:
                    elevator_properties.color = ELEVATOR_COLOR_DEFAULT;
                    draw_movie_list_from_relative_pixel_offset(window, 0, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                    SA_graphics_vram_flush(window);
                    elevator_mouse_down = SA_FALSE;
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_MOVE:
                    cursor_properties = event.events.mouse;
                    if (elevator_mouse_down == SA_TRUE) // Moving the elevator
                    {
                        elevator_properties.color = ELEVATOR_COLOR_CLICKED;
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - ELEVATOR_HEIGHT), &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                    }
                    else if ((int) event.events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && (int) event.events.mouse.x <= LIST_WIDTH && (int) event.events.mouse.y >= elevator_properties.position_y && (int) event.events.mouse.y <= elevator_properties.position_y + ELEVATOR_HEIGHT) // Hover over the elevator
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
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT) // Scrolling in list
                    {
                        draw_movie_list_from_relative_pixel_offset(window, -SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_DOWN:
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT) // Scrolling in list
                    {
                        draw_movie_list_from_relative_pixel_offset(window, SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_CLOSE_WINDOW:
                    SA_dynarray_free(&films_stats);
                    SA_dynarray_free(&films_infos);
                    printf("Bye bye\n");
                default:
                    break;
            }
        }
    } while (!event_read || event.event_type != SA_GRAPHICS_EVENT_CLOSE_WINDOW);
}

/// @brief Start the whole program
/// @param argc Number of command line arguments
/// @param argv Array of command line arguments
/// @return 
/// * 0 if everything went correctly
/// * 1 if there was a memory allocation error
/// * 2 if the films
/// * 3 if command line arguments are incorrect
int main(int argc, char* argv[])
{
    _return_code = 0;
    int remaining_index;
    SA_init();
    if (!parse_args(argc, argv, &_args_structure, &remaining_index))
    {
        _return_code = 3;
        goto EXIT_LBL;
    }
    SA_graphics_create_window("Statistiques", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, draw_callback, SA_GRAPHICS_QUEUE_EVERYTHING, NULL);

EXIT_LBL:
    SA_destroy();
    return _return_code;
}