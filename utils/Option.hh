#include <string>
#include <map>
#include <vector>


namespace UTIL {

class Option {
    enum class ValType { BOOL, INT, STRING};

public:
    static Option& instance() { static Option option; return option;}

    template <typename T>
    void addOption(std::string opt, T dftVal);

    template <typename T>
    void setOption(std::string opt, T value);

    template <typename T>
    T getOption(std::string opt);

private:
    Option();

private:
    typedef std::pair<ValType, size_t> pos;
    std::vector<bool>           _boolOpts;
    std::vector<int>            _intOpts;
    std::vector<std::string>    _strOpts;
    std::map<std::string, pos>  _options;
};

}
