#include "term.hpp"
#include "combine.hpp"
#include "utils.hpp"
#include <vector>
#include <map>
#include <set>

std::vector<Term> combineTerms(const std::vector<Term>& terms) {
    std::map<int, std::vector<Term>> groups = groupByOnes(terms);
    std::set<Term> combinedSet;
    std::vector<Term> primeImplicants;

    // Loop through adjacent groups
    for (auto it = groups.begin(); std::next(it) != groups.end(); it++) {
        const std::vector<Term>& groupA = it->second;
        const std::vector<Term>& groupB = std::next(it)->second;

        for (const Term& termA : groupA) {
            for (const Term& termB : groupB) {
                if (termA.canCombineWith(termB)) {
                    Term combined = termA.combineWith(termB);
                    combinedSet.insert(combined);
                    const_cast<Term&>(termA).markUsed();//not safe
                    const_cast<Term&>(termB).markUsed();
                }
            }
        }
    }

    // Add uncombined terms (prime implicants)
    for (const auto& group : groups) {
        for (const Term& term : group.second) {
            if (!term.isUsed()) {
                primeImplicants.push_back(term);
            }
        }
    }

    // Add combined terms to result
    for (const Term& t : combinedSet) {
        primeImplicants.push_back(t);
    }

    return primeImplicants;
}
