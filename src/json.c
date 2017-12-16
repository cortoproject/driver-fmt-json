
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "driver/fmt/json/json.h"

corto_int16 serializeNumber(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    CORTO_UNUSED(data);

    corto_type t = corto_value_typeof(value);

    /* JSON doesn't support hex notation, so convert to integer */
    if (corto_primitive(t)->kind == CORTO_BINARY) {
        t = corto_type(corto_uint64_o);
    }

    corto_void  *v = corto_value_ptrof(value);

    corto_int16 result = corto_ptr_cast(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        out);

    if (!strcmp(*out, "nan")) {
        corto_ptr_setstr(out, "null");
    }

    return result;
}

static corto_int16 serializeConstant(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    corto_string raw;
    corto_void *v = corto_value_ptrof(value);
    corto_type t = corto_value_typeof(value);

    CORTO_UNUSED(data);

    if (corto_ptr_cast(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        &raw))
    {
        goto error;
    }

    *out = corto_asprintf( "\"%s\"", raw);
    corto_dealloc(raw);


    return 0;
error:
    corto_dealloc(raw);
    return -1;
}

corto_int16 serializeBinary(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    return serializeNumber(value, out, data);
}

corto_int16 serializeBitmask(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    return serializeConstant(value, out, data);
}

corto_int16 serializeEnum(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    return serializeConstant(value, out, data);
}

corto_int16 serializeBoolean(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    CORTO_UNUSED(data);

    corto_bool b = *(corto_bool *)corto_value_ptrof(value);
    if (b) {
        *out = corto_strdup("true");
    } else {
        *out = corto_strdup("false");
    }

    return 0;
}

corto_int16 serializeText(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    CORTO_UNUSED(data);
    corto_type type = corto_value_typeof(value);
    corto_void *v = corto_value_ptrof(value);
    corto_primitiveKind kind = corto_primitive(type)->kind;
    corto_bool quotes = TRUE;

    if (kind == CORTO_CHARACTER || (kind == CORTO_TEXT && (*(corto_string *)v)))
    {
        corto_string raw = NULL;
        size_t length;
        if (corto_ptr_cast(
            corto_primitive(type),
            v,
            corto_primitive(corto_string_o), &raw))
        {
            goto error;
        }
        if (corto_typeof(type) != corto_type(corto_verbatim_o) || strcmp(corto_verbatim(type)->contentType, "text/json")) {
            length = stresc(NULL, 0, raw);
            *out = corto_alloc(length + 3);
            stresc(*out + 1, length, raw);
        } else {
            /* If string is a verbatim JSON string, copy string as-is */
            length = strlen(raw);
            *out = corto_alloc(length + 3);
            strcpy(*out, raw);
            quotes = FALSE;
        }
        if (quotes) {
            (*out)[0] = '"';
            (*out)[length + 1] = '"';
            (*out)[length + 2] = '\0';
        }
        corto_dealloc(raw);
    } else {
        *out = corto_alloc(sizeof("null"));
        strcpy(*out, "null");
    }

    return 0;
error:
    return -1;
}

static corto_int16 serializePrimitive(corto_walk_opt* s, corto_value *v, void *userData)
{
    CORTO_UNUSED(s);
    corto_type type = corto_value_typeof(v);
    json_ser_t *data = userData;
    corto_int16 result = 0;
    corto_string valueString;

    switch (corto_primitive(type)->kind) {
        case CORTO_BINARY:
            result = serializeBinary(v, &valueString, data);
            break;
        case CORTO_BITMASK:
            result = serializeBitmask(v, &valueString, data);
            break;
        case CORTO_BOOLEAN:
            result = serializeBoolean(v, &valueString, data);
            break;
        case CORTO_ENUM:
            result = serializeEnum(v, &valueString, data);
            break;
        case CORTO_CHARACTER:
        case CORTO_TEXT:
            result = serializeText(v, &valueString, data);
            break;
        case CORTO_UINTEGER:
        case CORTO_INTEGER:
        case CORTO_FLOAT:
            result = serializeNumber(v, &valueString, data);
            break;
    }
    if (result) {
        goto error;
    }
    if (!corto_buffer_appendstr(&data->buffer, valueString)) {
        goto finished;
    }
    corto_dealloc(valueString);
    return 0;
finished:
    corto_dealloc(valueString);
    return 1;
error:
    return -1;
}

static corto_int16 serializeReference(corto_walk_opt* s, corto_value *v, void *userData)
{
    CORTO_UNUSED(s);

    json_ser_t *data;
    void *o;
    corto_object object;
    corto_id id;

    data = userData;
    o = corto_value_ptrof(v);
    object = *(corto_object*)o;

    if (object) {
        if (corto_checkAttr(object, CORTO_ATTR_NAMED) || (corto_value_objectof(v) == object)) {
            corto_uint32 length;
            corto_fullpath(id, object);

            /* Escape value */
            corto_string escapedValue = corto_alloc((length = stresc(NULL, 0, id)) + 1);
            stresc(escapedValue, length + 1, id);

            if (!corto_buffer_append(&data->buffer, "\"%s\"", escapedValue)) {
                corto_dealloc(escapedValue);
                goto finished;
            }

            corto_dealloc(escapedValue);
        } else {
            corto_id type;
            corto_fullpath(type, corto_typeof(object));

            if (!corto_buffer_append(&data->buffer, "{\"type\":\"%s\",\"value\":", type)) {
                goto finished;
            }
            if (corto_walk(s, object, data)) {
                goto error;
            }
            if (!corto_buffer_appendstr(&data->buffer, "}")) {
                goto finished;
            }
        }
    } else {
        if (!corto_buffer_appendstr(&data->buffer, "null")) {
            goto finished;
        }
    }
    return 0;
finished:
    return 1;
error:
    return -1;
}

static corto_int16 serializeItem(corto_walk_opt* s, corto_value *info, void *userData)
{
    json_ser_t *data = userData;

    if (data->itemCount && !corto_buffer_appendstr(&data->buffer, ",")) {
        goto finished;
    }
    if (info->kind == CORTO_MEMBER) {
        corto_member member = info->is.member.t;
        corto_string name = corto_idof(member);
        if (!corto_buffer_append(&data->buffer, "\"%s\":", name)) {
            goto finished;
        }
    }
    if (corto_walk_value(s, info, userData)) {
        goto error;
    }

    data->itemCount += 1;

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeComplex(corto_walk_opt* s, corto_value* v, void* userData)
{
    json_ser_t privateData, *data = userData;
    corto_type type = corto_value_typeof(v);
    corto_bool useCurlyBraces = TRUE;

    if (type->kind == CORTO_COLLECTION && corto_collection(type)->kind != CORTO_MAP) {
        useCurlyBraces = FALSE;
    }

    privateData.buffer = CORTO_BUFFER_INIT;
    privateData.itemCount = 0;

    if (!corto_buffer_appendstr(&privateData.buffer, (useCurlyBraces ? "{" : "["))) {
        goto finished;
    }
    if (type->kind == CORTO_COMPOSITE) {
        if (corto_interface(type)->kind == CORTO_UNION) {
            corto_int32 *d = corto_value_ptrof(v);
            corto_buffer_append(&privateData.buffer, "\"_d\":");
            corto_value discriminatorValue = corto_value_value(d, corto_union(type)->discriminator);
            if (serializePrimitive(s, &discriminatorValue, &privateData)) {
                goto error;
            }
            privateData.itemCount = 1;
        }
        if (corto_walk_members(s, v, &privateData)) {
            goto error;
        }
    } else if (type->kind == CORTO_COLLECTION) {
        if (corto_walk_elements(s, v, &privateData)) {
            goto error;
        }
    } else {
        goto error;
    }

    if (!corto_buffer_appendstr(&privateData.buffer, (useCurlyBraces ? "}" : "]"))) {
        goto finished;
    }

    corto_string str = corto_buffer_str(&privateData.buffer);
    corto_buffer_appendstr(&data->buffer, str);
    corto_dealloc(str);

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeBase(corto_walk_opt* s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;

    if (!corto_buffer_append(&data->buffer, "\"super\":")) {
        goto finished;
    }
    if (corto_walk_value(s, v, userData)) {
        goto error;
    }
    data->itemCount += 1;

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeVoid(corto_walk_opt* s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;

    CORTO_UNUSED(s);
    CORTO_UNUSED(v);

    if (!corto_buffer_appendstr(&data->buffer, "{}")) {
        goto finished;
    }

    return 0;
finished:
    return 1;
}

static corto_int16 serializeAny(corto_walk_opt* s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;
    corto_any *ptr = corto_value_ptrof(v);
    corto_int16 result = 0;

    CORTO_UNUSED(s);
    CORTO_UNUSED(v);

    if (!corto_buffer_append(
        &data->buffer,
        "{\"type\":\"%s\",\"value\":",
        corto_fullpath(NULL, ptr->type)))
    {
        goto finished;
    }

    corto_value anyValue = corto_value_value(ptr->value, ptr->type);
    if ((result = corto_walk_value(s, &anyValue, userData))) {
        goto done;
    }

    if (!corto_buffer_appendstr( &data->buffer, "}")) {
        goto finished;
    }

done:
    return result;
finished:
    return 1;
}

static corto_int16 serializeObject(corto_walk_opt* s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;

    if (corto_value_typeof(v)->kind != CORTO_VOID) {
        if (corto_walk_value(s, v, userData)) {
            goto error;
        }
     } else {
        if (!corto_buffer_appendstr(&data->buffer, "")) {
            goto finished;
        }
     }

    return 0;
error:
    return -1;
finished:
    return 1;
}

corto_walk_opt corto_json_ser(corto_modifier access, corto_operatorKind accessKind, corto_walk_traceKind trace)
{
    corto_walk_opt s;

    corto_walk_init(&s);
    s.access = access;
    s.accessKind = accessKind;
    s.traceKind = trace;
    s.program[CORTO_VOID] = serializeVoid;
    s.program[CORTO_ANY] = serializeAny;
    s.program[CORTO_PRIMITIVE] = serializePrimitive;
    s.reference = serializeReference;
    s.program[CORTO_COMPOSITE] = serializeComplex;
    s.program[CORTO_COLLECTION] = serializeComplex;
    s.metaprogram[CORTO_ELEMENT] = serializeItem;
    s.metaprogram[CORTO_MEMBER] = serializeItem;
    s.metaprogram[CORTO_BASE] = serializeBase;
    s.metaprogram[CORTO_OBJECT] = serializeObject;

    return s;
}

corto_string json_serialize(corto_value *v)
{
    corto_log_push("json");
    corto_walk_opt serializer = corto_json_ser(
      CORTO_PRIVATE, CORTO_NOT, CORTO_WALK_TRACE_NEVER
    );
    serializer.aliasAction = CORTO_WALK_ALIAS_IGNORE;
    serializer.optionalAction = CORTO_WALK_OPTIONAL_IF_SET;
    json_ser_t jsonData = {CORTO_BUFFER_INIT, 0};
    corto_walk_value(&serializer, v, &jsonData);
    corto_string result = corto_buffer_str(&jsonData.buffer);
    corto_debug("serialized %s", result);
    corto_log_pop();
    return result;
}

corto_string json_fromValue(corto_value *v)
{
    return json_serialize(v);
}

corto_int16 json_toValue(corto_value *v, corto_string json)
{
    return json_deserialize(v, json);
}

corto_string json_copy(corto_string json)
{
    return corto_strdup(json);
}

void json_release(corto_string json)
{
    corto_dealloc(json);
}

/*
 * This method changes the id!
 */
static void json_splitId(char* fullpath, char** parent_out, char** id_out)
{
    char *ptr = strrchr(fullpath, '/');
    if (ptr) {
        *id_out = ptr + 1;
        *parent_out = fullpath;
        *ptr = '\0';
    } else {
        *id_out = fullpath;
        *parent_out = "/";
    }
}

static corto_object json_declare(corto_result *r)
{
    corto_object o = NULL;
    corto_object type = corto_resolve(NULL, r->type);
    if (!type) {
        corto_throw("cannot find '%s'", r->type);
        goto errorTypeNotFound;
    }
    if (!corto_instanceof(corto_type_o, type)) {
        corto_throw("'%s' is not a type", r->type);
        goto errorNotType;
    }

    if (r->id && r->parent) {
        corto_id fullId;
        if (strcmp(r->parent, "/")) {
            sprintf(fullId, "%s/%s", r->parent, r->id);
        } else {
            strcpy(fullId, r->id);
        }
        o = corto_declareChild(root_o, fullId, type);
        if (!o) {
            corto_throw("failed to create '%s': %s", corto_lasterr());
        }
        corto_release(type);
    } else {
        o = corto_declare(type);
        corto_release(type);
    }

    return o;
errorNotType:
    corto_release(type);
errorTypeNotFound:
    return NULL;
}

static corto_int16 json_toResultMeta(corto_result *r, JSON_Value *topValue, corto_string json) {

    if (!topValue) {
        corto_throw("error parsing '%s'", json);
        goto error;
    }

    JSON_Object* topObject = json_value_get_object(topValue);
    if (!topObject) {
        corto_throw("'%s' is not a JSON object", json);
        goto error;
    }

    const char* type = json_object_get_string(topObject, "type");
    if (!type) {
        corto_throw("missing 'type' field in '%s'", json);
        goto error;
    }

    const char* fullId = json_object_get_string(topObject, "id");

    /* Id is optional */

    char *parent = NULL, *id = NULL, *fullIdCpy = NULL;
    if (fullId) {
        fullIdCpy = corto_strdup(fullId);
        json_splitId(fullIdCpy, &parent, &id);
    }

    r->id = id ? corto_strdup(id) : NULL;
    r->parent = parent ? corto_strdup(parent) : NULL;
    r->type = (char*)type;

    if (fullIdCpy) corto_dealloc(fullIdCpy);

    return 0;
error:
    return -1;
}

corto_int16 json_toResult(corto_result *r, corto_string json) {
    JSON_Value* topValue = json_parse_string(json);
    if (!topValue) {
        corto_throw("error parsing JSON %s", json);
        goto error;
    }

    memset(r, 0, sizeof(corto_result));
    if (json_toResultMeta(r, topValue, json)) {
        goto error_toResultMeta;
    }

    r->type = corto_strdup(r->type);

    JSON_Object *object = json_value_get_object(topValue);
    JSON_Value *value = json_object_get_value(object, "value");
    r->value = (corto_word)json_serialize_to_string(value);

    json_value_free(topValue);
    return 0;

error:
error_toResultMeta:
    json_value_free(topValue);
    return -1;
}

corto_word json_fromResult(corto_result *r) {
    CORTO_UNUSED(r);
    return 0;
}

static
corto_int16 json_toObject_fromJsonObject(
    corto_object *o,
    JSON_Value *topValue,
    char *json)
{
    corto_result r;
    corto_object result = NULL;
    corto_bool newObject = FALSE;

    JSON_Object* topObject = json_value_get_object(topValue);

    memset(&r, 0, sizeof(corto_result));
    if (json_toResultMeta(&r, topValue, json)) {
        goto error_toResultMeta;
    }

    if (!o || !*o) {
        result = json_declare(&r);
        if (!result) {
            goto errorDeclare;
        }
        newObject = TRUE;
    } else {
        corto_type t = corto_resolve(NULL, r.type);
        if (t) {
            if (corto_typeof(*o) != t) {
                corto_throw("object '%s' is not of type '%s' (from '%s')",
                  corto_fullpath(NULL, *o),
                  corto_fullpath(NULL, t),
                  json);
                goto errorDeclare;
            }
            corto_release(t);
        } else {
            corto_throw("unresolved type '%s' from '%s'", r.type, json);
            goto errorDeclare;
        }
        if (o) {
            result = *o;
        }
    }

    if (result) {
        if (corto_isbuiltin(result)) {
            corto_throw("cannot deserialize JSON to builtin object '%s'",
                corto_fullpath(NULL, result));
            goto error_toResultMeta;
        }
    }

    JSON_Value* jsonValue = json_object_get_value(topObject, "value");
    if (jsonValue) {
        corto_value cortoValue = corto_value_object(result, NULL);
        if (json_deserialize_from_JSON_Value(&cortoValue, jsonValue)) {
            goto errorDeserialize;
        }
    }

    if (newObject) {
        if (corto_define(result)) {
            goto errorDefine;
        }
    }

    corto_dealloc(r.id);
    corto_dealloc(r.parent);

    if (o) {
        *o = result;
    }

    return 0;
errorDeserialize:
errorDefine:
    if (newObject) {
        corto_delete(result);
    }
    if (o) {
        *o = NULL;
    }
errorDeclare:
    corto_dealloc(r.id);
    corto_dealloc(r.parent);
error_toResultMeta:
    return -1;
}

corto_int16 json_toObject(corto_object *o, corto_string json)
{
    JSON_Value* topValue = json_parse_string(json);
    if (!topValue) {
        corto_throw("failed to parse %s", json);
        goto error;
    }

    if (json_value_get_type(topValue) == JSONArray) {
        JSON_Array *array = json_value_get_array(topValue);
        size_t count = json_array_get_count(array);
        size_t i;

        for (i = 0; i < count; i++) {
            JSON_Value *elem = json_array_get_value(array, i);

            if (json_value_get_type(elem) != JSONObject) {
                corto_throw("invalid element in JSON array, expected object");
                goto error;
            }

            if (json_toObject_fromJsonObject(o, elem, json)) {
                goto error;
            }
        }
    } else if (json_value_get_type(topValue) == JSONObject) {
        if (json_toObject_fromJsonObject(o, topValue, json)) {
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

corto_string json_fromObject(corto_object o) {
    corto_buffer buff = CORTO_BUFFER_INIT;
    corto_buffer_appendstr(&buff, "{");

    corto_buffer_append(&buff, "\"type\":\"%s\"", corto_fullpath(NULL, corto_typeof(o)));

    if (corto_checkAttr(o, CORTO_ATTR_NAMED)) {
        corto_buffer_append(&buff, ",\"id\":\"%s\"", corto_idof(o));
        corto_buffer_append(&buff, ",\"parent\":\"%s\"", corto_fullpath(NULL, corto_parentof(o)));
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

int cortomain(int argc, char* argv[])
{
    CORTO_UNUSED(argc);
    CORTO_UNUSED(argv);
    return 0;
}
