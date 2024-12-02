#include <cstring>
#include <utility>
#include "Dialogue.h"
#include "Utils.h"

Dialogue::Dialogue(char* _description, std::vector<Choice> _choices)
        : description(_description), choices(std::move(_choices))
{

}
Dialogue::Dialogue(Dialogue &&other) noexcept
: description(other.description), choices(other.choices)
{

}
void Dialogue::AddChoice(Choice choice)
{
    choices.push_back(choice);
}

unsigned int Dialogue::Size() const
{
    return choices.size();
}

int Dialogue::Activate() const
{
    std::cout << description << std::endl;

    unsigned int number = 1;
    for (const Choice choice : choices)
        std::cout << number++ << ": " << choice.text << std::endl;

    unsigned int userInput = 0;
    while (true)
    {
        std::cin >> userInput;
        if (userInput >= 0 && userInput <= choices.size())
            return userInput;
    }
}

Dialogue::Dialogue(const char *buffer, const jsmntok_t *tokens, int& choicesIndex)
{
    char* _description{};
    std::vector<Choice> _choices{};
    const int dialogueEnd = tokens[choicesIndex].end;
    choicesIndex++;
    while(tokens[choicesIndex].end < dialogueEnd)
    {
        if(tokens[choicesIndex].type != JSMN_STRING && tokens[choicesIndex].type != JSMN_PRIMITIVE)
        {
            choicesIndex++;
            continue;
        }
        char* parseProperty = Utils::Subchar(buffer, tokens[choicesIndex].start, tokens[choicesIndex].end);

        if(strcmp(parseProperty, "description") == 0)
        {
            _description = Utils::Subchar(buffer, tokens[choicesIndex+1].start, tokens[choicesIndex+1].end);
            choicesIndex+=2;
        }
        else if(strcmp(parseProperty, "choices") == 0)
        {
            choicesIndex++; //move into the choices array token
            int endOfChoices = tokens[choicesIndex].end;
            choicesIndex++; //move into the first choice
            while(tokens[choicesIndex].end < endOfChoices)
            {
                if(tokens[choicesIndex].type != JSMN_STRING)
                {
                    choicesIndex++;
                    continue;
                }
                parseProperty = Utils::Subchar(buffer, tokens[choicesIndex].start, tokens[choicesIndex].end);
                if(strcmp(parseProperty, "choice") == 0)
                {
                    const char* text = Utils::Subchar(buffer, tokens[choicesIndex+1].start, tokens[choicesIndex+1].end);
                    const char* action = Utils::Subchar(buffer, tokens[choicesIndex+3].start, tokens[choicesIndex+3].end);
                    const char* target = Utils::Subchar(buffer, tokens[choicesIndex+5].start, tokens[choicesIndex+5].end);

                    Choice choice{};
                    choice.text = text;
                    choice.action = std::stoi(action);
                    choice.target = std::stoi(target);
                    _choices.push_back(choice);
                    choicesIndex+=6;
                }
                choicesIndex++;
            }
        }
    }
    description = _description;
    choices = _choices;
    choicesIndex--; //decrement the index to point to the end of the dialogue object
}


