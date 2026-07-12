#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

int main()
{
    Plotter p1, p2;

    if (plotter_create(&p1) != PLOTTER_OK)
    {
        printf("Could not create plotter 1.\n");
        return 1;
    }

    if (plotter_create(&p2) != PLOTTER_OK)
    {
        printf("Could not create plotter 2.\n");
        return 1;
    }

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y_sin = malloc(n * sizeof(double));
    double *y_cos = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y_sin[i] = sin(t[i]);
        y_cos[i] = cos(t[i]);
    }

    plotter_set_window_size(&p1, 700, 500);
    plotter_set_grid(&p1, 0);

    FigureHandle2D fig1 = plotter_new_figure_2d(&p1);
    plotter_set_title_2d(&p1, fig1, "Plotter 1: sin(t), no grid, small window");
    SeriesHandle s1 = plotter_add_line(&p1, fig1, t, y_sin, n, "sin(t)");
    plotter_set_line_color(&p1, fig1, s1, "#FF0000");

    plotter_set_window_size(&p2, 1400, 900);
    plotter_set_grid(&p2, 1);

    FigureHandle2D fig2 = plotter_new_figure_2d(&p2);
    plotter_set_title_2d(&p2, fig2, "Plotter 2: cos(t), grid, large window");
    SeriesHandle s2 = plotter_add_line(&p2, fig2, t, y_cos, n, "cos(t)");
    plotter_set_line_color(&p2, fig2, s2, "#0000FF");

    plotter_show(&p1);
    plotter_show(&p2);

    printf("Two independent windows should now be open:\n");
    printf("  Plotter 1: small, red sin(t), NO grid\n");
    printf("  Plotter 2: large, blue cos(t), WITH grid\n");
    printf("If either window shows the wrong curve, size, or grid setting,\n");
    printf("something is leaking between the two Plotter instances.\n\n");

    FigureHandle2D fig1b = plotter_new_figure_2d(&p1);
    plotter_set_title_2d(&p1, fig1b, "Plotter 1, second plot: cos(t) now");
    SeriesHandle s1b = plotter_add_line(&p1, fig1b, t, y_cos, n, "cos(t)");
    plotter_set_line_color(&p1, fig1b, s1b, "#00AA00");

    plotter_save(&p1, "plotter1_second.png");
    printf("Saved plotter1_second.png. Plotter 2 should NOT have been affected by this save.\n");

    plotter_show(&p1);

    free(t);
    free(y_sin);
    free(y_cos);

    printf("\nClose both gnuplot windows manually when done inspecting them.\n");

    plotter_destroy(&p1);
    plotter_destroy(&p2);

    return 0;
}