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

    // --- Figura 2D: decaimiento exponencial, con titulo y labels propios ---
    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y[i] = exp(-0.3 * t[i]);
    }

    FigureHandle2D fig2d = plotter_new_figure_2d(&p);
    SeriesHandle s2d = plotter_add_line(&p, fig2d, t, y, n, "exp(-0.3t)");
    plotter_set_line_color(&p, fig2d, s2d, "#0000FF");

    plotter_set_title_2d(&p, fig2d, "Exponential Decay");
    plotter_set_xlabel_2d(&p, fig2d, "time (s)");
    plotter_set_ylabel_2d(&p, fig2d, "amplitude");

    // --- Figura 3D: paraboloide, con titulo y los tres labels propios ---
    int nx = 30, ny = 30;
    int grid_size = nx * ny;
    double *x3 = malloc(grid_size * sizeof(double));
    double *y3 = malloc(grid_size * sizeof(double));
    double *z3 = malloc(grid_size * sizeof(double));

    double range = 3.0;
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

    FigureHandle3D fig3d = plotter_new_figure_3d(&p);
    SeriesHandle s3d = plotter_add_surface(&p, fig3d, x3, y3, z3, nx, ny, "z = x^2 + y^2");
    plotter_set_surface_color(&p, fig3d, s3d, "#FF00FF");

    plotter_set_title_3d(&p, fig3d, "Paraboloid Surface");
    plotter_set_xlabel_3d(&p, fig3d, "x axis");
    plotter_set_ylabel_3d(&p, fig3d, "y axis");
    plotter_set_zlabel_3d(&p, fig3d, "height");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");
    printf("Expected: 2D figure titled 'Exponential Decay' with its own axis labels.\n");
    printf("Expected: 3D figure titled 'Paraboloid Surface' with x/y/z labels,\n");
    printf("          NOT sharing the 2D figure's title or labels.\n");

    free(t);
    free(y);
    free(x3);
    free(y3);
    free(z3);
    plotter_destroy(&p);

    return 0;
}