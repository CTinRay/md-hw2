#include "type.hpp"
#include <vector>


class TargetFunction {
public:
    Real virtual eval(std::vector<Label>&sample) const;
};
