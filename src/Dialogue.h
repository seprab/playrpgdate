#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>
#include <vector>
#include <iostream>
#include "jsmn.h"

class Dialogue
{
private:
    char* description;
    std::vector<char*> choices;

public:
    Dialogue();
    Dialogue(char* _description, std::vector<char*> _choices);
    Dialogue(char *buffer);

    void AddChoice(char* choice);
    unsigned int Size();

    int Activate();
};

#endif