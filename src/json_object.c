
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
    corto_object parent,
    corto_type type,
    corto_result *r)
{
    corto_object o = NULL;

    corto_assert(type != NULL, "NULL provided for type");

    if (r->id && r->parent) {
        corto_id fullId;
        if (strcmp(r->parent, "/")) {
            sprintf(fullId, "%s/%s", r->parent, r->id);
        } else {
            strcpy(fullId, r->id);
        }
        o = corto_declare(parent ? parent : root_o, fullId, type);
        if (!o) {
            corto_throw("failed to create '%s'", fullId);
            goto error;
        }
    } else {
        o = corto_declare(NULL, NULL, type);
    }

    return o;
error:
    return NULL;
}

static
int16_t json_toResultMeta(
    corto_object parent,
    corto_result *r,
    JSON_Value *topValue,
    char *id_buffer,
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

    char *parent_id = NULL, *id = NULL;
    if (fullId) {
        /* If parent is provided, prepend parent to id_buffer */
        if (parent && parent != root_o) {
            corto_fullpath(id_buffer, parent);
            strcat(id_buffer, "/");
            strcat(id_buffer, fullId);
        } else {
            strcpy(id_buffer, fullId);
        }
        json_splitId(id_buffer, &parent_id, &id);
    }

    /* If no id is provided, id will be randomized (if object is declared) */
    r->id = id;

    /* If no parent is provided, assume root */
    r->parent = parent_id;

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
            corto_throw("missing 'type' field for '%s' in '%s'", r->id, json);
            goto error;
        } else {
            corto_id id;
            r->type = corto_strdup(corto_fullpath(id, type));
        }
    }

    return 0;
error:
    return -1;
}

/* Serialize JSON encoded object to corto_result */
int16_t json_toResult(
    corto_fmt_opt* opt,
    corto_result *r,
    corto_string json)
{
    JSON_Value* topValue = json_parse_string(json);
    corto_id id_buffer;

    if (!topValue) {
        corto_throw("error parsing JSON %s", json);
        goto error;
    }

    memset(r, 0, sizeof(corto_result));
    if (json_toResultMeta(NULL, r, topValue, id_buffer, json)) {
        goto error_toResultMeta;
    }

    r->id = corto_strdup(r->id);
    r->parent = corto_strdup(r->parent);
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

corto_word json_fromResult(
    corto_fmt_opt* opt,
    corto_result *r)
{
    CORTO_UNUSED(r);
    return 0;
}

static
int16_t json_serialize_child_from_JSON_Value(
    corto_object parent,
    corto_object *o,
    JSON_Value *topValue,
    int parent_defined,
    char *json);

static
int16_t json_serialize_children(
    corto_object parent,
    JSON_Object *topObject,
    int parent_defined,
    char *json)
{
    JSON_Array *scope = json_object_get_array(topObject, "scope");

    int i;
    for (i = 0; i < json_array_get_count(scope); i++) {
        JSON_Value *child = json_array_get_value(scope, i);

        if (json_serialize_child_from_JSON_Value(
            parent, NULL, child, parent_defined, json))
        {
            corto_throw(NULL);
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
int16_t json_serialize_child_from_JSON_Value(
    corto_object parent,
    corto_object *o,
    JSON_Value *topValue,
    int parent_defined,
    char *json)
{
    corto_result r;
    corto_object result = NULL;
    bool newObject = FALSE;
    corto_id id_buffer;

    JSON_Object* topObject = json_value_get_object(topValue);

    memset(&r, 0, sizeof(corto_result));
    if (json_toResultMeta(parent, &r, topValue, id_buffer, json)) {
        goto error;
    }

    /* Resolve type */
    corto_type type = corto_resolve(NULL, r.type);
    if (!type) {
        corto_throw("cannot find '%s' ('%s')", r.type, json);
        goto error;
    }
    if (!corto_instanceof(corto_type_o, type)) {
        corto_throw("'%s' is not a type ('%s')", r.type, json);
        goto errorDeclare;
    }

    /* If parent is not yet defined and parentState of type is DECLARED, object
     * may only be created when parent is DECLARED. */
    if (parent_defined != -1) {
        if (type->options.parentState == CORTO_DECLARED) {
            if (parent_defined) {
                /* Object is already serialized */
                goto serialize_later_or_serialized;
            }
        } else {
            if (!parent_defined) {
                /* Object will be serialized after parent is defined */
                goto serialize_later_or_serialized;
            }
        }
    } else {
        /* Ignore parent_defined setting- used for top-level object in
         * JSON string. */
    }

    /* If object does not exist, declare it. Otherwise, check if type of the
     * provided object matches with the type in the JSON string */
    if (!o || !*o) {
        result = json_declare(parent, type, &r);
        if (!result) {
            goto errorDeclare;
        }
        newObject = TRUE;
    } else {
        if (corto_typeof(*o) != type) {
            corto_throw("object '%s' is not of type '%s' ('%s')",
              corto_fullpath(NULL, *o),
              corto_fullpath(NULL, type),
              json);
            goto errorDeclare;
        }
        if (o) {
            result = *o;
        }
    }
    corto_release(type);

    if (result) {
        /* Cannot serialize to builtin objects */
        if (corto_isbuiltin(result)) {
            corto_throw("cannot deserialize JSON to builtin object '%s'",
                corto_fullpath(NULL, result));
            goto error_toResultMeta;
        }
    } else {
        /* Failed to create a new object */
        corto_throw("failed to create JSON object from '%s'", json);
        goto error_toResultMeta;
    }

    /* Deserialize JSON into object value */
    JSON_Value* jsonValue = json_object_get_value(topObject, "value");
    if (jsonValue) {
        corto_value cortoValue = corto_value_object(result, NULL);
        if (json_deserialize_from_JSON_Value(&cortoValue, jsonValue)) {
            goto errorDeserialize;
        }
    }

    /* Serialize objects that should be defined before parent is defined */
    if (json_serialize_children(result, topObject, false, json)) {
        goto error;
    }

    /* If the object was created by the JSON deserializer, define it */
    if (newObject) {
        if (corto_define(result)) {
            goto errorDefine;
        }
    }

    /* Serialize objects that can/should be defined after parent is defined */
    if (json_serialize_children(result, topObject, true, json)) {
        goto error;
    }

    if (o) {
        *o = result;
    }

serialize_later_or_serialized:
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
    corto_release(type);
error_toResultMeta:
error:
    return -1;
}

/* Serialize object value to a corto object */
int16_t json_serialize_from_JSON_Value(
    corto_object *o,
    JSON_Value *topValue,
    char *json)
{
    return json_serialize_child_from_JSON_Value(NULL, o, topValue, -1, json);
}
