#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

int main()
{
    Plotter p;
    if (plotter_create(&p) != PLOTTER_OK)
    {
        printf("Could not create the plotter.\n");
        return 1;
    }

    plotter_set_window_size(&p, 800, 500);
    plotter_set_grid(&p, 0);                     
    plotter_set_legend_position(&p, "top left");   

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y1 = malloc(n * sizeof(double));
    double *y2 = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y1[i] = sin(t[i]);
        y2[i] = cos(t[i]);
    }

    FigureHandle2D fig = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig, "No grid, legend top-left, small window");

    SeriesHandle s1 = plotter_add_line(&p, fig, t, y1, n, "sin(t)");
    plotter_set_line_color(&p, fig, s1, "#FF0000");

    SeriesHandle s2 = plotter_add_line(&p, fig, t, y2, n, "cos(t)");
    plotter_set_line_color(&p, fig, s2, "#0000FF");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");
    printf("Expected: small window (800x500), no grid lines, legend in the top-left corner.\n");

    free(t);
    free(y1);
    free(y2);
    plotter_destroy(&p);

    return 0;
}