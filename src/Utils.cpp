
#include "Utils.h"
char* Utils::Subchar(const char* source, int start, int end)
{
    int length = end - start;
    //allocating the memory for the name equals the length of the target + 1 for the null terminator
    char* sub = new char[length];
    for(int i = 0; i < length; i++)
    {
        sub[i] = source[start + i];
    }
    return sub;
}