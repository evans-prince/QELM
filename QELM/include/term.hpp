#pragma once

#include <string>
#include <vector>
#include <set>

class Term {
public:
    //  Constructors
    Term(int decimal, int numVars, bool isDontCare = false);
    Term(const std::string& binString, std::set<int> covered, bool isDontCare = false);

    //  Getters
    std::string getBinary() const;
    std::set<int> getCoveredMinterms() const;
    bool isUsed() const;
    bool isDontCareTerm() const;

    //  Setters
    void markUsed();

    //  Core functionality
    bool canCombineWith(const Term& other) const;
    Term combineWith(const Term& other) const;
    int countOnes() const;

    //  Utility
    bool operator==(const Term& other) const;
    bool operator<(const Term& other) const;

private:
    std::string binary;
    std::set<int> coveredMinterms;
    bool used = false;
    bool isDontCare = false;
};
