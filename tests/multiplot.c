#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

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

static double paraboloid(double x, double y)
{
    return x * x + y * y;
}

static double saddle(double x, double y)
{
    return x * x - y * y;
}

static double ripple(double x, double y)
{
    double r = sqrt(x * x + y * y) + 1e-6;
    return sin(r) / r;
}

static double cone(double x, double y)
{
    return sqrt(x * x + y * y);
}

int main()
{
    Plotter p;
    if (plotter_create(&p) != PLOTTER_OK)
    {
        printf("Could not create the plotter.\n");
        return 1;
    }

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *sin_vals = malloc(n * sizeof(double));
    double *cos_vals = malloc(n * sizeof(double));
    double *tan_vals = malloc(n * sizeof(double));
    double *exp_vals = malloc(n * sizeof(double));
    double *log_vals = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        sin_vals[i] = sin(t[i]);
        cos_vals[i] = cos(t[i]);
        tan_vals[i] = tan(t[i] * 0.3);
        exp_vals[i] = exp(-t[i] * 0.2);
        log_vals[i] = log(t[i] + 1.0);
    }

    FigureHandle2D fig2d_1 = plotter_new_figure_2d(&p);
    plotter_add_line(&p, fig2d_1, t, sin_vals, n, "sin(t)");
    plotter_add_line(&p, fig2d_1, t, cos_vals, n, "cos(t)");
    plotter_add_line(&p, fig2d_1, t, tan_vals, n, "tan(0.3t)");

    FigureHandle2D fig2d_2 = plotter_new_figure_2d(&p);
    plotter_add_line(&p, fig2d_2, t, exp_vals, n, "exp(-0.2t)");
    plotter_add_line(&p, fig2d_2, t, log_vals, n, "log(t+1)");

    int nx = 30, ny = 30;
    int grid_size = nx * ny;

    double *x1 = malloc(grid_size * sizeof(double));
    double *y1 = malloc(grid_size * sizeof(double));
    double *z1a = malloc(grid_size * sizeof(double));
    double *z1b = malloc(grid_size * sizeof(double));
    generate_surface(x1, y1, z1a, nx, ny, 3.0, paraboloid);
    generate_surface(x1, y1, z1b, nx, ny, 3.0, saddle);

    double *x2 = malloc(grid_size * sizeof(double));
    double *y2 = malloc(grid_size * sizeof(double));
    double *z2a = malloc(grid_size * sizeof(double));
    double *z2b = malloc(grid_size * sizeof(double));
    generate_surface(x2, y2, z2a, nx, ny, 6.0, ripple);
    generate_surface(x2, y2, z2b, nx, ny, 3.0, cone);

    double *x3 = malloc(grid_size * sizeof(double));
    double *y3 = malloc(grid_size * sizeof(double));
    double *z3a = malloc(grid_size * sizeof(double));
    double *z3b = malloc(grid_size * sizeof(double));
    generate_surface(x3, y3, z3a, nx, ny, 3.0, paraboloid);
    generate_surface(x3, y3, z3b, nx, ny, 3.0, cone);

    FigureHandle3D fig3d_1 = plotter_new_figure_3d(&p);
    plotter_add_surface(&p, fig3d_1, x1, y1, z1a, nx, ny, "Paraboloid");
    plotter_add_surface(&p, fig3d_1, x1, y1, z1b, nx, ny, "Saddle");

    FigureHandle3D fig3d_2 = plotter_new_figure_3d(&p);
    plotter_add_surface(&p, fig3d_2, x2, y2, z2a, nx, ny, "Ripple");
    plotter_add_surface(&p, fig3d_2, x2, y2, z2b, nx, ny, "Cone");

    FigureHandle3D fig3d_3 = plotter_new_figure_3d(&p);
    plotter_add_surface(&p, fig3d_3, x3, y3, z3a, nx, ny, "Paraboloid 2");
    plotter_add_surface(&p, fig3d_3, x3, y3, z3b, nx, ny, "Cone 2");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(t); free(sin_vals); free(cos_vals); free(tan_vals); free(exp_vals); free(log_vals);
    free(x1); free(y1); free(z1a); free(z1b);
    free(x2); free(y2); free(z2a); free(z2b);
    free(x3); free(y3); free(z3a); free(z3b);
    plotter_destroy(&p);

    return 0;
}