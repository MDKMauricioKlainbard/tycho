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

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y1 = malloc(n * sizeof(double));
    double *y2 = malloc(n * sizeof(double));
    double *y3 = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y1[i] = sin(t[i]);
        y2[i] = sin(t[i]) + 1.5;
        y3[i] = sin(t[i]) + 3.0;
    }

    FigureHandle2D fig2d = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig2d, "Line width and dash type");
    plotter_set_xlabel_2d(&p, fig2d, "t");
    plotter_set_ylabel_2d(&p, fig2d, "y");

    SeriesHandle s1 = plotter_add_line(&p, fig2d, t, y1, n, "thin, solid");
    plotter_set_line_color(&p, fig2d, s1, "#FF0000");
    plotter_set_line_width(&p, fig2d, s1, 1.0);
    plotter_set_line_dashtype(&p, fig2d, s1, 1);

    SeriesHandle s2 = plotter_add_line(&p, fig2d, t, y2, n, "thick, solid");
    plotter_set_line_color(&p, fig2d, s2, "#0000FF");
    plotter_set_line_width(&p, fig2d, s2, 4.0);
    plotter_set_line_dashtype(&p, fig2d, s2, 1);

    SeriesHandle s3 = plotter_add_line(&p, fig2d, t, y3, n, "medium, dashed");
    plotter_set_line_color(&p, fig2d, s3, "#00AA00");
    plotter_set_line_width(&p, fig2d, s3, 2.5);
    plotter_set_line_dashtype(&p, fig2d, s3, 2);

    int nx = 25, ny = 25;
    int grid_size = nx * ny;

    double *x3d_a = malloc(grid_size * sizeof(double));
    double *y3d_a = malloc(grid_size * sizeof(double));
    double *z3d_a = malloc(grid_size * sizeof(double));

    double *x3d_b = malloc(grid_size * sizeof(double));
    double *y3d_b = malloc(grid_size * sizeof(double));
    double *z3d_b = malloc(grid_size * sizeof(double));

    double range = 3.0;
    for (int row = 0; row < ny; row++)
    {
        double yv = -range + (2.0 * range) * row / (ny - 1);
        for (int col = 0; col < nx; col++)
        {
            double xv = -range + (2.0 * range) * col / (nx - 1);
            int idx = row * nx + col;

            x3d_a[idx] = xv;
            y3d_a[idx] = yv;
            z3d_a[idx] = paraboloid(xv, yv);

            x3d_b[idx] = xv;
            y3d_b[idx] = yv;
            z3d_b[idx] = paraboloid(xv, yv) + 15.0;  
        }
    }

    FigureHandle3D fig3d = plotter_new_figure_3d(&p);
    plotter_set_title_3d(&p, fig3d, "Surface line width and dash type");
    plotter_set_xlabel_3d(&p, fig3d, "x");
    plotter_set_ylabel_3d(&p, fig3d, "y");
    plotter_set_zlabel_3d(&p, fig3d, "z");

    SeriesHandle sa = plotter_add_surface(&p, fig3d, x3d_a, y3d_a, z3d_a, nx, ny, "thin mesh");
    plotter_set_surface_color(&p, fig3d, sa, "#FF0000");
    plotter_set_surface_width(&p, fig3d, sa, 1.0);

    SeriesHandle sb = plotter_add_surface(&p, fig3d, x3d_b, y3d_b, z3d_b, nx, ny, "thick dashed mesh");
    plotter_set_surface_color(&p, fig3d, sb, "#0000FF");
    plotter_set_surface_width(&p, fig3d, sb, 3.0);
    plotter_set_surface_dashtype(&p, fig3d, sb, 2);

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");
    printf("Expected 2D: red thin solid, blue thick solid, green medium dashed.\n");
    printf("Expected 3D: red thin mesh (bottom), blue thick dashed mesh (top, offset).\n");

    free(t); free(y1); free(y2); free(y3);
    free(x3d_a); free(y3d_a); free(z3d_a);
    free(x3d_b); free(y3d_b); free(z3d_b);
    plotter_destroy(&p);

    return 0;
}