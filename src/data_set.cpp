#include "data_set.h"

void Data_Set::create(std::string name) {
    Data_Element de;
    de.name = name;

    _data.push_back(de);
}

void Data_Set::clear(void) {
    _data.clear();
}

void Data_Set::clear_data(void) {
    for (unsigned int i = 0; i < _data.size(); i++) {
        _data.at(i).data.clear();
    }
}

void Data_Set::add_data(std::string name, double val) {
    for (unsigned int i = 0; i < _data.size(); i++) {
        if (_data.at(i).name == name) {
            _data.at(i).data.push_back(val);
            break;
        }
    }
}

void Data_Set::set_data(std::string name, std::vector<double> data) {
    for (unsigned int i = 0; i < _data.size(); i++) {
        if (_data.at(i).name == name) {
            _data.at(i).data = data;
        }
    }
}

std::vector<std::string> Data_Set::get_names(void) {
    std::vector<std::string> ret;

    for (unsigned int i = 0; i < _data.size(); i++) {
        ret.push_back(_data.at(i).name);
    }

    return ret;
}

std::vector<char *> Data_Set::get_names_c(void) {
    std::vector<std::string> s = get_names();
    std::vector<char *> cret;
    cret.reserve(s.size());

    for (unsigned int i = 0; i < s.size(); i++) {
        cret.push_back(strdup(const_cast<char *>(s[i].c_str())));
    }

    return cret;
}

std::vector<double> Data_Set::get_data(std::string name) {
    std::vector<double> d;
    for (unsigned int i = 0; i < _data.size(); i++) {
        if (_data.at(i).name == name) {
            d = _data.at(i).data;
            break;
        }
    }

    return d;
}