
#include "json.h"

/** Public driver/fmt interface functions */

/* Serialize JSON to a corto object */
int16_t json_toObject(
    corto_fmt_opt* opt,
    corto_object *o,
    char *json)
{
    JSON_Value* topValue = json_parse_string(json);
    if (!topValue) {
        corto_throw("failed to parse '%s'", json);
        goto error;
    }

    if (json_value_get_type(topValue) == JSONArray) {
        JSON_Array *array = json_value_get_array(topValue);
        size_t count = json_array_get_count(array);
        size_t i;

        for (i = 0; i < count; i++) {
            JSON_Value *elem = json_array_get_value(array, i);

            if (json_value_get_type(elem) != JSONObject) {
                corto_throw(
                    "invalid element in JSON array, expected object");
                goto error;
            }

            if (json_serialize_from_JSON_Value(o, elem, json)) {
                goto error;
            }
        }
    } else if (json_value_get_type(topValue) == JSONObject) {
        if (json_serialize_from_JSON_Value(o, topValue, json)) {
            goto error_toResultMeta;
        }
    }

    json_value_free(topValue);

    return 0;
error:
error_toResultMeta:
    if (topValue) json_value_free(topValue);
    return -1;
}

/* Serialize a corto object to a JSON string */
corto_string json_fromObject(
    corto_fmt_opt* opt,
    corto_object o)
{
    corto_buffer buff = CORTO_BUFFER_INIT;
    corto_buffer_appendstr(&buff, "{");

    corto_buffer_append(
        &buff, "\"type\":\"%s\"", corto_fullpath(NULL, corto_typeof(o)));

    if (corto_check_attr(o, CORTO_ATTR_NAMED)) {
        corto_buffer_append(&buff, ",\"id\":\"%s\"", corto_idof(o));
        corto_buffer_append(
            &buff, ",\"parent\":\"%s\"",
            corto_fullpath(NULL, corto_parentof(o)));
    }

    if (corto_typeof(o)->kind != CORTO_VOID) {
        corto_value v = corto_value_object(o, NULL);
        corto_string json = json_serialize(&v);
        corto_buffer_append(&buff, ",\"value\":%s", json);
        corto_dealloc(json);
    }

    corto_buffer_appendstr(&buff, "}");

    return corto_buffer_str(&buff);
}

/* Serialize corto_value to JSON */
char* json_fromValue(
    corto_fmt_opt* opt,
    corto_value *v)
{
    return json_serialize(v);
}

/* Deserialize JSON to corto_value */
int16_t json_toValue(
    corto_fmt_opt* opt,
    corto_value *v,
    char *json)
{
    return json_deserialize(v, json);
}

/* Copy JSON string */
char* json_copy(
    char *json)
{
    return corto_strdup(json);
}

/* Free JSON string */
void json_release(
    char *json)
{
    free(json);
}

int cortomain(int argc, char* argv[])
{
    CORTO_UNUSED(argc);
    CORTO_UNUSED(argv);
    return 0;
}
