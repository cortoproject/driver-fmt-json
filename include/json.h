/* json.h
 *
 * This file is generated. Do not modify.
 */

#ifndef CORTO_FMT_JSON_H
#define CORTO_FMT_JSON_H

#include <corto/corto.h>

#ifdef __cplusplus
extern "C" {
#endif

int16_t json_toObject(
    corto_object* o,
    char *s);

char* json_fromObject(
    corto_object o);

int16_t json_toValue(
    corto_value *v,
    char *json);

char* json_fromValue(
    corto_value *v);

int16_t json_toResult(
    corto_result *r,
    char *json);

corto_word json_fromResult(
    corto_result *r);

void json_release(
    char *json);

char* json_copy(
    char *json);

#ifdef __cplusplus
}
#endif

#endif
