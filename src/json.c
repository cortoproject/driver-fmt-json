
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

    corto_type t = corto_value_getType(value);

    /* JSON doesn't support hex notation, so convert to integer */
    if (corto_primitive(t)->kind == CORTO_BINARY) {
        t = corto_type(corto_uint64_o);
    }

    corto_void  *v = corto_value_getPtr(value);

    corto_int16 result = corto_convert(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        out);

    return result;
}

static corto_int16 serializeConstant(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    corto_string raw;
    corto_void *v = corto_value_getPtr(value);
    corto_type t = corto_value_getType(value);

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

    corto_bool b = *(corto_bool *)corto_value_getPtr(value);
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
    corto_type type = corto_value_getType(value);
    corto_void *v = corto_value_getPtr(value);
    corto_primitiveKind kind = corto_primitive(type)->kind;

    if (kind == CORTO_CHARACTER || (kind == CORTO_TEXT && (*(corto_string *)v)))
    {
        corto_string raw;
        size_t length;
        int needEscape = 0;
        if (corto_convert(
            corto_primitive(type),
            v,
            corto_primitive(corto_string_o), &raw))
        {
            goto error;
        }
        if (*raw == '@') {
            needEscape = 1;
        }
        length = stresc(NULL, 0, raw);
        *out = corto_alloc(length + 3 + needEscape);
        (*out)[0] = '"';
        (*out)[1] = '@';
        stresc(*out + 1 + needEscape, length, raw);
        (*out)[length + needEscape + 1] = '"';
        (*out)[length + needEscape + 2] = '\0';
        corto_dealloc(raw);
    } else {
        *out = corto_alloc(sizeof("null"));
        strcpy(*out, "null");
    }

    return 0;
error:
    return -1;
}

static corto_int16 serializePrimitive(corto_serializer s, corto_value *v, void *userData) {
    CORTO_UNUSED(s);
    corto_type type = corto_value_getType(v);
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
    if (!corto_buffer_append(&data->buffer, "%s", valueString)) {
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

static corto_int16 serializeReference(corto_serializer s, corto_value *v, void *userData) {
    CORTO_UNUSED(s);

    json_ser_t *data;
    void *o;
    corto_object object;
    corto_id id;

    data = userData;
    o = corto_value_getPtr(v);
    object = *(corto_object*)o;

    if (object) {
        if (corto_checkAttr(object, CORTO_ATTR_SCOPED) || (corto_value_getObject(v) == object)) {
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
            if (corto_serialize(s, object, data)) {
                goto error;
            }
        }
    } else {
        if (!corto_buffer_append(&data->buffer, "null")) {
            goto finished;
        }
    }
    return 0;
finished:
    return 1;
error:
    return -1;
}

static corto_int16 serializeItem(corto_serializer s, corto_value *info, void *userData) {
    json_ser_t *data = userData;

    if (data->itemCount && !corto_buffer_append(&data->buffer, ",")) {
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

static corto_int16 serializeComplex(corto_serializer s, corto_value* v, void* userData) {
    json_ser_t privateData, *data = userData;
    corto_type type = corto_value_getType(v);
    corto_bool useCurlyBraces = TRUE;

    if (type->kind == CORTO_COLLECTION && corto_collection(type)->kind != CORTO_MAP) {
        useCurlyBraces = FALSE;
    }

    privateData.buffer = CORTO_BUFFER_INIT;
    privateData.itemCount = 0;

    if (!corto_buffer_append(&privateData.buffer, (useCurlyBraces ? "{" : "["))) {
        goto finished;
    }
    if (type->kind == CORTO_COMPOSITE) {
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

    if (!corto_buffer_append(&privateData.buffer, (useCurlyBraces ? "}" : "]"))) {
        goto finished;
    }

    corto_string str = corto_buffer_str(&privateData.buffer);
    corto_buffer_append(&data->buffer, str);

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeBase(corto_serializer s, corto_value* v, void* userData) {
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

static corto_int16 serializeObject(corto_serializer s, corto_value* v, void* userData) {
    json_ser_t *data = userData;

    if (corto_value_getType(v)->kind != CORTO_VOID) {
        if (corto_serializeValue(s, v, userData)) {
            goto error;
        }
     } else {
        if (!corto_buffer_append(&data->buffer, "{}")) {
            goto finished;
        }
     }

    return 0;
error:
    return -1;
finished:
    return 1;
}

struct corto_serializer_s corto_json_ser(corto_modifier access, corto_operatorKind accessKind, corto_serializerTraceKind trace) {
    struct corto_serializer_s s;

    corto_serializerInit(&s);
    s.access = access;
    s.accessKind = accessKind;
    s.traceKind = trace;
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

corto_string json_serialize(corto_object o) {
    struct corto_serializer_s serializer = corto_json_ser(
      CORTO_PRIVATE, CORTO_NOT, CORTO_SERIALIZER_TRACE_NEVER
    );
    serializer.aliasAction = CORTO_SERIALIZER_ALIAS_IGNORE;
    serializer.optionalAction = CORTO_SERIALIZER_OPTIONAL_IF_SET;
    json_ser_t jsonData = {CORTO_BUFFER_INIT, 0};
    corto_serialize(&serializer, o, &jsonData);
    return corto_buffer_str(&jsonData.buffer);
}

corto_string json_fromCorto(corto_object o) {
    return json_serialize(o);
}

corto_int16 json_toCorto(corto_object o, corto_string json) {
    return json_deserialize(o, json);
}

void json_release(corto_string json) {
    corto_dealloc(json);
}

int jsonMain(int argc, char* argv[]) {
    CORTO_UNUSED(argc);
    CORTO_UNUSED(argv);
    return 0;
}
