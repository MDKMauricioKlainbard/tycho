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
    double *x = malloc(n * sizeof(double));
    double *y_exp = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        x[i] = i * 0.1;
        y_exp[i] = exp(x[i]);
    }

    // --- Figura 1: y = exp(x) en escala LINEAL (curva se dispara verticalmente) ---
    FigureHandle2D fig_linear = plotter_new_figure_2d(&p);
    SeriesHandle s1 = plotter_add_line(&p, fig_linear, x, y_exp, n, "exp(x)");
    plotter_set_line_color(&p, fig_linear, s1, "#FF0000");
    plotter_set_title_2d(&p, fig_linear, "Linear scale");
    plotter_set_xlabel_2d(&p, fig_linear, "x");
    plotter_set_ylabel_2d(&p, fig_linear, "y");

    // --- Figura 2: y = exp(x) en escala LOGARITMICA en Y (deberia verse como una recta) ---
    FigureHandle2D fig_log = plotter_new_figure_2d(&p);
    SeriesHandle s2 = plotter_add_line(&p, fig_log, x, y_exp, n, "exp(x)");
    plotter_set_line_color(&p, fig_log, s2, "#0000FF");
    plotter_set_title_2d(&p, fig_log, "Log scale (Y)");
    plotter_set_xlabel_2d(&p, fig_log, "x");
    plotter_set_ylabel_2d(&p, fig_log, "y (log)");
    plotter_set_logscale_y_2d(&p, fig_log, 1);

    // --- Figura 3: misma curva, escala lineal de nuevo (verifica que no hereda logscale de fig_log) ---
    FigureHandle2D fig_linear2 = plotter_new_figure_2d(&p);
    SeriesHandle s3 = plotter_add_line(&p, fig_linear2, x, y_exp, n, "exp(x)");
    plotter_set_line_color(&p, fig_linear2, s3, "#00AA00");
    plotter_set_title_2d(&p, fig_linear2, "Linear scale again");
    plotter_set_xlabel_2d(&p, fig_linear2, "x");
    plotter_set_ylabel_2d(&p, fig_linear2, "y");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");
    printf("Expected: figure 1 (red) shoots up steeply near the right edge.\n");
    printf("Expected: figure 2 (blue) looks like a STRAIGHT LINE (log scale linearizes exp).\n");
    printf("Expected: figure 3 (green) looks like figure 1 again, NOT like figure 2.\n");
    printf("If figure 3 looks like figure 2, logscale state is leaking between figures.\n");

    free(x);
    free(y_exp);
    plotter_destroy(&p);

    return 0;
}