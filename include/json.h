/* json.h
 *
 * This file is generated. Do not modify.
 */

#ifndef CORTO_FMT_JSON_H
#define CORTO_FMT_JSON_H

#include <corto>

#ifdef __cplusplus
extern "C" {
#endif

int16_t json_toObject(
    corto_fmt_opt* opt,
    corto_object* o,
    char *s);

char* json_fromObject(
    corto_fmt_opt* opt,
    corto_object o);

int16_t json_toValue(
    corto_fmt_opt* opt,
    corto_value *v,
    char *json);

char* json_fromValue(
    corto_fmt_opt* opt,
    corto_value *v);

int16_t json_toResult(
    corto_fmt_opt* opt,
    corto_record *r,
    char *json);

corto_word json_fromResult(
    corto_fmt_opt* opt,
    corto_record *r);

void json_release(
    char *json);

char* json_copy(
    char *json);

#ifdef __cplusplus
}
#endif

#endif
