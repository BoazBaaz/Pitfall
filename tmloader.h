#pragma once

#include <iostream>

class TMLoader {
public:
    TMLoader(const char* filename);
    int customReadLine(char* buffer, size_t bufferSize);
    void customSplit(const char* input, char delimiter, char** tokens, int* numTokens);
    bool isOpen();
    void close();
private:
    FILE* file;
};