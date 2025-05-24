#ifndef quine_hpp
#define quine_hpp

#include "term.hpp"
#include <vector>
#include <set>
#include <string>

std::vector<Term> runQuine(const std::vector<Term>& minterms, const std::vector<Term>& dontCares);

std::string termsToSOP(const std::vector<Term>& terms, int numVars);


#endif /* quine_hpp */
