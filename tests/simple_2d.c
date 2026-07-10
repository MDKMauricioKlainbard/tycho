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
    double *y = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y[i] = sin(t[i]);
    }

    FigureHandle2D fig2d = plotter_new_figure_2d(&p);
    plotter_add_line(&p, fig2d, t, y, n, "sin(t)");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(t);
    free(y);
    plotter_destroy(&p);

    return 0;
}