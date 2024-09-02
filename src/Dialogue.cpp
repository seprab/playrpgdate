#include <cstring>
#include "Dialogue.h"
#include "Utils.h"

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

Dialogue::Dialogue(char *buffer)
{
    jsmn_parser p;
    jsmntok_t tokens[128]; //* We expect no more than 128 JSON tokens *//*
    jsmn_init(&p);
    int size = jsmn_parse(&p, buffer, sizeof(buffer), tokens, 128);
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            char* _description{};
            std::vector<char*> _choices{};
            int endOfObject = tokens[i].end;
            while(tokens[i].end < endOfObject)
            {
                i++;
                if(tokens[i].type != JSMN_STRING) continue;
                char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);

                if(strcmp(parseProperty, "description") == 0) _description = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                else if(strcmp(parseProperty, "choices") == 0)
                {
                    //TODO: Implement choices
                }
            };
            Dialogue dialogue(_description, _choices);
        }
    }
}
