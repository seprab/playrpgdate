
#include "Utils.h"
char* Utils::Subchar(const char* source, int start, int end)
{
    int length = end - start;
    //allocating the memory for the name equals the length of the target + 1 for the null terminator
    auto subchar = new char[length+1];
    for(int i = 0; i < length; i++)
    {
        subchar[i] = source[start + i];
    }
    subchar[length] = '\0'; //null terminator
    return subchar;
}