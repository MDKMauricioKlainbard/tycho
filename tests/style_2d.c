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

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *sin_vals = malloc(n * sizeof(double));
    double *cos_vals = malloc(n * sizeof(double));
    double *noisy_vals = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        sin_vals[i] = sin(t[i]);
        cos_vals[i] = cos(t[i]);
        noisy_vals[i] = sin(t[i]) + 0.1 * sin(t[i] * 15.0);
    }

    FigureHandle fig = plotter_new_figure_2d(&p);

    SeriesHandle s1 = plotter_add_line(&p, fig, t, sin_vals, n, "sin(t)");
    plotter_set_line_color(&p, fig, s1, "#FF0000");
    plotter_set_line_style(&p, fig, s1, "lines");

    SeriesHandle s2 = plotter_add_line(&p, fig, t, cos_vals, n, "cos(t)");
    plotter_set_line_color(&p, fig, s2, "#0000FF");
    plotter_set_line_style(&p, fig, s2, "linespoints");

    SeriesHandle s3 = plotter_add_line(&p, fig, t, noisy_vals, n, "sin(t) + noise");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(t);
    free(sin_vals);
    free(cos_vals);
    free(noisy_vals);
    plotter_destroy(&p);

    return 0;
}