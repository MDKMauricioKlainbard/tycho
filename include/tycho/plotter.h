#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>

/**
 * @brief A single 2D line to be drawn inside a Figure2D.
 *
 * Populated internally by plotter_add_line(); fields other than the raw
 * data pointers are meant to be modified through the plotter_set_line_*
 * functions, not set directly.
 */
typedef struct
{
    const double *x;      /**< X coordinates. Must have at least `count` elements. */
    const double *y;      /**< Y coordinates. Must have at least `count` elements. */
    int count;             /**< Number of points in x/y. */
    const char *title;    /**< Legend entry for this line. */
    const char *color;    /**< Gnuplot RGB color string (e.g. "#FF0000"), or NULL for automatic. */
    const char *style;    /**< Gnuplot plot style (e.g. "lines", "points", "linespoints"). */
    double line_width;     /**< Line width, or 0 for gnuplot's default. */
    int dash_type;         /**< Gnuplot dashtype index (1 = solid, 2 = dashed, ...), or 0 for default. */
} PlotSeries;

/**
 * @brief A single 3D surface to be drawn inside a Figure3D.
 *
 * The data must represent a regular nx-by-ny grid, flattened row-major
 * (index = row * nx + col). Populated internally by plotter_add_surface();
 * fields other than the raw data pointers are meant to be modified through
 * the plotter_set_surface_* functions.
 */
typedef struct
{
    const double *x;      /**< X coordinates, flattened grid of size nx*ny. */
    const double *y;      /**< Y coordinates, flattened grid of size nx*ny. */
    const double *z;      /**< Z (height) values, flattened grid of size nx*ny. */
    int nx;                /**< Number of columns in the grid. */
    int ny;                /**< Number of rows in the grid. */
    const char *title;    /**< Legend entry for this surface. */
    const char *color;    /**< Gnuplot RGB color string, or NULL for automatic. */
    const char *style;    /**< Gnuplot plot style (e.g. "lines", "points", "pm3d"). */
    double line_width;     /**< Line width, or 0 for gnuplot's default. */
    int dash_type;         /**< Gnuplot dashtype index, or 0 for default. */
} Surface3D;

/**
 * @brief A single 2D subplot ("axes") inside a Plotter window.
 *
 * Created with plotter_new_figure_2d(). Owns zero or more PlotSeries,
 * added via plotter_add_line().
 */
typedef struct
{
    PlotSeries *series;
    int series_count;
    int series_capacity;

    const char *title;
    const char *xlabel;
    const char *ylabel;

    int has_xrange;
    double xmin, xmax;

    int has_yrange;
    double ymin, ymax;

    int log_x;
    int log_y;
} Figure2D;

/**
 * @brief A single 3D subplot ("axes") inside a Plotter window.
 *
 * Created with plotter_new_figure_3d(). Owns zero or more Surface3D,
 * added via plotter_add_surface().
 */
typedef struct
{
    Surface3D *surfaces;
    int surfaces_count;
    int surfaces_capacity;

    const char *title;
    const char *xlabel;
    const char *ylabel;
    const char *zlabel;

    int has_xrange;
    double xmin, xmax;

    int has_yrange;
    double ymin, ymax;

    int has_zrange;
    double zmin, zmax;

    int log_x;
    int log_y;
    int log_z;
} Figure3D;

/**
 * @brief A single gnuplot window, holding any number of 2D and 3D figures.
 *
 * Must be initialized with plotter_create() and released with
 * plotter_destroy(). Figures are added with plotter_new_figure_2d() /
 * plotter_new_figure_3d(), and rendered together with plotter_show().
 */
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

    int window_width;
    int window_height;
} Plotter;

/** @brief Opaque handle to a Figure2D, returned by plotter_new_figure_2d(). */
typedef int FigureHandle2D;

/** @brief Opaque handle to a Figure3D, returned by plotter_new_figure_3d(). */
typedef int FigureHandle3D;

/** @brief Opaque handle to a series or surface within a figure. */
typedef int SeriesHandle;

typedef enum
{
    PLOTTER_OK = 0,
    PLOTTER_CREATION_FAILED = 1,   /**< Failed to launch the gnuplot process. */
    PLOTTER_ALLOC_FAILED = 2,       /**< An internal malloc/realloc failed. */
    PLOTTER_INVALID_HANDLE = 3      /**< A FigureHandle or SeriesHandle did not refer to an existing figure/series. */
} PlotterStatus;

/**
 * @brief Creates a Plotter, launching a persistent gnuplot process.
 *
 * @param p  Pointer to the Plotter to initialize.
 *
 * @return PLOTTER_OK on success, PLOTTER_CREATION_FAILED if gnuplot
 *         could not be launched (e.g. not installed or not on PATH).
 *
 * @note Must be paired with plotter_destroy() to close the underlying pipe.
 *
 * Typical full lifecycle:
 * @code
 * Plotter p;
 * plotter_create(&p);
 *
 * FigureHandle2D fig = plotter_new_figure_2d(&p);
 * SeriesHandle s = plotter_add_line(&p, fig, x, y, n, "my curve");
 * plotter_set_line_color(&p, fig, s, "#FF0000");
 *
 * plotter_show(&p);
 *
 * plotter_destroy(&p);
 * @endcode
 */
PlotterStatus plotter_create(Plotter *p);

/**
 * @brief Adds a new, empty 2D figure to the plotter.
 *
 * @param p  The plotter.
 * @return A FigureHandle2D to be used with plotter_add_line() and the
 *         plotter_set_*_2d functions, or -1 if an internal allocation failed.
 */
FigureHandle2D plotter_new_figure_2d(Plotter *p);

/**
 * @brief Adds a new, empty 3D figure to the plotter.
 *
 * @param p  The plotter.
 * @return A FigureHandle3D to be used with plotter_add_surface() and the
 *         plotter_set_*_3d functions, or -1 if an internal allocation failed.
 */
FigureHandle3D plotter_new_figure_3d(Plotter *p);

/**
 * @brief Adds a line to a 2D figure.
 *
 * @param p      The plotter.
 * @param fig    Handle of the figure to add the line to.
 * @param x      X coordinates. Not copied; must remain valid until plotter_show() is called.
 * @param y      Y coordinates. Not copied; must remain valid until plotter_show() is called.
 * @param count  Number of points in x/y.
 * @param title  Legend entry for this line. Not copied.
 *
 * @return A SeriesHandle to customize this line's appearance, or -1 on failure
 *         (invalid figure handle, or allocation failure).
 *
 * @warning x and y must each have at least `count` valid elements. This is
 *          not checked; a shorter array produces undefined behavior.
 */
SeriesHandle plotter_add_line(Plotter *p, FigureHandle2D fig, const double *x, const double *y, int count, const char *title);

/**
 * @brief Adds a surface to a 3D figure.
 *
 * @param p      The plotter.
 * @param fig    Handle of the figure to add the surface to.
 * @param x      X coordinates, flattened grid of size nx*ny (index = row*nx + col).
 * @param y      Y coordinates, same layout as x.
 * @param z      Z (height) values, same layout as x.
 * @param nx     Number of columns in the grid.
 * @param ny     Number of rows in the grid.
 * @param title  Legend entry for this surface. Not copied.
 *
 * @return A SeriesHandle to customize this surface's appearance, or -1 on failure.
 *
 * @warning x, y, and z must each have at least nx*ny valid elements, laid out
 *          in row-major order. This is not checked.
 *
 * Example: building a flattened grid for z = f(x, y):
 * @code
 * int nx = 30, ny = 30;
 * double *x = malloc(nx * ny * sizeof(double));
 * double *y = malloc(nx * ny * sizeof(double));
 * double *z = malloc(nx * ny * sizeof(double));
 *
 * for (int row = 0; row < ny; row++)
 * {
 *     double yv = -3.0 + 6.0 * row / (ny - 1);
 *     for (int col = 0; col < nx; col++)
 *     {
 *         double xv = -3.0 + 6.0 * col / (nx - 1);
 *         int idx = row * nx + col;
 *         x[idx] = xv;
 *         y[idx] = yv;
 *         z[idx] = xv * xv + yv * yv;   // e.g. a paraboloid
 *     }
 * }
 *
 * plotter_add_surface(&p, fig, x, y, z, nx, ny, "paraboloid");
 * @endcode
 */
SeriesHandle plotter_add_surface(Plotter *p, FigureHandle3D fig, const double *x, const double *y, const double *z, int nx, int ny, const char *title);

/** @brief Sets the color of a line (e.g. "#FF0000"), or NULL to use gnuplot's automatic color. */
PlotterStatus plotter_set_line_color(Plotter *p, FigureHandle2D fig, SeriesHandle series, const char *color);

/** @brief Sets the drawing style of a line (e.g. "lines", "points", "linespoints"). */
PlotterStatus plotter_set_line_style(Plotter *p, FigureHandle2D fig, SeriesHandle series, const char *style);

/** @brief Sets the color of a surface, or NULL to use gnuplot's automatic color. */
PlotterStatus plotter_set_surface_color(Plotter *p, FigureHandle3D fig, SeriesHandle surface, const char *color);

/** @brief Sets the drawing style of a surface (e.g. "lines", "points", "pm3d" for a solid, height-colored surface). */
PlotterStatus plotter_set_surface_style(Plotter *p, FigureHandle3D fig, SeriesHandle surface, const char *style);

/** @brief Sets the title of a 2D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_title_2d(Plotter *p, FigureHandle2D fig, const char *title);

/** @brief Sets the X axis label of a 2D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_xlabel_2d(Plotter *p, FigureHandle2D fig, const char *label);

/** @brief Sets the Y axis label of a 2D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_ylabel_2d(Plotter *p, FigureHandle2D fig, const char *label);

/** @brief Fixes the X axis range of a 2D figure. Without this, the axis autoscales. */
PlotterStatus plotter_set_xrange_2d(Plotter *p, FigureHandle2D fig, double xmin, double xmax);

/** @brief Fixes the Y axis range of a 2D figure. Without this, the axis autoscales. */
PlotterStatus plotter_set_yrange_2d(Plotter *p, FigureHandle2D fig, double ymin, double ymax);

/** @brief Sets the title of a 3D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_title_3d(Plotter *p, FigureHandle3D fig, const char *title);

/** @brief Sets the X axis label of a 3D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_xlabel_3d(Plotter *p, FigureHandle3D fig, const char *label);

/** @brief Sets the Y axis label of a 3D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_ylabel_3d(Plotter *p, FigureHandle3D fig, const char *label);

/** @brief Sets the Z axis label of a 3D figure. Pass NULL to clear it. */
PlotterStatus plotter_set_zlabel_3d(Plotter *p, FigureHandle3D fig, const char *label);

/** @brief Fixes the X axis range of a 3D figure. Without this, the axis autoscales. */
PlotterStatus plotter_set_xrange_3d(Plotter *p, FigureHandle3D fig, double xmin, double xmax);

/** @brief Fixes the Y axis range of a 3D figure. Without this, the axis autoscales. */
PlotterStatus plotter_set_yrange_3d(Plotter *p, FigureHandle3D fig, double ymin, double ymax);

/** @brief Fixes the Z axis range of a 3D figure. Without this, the axis autoscales. */
PlotterStatus plotter_set_zrange_3d(Plotter *p, FigureHandle3D fig, double zmin, double zmax);

/**
 * @brief Enables or disables base-10 logarithmic scale on the X axis of a 2D figure.
 * @warning Data points with x <= 0 are omitted by gnuplot when log scale is enabled.
 */
PlotterStatus plotter_set_logscale_x_2d(Plotter *p, FigureHandle2D fig, int enabled);

/**
 * @brief Enables or disables base-10 logarithmic scale on the Y axis of a 2D figure.
 * @warning Data points with y <= 0 are omitted by gnuplot when log scale is enabled.
 */
PlotterStatus plotter_set_logscale_y_2d(Plotter *p, FigureHandle2D fig, int enabled);

/** @brief Enables or disables base-10 logarithmic scale on the X axis of a 3D figure. */
PlotterStatus plotter_set_logscale_x_3d(Plotter *p, FigureHandle3D fig, int enabled);

/** @brief Enables or disables base-10 logarithmic scale on the Y axis of a 3D figure. */
PlotterStatus plotter_set_logscale_y_3d(Plotter *p, FigureHandle3D fig, int enabled);

/** @brief Enables or disables base-10 logarithmic scale on the Z axis of a 3D figure. */
PlotterStatus plotter_set_logscale_z_3d(Plotter *p, FigureHandle3D fig, int enabled);

/** @brief Sets the line width of a 2D series. Pass 0 to use gnuplot's default. */
PlotterStatus plotter_set_line_width(Plotter *p, FigureHandle2D fig, SeriesHandle series, double width);

/**
 * @brief Sets the dash pattern of a 2D series (gnuplot dashtype index).
 *
 * Common values: 1 = solid, 2 = dashed, 3 = dotted, 4 = dash-dot.
 * Pass 0 to use a solid line (gnuplot's default).
 */
PlotterStatus plotter_set_line_dashtype(Plotter *p, FigureHandle2D fig, SeriesHandle series, int dashtype);

/** @brief Sets the line width of a 3D surface's mesh. Pass 0 to use gnuplot's default. */
PlotterStatus plotter_set_surface_width(Plotter *p, FigureHandle3D fig, SeriesHandle surface, double width);

/** @brief Sets the dash pattern of a 3D surface's mesh (see plotter_set_line_dashtype). */
PlotterStatus plotter_set_surface_dashtype(Plotter *p, FigureHandle3D fig, SeriesHandle surface, int dashtype);

/**
 * @brief Enables or disables the background grid for the whole window.
 *
 * Applies immediately to the underlying gnuplot process; affects any
 * figure shown afterward.
 */
PlotterStatus plotter_set_grid(Plotter *p, int enabled);

/**
 * @brief Sets the legend position for the whole window.
 *
 * @param position  A gnuplot `set key` position keyword (e.g. "top left",
 *                   "bottom right", "outside"). Not validated; an invalid
 *                   value produces a warning from gnuplot itself, not this function.
 *
 * @code
 * plotter_set_legend_position(&p, "top left");
 * plotter_set_legend_position(&p, "bottom right");
 * plotter_set_legend_position(&p, "outside");
 * @endcode
 */
PlotterStatus plotter_set_legend_position(Plotter *p, const char *position);

/** @brief Resizes the plot window, in pixels. */
PlotterStatus plotter_set_window_size(Plotter *p, int width, int height);

/**
 * @brief Renders the currently pending figures to an image file instead of the screen.
 *
 * The output format is chosen from the file extension: ".pdf" produces a
 * PDF via gnuplot's pdfcairo terminal, anything else produces a PNG via
 * pngcairo. Uses the same window size configured with plotter_set_window_size().
 *
 * Unlike plotter_show(), this does not clear the pending figures, so you
 * can call plotter_save() and then plotter_show() (or vice versa) to get
 * both a file and an interactive window from the same set of figures.
 *
 * @param p         The plotter.
 * @param filename  Output path, e.g. "plot.png" or "plot.pdf".
 *
 * @return PLOTTER_OK.
 *
 * @code
 * FigureHandle2D fig = plotter_new_figure_2d(&p);
 * plotter_add_line(&p, fig, x, y, n, "curve");
 *
 * plotter_save(&p, "curve.png");   // write to disk
 * plotter_show(&p);                // also show interactively, clears pending state
 * @endcode
 */
PlotterStatus plotter_save(Plotter *p, const char *filename);

/**
 * @brief Renders every figure added so far, as a multiplot grid.
 *
 * 2D figures occupy one row, 3D figures occupy another; each row has as
 * many columns as its largest group needs. After rendering, all pending
 * figures are cleared, so new figures can be added and plotter_show()
 * called again to produce another plot in the same window.
 *
 * @code
 * FigureHandle2D fig1 = plotter_new_figure_2d(&p);
 * plotter_add_line(&p, fig1, x, y, n, "curve");
 * plotter_show(&p);   // renders one 2D figure
 *
 * // figures_2d_count is now back to 0 -- previous figures are gone,
 * // but their data (x, y arrays) is untouched and can still be reused
 * FigureHandle2D fig2 = plotter_new_figure_2d(&p);
 * plotter_add_line(&p, fig2, x, y, n, "curve again");
 * plotter_show(&p);   // renders a second, independent plot
 * @endcode
 */
void plotter_show(Plotter *p);

/**
 * @brief Releases all resources owned by the plotter.
 *
 * Closes the pipe to the gnuplot process. On some platforms this call
 * blocks until any open gnuplot window is closed manually. See the
 * "Notes on window persistence" section in the README for details.
 */
void plotter_destroy(Plotter *p);

#endif