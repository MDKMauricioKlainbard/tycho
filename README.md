# tycho

A small C library for plotting 2D lines and 3D surfaces via [gnuplot](http://www.gnuplot.info/), with support for multi-figure layouts (multiplot) and per-figure/per-series customization.

`tycho` does not implement any rendering itself — it drives a `gnuplot` process through a pipe (`popen`) and translates a small, structured C API into gnuplot commands. You need `gnuplot` installed and available on your `PATH`.

## Requirements

- A C11 compiler
- CMake >= 3.15
- `gnuplot` installed and reachable from the command line (`gnuplot --version` should work)

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

This produces a static library, `tycho`, along with the test executables under `tests/`.

## Using tycho in another project

```cmake
add_subdirectory(path/to/tycho)
target_link_libraries(your_target PRIVATE tycho)
```

Then in your code:

```c
#include <tycho/plotter.h>
```

## Quick example

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tycho/plotter.h>

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
    double *y = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        t[i] = i * 0.1;
        y[i] = sin(t[i]);
    }

    FigureHandle2D fig = plotter_new_figure_2d(&p);
    plotter_set_title_2d(&p, fig, "Simple sine wave");
    plotter_set_xlabel_2d(&p, fig, "t");
    plotter_set_ylabel_2d(&p, fig, "sin(t)");

    SeriesHandle s = plotter_add_line(&p, fig, t, y, n, "sin(t)");
    plotter_set_line_color(&p, fig, s, "#FF0000");

    plotter_show(&p);

    printf("Plot generated. Close the gnuplot window manually when done.\n");

    free(t);
    free(y);
    plotter_destroy(&p);

    return 0;
}
```

More complete examples — multiple lines per figure, multiple surfaces, multiplot layouts mixing 2D and 3D, axis ranges, log scales, and full styling — are available in `tests/`.

## Core concepts

`tycho` organizes plots in three levels:

- **`Plotter`** — one gnuplot window. Owns the pipe to the gnuplot process and holds every figure added to it until `plotter_show()` is called.
- **`Figure2D` / `Figure3D`** — one subplot ("axes") inside the window. Created with `plotter_new_figure_2d()` / `plotter_new_figure_3d()`, which return a `FigureHandle2D` / `FigureHandle3D`. A figure owns its title, axis labels, axis ranges, log scale settings, and one or more series.
- **`PlotSeries` / `Surface3D`** — one line (2D) or one surface (3D) inside a figure. Added with `plotter_add_line()` / `plotter_add_surface()`, which return a `SeriesHandle` used to customize color, style, line width, and dash type for that specific series.

### Typical flow

```c
Plotter p;
plotter_create(&p);

FigureHandle2D fig1 = plotter_new_figure_2d(&p);
SeriesHandle s1 = plotter_add_line(&p, fig1, x1, y1, n1, "series A");
plotter_add_line(&p, fig1, x2, y2, n2, "series B");   // second line, same figure

FigureHandle3D fig2 = plotter_new_figure_3d(&p);
plotter_add_surface(&p, fig2, x3, y3, z3, nx, ny, "surface A");

plotter_show(&p);   // renders everything added so far, in a grid:
                     // one row for 2D figures, one row for 3D figures

plotter_destroy(&p);
```

`plotter_show()` clears the pending figures afterward, so you can call `plotter_new_figure_2d()`/`plotter_new_figure_3d()` again and call `plotter_show()` a second time to produce a new plot in the same window.

### 2D vs 3D handles are distinct types

`FigureHandle2D` and `FigureHandle3D` are separate types (both plain `int` indices under the hood, but tracked in separate arrays). Functions that operate on a figure — `plotter_set_title_2d` vs `plotter_set_title_3d`, `plotter_set_xrange_2d` vs `plotter_set_xrange_3d`, etc. — are split accordingly. This avoids ambiguity when a 2D figure and a 3D figure happen to share the same numeric index.

## Notes on window persistence

`Plotter` opens gnuplot with `-persist`, so plot windows remain open after your program exits. `plotter_destroy()` closes the pipe to gnuplot, but on some platforms (notably Windows) this call blocks until you manually close the gnuplot window(s). If you want windows to stay open without blocking your program's exit, simply skip `plotter_destroy()` — the operating system reclaims the pipe when your process ends, and gnuplot keeps running independently thanks to `-persist`.

## What tycho does not do (yet)

- Exporting plots to a file (PNG/PDF) — currently interactive display only.
- Setting a fixed 3D camera angle (`set view`).
- Detecting or reporting if the underlying gnuplot process crashes.

## License

Personal project — no license specified yet.