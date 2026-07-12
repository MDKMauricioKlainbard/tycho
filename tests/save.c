#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

static double paraboloid(double x, double y)
{
    return x * x + y * y;
}

static void fill_grid(double *x, double *y, double *z, int nx, int ny, double range,
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

    int n = 100;
    double *t = malloc(n * sizeof(double));
    double *y1 = malloc(n * sizeof(double));
    double *y2 = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y1[i] = sin(t[i]);
        y2[i] = cos(t[i]);
    }

    int nx = 30, ny = 30;
    int grid_size = nx * ny;
    double *x3 = malloc(grid_size * sizeof(double));
    double *y3 = malloc(grid_size * sizeof(double));
    double *z3 = malloc(grid_size * sizeof(double));
    fill_grid(x3, y3, z3, nx, ny, 3.0, paraboloid);

    // ---------------------------------------------------------------
    // Case 1: save-only, single 2D figure, no window shown at all
    // ---------------------------------------------------------------
    FigureHandle2D fig1 = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig1, "Case 1: save only, never shown");
    SeriesHandle s1 = plotter_add_line(&p, fig1, t, y1, n, "sin(t)");
    plotter_set_line_color(&p, fig1, s1, "#FF0000");

    plotter_save(&p, "case1_save_only.png");
    printf("Case 1 done: case1_save_only.png (should contain ONLY sin(t))\n");
    // Note: figures are still pending here, since save() doesn't clear them.
    // We explicitly show() now to reset state before Case 2, so the two
    // cases don't bleed into each other.
    plotter_show(&p);

    // ---------------------------------------------------------------
    // Case 2: save + show from the SAME pending figures (2D and 3D together)
    // ---------------------------------------------------------------
    FigureHandle2D fig2 = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig2, "Case 2: sin and cos");
    SeriesHandle s2a = plotter_add_line(&p, fig2, t, y1, n, "sin(t)");
    plotter_set_line_color(&p, fig2, s2a, "#FF0000");
    SeriesHandle s2b = plotter_add_line(&p, fig2, t, y2, n, "cos(t)");
    plotter_set_line_color(&p, fig2, s2b, "#0000FF");

    FigureHandle3D fig3 = plotter_new_figure_3d(&p);
    plotter_set_title_3d(&p, fig3, "Case 2: paraboloid");
    SeriesHandle s3 = plotter_add_surface(&p, fig3, x3, y3, z3, nx, ny, "Paraboloid");
    plotter_set_surface_color(&p, fig3, s3, "#00AA00");

    plotter_save(&p, "case2_combined.png");
    printf("Case 2 saved: case2_combined.png (should have sin+cos on top, paraboloid below)\n");

    plotter_show(&p);
    printf("Case 2 also shown in a window. It should look IDENTICAL to case2_combined.png.\n");

    // ---------------------------------------------------------------
    // Case 3: after show() cleared everything, save() should produce
    // an EMPTY plot (or nothing), not leftovers from Case 2.
    // ---------------------------------------------------------------
    plotter_save(&p, "case3_should_be_empty.png");
    printf("Case 3 saved: case3_should_be_empty.png (should be BLANK -- no leftover figures from Case 2)\n");

    // ---------------------------------------------------------------
    // Case 4: PDF output, to confirm the extension-based format switch
    // ---------------------------------------------------------------
    FigureHandle2D fig4 = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig4, "Case 4: PDF output");
    SeriesHandle s4 = plotter_add_line(&p, fig4, t, y2, n, "cos(t)");
    plotter_set_line_color(&p, fig4, s4, "#AA00AA");

    plotter_save(&p, "case4_output.pdf");
    printf("Case 4 saved: case4_output.pdf (should contain only cos(t))\n");

    plotter_show(&p);
    printf("Case 4 also shown in a window.\n");

    printf("\nAll cases done. Close gnuplot windows manually when done inspecting them.\n");
    printf("Files to check in the working directory:\n");
    printf("  case1_save_only.png\n");
    printf("  case2_combined.png\n");
    printf("  case3_should_be_empty.png\n");
    printf("  case4_output.pdf\n");

    free(t);
    free(y1);
    free(y2);
    free(x3);
    free(y3);
    free(z3);
    plotter_destroy(&p);

    return 0;
}