
#include "driver/fmt/json/json.h"

static int16_t json_deserType(void *p, corto_type t, JSON_Value *v);

corto_type json_deserInlineType(
    JSON_Object *obj)
{
    JSON_Value* type = json_object_get_value(obj, "type");
    if (!type) {
        corto_throw("type member is mandatory for anonymous object");
        goto error;
    }
    if (json_value_get_type(type) != JSONString) {
        corto_throw("type member of anonymous object must be a string");
        goto error;
    }
    corto_type cortoType = corto_resolve(NULL, (char*)json_value_get_string(type));
    if (!cortoType) {
        corto_throw("type '%s' not found for anonymous object", json_value_get_string(type));
        goto error;
    }
    return cortoType;
error:
    return NULL;
}

static
char* json_valueTypeToString(
    JSON_Value *v)
{
    switch(json_value_get_type(v)) {
    case JSONBoolean: return "boolean";
    case JSONNumber: return "number";
    case JSONString: return "string";
    case JSONObject: return "object";
    case JSONArray: return "array";
    case JSONNull: return "null";
    default: return "unknown";
    }
}

static
int16_t json_deserBoolean(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONBoolean) {
        corto_throw("expected boolean, got %s", json_valueTypeToString(v));
        goto error;
    }

    *(bool *)o = json_value_get_boolean(v);

    return 0;
error:
    return -1;
}

static
int16_t json_deserCharacter(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONString) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected string, got %s (%s)", json_valueTypeToString(v), json);
        corto_dealloc(json);
        goto error;
    } else {
        const char *str = json_value_get_string(v);
        *(char*)o = *str;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deserNumber(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    corto_float64 number;

    if (json_value_get_type(v) == JSONNull) {
        number = NAN;
    } else if (json_value_get_type(v) != JSONNumber) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected number, got %s (%s)", json_valueTypeToString(v), json);
        corto_dealloc(json);
        goto error;
    } else {
        number = json_value_get_number(v);
    }

    corto_ptr_cast(
        corto_primitive(corto_float64_o),
        &number,
        t,
        o);

    return 0;
error:
    return -1;
}

static
int16_t json_deserText(
    void* p,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) == JSONNull) {
        corto_set_str(p, NULL);
    } else if (json_value_get_type(v) != JSONString) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected string, got %s (%s)", json_valueTypeToString(v), json);
        corto_dealloc(json);
        goto error;
    } else {
        const char *s = json_value_get_string(v);
        corto_set_str(p, (corto_string)s);
    }

    return 0;
error:
    return -1;
}

/* If deserializing verbatim, serialize JSON to string */
static
int16_t json_deserVerbatim(
    void* p,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    char *str = json_serialize_to_string((const JSON_Value*)v);
    corto_set_str(p, str);

    return 0;
}


static
int16_t json_deserConstant(
    void* p,
    corto_primitive t,
    JSON_Value *v)
{
    const char *s = json_value_get_string(v);
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONString) {
        corto_throw("expected string, got %s", json_valueTypeToString(v));
        goto error;
    }

    if (corto_ptr_cast(corto_string_o, (corto_string*)&s, t, p)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
bool json_deserPrimitive(
    void* p,
    corto_type t,
    JSON_Value *v)
{
    corto_assert(t->kind == CORTO_PRIMITIVE, "not deserializing primitive");

    corto_primitive ptype = corto_primitive(t);

    switch (ptype->kind) {
    case CORTO_BOOLEAN:
        if (json_deserBoolean(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_CHARACTER:
        if (json_deserCharacter(p, ptype, v)) {
            goto error;
        }
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
        if (corto_typeof(ptype) != corto_type(corto_verbatim_o) || strcmp(corto_verbatim(ptype)->contentType, "text/json")) {
            if (json_deserText(p, ptype, v)) {
                goto error;
            }
        } else {
            if (json_deserVerbatim(p, ptype, v)) {
                goto error;
            }
        }
        break;
    case CORTO_ENUM:
    case CORTO_BITMASK:
        if (json_deserConstant(p, ptype, v)) {
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deserReference(
    void* p,
    corto_type t,
    JSON_Value* v)
{
    switch(json_value_get_type(v)) {
    case JSONString: {
        const char* reference = json_value_get_string(v);
        corto_object o = corto_resolve(NULL, (corto_string)reference);
        if (!o) {
            corto_throw("unresolved reference \"%s\"", reference);
            goto error;
        }

        if (!corto_instanceof(t, o)) {
            corto_throw("%s is not an instance of \"%s\"", reference, corto_fullpath(NULL, t));
            goto error;
        }

        corto_set_ref(p, o);
        corto_release(o);
        break;
    }
    case JSONObject: {
        JSON_Object* obj = json_value_get_object(v);

        corto_type cortoType = json_deserInlineType(obj);

        corto_object cortoObj = *(corto_object*)p;
        if (!cortoObj || (corto_typeof(cortoObj) != cortoType)) {
            cortoObj = corto_create(NULL, NULL, cortoType);
            corto_set_ref(p, cortoObj);
            corto_release(cortoObj);
        }
        corto_release(cortoType);

        JSON_Value* value = json_object_get_value(obj, "value");
        if (json_deserType(cortoObj, cortoType, value)) {
            goto error;
        }
        break;
    }
    case JSONNull:
        corto_set_ref(p, NULL);
        break;
    default:
        corto_throw("expected string, null or object (reference), got %s", json_valueTypeToString(v));
        break;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deserAny(
    void* p,
    corto_type t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);
    JSON_Value *value = NULL;
    corto_type type = NULL;
    corto_any *dst = p;

    if (json_value_get_type(v) == JSONObject) {
        JSON_Object *obj = json_value_get_object(v);
        type = json_deserInlineType(obj);
        if (!type) {
            goto error;
        }

        value = json_object_get_value(obj, "value");
    } else {
        /* Figure out type from JSON value */
        value = v;
        switch (json_value_get_type(v)) {
        case JSONBoolean: type = corto_type(corto_bool_o); break;
        case JSONNumber: type = corto_type(corto_float64_o); break;
        case JSONNull: type = corto_type(corto_string_o); break;
        case JSONString: type = corto_type(corto_string_o); break;
        default:
            corto_throw("unexpected JSON '%s' for any-value",
                json_valueTypeToString(v));
            break;
        }
    }

    dst->type = type;
    dst->owner = TRUE;

    void *ptr;
    if (!type->reference) {
        ptr = corto_calloc(corto_type_sizeof(type));
        if (corto_ptr_init(ptr, type)) {
            goto error;
        }
        dst->value = ptr;
    } else {
        ptr = &dst->value;
    }

    if (json_deserType(ptr, type, value)) {
        goto error;
    }

    return 0;
error:
    return -1;

}

static
int16_t json_deserItem(
    void *p,
    corto_type t,
    JSON_Value *v)
{
    if (t->reference) {
        if (json_deserReference(p, t, v)) {
            corto_throw("failed to deserialize item of type '%s'",
                corto_fullpath(NULL, t));
            goto error;
        }
    } else {
        if (json_deserType(p, t, v)) {
            corto_throw("failed to deserialize item of type '%s'",
                corto_fullpath(NULL, t));
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
bool json_deserMustSkip(
    corto_member m,
    void *ptr)
{
    if (corto_instanceof(corto_target_o, corto_parentof(m))) {
        bool owned = corto_owned(ptr);
        bool isActual = !strcmp("actual", corto_idof(m));
        if ((owned && !isActual) || (!owned && isActual)) {
            return TRUE;
        }
    }
    return FALSE;
}

static
int16_t json_deserComposite(
    void* p,
    corto_type t,
    JSON_Value *v)
{
    corto_assert(t->kind == CORTO_COMPOSITE, "not deserializing composite");

    if (json_value_get_type(v) != JSONObject) {
        char *json = json_serialize_to_string(v);
        corto_throw(
            "expected object for type '%s', got %s ('%s')",
            corto_fullpath(NULL, t),
            json_valueTypeToString(v), json);
        free(json);
        goto error;
    }

    JSON_Object* o = json_value_get_object(v);
    size_t count = json_object_get_count(o);
    size_t i;
    bool isUnion = corto_interface(t)->kind == CORTO_UNION;
    corto_int32 discriminator = 0;
    corto_member unionMember = NULL;

    for (i = 0; i < count; i++) {
        const char* memberName = json_object_get_name(o, i);
        corto_member member_o;

        if (!strcmp(memberName, "_d") && isUnion) {
            JSON_Value* value = json_object_get_value(o, memberName);
            if (json_deserPrimitive(
                &discriminator,
                corto_union(t)->discriminator,
                value))
            {
                goto error;
            }
            unionMember = corto_union_findCase(t, discriminator);
            if (!unionMember) {
                corto_throw("discriminator '%d' invalid for union '%s'",
                    discriminator, corto_fullpath(NULL, t));
            }
        } else {
            corto_value mbr, v = corto_value_mem(p, t);
            if (corto_value_memberExpr(&v, (char*)memberName, &mbr)) {
                /* Ignore members not found in type */
                corto_catch();
                continue;
            }

            member_o = NULL;
            if (mbr.kind == CORTO_MEMBER) {
                member_o = mbr.is.member.t;
            }

            if (isUnion && (unionMember != member_o)) {
                corto_throw(
                    "member '%s' does not match discriminator '%d' (expected member '%s')",
                    memberName,
                    discriminator,
                    corto_idof(unionMember));
                goto error;
            } else if (isUnion) {
                corto_int32 prev = *(corto_int32*)p;
                if (prev != discriminator) {
                    corto_member prevMember = corto_union_findCase(t, prev);
                    corto_ptr_deinit(CORTO_OFFSET(p, prevMember->offset), prevMember->type);
                    memset(CORTO_OFFSET(p, member_o->offset), 0, member_o->type->size);
                }
                *(corto_int32*)p = discriminator;
            }

            if (!member_o || !json_deserMustSkip(member_o, p)) {
                JSON_Value* value = json_object_get_value(o, memberName);
                void *offset = corto_value_ptrof(&mbr);
                corto_type memberType = corto_value_typeof(&mbr);

                if (member_o && member_o->modifiers & CORTO_OBSERVABLE) {
                    offset = *(void**)offset;
                    if (json_deserType(offset, memberType, value)) {
                        corto_throw("failed to deserialize member '%s'", corto_idof(member_o));
                        goto error;
                    }
                } else {
                    if (member_o && member_o->modifiers & CORTO_OPTIONAL) {
                        if (*(void**)offset) {
                            corto_ptr_deinit(*(void**)offset, memberType);
                            memset(*(void**)offset, 0, memberType->size);
                        } else {
                            *(void**)offset = corto_calloc(memberType->size);
                        }
                        offset = *(void**)offset;
                    }
                    if (mbr.kind == CORTO_MEMBER) {
                        if (json_deserItem(offset, memberType, value)) {
                            corto_throw("failed to deserialize member '%s'", corto_idof(member_o));
                            goto error;
                        }
                    } else if (mbr.kind == CORTO_BASE) {
                        if (json_deserType(offset, memberType, value)) {
                            corto_throw("failed to deserialize 'super'");
                            goto error;
                        }
                    } else {
                        corto_error("unexpected value kind '%d'", mbr.kind);
                        goto error;
                    }
                }
            }
        }
    }

    return 0;
error:
    return -1;
}

static
void* json_deser_allocElem(
    void *ptr,
    corto_collection t,
    corto_int32 i)
{
    corto_int32 size = corto_type_sizeof(t->elementType);
    void *result = NULL;

    switch(t->kind) {
    case CORTO_SEQUENCE:
        ptr = ((corto_objectseq*)ptr)->buffer;
    case CORTO_ARRAY:
        result = CORTO_OFFSET(ptr, size * i);
        break;
    case CORTO_LIST: {
        corto_ll list = *(corto_ll*)ptr;
        if (corto_collection_requiresAlloc(t->elementType)) {
            result = corto_ll_get(list, i);
        } else {
            result = corto_ll_getPtr(list, i);
        }
        break;
    default:
        break;
    }
    }

    return result;
}

static
int16_t json_deserCollection(
    void* p,
    corto_type t,
    JSON_Value *v)
{
    corto_assert(t->kind == CORTO_COLLECTION, "not deserializing composite");
    corto_type elementType = corto_collection(t)->elementType;

    /* Deserialize elements */
    if (json_value_get_type(v) == JSONArray) {
        JSON_Array* a = json_value_get_array(v);
        size_t count = json_array_get_count(a);
        if (corto_ptr_resize(p, t, count)) {
            goto error;
        }

        size_t i;
        for (i = 0; i < count; i++) {
            void *elementPtr = json_deser_allocElem(p, corto_collection(t), i);
            corto_assert(elementPtr != NULL, "invalid element ptr for index %d returned by JSON deserializer", i);
            JSON_Value *elem = json_array_get_value(a, i);
            if (json_deserItem(elementPtr, elementType, elem)) {
                goto error;
            }
        }
    } else if (json_value_get_type(v) == JSONNull) {
        if (corto_ptr_resize(p, t, 0)) {
            goto error;
        }
    } else {
        corto_throw(
            "expected array, got '%s'",
            json_valueTypeToString(v));
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deserType(
    void *p,
    corto_type t,
    JSON_Value *v)
{
    switch (t->kind) {
    case CORTO_VOID:
        /* Nothing to deserialize */
        break;
    case CORTO_ANY:
        if (json_deserAny(p, t, v)) {
            goto error;
        }
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
        if (json_deserCollection(p, t, v)) {
            goto error;
        }
        break;
    default:
        corto_throw(
            "unsupported type, can't serialize JSON");
        break;
    }

    return 0;
error:
    corto_throw(NULL);
    return -1;
}

int16_t json_deserialize_from_JSON_Value(
    corto_value *v,
    JSON_Value *jsonValue)
{
    void *ptr = corto_value_ptrof(v);
    corto_type type = corto_value_typeof(v);

    if (json_deserType(ptr, type, jsonValue)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int16_t json_deserialize(
    corto_value *v,
    corto_string s)
{
    corto_assert(v != NULL, "NULL passed to json_deserialize");
    corto_log_push("json");

    char *json = s;
    if ((json[0] != '{') && (json[1] != '[') && (json[0] != '[')) {
        json = corto_asprintf("{\"value\": %s}", json);
    }

    corto_debug("deserialize string %s", json);

    JSON_Value *jsonValue = json_parse_string(json);
    if (!jsonValue) {
        corto_throw("invalid JSON '%s'", json);
        goto error;
    }

    corto_type type = corto_value_typeof(v);

    if (type->kind == CORTO_PRIMITIVE) {
        JSON_Object* jsonObj = json_value_get_object(jsonValue);
        if (!jsonObj) {
            corto_throw("invalid JSON for primitive value '%s'", json);
            goto error;
        }

        jsonValue = json_object_get_value(jsonObj, "value");
    }

    if (jsonValue && json_deserialize_from_JSON_Value(v, jsonValue)) {
        corto_throw("failed to deserialize '%s'", json);
        goto error;
    }

    if (json != s) {
        corto_dealloc(json);
    }

    if (jsonValue) {
        json_value_free(jsonValue);
    }
    corto_log_pop();
    return 0;
error:
    if (json != s) {
        corto_dealloc(json);
    }

    if (jsonValue) {
        json_value_free(jsonValue);
    }
    corto_log_pop();
    return -1;
}
