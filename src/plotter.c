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

#define INITIAL_CAPACITY 4

// ---------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------

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

static int grow_buffer(void **buffer, int *capacity, size_t elem_size)
{
    int new_capacity = (*capacity == 0) ? INITIAL_CAPACITY : (*capacity * 2);
    void *new_ptr = realloc(*buffer, new_capacity * elem_size);

    if (!new_ptr)
        return 1;

    *buffer = new_ptr;
    *capacity = new_capacity;
    return 0;
}

static void write_datablock_2d(FILE *pipe, const PlotSeries *series, int id)
{
    fprintf(pipe, "$D%d << EOD\n", id);
    for (int i = 0; i < series->count; i++)
    {
        fprintf(pipe, "%f %f\n", series->x[i], series->y[i]);
    }
    fprintf(pipe, "EOD\n");
}

static void write_datablock_3d(FILE *pipe, const Surface3D *surf, int id)
{
    fprintf(pipe, "$D%d << EOD\n", id);
    for (int row = 0; row < surf->ny; row++)
    {
        for (int col = 0; col < surf->nx; col++)
        {
            int idx = row * surf->nx + col;
            fprintf(pipe, "%f %f %f\n", surf->x[idx], surf->y[idx], surf->z[idx]);
        }
        fprintf(pipe, "\n");
    }
    fprintf(pipe, "EOD\n");
}

static void write_2d_plot_command(FILE *pipe, const Figure2D *fig, int *next_id)
{
    int first_id = *next_id;

    for (int s = 0; s < fig->series_count; s++)
    {
        write_datablock_2d(pipe, &fig->series[s], (*next_id)++);
    }

    fprintf(pipe, "plot ");
    for (int s = 0; s < fig->series_count; s++)
    {
        char *safe_title = sanitize_gnuplot_string(fig->series[s].title);
        fprintf(pipe, "$D%d with lines title '%s'%s",
                first_id + s,
                safe_title ? safe_title : "Untitled",
                (s < fig->series_count - 1) ? ", " : "\n");
        free(safe_title);
    }
}

static void write_3d_plot_command(FILE *pipe, const Figure3D *fig, int *next_id)
{
    int first_id = *next_id;

    for (int s = 0; s < fig->surfaces_count; s++)
    {
        write_datablock_3d(pipe, &fig->surfaces[s], (*next_id)++);
    }

    fprintf(pipe, "splot ");
    for (int s = 0; s < fig->surfaces_count; s++)
    {
        char *safe_title = sanitize_gnuplot_string(fig->surfaces[s].title);
        fprintf(pipe, "$D%d with lines title '%s'%s",
                first_id + s,
                safe_title ? safe_title : "Untitled",
                (s < fig->surfaces_count - 1) ? ", " : "\n");
        free(safe_title);
    }
}

// ---------------------------------------------------------------------
// API
// ---------------------------------------------------------------------

PlotterStatus plotter_create(Plotter *p)
{
    p->pipe = POPEN("gnuplot -persist", "w");
    if (!p->pipe)
        return PLOTTER_CREATION_FAILED;

    fprintf(p->pipe, "set terminal wxt size 1200,800\n");
    fprintf(p->pipe, "set grid\n");
    fprintf(p->pipe, "set key bottom right\n");

    p->figures_2d = NULL;
    p->figures_2d_count = 0;
    p->figures_2d_capacity = 0;

    p->figures_3d = NULL;
    p->figures_3d_count = 0;
    p->figures_3d_capacity = 0;

    return PLOTTER_OK;
}

FigureHandle plotter_new_figure_2d(Plotter *p)
{
    if (p->figures_2d_count == p->figures_2d_capacity)
    {
        if (grow_buffer((void **)&p->figures_2d, &p->figures_2d_capacity, sizeof(Figure2D)) != 0)
            return -1;
    }

    Figure2D *fig = &p->figures_2d[p->figures_2d_count];
    fig->series = NULL;
    fig->series_count = 0;
    fig->series_capacity = 0;

    FigureHandle handle = p->figures_2d_count++;

    if (p->figures_2d_count > p->figures_2d_used)
        p->figures_2d_used = p->figures_2d_count;

    return handle;
}

FigureHandle plotter_new_figure_3d(Plotter *p)
{
    if (p->figures_3d_count == p->figures_3d_capacity)
    {
        if (grow_buffer((void **)&p->figures_3d, &p->figures_3d_capacity, sizeof(Figure3D)) != 0)
            return -1;
    }

    Figure3D *fig = &p->figures_3d[p->figures_3d_count];
    fig->surfaces = NULL;
    fig->surfaces_count = 0;
    fig->surfaces_capacity = 0;

    FigureHandle handle = p->figures_3d_count++;

    if (p->figures_3d_count > p->figures_3d_used)
        p->figures_3d_used = p->figures_3d_count;

    return handle;
}

PlotterStatus plotter_add_line(Plotter *p, FigureHandle fig, const double *x, const double *y, int count, const char *title)
{
    if (fig < 0 || fig >= p->figures_2d_count)
        return PLOTTER_INVALID_HANDLE;

    Figure2D *f = &p->figures_2d[fig];

    if (f->series_count == f->series_capacity)
    {
        if (grow_buffer((void **)&f->series, &f->series_capacity, sizeof(PlotSeries)) != 0)
            return PLOTTER_ALLOC_FAILED;
    }

    f->series[f->series_count].x = x;
    f->series[f->series_count].y = y;
    f->series[f->series_count].count = count;
    f->series[f->series_count].title = title;
    f->series_count++;

    return PLOTTER_OK;
}

PlotterStatus plotter_add_surface(Plotter *p, FigureHandle fig, const double *x, const double *y, const double *z, int nx, int ny, const char *title)
{
    if (fig < 0 || fig >= p->figures_3d_count)
        return PLOTTER_INVALID_HANDLE;

    Figure3D *f = &p->figures_3d[fig];

    if (f->surfaces_count == f->surfaces_capacity)
    {
        if (grow_buffer((void **)&f->surfaces, &f->surfaces_capacity, sizeof(Surface3D)) != 0)
            return PLOTTER_ALLOC_FAILED;
    }

    f->surfaces[f->surfaces_count].x = x;
    f->surfaces[f->surfaces_count].y = y;
    f->surfaces[f->surfaces_count].z = z;
    f->surfaces[f->surfaces_count].nx = nx;
    f->surfaces[f->surfaces_count].ny = ny;
    f->surfaces[f->surfaces_count].title = title;
    f->surfaces_count++;

    return PLOTTER_OK;
}

void plotter_show(Plotter *p)
{
    int has_2d = p->figures_2d_count > 0;
    int has_3d = p->figures_3d_count > 0;

    if (!has_2d && !has_3d)
        return;

    int rows = has_2d + has_3d;
    int cols = p->figures_2d_count > p->figures_3d_count ? p->figures_2d_count : p->figures_3d_count;

    int next_id = 0;

    fprintf(p->pipe, "set multiplot layout %d,%d\n", rows, cols);

    for (int i = 0; i < p->figures_2d_count; i++)
    {
        write_2d_plot_command(p->pipe, &p->figures_2d[i], &next_id);
    }

    for (int i = 0; i < p->figures_3d_count; i++)
    {
        write_3d_plot_command(p->pipe, &p->figures_3d[i], &next_id);
    }

    fprintf(p->pipe, "unset multiplot\n");
    fflush(p->pipe);

    for (int i = 0; i < p->figures_2d_count; i++)
    {
        p->figures_2d[i].series_count = 0;
    }
    for (int i = 0; i < p->figures_3d_count; i++)
    {
        p->figures_3d[i].surfaces_count = 0;
    }
    p->figures_2d_count = 0;
    p->figures_3d_count = 0;
}

void plotter_destroy(Plotter *p)
{
    if (p->pipe)
    {
        PCLOSE(p->pipe);
        p->pipe = NULL;
    }

    for (int i = 0; i < p->figures_2d_used; i++)
    {
        free(p->figures_2d[i].series);
    }
    free(p->figures_2d);
    p->figures_2d = NULL;

    for (int i = 0; i < p->figures_3d_used; i++)
    {
        free(p->figures_3d[i].surfaces);
    }
    free(p->figures_3d);
    p->figures_3d = NULL;
}