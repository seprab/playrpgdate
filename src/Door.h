#ifndef DOOR_H
#define DOOR_H

#include <utility>
#include "Entity.h"

class Door : public Entity
{
private:
    bool locked{};
    int key{};
    std::pair<int, int> areas;

public:
    Door()=default;
    Door(const Door& other);
    Door(const Door&& other) noexcept ;
    Door(unsigned int _id, bool _locked, int _keyId, int _areaA, int _areaB);

    [[nodiscard]] bool GetLocked() const;
    void SetLocked(int _locked);
    [[nodiscard]] int GetKey() const;
    [[nodiscard]] std::pair<int, int> GetAreas() const;
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
};

#endif