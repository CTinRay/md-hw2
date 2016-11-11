#include "type.hpp"
#include <vector>

#ifndef TARGET_FUNCTIONS_HPP
#define TARGET_FUNCTIONS_HPP

class TargetFunction {
public:
    virtual Real eval(const std::vector<Label>&sample) const = 0;
};

#endif
