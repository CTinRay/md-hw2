#include "type.hpp"
#include <vector>

class SampleTarget {
public:
    Real virtual evalAndAccumulate(std::vector<Label>&sample);
    Real virtual getResult() const;
};
