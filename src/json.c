
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "corto/fmt/json/json.h"

/* agreed not to change anything except the first argument of the signature */
static corto_bool corto_ser_appendstrbuff(json_ser_t* data, char* str) {
    if (!data->ptr) {
        data->ptr = data->buffer;
    }
    if (!data->ptr) {
        data->buffer = corto_strdup(str);
        data->ptr = data->buffer;
    } else {
        corto_uint32 length, bufferLength;

        if (!data->length) {
            data->buffer = corto_realloc(data->buffer, strlen(data->buffer) + strlen(str) + 1);
            data->ptr = data->buffer;
        }

        length = strlen(str);
        bufferLength = strlen(data->buffer);

        if (data->maxlength && ((bufferLength + length) >= data->maxlength)) {
            strncat(data->ptr, str, data->maxlength - bufferLength);
            goto maxreached;
        } else {
            strcat(data->ptr, str);
        }
    }

    return TRUE;
maxreached:
    return FALSE;
}

/* agreed not to change anything except the first argument of the signature */
/* Append string to serializer-result */
static corto_bool corto_ser_appendstr(json_ser_t* data, corto_string fmt, ...) {
    char alloc[1024];
    char *buff = alloc;
    va_list args;
    corto_uint32 memRequired;
    corto_uint32 result = TRUE;

    if (data) {
        va_list argcpy;
        va_copy(argcpy, args);
        va_start(args, fmt);
        memRequired = vsnprintf(buff, 1024, fmt, args);
        if (memRequired >= 1024) {
            buff = corto_alloc(memRequired + 1);
            vsprintf(buff, fmt, argcpy);
        }
        va_end(args);
        result = corto_ser_appendstrbuff(data, buff);
        if (buff != alloc) {
            corto_dealloc(buff);
        }
    }

    return result;
}

corto_int16 serializeNumber(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    CORTO_UNUSED(data);

    corto_type t = corto_valueType(value);

    /* JSON doesn't support hex notation, so convert to integer */
    if (corto_primitive(t)->kind == CORTO_BINARY) {
        t = corto_type(corto_uint64_o);
    }

    corto_void  *v = corto_valueValue(value);

    corto_int16 result = corto_convert(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        out);

    return result;
}

static corto_int16 serializeNumericWithPrefix(
    corto_value *value,
    corto_string *out,
    const char *prefix,
    json_ser_t *data)
{
    corto_string raw;
    corto_void *v = corto_valueValue(value);
    corto_type t = corto_valueType(value);

    if (corto_convert(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        &raw))
    {
        goto error;
    }

    if (data->serializePrefix) {
        corto_asprintf(out, "\"%s %s\"", prefix, raw);
    } else {
        corto_asprintf(out, "\"%s\"", raw);
    }

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
    return serializeNumericWithPrefix(value, out, "@M", data);
}

corto_int16 serializeEnum(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    return serializeNumericWithPrefix(value, out, "@E", data);
}

corto_int16 serializeBoolean(
    corto_value *value,
    corto_string *out,
    json_ser_t *data)
{
    CORTO_UNUSED(data);

    corto_bool b = *(corto_bool *)corto_valueValue(value);
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
    corto_type type = corto_valueType(value);
    corto_void *v = corto_valueValue(value);
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
    corto_type type = corto_valueType(v);
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
    if (!corto_ser_appendstr(data, "%s", valueString)) {
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
    o = corto_valueValue(v);
    object = *(corto_object*)o;

    if (object) {
        if (corto_checkAttr(object, CORTO_ATTR_SCOPED) || (corto_valueObject(v) == object)) {
            corto_uint32 length;
            corto_fullpath(id, object);

            /* Escape value */
            corto_string escapedValue = corto_alloc((length = stresc(NULL, 0, id)) + 1);
            stresc(escapedValue, length + 1, id);
            if (data->serializePrefix) {
                if (!corto_ser_appendstr(data, "\"@R %s\"", escapedValue)) {
                    corto_dealloc(escapedValue);
                    goto finished;
                }
            } else {
                if (!corto_ser_appendstr(data, "\"%s\"", escapedValue)) {
                    corto_dealloc(escapedValue);
                    goto finished;
                }
            }

            corto_dealloc(escapedValue);
        } else {
            if (corto_serialize(s, object, data)) {
                goto error;
            }
        }
    } else {
        if (!corto_ser_appendstrbuff(data, "null")) {
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

    if (data->itemCount && !corto_ser_appendstr(data, ",")) {
        goto finished;
    }
    if (info->kind == CORTO_MEMBER) {
        corto_member member = info->is.member.t;
        corto_string name = corto_nameof(member);
        if (!corto_ser_appendstr(data, "\"%s\":", name)) {
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
    json_ser_t data = *(json_ser_t*)userData;
    corto_type type = corto_valueType(v);
    corto_bool useCurlyBraces = TRUE;

    data.itemCount = 0;

    if (type->kind == CORTO_COLLECTION && corto_collection(type)->kind != CORTO_MAP) {
        useCurlyBraces = FALSE;
    }

    if (!corto_ser_appendstr(&data, (useCurlyBraces ? "{" : "["))) {
        goto finished;
    }
    if (type->kind == CORTO_COMPOSITE) {
        if (corto_serializeMembers(s, v, &data)) {
            goto error;
        }
    } else if (type->kind == CORTO_COLLECTION) {
        if (corto_serializeElements(s, v, &data)) {
            goto error;
        }
    } else {
        goto error;
    }

    if (!corto_ser_appendstr(&data, (useCurlyBraces ? "}" : "]"))) {
        goto finished;
    }

    ((json_ser_t*)userData)->buffer = data.buffer;
    ((json_ser_t*)userData)->ptr = data.ptr;

    return 0;
error:
    return -1;
finished:
    return 1;
}

static corto_int16 serializeBase(corto_serializer s, corto_value* v, void* userData) {
    json_ser_t *data = userData;

    if (!corto_ser_appendstr(data, "\"super\":")) {
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

    if (corto_valueType(v)->kind != CORTO_VOID) {
        if (corto_serializeValue(s, v, userData)) {
            goto error;
        }
     }

    return 0;
error:
    return -1;
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
    struct corto_serializer_s serializer = corto_json_ser(CORTO_PRIVATE, CORTO_NOT, CORTO_SERIALIZER_TRACE_NEVER);
    serializer.aliasAction = CORTO_SERIALIZER_ALIAS_IGNORE;
    json_ser_t jsonData = {NULL, NULL, 0, 0, 0, FALSE, TRUE, FALSE, FALSE, FALSE};
    corto_serialize(&serializer, o, &jsonData);
    return jsonData.buffer;
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
