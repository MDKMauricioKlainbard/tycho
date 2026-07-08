#include <tycho/plotter.h>
#include <stdlib.h>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

PlotterStatus plotter_create(Plotter *p)
{
    p->pipe = POPEN("gnuplot -persist", "w");
    if (!p->pipe)
        return PLOTTER_CREATION_FAILED;

    fprintf(p->pipe, "set terminal wxt size 1000,700\n");
    fprintf(p->pipe, "set grid\n");
    fprintf(p->pipe, "set key bottom right\n");
    return PLOTTER_OK;
}

void plotter_set_title(const Plotter *p, const char *title)
{
    fprintf(p->pipe, "set title '%s'\n", title);
}

void plotter_set_labels(const Plotter *p, const char *xlabel, const char *ylabel)
{
    fprintf(p->pipe, "set xlabel '%s'\n", xlabel);
    fprintf(p->pipe, "set ylabel '%s'\n", ylabel);
}

void plotter_plot_xy(const Plotter *p, const double *x, const double *y, int count, const char *series_title)
{
    fprintf(p->pipe, "plot '-' with lines title '%s'\n", series_title);

    for (int i = 0; i < count; i++)
    {
        fprintf(p->pipe, "%f %f\n", x[i], y[i]);
    }

    fprintf(p->pipe, "e\n");
}

void plotter_destroy(Plotter *p)
{
    if (p->pipe) {
        PCLOSE(p->pipe);
        p->pipe = NULL;
    }
}