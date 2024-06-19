#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>
#include <vector>
#include <iostream>

class Dialogue
{
private:
    std::string description;
    std::vector<std::string> choices;

public:
    Dialogue();
    Dialogue(std::string _description, std::vector<std::string> _choices);

    void AddChoice(std::string choice);
    unsigned int Size();

    int Activate();
};

#endif