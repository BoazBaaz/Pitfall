// not a STL library
#include <iostream>

#include "tmloader.h"
#include "precomp.h"

TMLoader::TMLoader(const char* filename) {
    file = fopen(filename, "r");
    if (file == nullptr) {
        std::cerr << "Failed to open CSV file." << std::endl;
    }
}

int TMLoader::customReadLine(char* buffer, size_t bufferSize) {
    if (fgets(buffer, bufferSize, file) == nullptr) {
        return 1; // Reached end of file or error
    }
    return 0;
}

void TMLoader::customSplit(const char* input, char delimiter, char** tokens, int* numTokens) {
    int tokenCount = 0;
    char* token = strtok(const_cast<char*>(input), &delimiter);
    while (token != nullptr) {
        tokens[tokenCount++] = token;
        token = strtok(nullptr, &delimiter);
    }
    *numTokens = tokenCount;
}

bool TMLoader::isOpen() {
    return file != nullptr;
}

void TMLoader::close() {
    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }
}

//vector<vector<int>> csv;
//ifstream file("assets/yeah.csv");
//string line;
//while (getline(file, line)) {
//    vector<int> row;
//    stringstream stream(line);
//    string value;
//    while (getline(stream, value, ',')) {
//        row.push_back(stoi(value));
//    }
//    csv.push_back(row);
//}
//file.close();
//
//for (vector<int>& row : csv) {
//    for (int value : row) {
//        cout << value << " ";
//    }
//    cout << endl;
//}