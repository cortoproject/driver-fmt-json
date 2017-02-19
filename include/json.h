/* json.h
 *
 * This file is generated. Do not modify.
 */

#ifndef CORTO_FMT_JSON_H
#define CORTO_FMT_JSON_H

#include <corto/corto.h>

/* $header() */
#ifdef __cplusplus
extern "C" {
#endif
#include "corto/fmt/json/parson.h"

#define CORTO_JSON_ROOT_NAME "::"
#define CORTO_JSON_ROOT_PARENT ""

/* JSON serializer data */
typedef struct json_ser_t {
    corto_buffer buffer;
    unsigned int itemCount;
} json_ser_t;

struct corto_serializer_s corto_json_ser(corto_modifier access, corto_operatorKind accessKind, corto_serializerTraceKind trace);

corto_string json_serialize(corto_value *v);
corto_int16 json_deserialize(corto_value *v, corto_string s);
corto_int16 json_deserialize_from_JSON_Value(corto_value *v, JSON_Value *value);

corto_string json_typedescriptor(corto_type t);
corto_string json_fromValue(corto_value *v);
corto_int16 json_toValue(corto_value *v, corto_string json);
corto_int16 json_toObject(corto_object* o, corto_string s);
corto_string json_fromObject(corto_object o);
corto_int16 json_toResult(corto_result *r, corto_string json);
corto_word json_fromResult(corto_result *r);

void json_release(corto_string json);
corto_string json_copy(corto_string json);

/*
 * Expects a string in the format {"id": "parent/name", "value": "..."}
 */


#ifdef __cplusplus
}
#endif
/* $end */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
