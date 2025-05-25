#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <set>

#include "term.hpp"
#include "quine.hpp"

using namespace std;

// Helper to parse PLA format
bool parsePLA(const string& filename, int& numVars, int& numOutputs,
              vector<vector<Term>>& allMinterms, vector<vector<Term>>& allDontCares,
              vector<string>& inputLabels, vector<string>& outputLabels) {
    ifstream fin(filename);
    if (!fin) {
        cerr << "Error opening input file\n";
        return false;
    }

    string line;
    numVars = 0;
    numOutputs = 1; // default single output

    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue; // skip comments

        if (line.substr(0, 3) == ".i ") {
            numVars = stoi(line.substr(3));
        } else if (line.substr(0, 3) == ".o ") {
            numOutputs = stoi(line.substr(3));
            allMinterms.resize(numOutputs);// resize can cause memory bugs
            allDontCares.resize(numOutputs);
        } else if (line.substr(0, 4) == ".ilb") {
            istringstream ss(line.substr(5));
            inputLabels = {istream_iterator<string>(ss), {}};
        } else if (line.substr(0, 3) == ".ob") {
            istringstream ss(line.substr(4));
            outputLabels = {istream_iterator<string>(ss), {}};
        } else if (line.substr(0, 2) == ".e") {
            break; // end of PLA
        } else if (line[0] == '0' || line[0] == '1' || line[0] == '-') {
            istringstream iss(line);
            string inputBits, outputBits;
            iss >> inputBits >> outputBits;

            int decimal = 0;
            for (char c : inputBits) {
                decimal <<= 1;
                if (c == '1') decimal |= 1;
            }
            set<int> covered = {decimal};

            for (int i = 0; i < outputBits.size(); i++) {
                Term t(inputBits, covered, outputBits[i] == '-');
                if (outputBits[i] == '1') {
                    allMinterms[i].push_back(t);
                } else if (outputBits[i] == '-') {
                    allDontCares[i].push_back(t);
                }
            }
        }
    }

    fin.close();
    return true;
}

int main() {
    const string inputFile = "./data/input.txt";
    const string outputFile = "./data/output.txt";

    int numVars, numOutputs;
    vector<vector<Term>> allMinterms, allDontCares;
    vector<string> inputLabels, outputLabels;

    if (!parsePLA(inputFile, numVars, numOutputs, allMinterms, allDontCares, inputLabels, outputLabels)) {
        cerr << "Failed to parse PLA input\n";
        return 1;
    }

    ofstream fout(outputFile);
    if (!fout) {
        cerr << "Error opening output file\n";
        return 1;
    }

    if (numVars > 10) {
        fout << "Too many variables (" << numVars << ") to run Quine-McCluskey minimization.\n";
        cout << "Variables > 10, skipping minimization.\n";
    } else {
        for (int i = 0; i < numOutputs; i++) {
            fout << "# Output function " << (outputLabels.empty() ? to_string(i) : outputLabels[i]) << "\n";
            vector<Term> essentialPIs = runQuine(allMinterms[i], allDontCares[i]);
            string sop = termsToSOP(essentialPIs, numVars);
            fout << sop << "\n\n";
        }
        cout << "Minimization done! Check output.txt\n";
    }

    fout.close();
    return 0;
}
