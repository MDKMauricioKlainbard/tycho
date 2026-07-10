#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>

typedef struct
{
    const double *x;
    const double *y;
    int count;
    const char *title;
} PlotSeries;

typedef struct
{
    const double *x;
    const double *y;
    const double *z;
    int nx; 
    int ny;   
    const char *title;
} Surface3D;

typedef struct
{
    PlotSeries *series;
    int series_count;
    int series_capacity;
} Figure2D;

typedef struct
{
    Surface3D *surfaces;
    int surfaces_count;
    int surfaces_capacity;
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
    int figures_3d_used;       // idem
} Plotter;

typedef int FigureHandle;

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

PlotterStatus plotter_add_line(Plotter *p, FigureHandle fig, const double *x, const double *y, int count, const char *title);
PlotterStatus plotter_add_surface(Plotter *p, FigureHandle fig, const double *x, const double *y, const double *z, int nx, int ny, const char *title);

void plotter_show(Plotter *p);

void plotter_destroy(Plotter *p);

#endif