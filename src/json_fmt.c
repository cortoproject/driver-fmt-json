
#include "json.h"

/** Public driver/fmt interface functions */

/* Serialize JSON to a corto object */
int16_t json_toObject(
    corto_fmt_opt* opt,
    corto_object *o,
    char *json)
{
    JSON_Value* topValue = json_parse_string_with_comments(json);
    if (!topValue) {
        ut_throw("failed to parse '%s'", json);
        goto error;
    }

    if (json_value_get_type(topValue) == JSONArray) {
        JSON_Array *array = json_value_get_array(topValue);
        size_t count = json_array_get_count(array);
        size_t i;

        for (i = 0; i < count; i++) {
            JSON_Value *elem = json_array_get_value(array, i);

            if (json_value_get_type(elem) != JSONObject) {
                ut_throw(
                    "invalid element in JSON array, expected object");
                goto error;
            }

            if (json_serialize_from_JSON_Value(o, elem, json, opt)) {
                goto error;
            }
        }
    } else if (json_value_get_type(topValue) == JSONObject) {
        if (json_serialize_from_JSON_Value(o, topValue, json, opt)) {
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
    ut_strbuf buff = UT_STRBUF_INIT;
    ut_strbuf_appendstr(&buff, "{");

    ut_strbuf_append(
        &buff, "\"type\":\"%s\"", corto_fullpath(NULL, corto_typeof(o)));

    if (corto_check_attr(o, CORTO_ATTR_NAMED)) {
        ut_strbuf_append(&buff, ",\"id\":\"%s\"", corto_idof(o));
        ut_strbuf_append(
            &buff, ",\"parent\":\"%s\"",
            corto_fullpath(NULL, corto_parentof(o)));
    }

    if (corto_typeof(o)->kind != CORTO_VOID) {
        corto_value v = corto_value_object(o, NULL);
        corto_string json = json_serialize(&v, opt);
        ut_strbuf_append(&buff, ",\"value\":%s", json);
        corto_dealloc(json);
    }

    ut_strbuf_appendstr(&buff, "}");

    return ut_strbuf_get(&buff);
}

/* Serialize corto_value to JSON */
char* json_fromValue(
    corto_fmt_opt* opt,
    corto_value *v)
{
    return json_serialize(v, opt);
}

/* Deserialize JSON to corto_value */
int16_t json_toValue(
    corto_fmt_opt* opt,
    corto_value *v,
    char *json)
{
    return json_deserialize(v, json, opt);
}

/* Copy JSON string */
char* json_copy(
    char *json)
{
    return ut_strdup(json);
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
