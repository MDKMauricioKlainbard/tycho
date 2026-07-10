#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

static double paraboloid(double x, double y)
{
    return x * x + y * y;
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

    double *x = malloc(grid_size * sizeof(double));
    double *y = malloc(grid_size * sizeof(double));
    double *z = malloc(grid_size * sizeof(double));

    double range = 3.0;
    for (int row = 0; row < ny; row++)
    {
        double yv = -range + (2.0 * range) * row / (ny - 1);
        for (int col = 0; col < nx; col++)
        {
            double xv = -range + (2.0 * range) * col / (nx - 1);
            int idx = row * nx + col;
            x[idx] = xv;
            y[idx] = yv;
            z[idx] = paraboloid(xv, yv);
        }
    }

    FigureHandle fig3d = plotter_new_figure_3d(&p);
    plotter_add_surface(&p, fig3d, x, y, z, nx, ny, "Paraboloid");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(x);
    free(y);
    free(z);
    plotter_destroy(&p);

    return 0;
}