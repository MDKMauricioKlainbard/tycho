#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>

typedef struct
{
    const double *x;
    const double *y;
    int count;
    const char *title;
    const char *color;
    const char *style;
} PlotSeries;

typedef struct
{
    const double *x;
    const double *y;
    const double *z;
    int nx;
    int ny;
    const char *title;
    const char *color;
    const char *style;
} Surface3D;

typedef struct
{
    PlotSeries *series;
    int series_count;
    int series_capacity;

    int has_xrange;
    double xmin, xmax;

    int has_yrange;
    double ymin, ymax;
} Figure2D;

typedef struct
{
    Surface3D *surfaces;
    int surfaces_count;
    int surfaces_capacity;

    int has_xrange;
    double xmin, xmax;

    int has_yrange;
    double ymin, ymax;

    int has_zrange;
    double zmin, zmax;
} Figure3D;

typedef struct
{
    FILE *pipe;

    Figure2D *figures_2d;
    int figures_2d_count;
    int figures_2d_capacity;
    int figures_2d_used;

    Figure3D *figures_3d;
    int figures_3d_count;
    int figures_3d_capacity;
    int figures_3d_used;
} Plotter;

typedef int FigureHandle;
typedef int SeriesHandle;

typedef enum
{
    PLOTTER_OK = 0,
    PLOTTER_CREATION_FAILED = 1,
    PLOTTER_ALLOC_FAILED = 2,
    PLOTTER_INVALID_HANDLE = 3
} PlotterStatus;

PlotterStatus plotter_create(Plotter *p);

FigureHandle plotter_new_figure_2d(Plotter *p);
FigureHandle plotter_new_figure_3d(Plotter *p);

SeriesHandle plotter_add_line(Plotter *p, FigureHandle fig, const double *x, const double *y, int count, const char *title);
SeriesHandle plotter_add_surface(Plotter *p, FigureHandle fig, const double *x, const double *y, const double *z, int nx, int ny, const char *title);

PlotterStatus plotter_set_line_color(Plotter *p, FigureHandle fig, SeriesHandle series, const char *color);
PlotterStatus plotter_set_line_style(Plotter *p, FigureHandle fig, SeriesHandle series, const char *style);

PlotterStatus plotter_set_surface_color(Plotter *p, FigureHandle fig, SeriesHandle surface, const char *color);
PlotterStatus plotter_set_surface_style(Plotter *p, FigureHandle fig, SeriesHandle surface, const char *style);

PlotterStatus plotter_set_xrange(Plotter *p, FigureHandle fig, double xmin, double xmax);
PlotterStatus plotter_set_yrange(Plotter *p, FigureHandle fig, double ymin, double ymax);
PlotterStatus plotter_set_zrange(Plotter *p, FigureHandle fig, double zmin, double zmax);

void plotter_show(Plotter *p);
void plotter_destroy(Plotter *p);

#endif