#ifndef __DATA_SET_H__
#define __DATA_SET_H__

#include <string>
#include <vector>

struct Data_Element {
    std::string name;
    std::vector<double> data;
};

class Data_Set {
public:
    void create(std::string name);

    void clear(void);
    void clear_data(void);

    std::vector<Data_Element> * get_all(void) {
        return &_data;
    }

    void add_data(std::string name, double val);
    void set_data(std::string name, std::vector<double> data);

    std::vector<double> get_data(std::string name);

    std::vector<std::string> get_names(void);
    std::vector<char *> get_names_c(void);

private:
    std::vector<Data_Element> _data;
};

#endif