
#include <driver/fmt/json/json.h>
#include <parson/parson.h>

/* JSON serializer data */
typedef struct json_ser_t {
    corto_buffer *buffer;
    unsigned int itemCount;
    corto_fmt_opt *opt;
} json_ser_t;

char* json_serialize(
    corto_value *v,
    corto_fmt_opt *opt);

int16_t json_serialize_from_JSON_Value(
    corto_object *o,
    JSON_Value *topValue,
    char *json,
    corto_fmt_opt *opt);

int16_t json_deserialize(
    corto_value *v,
    corto_string s,
    corto_fmt_opt *opt);

int16_t json_deserialize_from_JSON_Value(
    corto_value *v,
    JSON_Value *jsonValue,
    corto_fmt_opt *opt);
