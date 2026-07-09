/**
 * @file plotter.h
 * @brief Thin C abstraction over gnuplot, driven through a pipe.
 *
 * A @ref Plotter opens a persistent `gnuplot` process via a pipe and lets
 * callers configure titles/labels and stream data series to it without
 * having to hand-write gnuplot commands. Requires `gnuplot` to be
 * installed and available on the system `PATH`.
 */

#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>

/**
 * @brief A plotting session backed by a pipe to a `gnuplot` process.
 *
 * Create with @ref plotter_create and release with @ref plotter_destroy.
 */
typedef struct
{
    FILE *pipe; /**< Write end of the pipe to the underlying `gnuplot` process. */
} Plotter;

/**
 * @brief A single named XY data series to be plotted.
 *
 * Used with @ref plotter_plot_multi_xy to draw several series on the same chart.
 *
 * @par Example
 * Build two series (sine and cosine) to plot together:
 * @code
 * double x[100], sine[100], cosine[100];
 * for (int i = 0; i < 100; i++)
 * {
 *     x[i] = i * 0.1;
 *     sine[i] = sin(x[i]);
 *     cosine[i] = cos(x[i]);
 * }
 *
 * PlotSeries series[2] = {
 *     { .x = x, .y = sine,   .count = 100, .title = "sin(x)" },
 *     { .x = x, .y = cosine, .count = 100, .title = "cos(x)" },
 * };
 * @endcode
 * See @ref plotter_plot_multi_xy for how to send this array to gnuplot.
 *
 * @warning @ref PlotSeries::title must not be NULL; it is passed directly to
 *          `strlen` while sanitizing the string for gnuplot.
 * @warning @ref PlotSeries::x and @ref PlotSeries::y must each have at least
 *          @ref PlotSeries::count elements, or the plot call will read out
 *          of bounds.
 */
typedef struct
{
    const double *x;    /**< X coordinates, of length @ref PlotSeries::count. */
    const double *y;    /**< Y coordinates, of length @ref PlotSeries::count. */
    int count;           /**< Number of (x, y) points in the series. */
    const char *title;   /**< Legend label for this series. */
} PlotSeries;

/**
 * @brief Result/status code returned by @ref plotter_create.
 */
typedef enum {
    PLOTTER_OK = 0,             /**< The plotting session was created successfully. */
    PLOTTER_CREATION_FAILED = 1 /**< The underlying `gnuplot` process could not be started. */
} PlotterStatus;

/**
 * @brief Start a new plotting session by launching `gnuplot -persist`.
 *
 * Applies a default terminal size, grid, and legend position.
 *
 * @param p Plotter instance to initialize.
 * @return ::PLOTTER_OK on success, or ::PLOTTER_CREATION_FAILED if the
 *         `gnuplot` process could not be started (e.g. not found in `PATH`).
 */
PlotterStatus plotter_create(Plotter *p);

/**
 * @brief Set the chart title.
 * @param p     Initialized plotter instance.
 * @param title Title text.
 */
void plotter_set_title(const Plotter *p, const char *title);

/**
 * @brief Set the axis labels.
 * @param p      Initialized plotter instance.
 * @param xlabel Label for the X axis.
 * @param ylabel Label for the Y axis.
 */
void plotter_set_labels(const Plotter *p, const char *xlabel, const char *ylabel);

/**
 * @brief Plot a single XY data series.
 * @param p            Initialized plotter instance.
 * @param x             X coordinates, of length @p count.
 * @param y             Y coordinates, of length @p count.
 * @param count         Number of (x, y) points.
 * @param series_title  Legend label for the series.
 */
void plotter_plot_xy(const Plotter *p, const double *x, const double *y, int count, const char *series_title);

/**
 * @brief Plot several XY data series on the same chart.
 * @param p        Initialized plotter instance.
 * @param series   Array of series to plot.
 * @param n_series Number of elements in @p series.
 *
 * @par Example
 * Plot the sine/cosine series built in the @ref PlotSeries example:
 * @code
 * Plotter p;
 * plotter_create(&p);
 * plotter_set_title(&p, "sin and cos");
 * plotter_set_labels(&p, "x", "y");
 *
 * plotter_plot_multi_xy(&p, series, 2);
 *
 * plotter_destroy(&p);
 * @endcode
 *
 * @warning @p n_series must be at least 1; calling this with 0 series sends
 *          an incomplete `plot` command to gnuplot.
 * @warning Every element of @p series must have a non-NULL `title`, and its
 *          `x`/`y` pointers must have at least `count` valid elements (see
 *          @ref PlotSeries). The data is written to the gnuplot pipe
 *          synchronously, so it only needs to remain valid for the duration
 *          of this call.
 */
void plotter_plot_multi_xy(const Plotter *p, const PlotSeries *series, int n_series);

/**
 * @brief Close the underlying `gnuplot` process and release the plotting session.
 * @param p Plotter instance to tear down.
 */
void plotter_destroy(Plotter *p);

#endif
