#ifndef __GNUPLOT_H__
#define __GNUPLOT_H__

#include <stdio.h>

#include <string>
#include <vector>

#ifdef _WIN32
    #define popen  _popen
    #define pclose _pclose
#endif

class Gnuplot {
public:
    Gnuplot(const char * path);
    Gnuplot(std::string path);

    ~Gnuplot(void);

    void plot(std::vector<double> x, std::vector<double> y);

private:
    void _send_data(const char * s);

    FILE * _p;
};

#endif