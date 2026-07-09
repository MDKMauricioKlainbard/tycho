# Tycho

Tycho is a C library designed to make it easier to use
[gnuplot](http://www.gnuplot.info/) from C code, avoiding the need to
hand-write gnuplot commands and manage the pipe yourself. It's a learning
project and **is in early/active development**, so the API may change.

## Requirements

- `gnuplot` installed and available in the system `PATH` (Tycho invokes it
  internally via `popen`/`_popen`).

## Current status

- Creation and teardown of a plotting session (`Plotter`), which opens and
  closes the pipe to `gnuplot -persist`.
- Setting the plot title and axis labels.
- Plotting a single XY series (`plotter_plot_xy`).
- Plotting multiple series on the same chart (`plotter_plot_multi_xy`).
- Basic string sanitization (quotes) before sending strings to gnuplot.

## Building

The project uses CMake and has no build-time external dependencies
(gnuplot is only needed at runtime, not for building).

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

This produces a static library `tycho`. To use it in another CMake
project:

```cmake
add_subdirectory(tycho)
target_link_libraries(my_project PRIVATE tycho)
```

## Quick example

```c
#include <tycho/plotter.h>

int main(void)
{
    Plotter p;
    if (plotter_create(&p) != PLOTTER_OK)
        return 1;

    plotter_set_title(&p, "Example");
    plotter_set_labels(&p, "x", "y");

    double x[] = {0, 1, 2, 3, 4};
    double y[] = {0, 1, 4, 9, 16};

    plotter_plot_xy(&p, x, y, 5, "y = x^2");

    plotter_destroy(&p);
    return 0;
}
```

## Roadmap (ideas, no timeline commitment)

- More chart types (bar charts, scatter plots, histograms)
- Export to file (PNG, SVG) in addition to interactive mode
- Finer control over line styles and colors

## License

Not defined yet.
