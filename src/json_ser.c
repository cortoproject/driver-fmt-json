
#include "json.h"

/* Serialize reference value */
static
int16_t json_serializeReference(
    corto_walk_opt* s,
    corto_value *v,
    void *userData)
{
    CORTO_UNUSED(s);

    json_ser_t *data;
    void *o;
    corto_object object;

    data = userData;
    o = corto_value_ptrof(v);
    object = *(corto_object*)o;

    if (object) {

        if (corto_check_attr(object, CORTO_ATTR_NAMED) ||
           (corto_value_objectof(v) == object))
        {
            uint32_t length;
            corto_id full, idbuff;
            char *id = full;

            corto_fullpath(full, object);
            if (data->opt) {
                corto_path_offset(idbuff, data->opt->from, full, -1, false);
                id = idbuff;
            }

            /* Escape value */
            corto_string escapedValue =
                corto_alloc((length = stresc(NULL, 0, '"', id)) + 1);
            stresc(escapedValue, length + 1, '"', id);

            if (!corto_buffer_append(
                data->buffer, "\"%s\"", escapedValue))
            {
                corto_dealloc(escapedValue);
                goto finished;
            }

            corto_dealloc(escapedValue);
        } else {
            corto_id type;
            corto_fullpath(type, corto_typeof(object));

            if (!corto_buffer_append(
                data->buffer, "{\"type\":\"%s\",\"value\":", type))
            {
                goto finished;
            }
            if (corto_walk(s, object, data)) {
                goto error;
            }
            if (!corto_buffer_appendstr(data->buffer, "}")) {
                goto finished;
            }
        }
    } else {
        if (!corto_buffer_appendstr(data->buffer, "null")) {
            goto finished;
        }
    }

    return 0;
finished:
    return 1;
error:
    return -1;
}

/* Serialize number */
static
int16_t json_serializeNumber(
    corto_buffer *buf,
    corto_value *value,
    json_ser_t *data)
{
    CORTO_UNUSED(data);
    char *str = NULL;

    corto_type t = corto_value_typeof(value);

    /* JSON doesn't support hex notation, so convert to integer */
    if (corto_primitive(t)->kind == CORTO_BINARY) {
        t = corto_type(corto_uint64_o);
    }

    void *v = corto_value_ptrof(value);

    int16_t result = corto_ptr_cast(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        &str);

    if (!strcmp(str, "nan")) {
        corto_set_str(&str, "null");
    }

    corto_buffer_appendstr_zerocpy(buf, str);

    return result;
}

/* Serialize enum value */
static
int16_t json_serializeEnum(
    corto_buffer *buf,
    corto_value *value,
    json_ser_t *data)
{
    void *v = corto_value_ptrof(value);
    corto_type t = corto_value_typeof(value);
    corto_constant *c = corto_enum_constant_from_value(t, *(corto_int32*)v);
    if (!c) {
        corto_throw("failed to obtain enumeration constant");
        goto error;
    }

    corto_buffer_appendstr(buf, "\"");
    corto_buffer_appendstr(buf, corto_idof(c));
    corto_buffer_appendstr(buf, "\"");

    return 0;
error:
    return -1;
}

/* Serialize bitmask value */
static
int16_t json_serializeBitmask(
    corto_buffer *buf,
    corto_value *value,
    json_ser_t *data)
{
    void *v = corto_value_ptrof(value);
    corto_type t = corto_value_typeof(value);
    char *str = NULL;

    corto_try (
      corto_ptr_cast(
        corto_primitive(t),
        v,
        corto_primitive(corto_string_o),
        &str), NULL);

    corto_buffer_appendstr(buf, "\"");
    corto_buffer_appendstr_zerocpy(buf, str);
    corto_buffer_appendstr(buf, "\"");

    return 0;
error:
    return -1;
}

/* Serialize boolean value (PRIMITIVE) */
static
int16_t json_serializeBoolean(
    corto_buffer *buf,
    corto_value *value,
    json_ser_t *data)
{
    CORTO_UNUSED(data);

    bool b = *(bool*)corto_value_ptrof(value);
    if (b) {
        corto_buffer_appendstr(buf, "true");
    } else {
        corto_buffer_appendstr(buf, "false");
    }

    return 0;
}

/* Serialize text value (PRIMITIVE) */
static
int16_t json_serializeText(
    corto_buffer *buf,
    corto_value *value,
    json_ser_t *data)
{
    CORTO_UNUSED(data);
    corto_type type = corto_value_typeof(value);
    void *v = corto_value_ptrof(value);
    char *str = *(char**)v;
    corto_primitiveKind kind = ((corto_primitive)type)->kind;

    if (kind == CORTO_TEXT && str)
    {
        size_t length;
        if (corto_typeof(type) != corto_type(corto_verbatim_o) ||
            strcmp(corto_verbatim(type)->contentType, "text/json"))
        {
            /* Escape string, append to buffer */
            char *out = NULL;
            length = stresc(NULL, 0, '"', str);
            out = corto_alloc(length + 3);
            stresc(out + 1, length, '"', str);
            out[0] = '"';
            out[length + 1] = '"';
            out[length + 2] = '\0';
            corto_buffer_appendstr_zerocpy(buf, out);
        } else {
            /* If string is a verbatim JSON string, copy string as-is */
            corto_buffer_appendstr_zerocpy_const(buf, str);
        }
    } else if (kind == CORTO_CHARACTER) {
        char esc[3] = {*(char*)v, 0};
        chresc(esc, *esc, '"');
        corto_buffer_appendstrn(buf, "\"", 1);
        corto_buffer_appendstr(buf, esc);
        corto_buffer_appendstrn(buf, "\"", 1);
    } else {
        corto_buffer_appendstr(buf, "null");
    }

    return 0;
}

/* Serialize CORTO_PRIMITIVE (type program) */
static
int16_t json_serializePrimitive(
    corto_walk_opt* s,
    corto_value *v,
    void *userData)
{
    CORTO_UNUSED(s);
    corto_type type = corto_value_typeof(v);
    json_ser_t *data = userData;
    int16_t result = 0;

    switch (corto_primitive(type)->kind) {
        case CORTO_BINARY:
            result = json_serializeNumber(data->buffer, v, data);
            break;
        case CORTO_BITMASK:
            result = json_serializeBitmask(data->buffer, v, data);
            break;
        case CORTO_BOOLEAN:
            result = json_serializeBoolean(data->buffer, v, data);
            break;
        case CORTO_ENUM:
            result = json_serializeEnum(data->buffer, v, data);
            break;
        case CORTO_CHARACTER:
        case CORTO_TEXT:
            result = json_serializeText(data->buffer, v, data);
            break;
        case CORTO_UINTEGER:
        case CORTO_INTEGER:
        case CORTO_FLOAT:
            result = json_serializeNumber(data->buffer, v, data);
            break;
    }
    if (result) {
        goto error;
    }
    return 0;
error:
    return -1;
}

/* Serialize CORTO_COMPOSITE/CORTO_COLLECTION (type program) */
static
int16_t json_serializeComplex(
    corto_walk_opt* s,
    corto_value* v,
    void* userData)
{
    json_ser_t privateData, *data = userData;
    corto_type type = corto_value_typeof(v);
    bool useCurlyBraces = TRUE;

    if (type->kind == CORTO_COLLECTION &&
        corto_collection(type)->kind != CORTO_MAP)
    {
        useCurlyBraces = FALSE;
    }

    privateData.buffer = data->buffer;
    privateData.itemCount = 0;
    privateData.opt = data->opt;

    if (!corto_buffer_appendstr(
            data->buffer, (useCurlyBraces ? "{" : "["))) {
        goto finished;
    }
    if (type->kind == CORTO_COMPOSITE) {
        if (corto_interface(type)->kind == CORTO_UNION) {
            int32_t *d = corto_value_ptrof(v);
            corto_buffer_appendstr(data->buffer, "\"_d\":");
            corto_value discriminatorValue =
                corto_value_value(d, corto_union(type)->discriminator);
            if (json_serializePrimitive(s, &discriminatorValue, &privateData)) {
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

    if (!corto_buffer_appendstr(
        data->buffer, (useCurlyBraces ? "}" : "]")))
    {
        goto finished;
    }

    return 0;
error:
    return -1;
finished:
    return 1;
}

/* Serialize CORTO_VOID (type program) */
static
int16_t json_serializeVoid(
    corto_walk_opt* s,
    corto_value* v,
    void* userData)
{
    json_ser_t *data = userData;

    CORTO_UNUSED(s);
    CORTO_UNUSED(v);

    if (!corto_buffer_appendstr(data->buffer, "{}")) {
        goto finished;
    }

    return 0;
finished:
    return 1;
}

/* Serialize CORTO_ANY (type program) */
static
int16_t json_serializeAny(
    corto_walk_opt* s,
    corto_value* v,
    void* userData)
{
    json_ser_t *data = userData;
    corto_any *ptr = corto_value_ptrof(v);
    int16_t result = 0;

    CORTO_UNUSED(s);
    CORTO_UNUSED(v);

    if (!corto_buffer_append(
        data->buffer,
        "{\"type\":\"%s\",\"value\":",
        corto_fullpath(NULL, ptr->type)))
    {
        goto finished;
    }

    corto_value anyValue = corto_value_value(ptr->value, ptr->type);
    if ((result = corto_walk_value(s, &anyValue, userData))) {
        goto done;
    }

    if (!corto_buffer_appendstr( data->buffer, "}")) {
        goto finished;
    }

done:
    return result;
finished:
    return 1;
}

/* Serialize CORTO_MEMBER/CORTO_ELEMENT (meta program) */
static
int16_t json_serializeItem(
    corto_walk_opt* s,
    corto_value *info,
    void *userData)
{
    json_ser_t *data = userData;

    if (data->itemCount && !corto_buffer_appendstr(data->buffer, ",")) {
        goto finished;
    }
    if (info->kind == CORTO_MEMBER) {
        corto_member member = info->is.member.t;
        corto_string name = corto_idof(member);
        if (!corto_buffer_append(data->buffer, "\"%s\":", name)) {
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

/* Serialize CORTO_BASE (meta program) */
static
int16_t json_serializeBase(
    corto_walk_opt* s,
    corto_value* v,
    void* userData)
{
    json_ser_t *data = userData;

    if (!corto_buffer_append(data->buffer, "\"super\":")) {
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

/* Serialize CORTO_OBJECT (meta program) */
static
int16_t json_serializeObject(
    corto_walk_opt* s,
    corto_value* v,
    void* userData)
{
    json_ser_t *data = userData;

    if (corto_value_typeof(v)->kind != CORTO_VOID) {
        if (corto_walk_value(s, v, userData)) {
            goto error;
        }
     } else {
        if (!corto_buffer_appendstr(data->buffer, "")) {
            goto finished;
        }
     }

    return 0;
error:
    return -1;
finished:
    return 1;
}

/* Create JSON serializer */
static
corto_walk_opt json_ser(
    corto_modifier access,
    corto_operatorKind accessKind,
    corto_walk_traceKind trace)
{
    corto_walk_opt s;

    corto_walk_init(&s);
    s.access = access;
    s.accessKind = accessKind;
    s.traceKind = trace;
    s.program[CORTO_VOID] = json_serializeVoid;
    s.program[CORTO_ANY] = json_serializeAny;
    s.program[CORTO_PRIMITIVE] = json_serializePrimitive;
    s.reference = json_serializeReference;
    s.program[CORTO_COMPOSITE] = json_serializeComplex;
    s.program[CORTO_COLLECTION] = json_serializeComplex;
    s.metaprogram[CORTO_ELEMENT] = json_serializeItem;
    s.metaprogram[CORTO_MEMBER] = json_serializeItem;
    s.metaprogram[CORTO_BASE] = json_serializeBase;
    s.metaprogram[CORTO_OBJECT] = json_serializeObject;

    return s;
}

/* JSON serializer entry function */
char* json_serialize(
    corto_value *v,
    corto_fmt_opt *opt)
{
    corto_log_push("json");
    corto_walk_opt serializer = json_ser(
      CORTO_LOCAL, CORTO_NOT, CORTO_WALK_TRACE_NEVER
    );
    serializer.aliasAction = CORTO_WALK_ALIAS_IGNORE;
    serializer.optionalAction = CORTO_WALK_OPTIONAL_IF_SET;
    corto_buffer b = CORTO_BUFFER_INIT;
    json_ser_t jsonData = {&b, 0, opt};

    corto_walk_value(&serializer, v, &jsonData);
    corto_string result = corto_buffer_str(jsonData.buffer);
    corto_debug("serialized %s", result);
    corto_log_pop();
    return result;
}
