#include "term.hpp"
#include <bitset>
#include <algorithm>

using namespace std;

// Constructor from decimal
Term::Term(int decimal, int numVars, bool isDontCare) {
    binary = bitset<32>(decimal).to_string().substr(32 - numVars);
    coveredMinterms.insert(decimal);
    this->isDontCare = isDontCare;
}

// Constructor from binary string
Term::Term(const string& binString, set<int> covered, bool isDontCare) {
    binary = binString;
    coveredMinterms = covered;
    this->isDontCare = isDontCare;
}

string Term::getBinary() const { return binary; }
set<int> Term::getCoveredMinterms() const { return coveredMinterms; }
bool Term::isUsed() const { return used; }
bool Term::isDontCareTerm() const { return isDontCare; }
void Term::markUsed() { used = true; }

bool Term::canCombineWith(const Term& other) const {
    string a = this->getBinary();
    string b = other.getBinary();
    
    if (a.length() != b.length()) return false;
    int diffCount = 0;

    for (size_t i = 0; i < a.length(); i++) {
        if (a[i] != b[i]) {
            diffCount++;
            if (diffCount > 1) return false;
        }
    }

    return diffCount == 1;
}

Term Term::combineWith(const Term& other) const {
    if (!canCombineWith(other)) {
        throw "Error: minterm forced to be combined";
    }

    string a = this->getBinary();
    string b = other.getBinary();
    string result = "";

    for (size_t i = 0; i < a.length(); i++) {
        if (a[i] == b[i]) {
            result += a[i];
        } else {
            result += '-';
        }
    }

    set<int> mergedMinterms = this->getCoveredMinterms();
    for (int m : other.getCoveredMinterms()) {
        mergedMinterms.insert(m);
    }
    
    if(countOnes()==0){
        return Term(result,mergedMinterms,true);
    }
    return Term(result, mergedMinterms);
}

int Term::countOnes() const {
    int count = 0;
    for (char bit : getBinary()) {
        if (bit == '1') count++;
    }
    return count;
}

bool Term::operator==(const Term& other) const {
    return binary == other.binary &&
           coveredMinterms == other.coveredMinterms &&
           isDontCare == other.isDontCare;
}

bool Term::operator<(const Term& other) const {
    if (binary != other.binary) {
        return binary < other.binary;
    }
    if (coveredMinterms != other.coveredMinterms) {
        return coveredMinterms < other.coveredMinterms;
    }
    return isDontCare < other.isDontCare;
}
