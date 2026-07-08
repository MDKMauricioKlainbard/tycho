#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>

typedef struct
{
    FILE *pipe;
} Plotter;

typedef struct
{
    const double *x;
    const double *y;
    int count;
    const char *title;
} PlotSeries;

typedef enum {
    PLOTTER_OK = 0,
    PLOTTER_CREATION_FAILED = 1
} PlotterStatus;

PlotterStatus plotter_create(Plotter *p);
void plotter_set_title(const Plotter *p, const char *title);
void plotter_set_labels(const Plotter *p, const char *xlabel, const char *ylabel);

void plotter_plot_xy(const Plotter *p, const double *x, const double *y, int count, const char *series_title);
void plotter_plot_multi_xy(const Plotter *p, const PlotSeries *series, int n_series);

void plotter_destroy(Plotter *p);

#endif