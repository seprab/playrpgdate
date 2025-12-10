#ifndef ENTITY_H
#define ENTITY_H
#include <memory>
#include <string>
#include <unordered_map>
#include <pd_api/pd_api_gfx.h>
#include "jsmn.h"
#include "pdcpp/graphics/Point.h"

class EntityManager;


class Entity
{
protected:
    static std::unordered_map<std::string, LCDBitmap*> bitmapCache;

public:
    explicit Entity() = default;
    explicit Entity(unsigned int _id);
    Entity(const Entity& other) = default;
    Entity(Entity&& other) noexcept = default;
    virtual ~Entity() = default;

    [[nodiscard]] unsigned int GetId() const {return id;}
    [[nodiscard]] const char* GetName() const {return name.c_str();}
    [[nodiscard]] const char* GetDescription() const {return description.c_str();}
    [[nodiscard]] const char* GetImagePath() const {return image_path.c_str();}
    [[nodiscard]] pdcpp::Point<int> GetPosition() const {return position;}
    [[nodiscard]] pdcpp::Point<int> GetTiledPosition() const {return tiledPosition;}
    [[nodiscard]] pdcpp::Point<int> GetCenteredPosition() const {return {position.x + size.x / 2, position.y + size.y / 2};}
    [[nodiscard]] pdcpp::Point<int> GetSize() const {return size;}
    [[nodiscard]] float GetMaxHP() const {return maxHP;}
    [[nodiscard]] float GetHP() const {return hp;}

    void SetHP(float _hp) {hp = _hp;}
    void SetMaxHP(float _maxHP) {maxHP = _maxHP;}
    void SetName(const std::string& _name) {name = _name;}
    void SetDescription(const std::string& _description) {description = _description;}
    void SetImagePath(const std::string& _image_path) {image_path = _image_path;}
    void SetPosition(pdcpp::Point<int> _position);
    void SetTiledPosition(pdcpp::Point<int> _tiledPosition);
    void SetSize(pdcpp::Point<int> _size) {size = _size;}

    void Damage(float damage);
    void Heal(float heal);

    virtual void Draw();
    void DrawHealthBar() const;

    void LoadBitmap();
    void LoadBitmap(const std::string& path);
    void DrawBitmap() const;
    void DrawBitmap(int x, int y);
    bool CalculateFlashing();
    [[nodiscard]] bool IsAlive() const { return hp > 0; }

    virtual std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) = 0;

private:
    unsigned int id{};
    std::string name;
    std::string image_path;
    pdcpp::Point<int> position = pdcpp::Point<int>(0, 0);
    pdcpp::Point<int> tiledPosition{0, 0}; // Position in tiles
    pdcpp::Point<int> size = pdcpp::Point<int>(0, 0);
    float hp{};
    float maxHP{};
    std::string description;
    LCDBitmap* bitmap{};
    bool isBitmapVisible = true;
    int flashTimer = 0;
    bool isFlashing = false;
};

#endif