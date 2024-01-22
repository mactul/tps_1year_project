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
static SA_bool movie_search(SA_DynamicArray* film_infos, SA_DynamicArray* film_stats, const char* substring, SA_DynamicArray** film_stats_filtered)
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
static void search_bar_highlight_redraw(SA_GraphicsWindow* window, SA_bool do_highlight)
{
    uint32_t outline_color = do_highlight == SA_TRUE ? WINDOW_BACKGROUND_SELECTED : SEARCH_BG_COLOR;
    SA_graphics_vram_draw_hollow_rectangle(window, 0, HEADER_HEIGHT, LIST_WIDTH - 1, SEARCH_BAR_HEIGHT - 1, outline_color, 1);
}

/// @brief Handle a click inside a graphical window
/// @param function_arguments Structure pointer of various arguments
static void event_handler_mouse_down(FunctionArguments* function_arguments)
{
    if (!*(function_arguments->query_has_results))
    {
        return;
    }
    *(function_arguments->search_bar_highlight) = SA_FALSE;
    if (function_arguments->event->events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && function_arguments->event->events.mouse.x < LIST_WIDTH) // Click (or hold) on elevator
    {
        function_arguments->elevator_properties->color = ELEVATOR_COLOR_CLICKED;
        double elevator_track_length_adapted = WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT;
        double elevator_position = (int) function_arguments->event->events.mouse.y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT / 2;
        double percentage_offset = elevator_position / elevator_track_length_adapted;
        draw_movie_list_from_percentage_offset(function_arguments, percentage_offset);
        SA_graphics_vram_flush(function_arguments->window);
        *(function_arguments->elevator_mouse_down) = SA_TRUE;
    }
    else if (function_arguments->event->events.mouse.x < LIST_WIDTH - ELEVATOR_WIDTH && function_arguments->event->events.mouse.y > HEADER_HEIGHT + SEARCH_BAR_HEIGHT) // Click on a movie
    {
        draw_movie_info(function_arguments, function_arguments->event->events.mouse.y);
        draw_movie_list_from_relative_pixel_offset(function_arguments, 0);
        SA_graphics_vram_flush(function_arguments->window);
    }
    else if (function_arguments->event->events.mouse.x < LIST_WIDTH && function_arguments->event->events.mouse.y > HEADER_HEIGHT && function_arguments->event->events.mouse.y <= HEADER_HEIGHT + SEARCH_BAR_HEIGHT)
    {
        *(function_arguments->search_bar_highlight) = SA_TRUE;
    }
    search_bar_highlight_redraw(function_arguments->window, *(function_arguments->search_bar_highlight));
    SA_graphics_vram_flush(function_arguments->window);
}

/// @brief Handle a click inside a graphical window
/// @param function_arguments Structure pointer of various arguments
static void event_handler_key_down(FunctionArguments* function_arguments)
{
    SA_DynamicArray* film_stats_to_count = *(function_arguments->display_query) ? *(function_arguments->film_stats_filtered) : function_arguments->films_stats;
    if (!*(function_arguments->search_bar_highlight))
    {
        if (function_arguments->event->events.key.keycode == 0x6f && *(function_arguments->selected_index) > 0) // Up arrow
        {
            *(function_arguments->selected_index) -= 1;
            draw_movie_list_from_relative_pixel_offset(function_arguments, -LIST_ENTRY_HEIGHT);
            draw_movie_info(function_arguments, *(function_arguments->selected_index) * LIST_ENTRY_HEIGHT - *(function_arguments->pixel_offset) + HEADER_HEIGHT + SEARCH_BAR_HEIGHT);
            SA_graphics_vram_flush(function_arguments->window);
        }
        else if (function_arguments->event->events.key.keycode == 0x74 && (uint64_t) *(function_arguments->selected_index) < SA_dynarray_size(film_stats_to_count) - 1) // Down arrow
        {
            *(function_arguments->selected_index) += 1;
            draw_movie_list_from_relative_pixel_offset(function_arguments, LIST_ENTRY_HEIGHT);
            draw_movie_info(function_arguments, *(function_arguments->selected_index) * LIST_ENTRY_HEIGHT - *(function_arguments->pixel_offset) + HEADER_HEIGHT + SEARCH_BAR_HEIGHT);
            SA_graphics_vram_flush(function_arguments->window);
        }
        return;
    }
    function_arguments->text_input_string = SA_graphics_get_text_input_value(function_arguments->text_input);
    *(function_arguments->display_query) = (function_arguments->text_input_string[0] != '\0');
    *(function_arguments->query_has_results) = movie_search(function_arguments->films_infos, function_arguments->films_stats, function_arguments->text_input_string, function_arguments->film_stats_filtered);
    if (*(function_arguments->query_has_results))
    {
        *(function_arguments->selected_index) = 0;
        draw_movie_list_from_percentage_offset(function_arguments, 0.0);
        draw_movie_info(function_arguments, HEADER_HEIGHT + SEARCH_BAR_HEIGHT);
        SA_graphics_vram_flush(function_arguments->window);
    }
    else
    {
        // Clear search area
        SA_graphics_vram_draw_rectangle(function_arguments->window, 0, HEADER_HEIGHT + SEARCH_BAR_HEIGHT, LIST_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT, WINDOW_BACKGROUND);
        // Clear main area
        SA_graphics_vram_draw_rectangle(function_arguments->window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_WIDTH - LIST_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT, WINDOW_BACKGROUND);
        SA_graphics_vram_flush(function_arguments->window);
        *(function_arguments->display_query) = SA_FALSE;
    }
}

/// @brief Draw the loading screen inside a graphical window
/// @param window The window to draw in
static void draw_waiting_screen(SA_GraphicsWindow* window)
{
    // Waiting screen, this should not even display as reading stats.bin is almost instantaneous
    SA_graphics_vram_draw_rectangle(window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BACKGROUND);
    const char wait_text1[] = "Merci de patienter";
    const char wait_text2[] = "Les données sont en train de charger";
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2, (WINDOW_WIDTH - SA_strlen(wait_text1)) / 2, wait_text1, WINDOW_FOREGROUND);
    SA_graphics_vram_draw_text(window, WINDOW_HEIGHT / 2 + 20, (WINDOW_WIDTH - SA_strlen(wait_text2)) / 2, wait_text2, WINDOW_FOREGROUND);
}

/// @brief Draw everything needed from the initial view in a graphical window
/// @param function_arguments Structure of various arguments
static void draw_initial_view(FunctionArguments* function_arguments)
{
    // Clear screen again
    SA_graphics_vram_draw_rectangle(function_arguments->window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BACKGROUND);

    // Initial view, main area is blank
    SA_graphics_vram_draw_text(function_arguments->window, 10, 25, "Movie stats - recommended for you", WINDOW_FOREGROUND);

    // Separators
    SA_graphics_vram_draw_horizontal_line(function_arguments->window, 0, WINDOW_WIDTH, HEADER_HEIGHT, WINDOW_FOREGROUND, 2);
    SA_graphics_vram_draw_vertical_line(function_arguments->window, LIST_WIDTH, HEADER_HEIGHT, WINDOW_HEIGHT, WINDOW_FOREGROUND, 2);
    draw_movie_list_from_percentage_offset(function_arguments, 0.0);
    
    draw_movie_info(function_arguments, HEADER_HEIGHT + SEARCH_BAR_HEIGHT);

    function_arguments->text_input = SA_graphics_create_text_input(function_arguments->window, 1, HEADER_HEIGHT + 1, SEARCH_BG_COLOR, WINDOW_FOREGROUND_SELECTED, 80, 9, 13);
    // 12 is font height in below call
    SA_graphics_vram_draw_text(function_arguments->window, 10, HEADER_HEIGHT + 14 + 12, "Search", SEARCH_BAR_PLACEHOLDER_COLOR);
    search_bar_highlight_redraw(function_arguments->window, *(function_arguments->search_bar_highlight));

    SA_graphics_vram_flush(function_arguments->window);
}

/// @brief Function to handle all events
/// @param function_arguments Structure of various arguments
static void event_dispatcher(FunctionArguments* function_arguments)
{
    switch(function_arguments->event->event_type)
    {
        case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_DOWN:
            event_handler_mouse_down(function_arguments);
            break;
        case SA_GRAPHICS_EVENT_MOUSE_LEFT_CLICK_UP:
            function_arguments->elevator_properties->color = ELEVATOR_COLOR_DEFAULT;
            redraw_elevator(function_arguments->window, function_arguments->elevator_properties);
            SA_graphics_vram_flush(function_arguments->window);
            *(function_arguments->elevator_mouse_down) = SA_FALSE;
            break;
        case SA_GRAPHICS_EVENT_MOUSE_MOVE:
            if (!*(function_arguments->query_has_results))
            {
                break;
            }
            *(function_arguments->cursor_properties) = function_arguments->event->events.mouse;
            if (*(function_arguments->elevator_mouse_down) == SA_TRUE) // Moving the elevator
            {
                function_arguments->elevator_properties->color = ELEVATOR_COLOR_CLICKED;
                double elevator_track_length_adapted = WINDOW_HEIGHT - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT;
                double elevator_position = (int) function_arguments->event->events.mouse.y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT - ELEVATOR_HEIGHT / 2;
                double percentage_offset = elevator_position / elevator_track_length_adapted;
                draw_movie_list_from_percentage_offset(function_arguments, percentage_offset);
            }
            else if ((int) function_arguments->event->events.mouse.x >= LIST_WIDTH - ELEVATOR_WIDTH && (int) function_arguments->event->events.mouse.x <= LIST_WIDTH && (int) function_arguments->event->events.mouse.y >= function_arguments->elevator_properties->position_y && (int) function_arguments->event->events.mouse.y <= function_arguments->elevator_properties->position_y + ELEVATOR_HEIGHT) // Hover over the elevator
            {
                function_arguments->elevator_properties->color = ELEVATOR_COLOR_HOVER;
                redraw_elevator(function_arguments->window, function_arguments->elevator_properties);
            }
            else
            {
                function_arguments->elevator_properties->color = ELEVATOR_COLOR_DEFAULT;
                redraw_elevator(function_arguments->window, function_arguments->elevator_properties);
            }
            SA_graphics_vram_flush(function_arguments->window);
            break;
        case SA_GRAPHICS_EVENT_SCROLL_UP:
            if (!*(function_arguments->query_has_results))
            {
                break;
            }
            if ((int) function_arguments->cursor_properties->x <= LIST_WIDTH && (int) function_arguments->cursor_properties->y >= HEADER_HEIGHT) // Scrolling in list
            {
                draw_movie_list_from_relative_pixel_offset(function_arguments, -SCROLL_PIXEL_COUNT);
                SA_graphics_vram_flush(function_arguments->window);
            }
            break;
        case SA_GRAPHICS_EVENT_SCROLL_DOWN:
            if (!*(function_arguments->query_has_results))
            {
                break;
            }
            if ((int) function_arguments->cursor_properties->x <= LIST_WIDTH && (int) function_arguments->cursor_properties->y >= HEADER_HEIGHT) // Scrolling in list
            {
                draw_movie_list_from_relative_pixel_offset(function_arguments, SCROLL_PIXEL_COUNT);
                SA_graphics_vram_flush(function_arguments->window);
            }
            break;
        case SA_GRAPHICS_EVENT_KEY_DOWN:
            event_handler_key_down(function_arguments);
            break;
        case SA_GRAPHICS_EVENT_CLOSE_WINDOW:
            printf("Bye bye\n");
            break;
        default:
            break;
    }
}

/// @brief This function receives all the events linked to a window
/// @param window The window that produced the event
static void draw_callback(SA_GraphicsWindow *window)
{
    draw_waiting_screen(window);

    int selected_index = 0;
    SA_bool display_query = SA_FALSE;
    SA_bool query_has_results = SA_TRUE;
    SA_bool search_bar_highlight = SA_FALSE;
    SA_DynamicArray* film_stats_filtered = NULL;

    const char* text_input_string = NULL;

    int pixel_offset = 0;
    ElevatorProperties elevator_properties = {.color = ELEVATOR_COLOR_DEFAULT, .position_y = 0};
    SA_EventMouse cursor_properties = {.x = 0, .y = 0};

    SA_bool event_read;
    SA_GraphicsEvent event;

    SA_bool elevator_mouse_down = SA_FALSE;

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

    FunctionArguments function_arguments = {
        .cursor_properties = &cursor_properties,
        .display_query = &display_query,
        .elevator_mouse_down = &elevator_mouse_down,
        .elevator_properties = &elevator_properties,
        .event = &event,
        .film_stats_filtered = &film_stats_filtered,
        .films_infos = films_infos,
        .films_stats = films_stats,
        .pixel_offset = &pixel_offset,
        .query_has_results = &query_has_results,
        .search_bar_highlight = &search_bar_highlight,
        .selected_index = &selected_index,
        .text_input = NULL,
        .text_input_string = text_input_string,
        .window = window
    };

    draw_initial_view(&function_arguments);

    do {
        if ((event_read = SA_graphics_wait_next_event(window, &event)))
        {
            SA_bool text_focus = SA_graphics_handle_text_input_events(function_arguments.text_input, &event);
            if(!text_focus)
            {
                if(SA_graphics_get_text_input_value(function_arguments.text_input)[0] == '\0')
                {
                    SA_graphics_vram_draw_text(window, 10, HEADER_HEIGHT + 14 + 12, "Search", SEARCH_BAR_PLACEHOLDER_COLOR);
                    SA_graphics_vram_flush(window);
                }
            }
            else
            {
                SA_graphics_redraw_text_input(function_arguments.text_input);
            }
            event_dispatcher(&function_arguments);
        }
    } while (!event_read || event.event_type != SA_GRAPHICS_EVENT_CLOSE_WINDOW);

    SA_dynarray_free(&films_stats);
    SA_dynarray_free(&films_infos);
    SA_dynarray_free(&film_stats_filtered);
    fclose(films);
    SA_graphics_free_text_input(&(function_arguments.text_input));
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