#include "Dialogue.h"

Dialogue::Dialogue()
{

}

Dialogue::Dialogue(char* _description, std::vector<char*> _choices)
        : description(_description), choices(_choices)
{

}

void Dialogue::AddChoice(char* choice)
{
    choices.push_back(choice);
}

unsigned int Dialogue::Size()
{
    return choices.size();
}

int Dialogue::Activate()
{
    std::cout << description << std::endl;

    unsigned int number = 1;
    for (auto choice : choices)
        std::cout << number++ << ": " << choice << std::endl;

    unsigned int userInput = 0;
    while (true)
    {
        std::cin >> userInput;
        if (userInput >= 0 && userInput <= choices.size())
            return userInput;
    }
}