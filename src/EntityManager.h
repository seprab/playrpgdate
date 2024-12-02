#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <memory>
#include <pd_api.h>
#include "jsmn.h"

class EntityManager
{
private:
    static EntityManager* instance;
    static PlaydateAPI* pd;
    std::map<unsigned int, std::shared_ptr<void>> data;


public:
    explicit EntityManager(PlaydateAPI* pdApi);

    // Deleted copy constructor and assignment operator
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    // Method to get the instance of the class
    static EntityManager* GetInstance();

    ~EntityManager();

    static PlaydateAPI* GetPD();

    template <typename T>
    void LoadJSON(const char* fileName, int limitOfTokens = 128);

    template <typename T>
    void DecodeJson(jsmn_parser *parser, char *charBuffer, size_t len, int tokenLimit);

    std::shared_ptr<void> GetEntity(unsigned int id);
};

#endif