#ifndef TYPE_H
#include "type.hpp"
#endif

class TargetFunction {
public:
    Real virtual eval(std::vector<Label>&sample) const;
};
