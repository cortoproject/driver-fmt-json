/* json.h
 *
 * This file is generated. Do not modify.
 */

#ifndef JSON_H
#define JSON_H

#include "corto/corto.h"
#include "corto/fmt/json/_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* $header() */
#include "corto/fmt/json/parson.h"

#define CORTO_JSON_ROOT_NAME "::"
#define CORTO_JSON_ROOT_PARENT ""

/* JSON serializer data */
typedef struct json_ser_t {
    /* Public */
    corto_string buffer;
    corto_string ptr;
    unsigned int length;
    unsigned int maxlength;
    unsigned int itemCount;
    corto_bool serializeMeta;
    corto_bool serializeValue;
    corto_bool serializeScope;
    corto_bool alwaysIncludeHeaders;
    corto_bool serializePrefix;
} json_ser_t;

struct corto_serializer_s corto_json_ser(corto_modifier access, corto_operatorKind accessKind, corto_serializerTraceKind trace);

corto_string json_serialize(corto_object o);
corto_int16 json_deserialize(corto_object o, corto_string s);

corto_string json_fromCorto(corto_object o);
corto_int16 json_toCorto(corto_object o, corto_string json);
void json_release(corto_string json);

/* $end */

#ifdef __cplusplus
}
#endif
#endif
