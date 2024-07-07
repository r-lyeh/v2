#include "cimplot/implot/implot.h"
#include "cimplot/implot/implot_internal.h"
#include "cimplot/implot/implot.cpp"
#include "cimplot/implot/implot_items.cpp"
#include "cimplot/implot/implot_demo.cpp"

extern "C"
void igShowImPlotDemoWindow(bool* p_open) {
    ImPlot::ShowDemoWindow(p_open);
}

