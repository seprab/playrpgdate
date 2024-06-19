#include <pd_api/pd_api_file.h>
#include <utility>
#include "Item.h"

PlaydateAPI* Item::pd = nullptr;

Item::Item(std::string _id, PlaydateAPI* _pd, const char* fileName) :
Entity(std::move(_id))
{
    pd = _pd;
    SDFile* file = pd->file->open(fileName, kFileRead);
    if(file==nullptr) {
        pd->system->logToConsole("Error: Could not open file %s", fileName);
        pd->system->logToConsole("File Error: %s", pd->file->geterr());
        return;
    }

    json_decoder decoder;
    decoder.decodeError = &DecoderError;
    decoder.didDecodeSublist = DidDecodeSublist;
    decoder.willDecodeSublist = &WillDecodeSublist;
    decoder.didDecodeTableValue = &DidDecodeTableValue;
    decoder.didDecodeArrayValue = &DidDecodeArrayValue;
    decoder.shouldDecodeTableValueForKey = &ShouldDecodeTableValueForKey;
    decoder.shouldDecodeArrayValueAtIndex = &ShouldDecodeArrayValueAtIndex;
    decoder.userdata = this;

    json_reader reader;
    reader.read = readfile;
    reader.userdata = file;

    pd->json->decode( &decoder, reader, nullptr );
    pd->file->close(file);
}
const char* Item::GetName() const
{
    return name;
}

const char* Item::GetDescription() const
{
    return description;
}


void Item::SetName(char *string) {
    name = string;
}

void Item::DecoderError(json_decoder* decoder, const char *error, int line)
{
    pd->system->logToConsole("Error: %s at line %i", error, line);
}
void Item::DidDecodeTableValue(json_decoder* decoder, const char *key, json_value value)
{
    pd->system->logToConsole("DidDecodeTableValue key: %s", key);
}
void Item::DidDecodeArrayValue(json_decoder* decoder, int pos, json_value _value)
{
    pd->system->logToConsole("DidDecodeArrayValue position: %i", pos);
    pd->system->logToConsole("DidDecodeArrayValue value: %s", json_stringValue(_value));
}
int Item::ShouldDecodeTableValueForKey( json_decoder* decoder, const char* key )
{
    pd->system->logToConsole("ShouldDecodeTableValueForKey: %s", key);
    return 1;
}
int Item::ShouldDecodeArrayValueAtIndex( json_decoder* decoder, int index )
{
    pd->system->logToConsole("ShouldDecodeArrayValueAtIndex: %i", index);
    return 1;
}
void Item::WillDecodeSublist(json_decoder* decoder, const char* name, json_value_type type)
{
    pd->system->logToConsole("WillDecodeSublist Name: %s", name);
}
void *Item::DidDecodeSublist(json_decoder* decoder, const char *name, json_value_type type) {
    pd->system->logToConsole("DidDecodeSublist: %s", name);
    return decoder->userdata;
}
int Item::readfile(void* _readud, uint8_t* buf, int _bufsize)
{
    return pd->file->read((SDFile*)_readud, buf, _bufsize);
}

void Item::SetDescription(char *_description) {
    description = _description;
}
