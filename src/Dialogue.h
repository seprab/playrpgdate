#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>
#include <vector>
#include <iostream>
#include "jsmn.h"
struct Choice
{
    const char* text;
    int action;
    int target;
};
class Dialogue
{
public:
    Dialogue() = default;
    Dialogue(char* _description, std::vector<Choice> _choices);
    Dialogue(const char *buffer, const jsmntok_t *tokens, int& choicesIndex);
    Dialogue(const Dialogue& other)=default;
    Dialogue(Dialogue&& other) noexcept;

    void AddChoice(Choice choice);
    [[nodiscard]] unsigned int Size() const;

    [[nodiscard]] int Activate() const;

private:
    char* description{};
    std::vector<Choice> choices;

};

#endif