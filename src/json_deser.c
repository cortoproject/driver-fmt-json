
#include "corto/fmt/json/json.h"

static corto_int16 json_deserType(void *p, corto_type t, JSON_Value *v);

static corto_int16 json_deserBoolean(void* o, corto_primitive t, JSON_Value* v) {
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONBoolean) {
        corto_error("no deserializing JSON boolean");
        goto error;
    }

    *(corto_bool *)o = json_value_get_boolean(v);

    return 0;
error:
    return -1;
}

static corto_int16 json_deserNumber(void* o, corto_primitive t, JSON_Value* v) {
    if (json_value_get_type(v) != JSONNumber) {
        corto_error("not deserializing JSON number");
        goto error;
    }

    corto_float64 number = json_value_get_number(v);
    corto_convert(
        corto_primitive(corto_float64_o),
        &number,
        t,
        o);

    return 0;
error:
    return -1;
}

static corto_int16 json_deserText(void* p, corto_primitive t, JSON_Value *v) {
    const char *s = json_value_get_string(v);
    CORTO_UNUSED(t);

    corto_setstr(p, (corto_string)s);

    return 0;
}

corto_bool json_deserPrimitive(void* p, corto_type t, JSON_Value* v) {
    corto_assert(t->kind == CORTO_PRIMITIVE, "not deserializing primitive");

    corto_primitive ptype = corto_primitive(t);

    switch (ptype->kind) {
    case CORTO_BOOLEAN:
        if (json_deserBoolean(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_CHARACTER:
        break;
    case CORTO_INTEGER:
    case CORTO_UINTEGER:
    case CORTO_FLOAT:
    case CORTO_BINARY:
        if (json_deserNumber(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_TEXT:
        if (json_deserText(p, ptype, v)) {
            goto error;
        }
    case CORTO_ENUM:
        break;
    case CORTO_BITMASK:
        break;
    }

    return 0;
error:
    return 0;
}

corto_int16 json_deserReference(void* p, corto_type t, JSON_Value* v) {
    const char* reference = json_value_get_string(v);
    if (!reference) {
        corto_error("not deserializing JSON string");
        goto error;
    }

    corto_object o = corto_resolve(NULL, (corto_string)reference);
    if (!o) {
        corto_error("cannot find reference \"%s\"", reference);
        goto error;
    }

    if (!corto_instanceof(t, o)) {
        corto_error("%s is not an instance of %s", reference, corto_nameof(t));
    }

    corto_setref(p, o);

    corto_release(o);

    return 0;
error:
    return -1;
}

static corto_int16 json_deserItem(void *p, corto_type t, JSON_Value *v) {

    if (t->reference) {
        if (json_deserReference(p, t, v)) {
            goto error;
        }
    } else {
        if (json_deserType(p, t, v)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static corto_int16 json_deserComposite(void* p, corto_type t, JSON_Value *v) {
    corto_assert(t->kind == CORTO_COMPOSITE, "not deserializing composite");

    if (json_value_get_type(v) != JSONObject) {
        corto_error("composite value is not JSON object");
        goto error;
    }

    JSON_Object* o = json_value_get_object(v);
    size_t count = json_object_get_count(o);
    size_t i;

    for (i = 0; i < count; i++) {
        const char* memberName = json_object_get_name(o, i);
        corto_member member_o;
        JSON_Value* value;

        member_o = corto_interface_resolveMember(t, (char*)memberName);

        /* Ensure that we're not resolving members from a base type */
        if (!member_o || (corto_parentof(member_o) != t)) {
            corto_seterr(
                "cannot find member '%s' in type '%s'",
                memberName,
                corto_fullpath(NULL, t));
            goto error;
        }

        value = json_object_get_value(o, memberName);

        void *offset = CORTO_OFFSET(p, member_o->offset);
        if (json_deserItem(offset, member_o->type, value)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static corto_int16 json_deserType(void *p, corto_type t, JSON_Value *v) {
    switch (t->kind) {
    case CORTO_VOID:
        /* Nothing to deserialize */
        break;
    case CORTO_PRIMITIVE:
        if (json_deserPrimitive(p, t, v)) {
            goto error;
        }
        break;
    case CORTO_COMPOSITE:
        if (json_deserComposite(p, t, v)) {
            goto error;
        }
        break;
    case CORTO_COLLECTION:
        break;
    default:
        corto_seterr(
            "unsupported type, can't serialize JSON");
        break;
    }

    return 0;
error:
    return -1;
}

corto_int16 json_deserialize(corto_object o, corto_string s) {
    char *json = s;
    if ((json[0] != '{') && (json[1] != '[')) {
        corto_asprintf(&json, "{\"value\": %s}", s);
    }

    JSON_Value *jsonValue = json_parse_string(json);

    if (o) {
        JSON_Value *value = jsonValue;

        if (!jsonValue) {
            corto_seterr("invalid JSON '%s'", s);
            goto error;
        }

        if (corto_typeof(o)->kind == CORTO_PRIMITIVE) {
            JSON_Object* jsonObj = json_value_get_object(jsonValue);
            if (!jsonObj) {
                corto_seterr("invalid JSON for primitive value '%s'", json);
                goto error;
            }

            size_t count = json_object_get_count(jsonObj);
            if (count == 1) {
                value = json_object_get_value(jsonObj, "value");
            } else {
                corto_seterr("invalid JSON for primitive value '%s'", json);
                goto error;
            }
        }

        if (json_deserType(o, corto_typeof(o), value)) {
            goto error;
        }
    } else {
        corto_seterr("no object provided to JSON deserializer");
        goto error;
    }

    if (json != s) {
        corto_dealloc(json);
    }

    return 0;
error:
    if (json != s) {
        corto_dealloc(json);
    }

    if (jsonValue) {
        json_value_free(jsonValue);
    }
    return -1;
}
