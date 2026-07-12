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

    PlotterStatus status = plotter_set_terminal(&p, "qt");
    printf("plotter_set_terminal(\"qt\") returned status=%d\n", status);

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y[i] = sin(t[i]);
    }

    FigureHandle2D fig = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig, "This window should use the QT terminal");
    SeriesHandle s = plotter_add_line(&p, fig, t, y, n, "sin(t)");
    plotter_set_line_color(&p, fig, s, "#FF0000");

    plotter_show(&p);

    printf("Window shown using 'qt' terminal.\n");
    printf("Compare against the default 'wxt' terminal behavior you're used to.\n");

    FigureHandle2D fig2 = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig2, "Saved regardless of interactive terminal");
    SeriesHandle s2 = plotter_add_line(&p, fig2, t, y, n, "sin(t)");
    plotter_set_line_color(&p, fig2, s2, "#0000FF");

    plotter_save(&p, "terminal_test_save.png");
    printf("Saved terminal_test_save.png -- should look normal (PNG), unaffected by the qt setting.\n");

    plotter_show(&p);
    printf("Window shown again. Should STILL be using 'qt', not fall back to wxt after save().\n");

    PlotterStatus bad_status = plotter_set_terminal(&p, "this_terminal_name_is_way_too_long_to_fit");
    printf("plotter_set_terminal with an overly long name returned status=%d (expected non-zero/error)\n", bad_status);

    printf("\nClose the gnuplot window(s) manually when done.\n");

    free(t);
    free(y);
    plotter_destroy(&p);

    return 0;
}