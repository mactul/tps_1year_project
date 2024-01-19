#include "film_gui/gui.h"
#include "src/data_structs.h"

#define AVG_NOTE_LABEL "Average note: "
#define RATING_COUNT_LABEL "Number of rating over the last 10 years : "

#define GRAPH1_LABEL "Average note over years"
#define GRAPH2_LABEL "Number of ratings per year"

#define AVG_NOTE_Y_OFFSET 50
#define RATING_COUNT_Y_OFFSET 80

static void draw_summary(SA_GraphicsWindow* window, double avg_note, uint32_t ratings_total_count)
{
    char rating_count_text[50] = RATING_COUNT_LABEL;
    SA_uint64_to_str(rating_count_text + SA_strlen(rating_count_text), ratings_total_count);
    // drawing the rating count
    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + RATING_COUNT_Y_OFFSET, rating_count_text, WINDOW_FOREGROUND);

    // drawing the AVG_NOTE_LABEL
    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + AVG_NOTE_Y_OFFSET, AVG_NOTE_LABEL, WINDOW_FOREGROUND);

    // drawing the 5 stars
    for(int i = 0; i < 5; i++)
    {
        if(avg_note >= 1.0)
        {
            draw_star(window, LIST_WIDTH + MARGIN_DESC_TEXT + SA_strlen(AVG_NOTE_LABEL) * FONT_WIDTH + 40 * i, HEADER_HEIGHT + AVG_NOTE_Y_OFFSET - 10, 1.0);
            avg_note -= 1.0;
        }
        else
        {
            draw_star(window, LIST_WIDTH + MARGIN_DESC_TEXT + SA_strlen(AVG_NOTE_LABEL) * FONT_WIDTH + 40 * i, HEADER_HEIGHT + AVG_NOTE_Y_OFFSET - 10, avg_note);
            avg_note = 0.0;
        }
    }
}

/// @brief Draw movie statistics in the info window
/// @param window The window in which to draw
/// @param mouse_y Vertical position of the cursor to find the movie index
/// @param pixel_offset Offset of the list that is shown inside the window
/// @param films_infos Array of structures containing the title and release year for every movie
/// @param films_stats Array of structures containing stats about every movie
/// @param selected_index Index of the currently selected film in the list
/// @param display_query If the movie info should be fetched from the filtered array of film_stats
/// @param film_stats_filtered Filtered array of structures containing stats about matched movies
void draw_movie_info(SA_GraphicsWindow* window, uint32_t mouse_y, int* pixel_offset, SA_DynamicArray* films_infos, SA_DynamicArray* films_stats, int* selected_index, SA_bool* display_query, SA_DynamicArray* film_stats_filtered)
{
    int movie_pixel_offset = mouse_y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT + *pixel_offset;
    *selected_index = movie_pixel_offset / LIST_ENTRY_HEIGHT;

    SA_DynamicArray* film_stats_to_use = *display_query == SA_TRUE ? film_stats_filtered : films_stats;

    if ((uint64_t) *selected_index >= SA_dynarray_size(film_stats_to_use))
    {
        return; // Can't click on an empty element in the list
    }

    // Get film stats and infos
    FilmStats* fstats = _SA_dynarray_get_element_ptr(film_stats_to_use, *selected_index);
    FilmInfo* info = _SA_dynarray_get_element_ptr(films_infos, fstats->film_id);

    SA_GraphicsRectangle graphics_rectangle_avg_ratings = {.height = GRAPH_HEIGHT, .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD, .top_left_corner_x = LIST_WIDTH + GRAPH_PAD, .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - 2 * GRAPH_HEIGHT - 2 * GRAPH_PAD};
    SA_GraphicsRectangle graphics_rectangle_ratings_count = {.height = GRAPH_HEIGHT, .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD, .top_left_corner_x = LIST_WIDTH + GRAPH_PAD, .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - GRAPH_HEIGHT};
    
    double years[NUMBER_OF_YEARS_LOGGED_IN_STATS] = {0};
    double ratings[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    double ratings_count[NUMBER_OF_YEARS_LOGGED_IN_STATS];
    int ratings_total_count = 0;
    double ratings_sum = 0;
    double years_of_ratings_count = 0.0;

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

     // Clear main area
    SA_graphics_vram_draw_rectangle(window, LIST_WIDTH + 1, HEADER_HEIGHT, WINDOW_WIDTH - LIST_WIDTH - 1, WINDOW_HEIGHT - HEADER_HEIGHT, WINDOW_BACKGROUND);

    // Main area title (movie name)
    SA_graphics_vram_draw_text(window, ((WINDOW_WIDTH + LIST_WIDTH) / 2 - SA_strlen(info->name) * FONT_WIDTH) / 2 + LIST_WIDTH, HEADER_HEIGHT + 20, info->name, WINDOW_FOREGROUND);

    draw_summary(window, ratings_sum / years_of_ratings_count, ratings_total_count);

    // First graph
    SA_graphics_plot_continuous_graph(window, years, ratings, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_avg_ratings, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);
    // Second graph
    SA_graphics_plot_continuous_graph(window, years, ratings_count, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_ratings_count, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);

    // First graph title
    SA_graphics_vram_draw_text(window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(GRAPH1_LABEL)) / 2 + LIST_WIDTH, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 20, GRAPH1_LABEL, WINDOW_FOREGROUND);
    // Second graph title
    SA_graphics_vram_draw_text(window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(GRAPH2_LABEL)) / 2 + LIST_WIDTH, graphics_rectangle_ratings_count.top_left_corner_y + graphics_rectangle_ratings_count.height + 20, GRAPH2_LABEL, WINDOW_FOREGROUND);
    

    SA_graphics_vram_draw_horizontal_line(window, LIST_WIDTH + GRAPH_PAD, WINDOW_WIDTH - GRAPH_PAD, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 30, WINDOW_FOREGROUND_ALTERNATE, 1); // Separator
}