
#include "json.h"

/*
 * Serialize from/to JSON object encoding and corto objects:
 *
 * {
 *     "id": "MyObject",
 *     "type": "int32",
 *     "value": 10,
 *     "scope": [
 *         {"id": "MyChild", "type": "float32", ...}
 *     ]
 * }
 *
 */

/* Extract id from full path. This method changes the id. */
static
void json_splitId(
    char* fullpath,
    char** parent_out,
    char** id_out)
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

/* Declare corto object from corto_result */
static
corto_object json_declare(
    corto_result *r)
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
        o = corto_declare(root_o, fullId, type);
        if (!o) {
            corto_throw("failed to create '%s'", fullId);
        }
        corto_release(type);
    } else {
        o = corto_declare(NULL, NULL, type);
        corto_release(type);
    }

    return o;
errorNotType:
    corto_release(type);
errorTypeNotFound:
    return NULL;
}

static
int16_t json_toResultMeta(
    corto_result *r,
    JSON_Value *topValue,
    corto_string json)
{
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
    const char* fullId = json_object_get_string(topObject, "id");

    char *parent = NULL, *id = NULL, *fullIdCpy = NULL;
    if (fullId) {
        fullIdCpy = corto_strdup(fullId);
        json_splitId(fullIdCpy, &parent, &id);
    }

    /* If no id is provided, randomize id */
    r->id = id ? corto_strdup(id) : NULL;

    /* If no parent is provided, assume root */
    r->parent = parent ? corto_strdup(parent) : NULL;

    /* If no type is provided, check if parent type has a defaultType */
    r->type = (char*)type;
    if (!type) {
        corto_object parent;
        corto_type type = NULL;
        if (!r->parent) {
            parent = root_o;
        } else {
            parent = corto(
                CORTO_LOOKUP,
                {.parent = root_o, .id = r->parent}
            );
        }
        if (parent) {
            if (corto_instanceof(corto_tableinstance_o, parent)) {
                type = corto_tableinstance(parent)->type;
            } else {
                corto_type parentType = corto_typeof(parent);
                type = parentType->options.defaultType;
            }
            corto_release(parent);
        }
        if (!type) {
            corto_throw("missing 'type' field in '%s'", json);
            goto error;
        } else {
            corto_id id;
            r->type = corto_strdup(corto_fullpath(id, type));
        }
    }

    if (fullIdCpy) corto_dealloc(fullIdCpy);

    return 0;
error:
    return -1;
}

/* Serialize JSON encoded object to corto_result */
int16_t json_toResult(
    corto_result *r,
    corto_string json)
{
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

/* Serialize object value to a corto object */
int16_t json_serialize_from_JSON_Value(
    corto_object *o,
    JSON_Value *topValue,
    char *json)
{
    corto_result r;
    corto_object result = NULL;
    bool newObject = FALSE;

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
