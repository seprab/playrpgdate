#ifndef ENTITY_H
#define ENTITY_H
#include <memory>
#include <pd_api/pd_api_gfx.h>
#include "jsmn.h"
#include "pdcpp/graphics/Point.h"

class EntityManager;


class Entity
{
public:
    explicit Entity() = default;
    explicit Entity(unsigned int _id);
    Entity(const Entity& other) = default;
    Entity(Entity&& other) noexcept = default;
    virtual ~Entity() = default;



    [[nodiscard]] unsigned int GetId() const {return id;}
    [[nodiscard]] const char * GetName() const {return name;}
    [[nodiscard]] const char * GetDescription() const {return description;}
    [[nodiscard]] const char * GetImagePath() const {return image_path;}
    [[nodiscard]] pdcpp::Point<int> GetPosition() const {return position;}
    [[nodiscard]] pdcpp::Point<int> GetCenteredPosition() const {return {position.x + size.x / 2, position.y + size.y / 2};}
    [[nodiscard]] pdcpp::Point<int> GetSize() const {return size;}
    [[nodiscard]] float GetMaxHP() const {return maxHP;}
    [[nodiscard]] float GetHP() const {return hp;}

    void SetHP(float _hp) {hp = _hp;}
    void SetMaxHP(float _maxHP) {maxHP = _maxHP;}
    void SetName(const char* _name) {name = _name;}
    void SetDescription(char* _description) {description = _description;}
    void SetImagePath(char* _image_path) {image_path = _image_path;}
    void SetPosition(pdcpp::Point<int> _position) {position = _position;}
    void SetSize(pdcpp::Point<int> _size) {size = _size;}

    void LoadBitmap();
    void LoadBitmap(char* path);
    void DrawBitmap();
    void DrawBitmap(int x, int y);

    virtual std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) = 0;

private:
    unsigned int id{};
    const char* name{};
    char* image_path{};
    pdcpp::Point<int> position = pdcpp::Point<int>(0, 0);
    pdcpp::Point<int> size = pdcpp::Point<int>(0, 0);
    float hp{};
    float maxHP{};
    char* description{};
    LCDBitmap* bitmap{};
};

#endif