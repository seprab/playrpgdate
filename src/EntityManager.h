#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <memory>
#include <utility>
#include <pd_api.h>
#include "jsmn.h"
#include "UI.h"
#include "Player.h"

class EntityManager
{
private:
    static EntityManager* instance;
    std::map<unsigned int, std::shared_ptr<void>> data;
    std::shared_ptr<Player> player;

public:
    explicit EntityManager();

    // Deleted copy constructor and assignment operator
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    // Method to get the instance of the class
    static EntityManager* GetInstance();

    ~EntityManager();

    template <typename T>
    void LoadJSON(const char* fileName, int limitOfTokens = 128);

    template <typename T>
    void DecodeJson(jsmn_parser *parser, char *charBuffer, size_t len, int tokenLimit);

    std::shared_ptr<void> GetEntity(unsigned int id);
    [[nodiscard]] std::shared_ptr<Player> GetPlayer() const {return player;};
    void SetPlayer(const std::shared_ptr<Player>& Player){player = Player;}
};

#endif