#include "utils.hpp"

std::map<int, std::vector<Term>> groupByOnes(const std::vector<Term>& terms) {
    std::map<int, std::vector<Term>> grouped;
    for (const Term& t : terms) {
        int ones = t.countOnes();
        grouped[ones].push_back(t);
    }
    return grouped;
}
