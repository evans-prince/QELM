#ifndef utils_hpp
#define utils_hpp

#include "term.hpp"
#include <vector>
#include <map>

using namespace std;

map<int, vector<Term>> groupByOnes(const vector<Term>& terms);

int countLiterals(const vector<Term>& terms);
#endif /* utils_hpp */
