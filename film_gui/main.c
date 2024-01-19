#include "film_gui/gui.h"
#include "film_gui/arg-handler.h"
#include "src/data_structs.h"
#include "src/stats_io/parser_stats.h"
#include "src/dataset_io/parser_txt.h"
#include <SA/SA.h>
#include <stdio.h>


static GuiArguments _args_structure;
static int _return_code;
static char movie_title_file_path[256];


/// @brief Finds all films in film_stats where the corresponding entry in film_infos matches a search pattern and copies them in film_stats_filtered
/// @param film_infos Array of FilmInfo
/// @param film_stats Source (unfiltered) array of FilmStats
/// @param substring Search pattern
/// @param film_stats_filtered Destination (filtered) array of FilmStats
/// @return SA_FALSE if no film titles matches, SA_TRUE if there is at least one match
SA_bool movie_search(SA_DynamicArray* film_infos, SA_DynamicArray* film_stats, const char* substring, SA_DynamicArray** film_stats_filtered)
{
    SA_dynarray_free(film_stats_filtered);
    *film_stats_filtered = SA_dynarray_create(FilmStats);
    char* film_name;
    FilmStats current_film_stat;
    FilmInfo* current_film_info;
    for (uint64_t i = 0; i < SA_dynarray_size(film_stats); i++)
    {
        current_film_stat = SA_dynarray_get(FilmStats, film_stats, i);
        current_film_info = _SA_dynarray_get_element_ptr(film_infos, current_film_stat.film_id);
        film_name = current_film_info->name;
        if (SA_str_search_case_unsensitive(film_name, substring) == -1)
        {
            continue;
        }
        SA_dynarray_append(FilmStats, *film_stats_filtered, current_film_stat);
    }
    return SA_dynarray_size(*film_stats_filtered) != 0;
}

/// @brief Draw a highlight around the search bar inside a window
/// @param window The window to draw in
/// @param do_highlight If the highlight should be visible or "hidden"
void search_bar_highlight_redraw(SA_GraphicsWindow* window, SA_bool do_highlight)
{
    uint32_t outline_color = do_highlight == SA_TRUE ? WINDOW_BACKGROUND_SELECTED : SEARCH_BG_COLOR;
    SA_graphics_vram_draw_hollow_rectangle(window, 0, HEADER_HEIGHT, LIST_WIDTH - 1, SEARCH_BAR_HEIGHT - 1, outline_color, 1);
}

/// @brief Handle a click inside a graphical window
/// @param window The window in which to draw
/// @param event Details of this event
/// @param query_has_results If the current search term has matches
/// @param search_bar_highlight If the search bar is currently focused
/// @param elevator_properties Position and brightness of the scrollbar
/// @param elevator_mouse_down If the cursor is clicked on the scrollbar
/// @param pixel_offset Height of the first visible pixel of the movie list
/// @param films_infos Array of FilmInfo structures
/// @param films_stats Array of FilmStats structures
/// @param selected_index Index of the currently selected movie in the visible list
/// @param display_query If the list area should display the filtered list
/// @param film_stats_filtered Array of FilmStats structures filtered to match the search query
void event_handler_mouse_down(SA_GraphicsWindow* window, SA_GraphicsEvent* event, SA_bool query_has_results, SA_bool* search_bar_highlight, ElevatorProperties* elevator_properties, SA_bool* elevator_mouse_down, int* pixel_offset, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index, SA_bool* display_query, SA_DynamicArray* film_stats_filtered)
{
    if (!query_has_results)
    {
        return;
    }
    *search_bar_highlight = SA_FALSE;
    if (event->events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && event->events.mouse.x < LIST_WIDTH) // Click (or hold) on elevator
    {
        elevator_properties->color = ELEVATOR_COLOR_CLICKED;
        draw_movie_list_from_percentage_offset(window, ((double) ((int) event->events.mouse.y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT), pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
        SA_graphics_vram_flush(window);
        *elevator_mouse_down = SA_TRUE;
    }
    else if (event->events.mouse.x < LIST_WIDTH - ELEVATOR_WIDTH && event->events.mouse.y > HEADER_HEIGHT + SEARCH_BAR_HEIGHT) // Click on a movie
    {
        draw_movie_info(window, event->events.mouse.y, pixel_offset, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
        draw_movie_list_from_relative_pixel_offset(window, 0, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, film_stats_filtered);
        SA_graphics_vram_flush(window);
    }
    else if (event->events.mouse.x < LIST_WIDTH && event->events.mouse.y > HEADER_HEIGHT && event->events.mouse.y <= HEADER_HEIGHT + SEARCH_BAR_HEIGHT)
    {
        *search_bar_highlight = SA_TRUE;
    }
    search_bar_highlight_redraw(window, *search_bar_highlight);
    SA_graphics_vram_flush(window);
}

/// @brief Handle a click inside a graphical window
/// @param window The window in which to draw
/// @param event Details of this event
/// @param display_query If the list area should display the filtered list
/// @param search_bar_highlight If the search bar is currently focused
/// @param selected_index Index of the currently selected movie in the visible list
/// @param pixel_offset Height of the first visible pixel of the movie list
/// @param text_input_string String that is currently displayed inside the text field
/// @param text_input Text input field
/// @param query_has_results If the current search term has matches
/// @param elevator_properties Position and brightness of the scrollbar
/// @param films_infos Array of FilmInfo structures
/// @param films_stats Array of FilmStats structures
/// @param film_stats_filtered Array of FilmStats structures filtered to match the search query
void event_handler_key_down(SA_GraphicsWindow* window, SA_GraphicsEvent* event, SA_bool* display_query, SA_bool* search_bar_highlight, int* selected_index, int* pixel_offset, const char* text_input_string, SA_GraphicsTextInput* text_input, SA_bool* query_has_results, ElevatorProperties* elevator_properties, SA_DynamicArray* films_stats, SA_DynamicArray* films_infos, SA_DynamicArray** film_stats_filtered)
{
    SA_DynamicArray* film_stats_to_count = *display_query ? *film_stats_filtered : films_stats;
    if (!*search_bar_highlight)
    {
        if (event->events.key.keycode == 0x6f && *selected_index > 0) // Up arrow
        {
            *selected_index -= 1;
            draw_movie_list_from_relative_pixel_offset(window, -LIST_ENTRY_HEIGHT, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
            draw_movie_info(window, *selected_index * LIST_ENTRY_HEIGHT - *pixel_offset + HEADER_HEIGHT + SEARCH_BAR_HEIGHT, pixel_offset, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
            SA_graphics_vram_flush(window);
        }
        else if (event->events.key.keycode == 0x74 && (uint64_t) *selected_index < SA_dynarray_size(film_stats_to_count) - 1) // Down arrow
        {
            *selected_index += 1;
            draw_movie_list_from_relative_pixel_offset(window, LIST_ENTRY_HEIGHT, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
            draw_movie_info(window, *selected_index * LIST_ENTRY_HEIGHT - *pixel_offset + HEADER_HEIGHT + SEARCH_BAR_HEIGHT, pixel_offset, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
            SA_graphics_vram_flush(window);
        }
        return;
    }
    text_input_string = SA_graphics_get_text_input_value(text_input);
    *display_query = (text_input_string[0] != '\0');
    *query_has_results = movie_search(films_infos, films_stats, text_input_string, film_stats_filtered);
    if (*query_has_results)
    {
        *selected_index = 0;
        draw_movie_list_from_percentage_offset(window, 0.0, pixel_offset, elevator_properties, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
        draw_movie_info(window, HEADER_HEIGHT + SEARCH_BAR_HEIGHT, pixel_offset, films_infos, films_stats, selected_index, display_query, *film_stats_filtered);
        SA_graphics_vram_flush(window);
    }
    else
    {
        // Clear search area
        SA_graphics_vram_draw_rectangle(window, 0, HEADER_HEIGHT + SEARCH_BAR_HEIGHT, LIST_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT, WINDOW_BACKGROUND);
        // Clear main area
        SA_graphics_vram_draw_rectangle(window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_WIDTH - LIST_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT, WINDOW_BACKGROUND);
        SA_graphics_vram_flush(window);
        *display_query = SA_FALSE;
    }
}

/// @brief This function receives all the events linked to a window
/// @param window The window that produced the event
void draw_callback(SA_GraphicsWindow *window)
{
    // Waiting screen, this should not even display as reading stats.bin is almost instantaneous
    SA_graphics_vram_draw_rectangle(window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BACKGROUND);
    const char wait_text1[] = "Merci de patienter";
    const char wait_text2[] = "Les données sont en train de charger";
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2, (WINDOW_WIDTH - SA_strlen(wait_text1)) / 2, wait_text1, WINDOW_FOREGROUND);
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2 + 20, (WINDOW_WIDTH - SA_strlen(wait_text2)) / 2, wait_text2, WINDOW_FOREGROUND);

    int selected_index = 0;
    SA_bool display_query = SA_FALSE;
    SA_bool query_has_results = SA_TRUE;
    SA_bool search_bar_highlight = SA_FALSE;
    SA_DynamicArray* film_stats_filtered = NULL;

    SA_DynamicArray* films_infos = get_films_infos(movie_title_file_path);
    if (films_infos == NULL)
    {
        _return_code = RETURN_CODE_ERROR_FILE;
        return;
    }

    FILE* films = fopen(_args_structure.stats_bin_file, "rb");
    if (films == NULL)
    {
        SA_dynarray_free(&films_infos);
        _return_code = RETURN_CODE_ERROR_FILE;
        return;
    }
    
    SA_DynamicArray* films_stats = read_stats(films);
    if (films_stats == NULL)
    {
        SA_dynarray_free(&films_infos);
        fclose(films);
        _return_code = RETURN_CODE_ERROR_MEMORY;
        return;
    }

    // Clear screen again
    SA_graphics_vram_draw_rectangle(window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BACKGROUND);

    // Initial view, main area is blank
    SA_graphics_vram_draw_text(window, 10, 25, "Movie stats - recommended for you", WINDOW_FOREGROUND);
    int pixel_offset = 0;
    ElevatorProperties elevator_properties = {.color = ELEVATOR_COLOR_DEFAULT, .position_y = 0};
    SA_EventMouse cursor_properties = {.x = 0, .y = 0};

    // Separators
    SA_graphics_vram_draw_horizontal_line(window, 0, WINDOW_WIDTH, HEADER_HEIGHT, WINDOW_FOREGROUND, 2);
    SA_graphics_vram_draw_vertical_line(window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_HEIGHT, WINDOW_FOREGROUND, 2);
    draw_movie_list_from_percentage_offset(window,  0.0, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);
    
    draw_movie_info(window, HEADER_HEIGHT + 1, &pixel_offset, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);

    SA_GraphicsTextInput* text_input = SA_graphics_create_text_input(window, 1, HEADER_HEIGHT + 1, SEARCH_BG_COLOR, WINDOW_FOREGROUND_SELECTED, 80, 9, 13);
    // 12 is font height in below call
    SA_graphics_vram_draw_text(window, 10, HEADER_HEIGHT + 14 + 12, "Search", SEARCH_BAR_PLACEHOLDER_COLOR);
    search_bar_highlight_redraw(window, search_bar_highlight);

    SA_graphics_vram_flush(window);

    SA_bool event_read;
    SA_GraphicsEvent event;

    SA_bool elevator_mouse_down = SA_FALSE;

    do {
        if ((event_read = SA_graphics_wait_next_event(window, &event)))
        {
            const char* text_input_string = NULL;
            SA_bool text_focus = SA_graphics_handle_text_input_events(text_input, &event);
            if(!text_focus)
            {
                if(SA_graphics_get_text_input_value(text_input)[0] == '\0')
                {
                    SA_graphics_vram_draw_text(window, 10, HEADER_HEIGHT + 14 + 12, "Search", SEARCH_BAR_PLACEHOLDER_COLOR);
                    SA_graphics_vram_flush(window);
                }
            }
            else
            {
                SA_graphics_redraw_text_input(text_input);
            }

            switch(event.event_type)
            {
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_DOWN:
                    event_handler_mouse_down(window, &event, query_has_results, &search_bar_highlight, &elevator_properties, &elevator_mouse_down, &pixel_offset, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_UP:
                    elevator_properties.color = ELEVATOR_COLOR_DEFAULT;
                    redraw_elevator(window, &elevator_properties);
                    SA_graphics_vram_flush(window);
                    elevator_mouse_down = SA_FALSE;
                    break;
                case SA_GRAPHICS_EVENT_MOUSE_MOVE:
                    if (!query_has_results)
                    {
                        break;
                    }
                    cursor_properties = event.events.mouse;
                    if (elevator_mouse_down == SA_TRUE) // Moving the elevator
                    {
                        elevator_properties.color = ELEVATOR_COLOR_CLICKED;
                        draw_movie_list_from_percentage_offset(window, ((double) ((int) event.events.mouse.y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT / 2)) / (WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT), &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);
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
                    if (!query_has_results)
                    {
                        break;
                    }
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT) // Scrolling in list
                    {
                        draw_movie_list_from_relative_pixel_offset(window, -SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_SCROLL_DOWN:
                    if (!query_has_results)
                    {
                        break;
                    }
                    if ((int) cursor_properties.x <= LIST_WIDTH && (int) cursor_properties.y >= HEADER_HEIGHT) // Scrolling in list
                    {
                        draw_movie_list_from_relative_pixel_offset(window, SCROLL_PIXEL_COUNT, &pixel_offset, &elevator_properties, films_infos, films_stats, &selected_index, &display_query, film_stats_filtered);
                        SA_graphics_vram_flush(window);
                    }
                    break;
                case SA_GRAPHICS_EVENT_KEY_DOWN:
                    event_handler_key_down(window, &event, &display_query, &search_bar_highlight, &selected_index, &pixel_offset, text_input_string, text_input, &query_has_results, &elevator_properties, films_stats, films_infos, &film_stats_filtered);
                    break;
                case SA_GRAPHICS_EVENT_CLOSE_WINDOW:
                    SA_dynarray_free(&films_stats);
                    SA_dynarray_free(&films_infos);
                    SA_dynarray_free(&film_stats_filtered);
                    fclose(films);
                    SA_graphics_free_text_input(&text_input);
                    printf("Bye bye\n");
                default:
                    break;
            }
        }
    } while (!event_read || event.event_type != SA_GRAPHICS_EVENT_CLOSE_WINDOW);
}

/// @brief This program shows all the recommendations in a graphical window
/// @param argc Number of command line arguments
/// @param argv Array of command line arguments
/// @return 
/// * RETURN_CODE_OK if everything went correctly
/// * RETURN_CODE_ERROR_MEMORY if there was a memory allocation error
/// * RETURN_CODE_ERROR_FILE if a file does not exist
/// * RETURN_CODE_ERROR_ARGUMENTS if command line arguments are incorrect
int main(int argc, char* argv[])
{
    _return_code = RETURN_CODE_OK;
    int remaining_index;
    SA_init();
    if (!parse_args(argc, argv, &_args_structure, &remaining_index))
    {
        _return_code = RETURN_CODE_ERROR_ARGUMENTS;
        goto EXIT_LBL;
    }
    SA_strncpy(movie_title_file_path, argv[remaining_index], 256);
    SA_graphics_create_window("Statistiques", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, draw_callback, SA_GRAPHICS_QUEUE_EVERYTHING, NULL);

EXIT_LBL:
    SA_destroy();

    switch(_return_code)
    {
        case RETURN_CODE_ERROR_MEMORY:
            SA_print_error("Memory error\n");
            break;
        case RETURN_CODE_ERROR_FILE:
            SA_print_error("File I/O error\n");
            break;
        case RETURN_CODE_ERROR_ARGUMENTS:
            SA_print_error("Invalid arguments\n");
            break;
        case RETURN_CODE_SIGNAL_ABORT:
            SA_print_error("Stopped by user\n");
            break;
        default:
            break;
    }
    return _return_code;
}