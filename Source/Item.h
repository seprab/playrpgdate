#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <pd_api/pd_api_json.h>
#include <pd_api.h>
#include "Entity.h"

class EntityManager;

class Item : public Entity
{
private:
    char* name{};
    char* description{};
    static PlaydateAPI* pd;

    static int readfile(void *readud, uint8_t *buf, int bufsize);
    static void DecoderError(json_decoder *, const char *error, int line);
    static void DidDecodeTableValue(json_decoder *, const char *message, json_value outval);
    static void DidDecodeArrayValue(json_decoder *, int count, json_value outval);
    static void WillDecodeSublist(json_decoder *decoder, const char *name, json_value_type type);
    static int ShouldDecodeTableValueForKey(json_decoder *decoder, const char *key);
    static int ShouldDecodeArrayValueAtIndex(json_decoder *decoder, int index);
    static void *DidDecodeSublist(json_decoder *decoder, const char *name, json_value_type type);

public:
    explicit Item(std::string _id, PlaydateAPI* _pd, const char* fileName);
    [[nodiscard]] const char * GetName() const;
    [[nodiscard]] const char * GetDescription() const;

    void SetName(char * _name);
    void SetDescription(char * _description);
};

#endif