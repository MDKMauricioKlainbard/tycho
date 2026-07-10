#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

static double paraboloid(double x, double y)
{
    return x * x + y * y;
}

static double saddle(double x, double y)
{
    return x * x - y * y;
}

static void generate_surface(double *x, double *y, double *z, int nx, int ny, double range,
                             double (*f)(double, double))
{
    for (int row = 0; row < ny; row++)
    {
        double yv = -range + (2.0 * range) * row / (ny - 1);
        for (int col = 0; col < nx; col++)
        {
            double xv = -range + (2.0 * range) * col / (nx - 1);
            int idx = row * nx + col;
            x[idx] = xv;
            y[idx] = yv;
            z[idx] = f(xv, yv);
        }
    }
}

int main()
{
    Plotter p;
    if (plotter_create(&p) != PLOTTER_OK)
    {
        printf("Could not create the plotter.\n");
        return 1;
    }

    int nx = 30, ny = 30;
    int grid_size = nx * ny;

    double *x1 = malloc(grid_size * sizeof(double));
    double *y1 = malloc(grid_size * sizeof(double));
    double *z1 = malloc(grid_size * sizeof(double));
    generate_surface(x1, y1, z1, nx, ny, 3.0, paraboloid);

    double *x2 = malloc(grid_size * sizeof(double));
    double *y2 = malloc(grid_size * sizeof(double));
    double *z2 = malloc(grid_size * sizeof(double));
    generate_surface(x2, y2, z2, nx, ny, 3.0, saddle);


    FigureHandle fig = plotter_new_figure_3d(&p);

    SeriesHandle s1 = plotter_add_surface(&p, fig, x1, y1, z1, nx, ny, "Paraboloid");
    plotter_set_surface_color(&p, fig, s1, "#0000FF");
    plotter_set_surface_style(&p, fig, s1, "lines");

    SeriesHandle s2 = plotter_add_surface(&p, fig, x2, y2, z2, nx, ny, "Saddle");
    plotter_set_surface_color(&p, fig, s2, "#FF00FF");
    plotter_set_surface_style(&p, fig, s2, "pm3d");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(x1);
    free(y1);
    free(z1);
    free(x2);
    free(y2);
    free(z2);
    plotter_destroy(&p);

    return 0;
}