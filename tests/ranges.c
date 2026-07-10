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

    int n = 200;
    double *t = malloc(n * sizeof(double));
    double *y = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y[i] = sin(t[i]);
    }

    FigureHandle fig2d = plotter_new_figure_2d(&p);
    SeriesHandle s2d = plotter_add_line(&p, fig2d, t, y, n, "sin(t)");
    plotter_set_line_color(&p, fig2d, s2d, "#FF0000");

    plotter_set_xrange(&p, fig2d, 5.0, 10.0);
    plotter_set_yrange(&p, fig2d, -0.5, 0.5);

    int nx = 40, ny = 40;
    int grid_size = nx * ny;
    double *x3 = malloc(grid_size * sizeof(double));
    double *y3 = malloc(grid_size * sizeof(double));
    double *z3 = malloc(grid_size * sizeof(double));

    double range = 5.0;
    for (int row = 0; row < ny; row++)
    {
        double yv = -range + (2.0 * range) * row / (ny - 1);
        for (int col = 0; col < nx; col++)
        {
            double xv = -range + (2.0 * range) * col / (nx - 1);
            int idx = row * nx + col;
            x3[idx] = xv;
            y3[idx] = yv;
            z3[idx] = paraboloid(xv, yv);
        }
    }

    FigureHandle fig3d = plotter_new_figure_3d(&p);
    SeriesHandle s3d = plotter_add_surface(&p, fig3d, x3, y3, z3, nx, ny, "Paraboloid");
    plotter_set_surface_color(&p, fig3d, s3d, "#0000FF");

    plotter_set_zrange(&p, fig3d, 0.0, 20.0);

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");
    printf("Expected: 2D figure shows only t in [5,10], y in [-0.5,0.5].\n");
    printf("Expected: 3D figure is clipped at z=20 (flat top), instead of the full bowl up to z=50.\n");

    free(t);
    free(y);
    free(x3);
    free(y3);
    free(z3);
    plotter_destroy(&p);

    return 0;
}