#include "type.hpp"
#include "factor-graph.hpp"
#include <vector>

#ifndef TARGET_FUNCTIONS_HPP
#define TARGET_FUNCTIONS_HPP

class TargetFunction {
public:
    virtual Real eval(const EvalGraph&graph) const = 0;
};

#endif
