#include <tycho/plotter.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

static char *sanitize_gnuplot_string(const char *input)
{
    int len = strlen(input);
    int quote_count = 0;

    for (int i = 0; i < len; i++)
    {
        if (input[i] == '\'')
            quote_count++;
    }

    char *output = malloc(len + quote_count + 1);
    if (!output)
        return NULL;

    if (quote_count == 0)
    {
        strcpy(output, input);
        return output;
    }

    int j = 0;
    for (int i = 0; i < len; i++)
    {
        if (input[i] == '\'')
        {
            output[j++] = '\'';
            output[j++] = '\'';
        }
        else
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';

    return output;
}

PlotterStatus plotter_create(Plotter *p)
{
    p->pipe = POPEN("gnuplot -persist", "w");
    if (!p->pipe)
        return PLOTTER_CREATION_FAILED;

    fprintf(p->pipe, "set terminal wxt size 1000,700\n");
    fprintf(p->pipe, "set grid\n");
    fprintf(p->pipe, "set key bottom right\n");
    return PLOTTER_OK;
}

void plotter_set_title(const Plotter *p, const char *title)
{
    fprintf(p->pipe, "set title '%s'\n", title);
}

void plotter_set_labels(const Plotter *p, const char *xlabel, const char *ylabel)
{
    char *safe_xlabel = sanitize_gnuplot_string(xlabel);
    char *safe_ylabel = sanitize_gnuplot_string(ylabel);

    fprintf(p->pipe, "set xlabel '%s'\n", safe_xlabel ? safe_xlabel : "x");
    fprintf(p->pipe, "set ylabel '%s'\n", safe_ylabel ? safe_ylabel : "y");

    free(safe_xlabel);
    free(safe_ylabel);
}

void plotter_plot_xy(const Plotter *p, const double *x, const double *y, int count, const char *series_title)
{
    char *safe_title = sanitize_gnuplot_string(series_title);
    fprintf(p->pipe, "plot '-' with lines title '%s'\n", safe_title ? safe_title : "Untitled");

    for (int i = 0; i < count; i++)
    {
        fprintf(p->pipe, "%f %f\n", x[i], y[i]);
    }

    fprintf(p->pipe, "e\n");
    free(safe_title);
}

void plotter_plot_multi_xy(const Plotter *p, const PlotSeries *series, int n_series)
{
    fprintf(p->pipe, "plot ");
    for (int s = 0; s < n_series; s++)
    {
        char *safe_title = sanitize_gnuplot_string(series[s].title);
        fprintf(p->pipe, "'-' with lines title '%s'%s", safe_title, (s < n_series - 1) ? ", " : "\n");
        free(safe_title);
    }

    for (int s = 0; s < n_series; s++)
    {
        for (int i = 0; i < series[s].count; i++)
        {
            fprintf(p->pipe, "%f %f\n", series[s].x[i], series[s].y[i]);
        }
        fprintf(p->pipe, "e\n");
    }
}

void plotter_destroy(Plotter *p)
{
    if (p->pipe)
    {
        PCLOSE(p->pipe);
        p->pipe = NULL;
    }
}