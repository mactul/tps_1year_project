#include <SA/SA.h>
#include <stdlib.h>
#include <math.h>

#define SIGN(x) ((x) < 0 ? -1: 1)
#define ABS(x) ((x) < 0 ? -(x): (x))

#define STAR_EDGE_SIZE 12

/// @brief Compare integers (used by qsort)
/// @param e1 Pointer to the first int
/// @param e2 Pointer to the second int
/// @return > 0 if the first int is higher, < 0 if the second one is higher, 0 if they are equal
static inline int cmp(const void* e1, const void* e2)
{
    return *((int*)e1) - *((int*)e2);
}

/// @brief Calculate the width of a star drawn in a window
/// @return Size of the star from left to right
static inline double get_star_size()
{
    return 2 * STAR_EDGE_SIZE * (cos(2 * M_PI / 5) + 1);
}

/// @brief Fill a shape described by an array of vertices by a specified number of pixel columns from left to right inside a window
/// @param window The window in which to draw the filled shape
/// @param x Array of horizontal position of vertices
/// @param y Array of vertical position of vertices
/// @param nb_vertices Number of vertices (length of both x and y)
/// @param color Fill color
/// @param max_x Number of pixel columns to fill inside the polygon
void fill_polygon_before_max_x(SA_GraphicsWindow* window, const int *x, const int *y, int nb_vertices, uint32_t color, int max_x)
{
    if(nb_vertices < 3)
        return;
    
    int cut_in_line[nb_vertices];
    int xmin = 0;
    int xmax = 0;

    for(int i=1 ; i<nb_vertices ; i++)
    {
        if(x[i] < xmin)
        {
            xmin = x[i];
        }
        if(x[i] > xmax && x[i] <= max_x)
        {
            xmax = x[i];
        }
    }
    for(int i = xmin; i <= xmax; i++)
    {
        int j;
        int nb_cut = 0;
        for(j = 0; j < nb_vertices; j++)
        {
            int next_j = (j+1) % nb_vertices;
            if((x[j]<=i && x[next_j]>=i) || (x[j]>=i && x[next_j]<=i))
            {
                int dx = ABS(x[j]-x[next_j]);
                if(dx)
                {
                    int dy = y[next_j]-y[j];
                    int a = i - x[j] + SIGN(i - x[j]) / 2;
                    cut_in_line[nb_cut] = y[j] + ABS(a)*dy/dx;
                    nb_cut++;
                }
            }
        }
        qsort(cut_in_line, 0, nb_cut-1, cmp);
        j = 0;
        while(j < nb_cut-2 && cut_in_line[j] == cut_in_line[j+1])
        {
            j++;
        }
        while(j < nb_cut)
        {
            if(j == nb_cut-1)
            {
                SA_graphics_vram_draw_vertical_line(window, i, cut_in_line[j-1]+1, cut_in_line[j], color, 1);
            }
            else
            {
                int dy = 1;
                while(j+dy < nb_cut-1 && cut_in_line[j+dy] == cut_in_line[j+dy+1])
                {
                    dy++;
                }
                SA_graphics_vram_draw_vertical_line(window, i, cut_in_line[j], cut_in_line[j+dy], color, 1);
                j += dy;
            }
            j++;
        }
    }
    return;
}

/// @brief Draw a line in a window from a polar coordinate system
/// @param window The window in which to draw the line
/// @param x Horizontal position of the line, will be modified to be the end of the line after execution
/// @param y Vertical position of the line, will be modified to be the end of the line after execution
/// @param size Length of the line
/// @param angle Angle of the line (0 means to the right, reversed as our y axis is inverted)
void draw_vec(SA_GraphicsWindow* window, double* x, double* y, double size, double angle)
{
    double old_x = *x;
    double old_y = *y;
    *x = *x + size * cos(angle);
    *y = *y - size * sin(angle);
    SA_graphics_vram_draw_line(window, old_x+0.5, old_y+0.5, *x+0.5, *y+0.5, 0xffd000, 1);
}

/// @brief Draw a star in a graphical window
/// @param window The window in which to draw the star
/// @param x Horizontal position of the star
/// @param y Vertical position of the star
/// @param percent_filling Percentage of the star that will be filled (from left to right), e.g. to show 0.3 stars
void draw_star(SA_GraphicsWindow* window, double x, double y, float percent_filling)
{
    double angle = 0;
    int x_array[11] = {x};
    int y_array[11] = {y};
    int j = 1;
    for(int i = 0; i < 5; i++)
    {
        draw_vec(window, &x, &y, STAR_EDGE_SIZE, angle);
        x_array[j] = x + 0.5;
        y_array[j] = y + 0.5;
        j++;
        angle += 2 * M_PI / 5;
        draw_vec(window, &x, &y, STAR_EDGE_SIZE, angle);
        x_array[j] = x + 0.5;
        y_array[j] = y + 0.5;
        j++;
        angle -= 4 * M_PI / 5;
    }

    fill_polygon_before_max_x(window, x_array, y_array, 11, 0xffff00, x + percent_filling * get_star_size() + 0.5);
}