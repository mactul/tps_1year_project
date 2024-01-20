#include "film_gui/gui.h"
#include "src/data_structs.h"

#define AVG_NOTE_LABEL "Average note : "
#define RATING_COUNT_LABEL "Number of rating over the last 10 years : "
#define FILM_ID_LABEL "Film ID : "

#define GRAPH1_LABEL "Average note over years"
#define GRAPH2_LABEL "Number of ratings per year"

#define AVG_NOTE_Y_OFFSET 50
#define RATING_COUNT_Y_OFFSET 80
#define FILM_ID_Y_OFFSET 110

/// @brief Draw the number of ratings and average rating of the movie with stars in a window
/// @param window The window in which to draw
/// @param avg_note Average rating
/// @param ratings_total_count Number of ratings
/// @param film_id Id of the film for which to draw the summary
static void draw_summary(SA_GraphicsWindow* window, double avg_note, uint32_t ratings_total_count, uint32_t film_id)
{
    char rating_count_text[50] = RATING_COUNT_LABEL;
    char film_id_text[50] = FILM_ID_LABEL;
    SA_uint64_to_str(rating_count_text + SA_strlen(rating_count_text), ratings_total_count);
    SA_uint64_to_str(film_id_text + SA_strlen(film_id_text), film_id);
    // drawing the rating count
    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + RATING_COUNT_Y_OFFSET, rating_count_text, WINDOW_FOREGROUND);

    // drawing the AVG_NOTE_LABEL
    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + AVG_NOTE_Y_OFFSET, AVG_NOTE_LABEL, WINDOW_FOREGROUND);

    // drawing the film_id
    SA_graphics_vram_draw_text(window, LIST_WIDTH + MARGIN_DESC_TEXT, HEADER_HEIGHT + FILM_ID_Y_OFFSET, film_id_text, WINDOW_FOREGROUND);

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
/// @param function_arguments Structure of various arguments
/// @param mouse_y Vertical position of the cursor to find the movie index
void draw_movie_info(FunctionArguments* function_arguments, uint32_t mouse_y)
{
    int movie_pixel_offset = mouse_y - HEADER_HEIGHT - SEARCH_BAR_HEIGHT + *(function_arguments->pixel_offset);
    *(function_arguments->selected_index) = movie_pixel_offset / LIST_ENTRY_HEIGHT;

    SA_DynamicArray* film_stats_to_use = *(function_arguments->display_query) == SA_TRUE ? *(function_arguments->film_stats_filtered) : function_arguments->films_stats;

    if ((uint64_t) *(function_arguments->selected_index) >= SA_dynarray_size(film_stats_to_use))
    {
        return; // Can't click on an empty element in the list
    }

    // Get film stats and infos
    FilmStats* fstats = _SA_dynarray_get_element_ptr(film_stats_to_use, *(function_arguments->selected_index));
    FilmInfo* info = _SA_dynarray_get_element_ptr(function_arguments->films_infos, fstats->film_id);

    SA_GraphicsRectangle graphics_rectangle_avg_ratings = {
        .height = GRAPH_HEIGHT,
        .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD,
        .top_left_corner_x = LIST_WIDTH + GRAPH_PAD,
        .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - 2 * GRAPH_HEIGHT - 2 * GRAPH_PAD
    };
    SA_GraphicsRectangle graphics_rectangle_ratings_count = {
        .height = GRAPH_HEIGHT,
        .width = WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD,
        .top_left_corner_x = LIST_WIDTH + GRAPH_PAD,
        .top_left_corner_y = WINDOW_HEIGHT - GRAPH_PAD - GRAPH_HEIGHT
    };
    
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
    SA_graphics_vram_draw_rectangle(function_arguments->window, LIST_WIDTH + 1, HEADER_HEIGHT, WINDOW_WIDTH - LIST_WIDTH - 1, WINDOW_HEIGHT - HEADER_HEIGHT, WINDOW_BACKGROUND);

    // Main area title (movie name)
    SA_graphics_vram_draw_text(function_arguments->window, ((WINDOW_WIDTH + LIST_WIDTH) / 2 - SA_strlen(info->name) * FONT_WIDTH) / 2 + LIST_WIDTH, HEADER_HEIGHT + 20, info->name, WINDOW_FOREGROUND);

    draw_summary(function_arguments->window, ratings_sum / years_of_ratings_count, ratings_total_count, fstats->film_id);

    // First graph
    SA_graphics_plot_continuous_graph(function_arguments->window, years, ratings, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_avg_ratings, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);
    // Second graph
    SA_graphics_plot_continuous_graph(function_arguments->window, years, ratings_count, NUMBER_OF_YEARS_LOGGED_IN_STATS, &graphics_rectangle_ratings_count, 0x0, GRAPH_PLOT_COLOR, WINDOW_BACKGROUND);

    // First graph title
    SA_graphics_vram_draw_text(function_arguments->window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(GRAPH1_LABEL)) / 2 + LIST_WIDTH, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 20, GRAPH1_LABEL, WINDOW_FOREGROUND);
    // Second graph title
    SA_graphics_vram_draw_text(function_arguments->window, (WINDOW_WIDTH - LIST_WIDTH - 2 * GRAPH_PAD - SA_strlen(GRAPH2_LABEL)) / 2 + LIST_WIDTH, graphics_rectangle_ratings_count.top_left_corner_y + graphics_rectangle_ratings_count.height + 20, GRAPH2_LABEL, WINDOW_FOREGROUND);
    

    SA_graphics_vram_draw_horizontal_line(function_arguments->window, LIST_WIDTH + GRAPH_PAD, WINDOW_WIDTH - GRAPH_PAD, graphics_rectangle_avg_ratings.top_left_corner_y + graphics_rectangle_avg_ratings.height + 30, WINDOW_FOREGROUND_ALTERNATE, 1); // Separator
}