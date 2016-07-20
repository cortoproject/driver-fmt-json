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

corto_string json_serialize(corto_object o);
corto_int16 json_deserialize(corto_object o, corto_string s);

corto_string json_typedescriptor(corto_type t);
corto_string json_fromCorto(corto_object o);
corto_int16 json_toCorto(corto_object o, corto_string json);
void json_release(corto_string json);

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

