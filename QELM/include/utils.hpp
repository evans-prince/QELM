#ifndef utils_hpp
#define utils_hpp

#include "term.hpp"
#include <vector>
#include <map>

std::map<int, std::vector<Term>> groupByOnes(const std::vector<Term>& terms);

#endif /* utils_hpp */
