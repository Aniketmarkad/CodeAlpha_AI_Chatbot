#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

vector<string> split(const string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void trim(string& str) {
    str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), str.end());
}


int levenshtein_distance(const string& s1, const string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

struct Record {
    string phrase;
    vector<string> responses;
};

vector<string> readLinesIntoArr(ifstream& file) {
    string line;
    vector<string> fileLines;

    while (getline(file, line)) {
        fileLines.push_back(line);
    }
    return fileLines;
}

vector<Record> getRecords(const string& fileName) {
    const string phraseTag = "<phrase>";
    const string respTag = "<response>";
    vector<Record> records;
    vector<string> rawFile;
    vector<string> splitString;
    string line;
    char delim = '>';
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << fileName << endl;
        return records;
    }
    rawFile = readLinesIntoArr(file);
    file.close();

    Record* curRec = nullptr;
    for (const string& line : rawFile) {
        if (line.find(phraseTag) != string::npos) {
            Record tempRec;
            splitString = split(line, delim);
            if (splitString.size() > 1) {
                tempRec.phrase = splitString[1];
                trim(tempRec.phrase);
                records.push_back(tempRec);
                curRec = &records.back();
            }
        } else if (line.find(respTag) != string::npos && curRec) {
            splitString = split(line, delim);
            if (splitString.size() > 1) {
                string response = splitString[1];
                trim(response);
                curRec->responses.push_back(response);
            }
        }
    }
    return records;
}

string getResponse(const string& input, const vector<Record>& records) {
    if (records.empty()) return "Sorry, I have no responses available.";

    int minDistance = levenshtein_distance(input, records[0].phrase);
    int bestIndex = 0;

    for (int i = 1; i < records.size(); ++i) {
        int distance = levenshtein_distance(input, records[i].phrase);
        if (distance < minDistance) {
            minDistance = distance;
            bestIndex = i;
        }
    }

    if (!records[bestIndex].responses.empty()) {
        int randIndex = rand() % records[bestIndex].responses.size();
        return records[bestIndex].responses[randIndex];
    }
    return "I couldn't find an appropriate response.";
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for randomness
    string input;
    vector<Record> records = getRecords("records.txt");

    if (records.empty()) {
        cerr << "Error: No valid records found in file." << endl;
        return 1;
    }

    cout << "ChatBot: Hello! Type 'q' to quit.\n";
    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input == "q") break;

        string response = getResponse(input, records);
        cout << "ChatBot: " << response << endl;
    }

    return 0;
}
