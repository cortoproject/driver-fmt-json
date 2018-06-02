
#include "json.h"

typedef struct json_deser_t {
    corto_fmt_opt *opt;
} json_deser_t;

static
int16_t json_deser_type(
    void *p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data);

static
corto_type json_deser_inline_type(
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
char* json_type_str(
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
int16_t json_deser_boolean(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONBoolean) {
        corto_throw("expected boolean, got %s", json_type_str(v));
        goto error;
    }

    *(bool *)o = json_value_get_boolean(v);

    return 0;
error:
    return -1;
}

static
int16_t json_deser_character(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONString) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected string, got %s (%s)", json_type_str(v), json);
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
int16_t json_deser_number(
    void* o,
    corto_primitive t,
    JSON_Value *v)
{
    corto_float64 number;

    if (json_value_get_type(v) == JSONNull) {
        number = NAN;
    } else if (json_value_get_type(v) != JSONNumber) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected number, got %s (%s)", json_type_str(v), json);
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
int16_t json_deser_text(
    void* p,
    corto_primitive t,
    JSON_Value *v)
{
    CORTO_UNUSED(t);

    if (json_value_get_type(v) == JSONNull) {
        corto_set_str(p, NULL);
    } else if (json_value_get_type(v) != JSONString) {
        corto_string json = json_serialize_to_string(v);
        corto_throw("expected string, got %s (%s)", json_type_str(v), json);
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
int16_t json_deser_verbatim(
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
int16_t json_deser_constant(
    void* p,
    corto_primitive t,
    JSON_Value *v)
{
    const char *s = json_value_get_string(v);
    CORTO_UNUSED(t);

    if (json_value_get_type(v) != JSONString) {
        corto_throw("expected string, got %s", json_type_str(v));
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
bool json_deser_primitive(
    void* p,
    corto_type t,
    JSON_Value *v)
{
    corto_assert(t->kind == CORTO_PRIMITIVE, "not deserializing primitive");

    corto_primitive ptype = corto_primitive(t);

    switch (ptype->kind) {
    case CORTO_BOOLEAN:
        if (json_deser_boolean(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_CHARACTER:
        if (json_deser_character(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_INTEGER:
    case CORTO_UINTEGER:
    case CORTO_FLOAT:
    case CORTO_BINARY:
        if (json_deser_number(p, ptype, v)) {
            goto error;
        }
        break;
    case CORTO_TEXT:
        if (corto_typeof(ptype) != corto_type(corto_verbatim_o) || strcmp(corto_verbatim(ptype)->contentType, "text/json")) {
            if (json_deser_text(p, ptype, v)) {
                goto error;
            }
        } else {
            if (json_deser_verbatim(p, ptype, v)) {
                goto error;
            }
        }
        break;
    case CORTO_ENUM:
    case CORTO_BITMASK:
        if (json_deser_constant(p, ptype, v)) {
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deser_reference(
    void* p,
    corto_type t,
    JSON_Value* v,
    json_deser_t *data)
{
    switch(json_value_get_type(v)) {
    case JSONString: {
        const char* reference = json_value_get_string(v);
        const char *ref_ptr = reference;
        corto_id ref;
        if (data->opt && data->opt->from && reference[0] != '/') {
            corto_path_combine(ref, data->opt->from, reference);
            ref_ptr = ref;
        }
        corto_object o = corto_resolve(NULL, (corto_string)ref_ptr);
        if (!o) {
            corto_throw("unresolved reference \"%s\"", ref_ptr);
            goto error;
        }

        if (!corto_instanceof(t, o)) {
            corto_throw("'%s' is not an instance of '%s' (type is '%s')",
                reference,
                corto_fullpath(NULL, t),
                corto_fullpath(NULL,
                corto_typeof(o)));
            goto error;
        }

        corto_set_ref(p, o);
        corto_release(o);
        break;
    }
    case JSONObject: {
        JSON_Object* obj = json_value_get_object(v);

        corto_type cortoType = json_deser_inline_type(obj);

        corto_object cortoObj = *(corto_object*)p;
        if (!cortoObj || (corto_typeof(cortoObj) != cortoType)) {
            cortoObj = corto_create(NULL, NULL, cortoType);
            corto_set_ref(p, cortoObj);
            corto_release(cortoObj);
        }
        corto_release(cortoType);

        JSON_Value* value = json_object_get_value(obj, "value");
        if (json_deser_type(cortoObj, cortoType, value, data)) {
            goto error;
        }
        break;
    }
    case JSONNull:
        corto_set_ref(p, NULL);
        break;
    default:
        corto_throw("expected string, null or object (reference), got %s", json_type_str(v));
        break;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deser_any(
    void* p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data)
{
    CORTO_UNUSED(t);
    JSON_Value *value = NULL;
    corto_type type = NULL;
    corto_any *dst = p;

    if (json_value_get_type(v) == JSONObject) {
        JSON_Object *obj = json_value_get_object(v);
        type = json_deser_inline_type(obj);
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
                json_type_str(v));
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

    if (json_deser_type(ptr, type, value, data)) {
        goto error;
    }

    return 0;
error:
    return -1;

}

static
int16_t json_deser_item(
    void *p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data)
{
    if (t->reference) {
        if (json_deser_reference(p, t, v, data)) {
            corto_throw("failed to deserialize item of type '%s'",
                corto_fullpath(NULL, t));
            goto error;
        }
    } else {
        if (json_deser_type(p, t, v, data)) {
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
int16_t json_deser_inc(
    void *ptr,
    corto_type t,
    const char *key,
    double inc,
    json_deser_t *data)
{
    corto_value v = corto_value_pointer(ptr, t);
    corto_value member = corto_value_init();

    corto_try(corto_value_field(&v, key, &member),
        "failed to find member for '$inc'");

    corto_value inc_v = corto_value_float(inc);

    corto_try( corto_value_binaryOp(CORTO_ADD, &member, &inc_v, &member), NULL);

    return 0;
error:
    return -1;
}

static
int16_t json_deser_oper(
    void *p,
    corto_type t,
    const char *key,
    JSON_Value *v,
    json_deser_t *data)
{
    if (!strcmp(key, "$inc") || !strcmp(key, "$dec")) {
        JSON_Object *obj = json_value_get_object(v);
        if (!obj) {
            corto_throw("expected object for '$inc' operation, got %s",
              json_type_str(v));
            goto error;
        }

        bool is_inc = !strcmp(key, "$inc");

        int i;
        for (i = 0; i < json_object_get_count(obj); i ++) {
            const char *member = json_object_get_name(obj,  i);
            JSON_Value *value = json_object_get_value_at(obj, i);
            if (json_value_get_type(value) != JSONNumber) {
                corto_throw("expected number for '$inc', got %s",
                  json_type_str(v));
                goto error;
            }

            double inc = json_value_get_number(value);
            corto_try(
              json_deser_inc(
                p, t, member, is_inc ? inc : -inc, data), NULL);
        }
    } else {
        corto_throw("unsupported collection operation '%s'", key);
        goto error;
    }

    return 0;
error:
    return -1;
}

static
bool json_deser_must_skip(
    corto_member m,
    void *ptr)
{
    if (corto_instanceof(corto_target_o, corto_parentof(m))) {
        bool owned = corto_owned(ptr);
        bool isActual = !strcmp("actual", corto_idof(m));
        if ((owned && !isActual) || (!owned && isActual)) {
            return TRUE;
        }
    } else if (m->modifiers & CORTO_LOCAL) {
        return TRUE;
    }
    return FALSE;
}

static
int16_t json_deser_composite(
    void* p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data)
{
    corto_assert(t->kind == CORTO_COMPOSITE, "not deserializing composite");

    if (json_value_get_type(v) != JSONObject) {
        char *json = json_serialize_to_string(v);
        corto_throw(
            "expected object for type '%s', got %s ('%s')",
            corto_fullpath(NULL, t),
            json_type_str(v), json);
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
        JSON_Value* value = json_object_get_value_at(o, i);
        corto_member member_o;

        if (memberName[0] == '$') {
            corto_try( json_deser_oper(p, t, memberName, value, data), NULL);
        } else
        if (!strcmp(memberName, "_d") && isUnion) {
            if (json_deser_primitive(
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
            if (corto_value_field(&v, (char*)memberName, &mbr)) {
                /* Ignore members not found in type */
                corto_catch();
                continue;
            }

            member_o = NULL;
            if (mbr.kind == CORTO_MEMBER) {
                member_o = mbr.is.member.member;
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
                    corto_member prevMember =
                        corto_union_findCase(t, prev);
                    corto_ptr_deinit(
                        CORTO_OFFSET(p, prevMember->offset), prevMember->type);
                    memset(
                        CORTO_OFFSET(
                            p, member_o->offset), 0, member_o->type->size);
                }
                *(corto_int32*)p = discriminator;
            }

            if (!member_o || !json_deser_must_skip(member_o, p)) {
                void *offset = corto_value_ptrof(&mbr);
                corto_type memberType = corto_value_typeof(&mbr);

                if (member_o && member_o->modifiers & CORTO_OBSERVABLE) {
                    offset = *(void**)offset;
                    if (json_deser_type(offset, memberType, value, data)) {
                        corto_throw(
                            "failed to deserialize member '%s'",
                            corto_idof(member_o));
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
                        if (json_deser_item(offset, memberType, value, data)) {
                            corto_throw(
                                "failed to deserialize member '%s'",
                                corto_idof(member_o));
                            goto error;
                        }
                    } else if (mbr.kind == CORTO_BASE) {
                        if (json_deser_type(offset, memberType, value, data)) {
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
void* json_deser_get_elem(
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
int16_t json_deser_collection_oper(
    void *p,
    corto_collection t,
    const char *key,
    JSON_Value *v,
    json_deser_t *data)
{
    if (t->kind != CORTO_LIST) {
        corto_throw(
          "collection operations are currently only supported for lists");
        goto error;
    } else {
        corto_ll list = *(corto_ll*)p;

        if (!strcmp(key, "$append")) {
            void *ptr = NULL;
            if (corto_collection_requiresAlloc(t->elementType)) {
                ptr = corto_ptr_new(t->elementType);
                corto_ll_append(list, ptr);
            } else {
                ptr = corto_ll_append(list, NULL);
            }

            corto_try (json_deser_item(ptr, t->elementType, v, data), NULL);

        } else if (!strcmp(key, "$set")) {
            void *ptr = NULL;
            JSON_Object *obj = json_value_get_object(v);
            if (!obj) {
                corto_throw("expected object for '$set' operation, got %s",
                  json_type_str(v));
                goto error;
            }

            int i;
            for (i = 0; i < json_object_get_count(obj); i++) {
                const char *key = json_object_get_name(obj, i);
                JSON_Value *value = json_object_get_value_at(obj, i);

                int index = atoi(key);
                if (index > corto_ll_count(list)) {
                    corto_throw("index %d in set exceeds list bounds (%d)",
                      index, corto_ll_count(list));
                    goto error;
                }

                if (corto_collection_requiresAlloc(t->elementType)) {
                    ptr = corto_ll_get(list, index);
                } else {
                    ptr = corto_ll_getPtr(list, index);
                }

                corto_try (
                  json_deser_item(ptr, t->elementType, value, data), NULL);
            }

        } else if (!strcmp(key, "$remove")) {
            JSON_Array *array = json_value_get_array(v);
            if (!array) {
                corto_throw("expected array for '$remove' operation, got %s",
                  json_type_str(v));
                goto error;
            }

            int i, removed = 0, last = -1, count = corto_ll_count(list);
            for (i = 0; i < json_array_get_count(array); i++) {
                JSON_Value *value = json_array_get_value(array, i);
                if (json_value_get_type(value) != JSONNumber) {
                    corto_throw("expected number in '$remove' array, got '%s'",
                      json_type_str(v));
                }

                int index = json_value_get_number(value);
                if (index < last) {
                    corto_throw(
              "index %d in '$remove' cannot be lower than previous index (%d)",
                      index, last);
                    goto error;
                }

                if (index > count) {
                    corto_throw("index %d in '$remove' exceeds list bounds (%d)",
                      index, corto_ll_count(list));
                    goto error;
                }

                index -= removed;

                if (!corto_ll_remove_at(list, index)) {
                    corto_throw("failed to remove index %d", index);
                }

                last = index;
                removed ++;
            }
        } else {
            /* Try non-list operations */
            corto_try(json_deser_oper(p, corto_type(t), key, v, data), NULL);
        }
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deser_collection(
    void* p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data)
{
    corto_assert(t->kind == CORTO_COLLECTION, "not deserializing collection");
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
            void *elementPtr = json_deser_get_elem(p, corto_collection(t), i);
            corto_assert(
              elementPtr != NULL,
              "invalid element ptr for index %d returned by JSON deserializer",
              i);
            JSON_Value *elem = json_array_get_value(a, i);
            if (json_deser_item(elementPtr, elementType, elem, data)) {
                goto error;
            }
        }
    } else if (json_value_get_type(v) == JSONNull) {
        if (corto_ptr_resize(p, t, 0)) {
            goto error;
        }
    } else if (json_value_get_type(v) == JSONObject) {
        /* Collection operations */
        JSON_Object *obj = json_value_get_object(v);
        int i;
        for (i = 0; i < json_object_get_count(obj); i++) {
            const char *key = json_object_get_name(obj, i);
            JSON_Value *value = json_object_get_value_at(obj, i);
            if (json_deser_collection_oper(
                p, corto_collection(t), key, value, data))
            {
                goto error;
            }
        }
    } else {
        corto_throw(
            "expected array, got '%s'",
            json_type_str(v));
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int16_t json_deser_type(
    void *p,
    corto_type t,
    JSON_Value *v,
    json_deser_t *data)
{
    switch (t->kind) {
    case CORTO_VOID:
        /* Nothing to deserialize */
        break;
    case CORTO_ANY:
        if (json_deser_any(p, t, v, data)) {
            goto error;
        }
        break;
    case CORTO_PRIMITIVE:
        if (json_deser_primitive(p, t, v)) {
            goto error;
        }
        break;
    case CORTO_COMPOSITE:
        if (json_deser_composite(p, t, v, data)) {
            goto error;
        }
        break;
    case CORTO_COLLECTION:
        if (json_deser_collection(p, t, v, data)) {
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
    JSON_Value *jsonValue,
    corto_fmt_opt *opt)
{
    void *ptr = corto_value_ptrof(v);
    corto_type type = corto_value_typeof(v);
    json_deser_t walkData = {.opt = opt};

    if (json_deser_type(ptr, type, jsonValue, &walkData)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int16_t json_deserialize(
    corto_value *v,
    char *json,
    corto_fmt_opt *opt)
{
    corto_assert(v != NULL, "NULL passed to json_deserialize");
    corto_log_push("json");
    corto_debug("deserialize string %s", json);

    JSON_Value *jsonValue = json_parse_string(json);
    if (!jsonValue) {
        corto_throw("invalid JSON '%s'", json);
        goto error;
    }

    if (jsonValue && json_deserialize_from_JSON_Value(v, jsonValue, opt)) {
        corto_throw("failed to deserialize '%s'", json);
        goto error;
    }

    if (jsonValue) {
        json_value_free(jsonValue);
    }
    corto_log_pop();
    return 0;
error:
    if (jsonValue) {
        json_value_free(jsonValue);
    }
    corto_log_pop();
    return -1;
}
