#include "gnuplot.h"

Gnuplot::Gnuplot(const char * path): _p(NULL) {
    char s[2048];
    snprintf(s, 2048, "\"%s\"", path);

    _p = popen(s, "w");

    if (_p == NULL) {
        fprintf(stderr, "unable to open '%s'\n", path);
    } else {
        _send_data("set term wx");
    }
}

Gnuplot::Gnuplot(std::string path) {
    Gnuplot(path.c_str());
}

Gnuplot::~Gnuplot(void) {
    if (_p) {
        // pclose(_p);
    }
}

void Gnuplot::_send_data(const char * s) {
    if (!_p) {
        return;
    }

    fputs(s, _p);
    fputs("\n", _p);
    fflush(_p);
}

void Gnuplot::plot(std::vector<double> x, std::vector<double> y) {
    if (!_p) {
        return;
    }

    if (x.size() != y.size()) {
        return;
    }

    fprintf(_p, "plot '-' with lines\n");
    for (unsigned i = 0; i < x.size(); i++) {
        fprintf(_p, "%f %f\n", x.at(i), y.at(i));
    }
    fprintf(_p, "e\n");
    // fprintf(_p, "pause -1\n");
    fflush(_p);
}