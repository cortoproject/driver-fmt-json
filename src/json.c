
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "corto/fmt/json/json.h"

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

    corto_int16 result = corto_convert(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        out);

    if (!strcmp(*out, "nan")) {
        corto_setstr(out, "null");
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

    if (corto_convert(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        &raw))
    {
        goto error;
    }

    corto_asprintf(out, "\"%s\"", raw);
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
        if (corto_convert(
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

static corto_int16 serializePrimitive(corto_serializer s, corto_value *v, void *userData)
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

static corto_int16 serializeReference(corto_serializer s, corto_value *v, void *userData)
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
        if (corto_checkAttr(object, CORTO_ATTR_SCOPED) || (corto_value_objectof(v) == object)) {
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
            if (corto_serialize(s, object, data)) {
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

static corto_int16 serializeItem(corto_serializer s, corto_value *info, void *userData)
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
    if (corto_serializeValue(s, info, userData)) {
        goto error;
    }

    data->itemCount += 1;

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeComplex(corto_serializer s, corto_value* v, void* userData)
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
        if (corto_serializeMembers(s, v, &privateData)) {
            goto error;
        }
    } else if (type->kind == CORTO_COLLECTION) {
        if (corto_serializeElements(s, v, &privateData)) {
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

static corto_int16 serializeBase(corto_serializer s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;

    if (!corto_buffer_append(&data->buffer, "\"super\":")) {
        goto finished;
    }
    if (corto_serializeValue(s, v, userData)) {
        goto error;
    }
    data->itemCount += 1;

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeVoid(corto_serializer s, corto_value* v, void* userData)
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

static corto_int16 serializeAny(corto_serializer s, corto_value* v, void* userData)
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
    if ((result = corto_serializeValue(s, &anyValue, userData))) {
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

static corto_int16 serializeObject(corto_serializer s, corto_value* v, void* userData)
{
    json_ser_t *data = userData;

    if (corto_value_typeof(v)->kind != CORTO_VOID) {
        if (corto_serializeValue(s, v, userData)) {
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

struct corto_serializer_s corto_json_ser(corto_modifier access, corto_operatorKind accessKind, corto_serializerTraceKind trace)
{
    struct corto_serializer_s s;

    corto_serializerInit(&s);
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
    struct corto_serializer_s serializer = corto_json_ser(
      CORTO_PRIVATE, CORTO_NOT, CORTO_SERIALIZER_TRACE_NEVER
    );
    serializer.aliasAction = CORTO_SERIALIZER_ALIAS_IGNORE;
    serializer.optionalAction = CORTO_SERIALIZER_OPTIONAL_IF_SET;
    json_ser_t jsonData = {CORTO_BUFFER_INIT, 0};
    corto_serializeValue(&serializer, v, &jsonData);
    corto_string result = corto_buffer_str(&jsonData.buffer);
    corto_debug("json: serialized %s", result);
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
        corto_seterr("json: cannot find '%s'", r->type);
        goto errorTypeNotFound;
    }
    if (!corto_instanceof(corto_type_o, type)) {
        corto_seterr("json: '%s' is not a type", r->type);
        goto errorNotType;
    }

    if (r->id && r->parent) {
        corto_object parent = corto_lookup(root_o, r->parent);
        if (!parent) {
            corto_seterr("json: cannot find '%s'", r->parent);
            goto errorNoParent;
        }

        o = corto_declareChild(parent, r->id, type);
        corto_release(parent);
        corto_release(type);
    } else {
        o = corto_declare(type);
        corto_release(type);
    }

    return o;
errorNoParent:
errorNotType:
    corto_release(type);
errorTypeNotFound:
    return NULL;
}

static corto_int16 json_toResultMeta(corto_result *r, JSON_Value **topValue, JSON_Value **jsonValue, corto_string json) {
    *topValue = json_parse_string(json);
    if (!*topValue) {
        corto_seterr("json: error parsing '%s'", json);
        goto error;
    }

    JSON_Object* topObject = json_value_get_object(*topValue);
    if (!topObject) {
        corto_seterr("json: '%s' is not a JSON object", json);
        goto error;
    }

    const char* type = json_object_get_string(topObject, "type");
    if (!type) {
        corto_seterr("json: missing 'type' field in '%s'", json);
        goto error;
    }

    const char* fullId = json_object_get_string(topObject, "id");

    /* Id is optional */

    *jsonValue = json_object_get_value(topObject, "value");
    if (!*jsonValue) {
        corto_seterr("json: missing 'value' field in '%s'", json);
        goto error;
    }

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
    JSON_Value *topValue = NULL, *jsonValue = NULL;

    memset(r, 0, sizeof(corto_result));
    if (json_toResultMeta(r, &topValue, &jsonValue, json)) {
        goto error_toResultMeta;
    }

    r->type = corto_strdup(r->type);
    r->value = (corto_word)json_serialize_to_string(topValue);

    json_value_free(topValue);
    return 0;

error_toResultMeta:
    json_value_free(topValue);
    return -1;
}

corto_word json_fromResult(corto_result *r) {
    CORTO_UNUSED(r);
    return 0;
}

corto_int16 json_toObject(corto_object *o, corto_string json)
{
    corto_bool newObject = FALSE;
    corto_result r;
    JSON_Value *topValue = NULL, *jsonValue = NULL;

    memset(&r, 0, sizeof(corto_result));
    if (json_toResultMeta(&r, &topValue, &jsonValue, json)) {
        goto error_toResultMeta;
    }

    if (!*o) {
        *o = json_declare(&r);
        if (!*o) {
            goto errorDeclare;
        }
        newObject = TRUE;
    } else {
        corto_type t = corto_resolve(NULL, r.type);
        if (t) {
            if (corto_typeof(*o) != t) {
                corto_seterr("json: object '%s' is not of type '%s' (from '%s')",
                  corto_fullpath(NULL, *o),
                  corto_fullpath(NULL, t),
                  json);
                goto errorDeclare;
            }
            corto_release(t);
        } else {
            corto_seterr("json: unresolved type '%s' from '%s'", r.type, json);
            goto errorDeclare;
        }
    }

    corto_value cortoValue = corto_value_object(*o, NULL);
    if (json_deserialize_from_JSON_Value(&cortoValue, jsonValue)) {
        goto errorDeserialize;
    }

    if (newObject) {
        if (corto_define(*o)) {
            goto errorDefine;
        }
    }

    corto_dealloc(r.id);
    corto_dealloc(r.parent);
    json_value_free(topValue);

    return 0;
errorDeserialize:
errorDefine:
    if (newObject) {
        corto_delete(*o);
    }
    *o = NULL;
errorDeclare:
    corto_dealloc(r.id);
    corto_dealloc(r.parent);
error_toResultMeta:
    if (topValue) json_value_free(topValue);
    return -1;
}

corto_string json_fromObject(corto_object o) {
    corto_buffer buff = CORTO_BUFFER_INIT;
    corto_buffer_appendstr(&buff, "{");

    corto_buffer_append(&buff, "\"type\":\"%s\"", corto_fullpath(NULL, corto_typeof(o)));

    if (corto_checkAttr(o, CORTO_ATTR_SCOPED)) {
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
