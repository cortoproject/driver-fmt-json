/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
void test_DeserializeObject_tc_deserBool(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"bool\","
        "   \"value\":true"
        "}";

    bool *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_bool_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(*dst == true);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserChildDefaultType(
    test_DeserializeObject this)
{
    /* Insert implementation */
}

void test_DeserializeObject_tc_deserCollection(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"test/intList\","
        "   \"value\":[10, 20, 30]"
        "}";

    corto_ll *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)test_intList_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_ll_count(*dst) == 3);
    test_assert((intptr_t)corto_ll_get(*dst, 0) == 10);
    test_assert((intptr_t)corto_ll_get(*dst, 1) == 20);
    test_assert((intptr_t)corto_ll_get(*dst, 2) == 30);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserComposite(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"test/Point\","
        "   \"value\":{\"x\":10, \"y\":20}"
        "}";

    test_Point *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)test_Point_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assertint(dst->x, 10);
    test_assertint(dst->y, 20);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserMultipleChildren(
    test_DeserializeObject this)
{
    /* Insert implementation */
}

void test_DeserializeObject_tc_deserNumber(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"int32\","
        "   \"value\":10"
        "}";

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(*dst == 10);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserSingleChild(
    test_DeserializeObject this)
{

}

void test_DeserializeObject_tc_deserString(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"string\","
        "   \"value\":\"Hello World\""
        "}";

    corto_string *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_string_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assertstr(*dst, "Hello World");
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserStructType(
    test_DeserializeObject this)
{
    /*const char *json =
        "{"
        "   \"id\":\"data/Point\","
        "   \"type\":\"struct\","
        "   \"scope\":["
        "      {\"id\":\"x\", \"value\":{\"type\":\"int32\"}},"
        "      {\"id\":\"y\", \"value\":{\"type\":\"int32\"}}"
        "   ]"
        "}";

    corto_struct dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "Point");
    test_assert(corto_typeof(dst) == (corto_type)corto_struct_o);
    test_assert(corto_collection(type)->kind == CORTO_LIST);
    test_assert(corto_collection(type)->elementType == (corto_type)corto_int32_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_ll_count(*dst) == 3);
    test_assert((intptr_t)corto_ll_get(*dst, 0) == 10);
    test_assert((intptr_t)corto_ll_get(*dst, 1) == 20);
    test_assert((intptr_t)corto_ll_get(*dst, 2) == 30);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);*/
}

void test_DeserializeObject_tc_deserAnonymousCollection(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"list{int32}\","
        "   \"value\":[10, 20, 30]"
        "}";

    corto_ll *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");
    corto_type type = corto_typeof(dst);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(type->kind == CORTO_COLLECTION);
    test_assert(corto_collection(type)->kind == CORTO_LIST);
    test_assert(corto_collection(type)->elementType == (corto_type)corto_int32_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_ll_count(*dst) == 3);
    test_assert((intptr_t)corto_ll_get(*dst, 0) == 10);
    test_assert((intptr_t)corto_ll_get(*dst, 1) == 20);
    test_assert((intptr_t)corto_ll_get(*dst, 2) == 30);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserDefaultType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent/obj\","
        "   \"value\":10"
        "}";

    corto_object parent = corto_create(
        root_o, "data/parent", test_DefaultType_o);
    test_assert(parent != NULL);

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(*dst == 10);
    test_assert(corto_parentof(dst) == parent);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
}

void test_DeserializeObject_tc_deserTableType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent/obj\","
        "   \"value\":10"
        "}";

    corto_tableinstance parent = corto_create(
        root_o, "data/parent", corto_tableinstance_o);
    test_assert(parent != NULL);

    corto_set_ref(&parent->type, corto_int32_o);

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(*dst == 10);
    test_assert(corto_parentof(dst) == parent);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
}

void test_DeserializeObject_tc_deserDefaultValue(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"list{int32}\""
        "}";

    corto_ll *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);

    corto_object data_o = corto_lookup(NULL, "data");
    corto_type type = corto_typeof(dst);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(type->kind == CORTO_COLLECTION);
    test_assert(corto_collection(type)->kind == CORTO_LIST);
    test_assert(corto_collection(type)->elementType == (corto_type)corto_int32_o);
    test_assert(corto_ll_count(*dst) == 0);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserDefaultTypeAndValue(
    test_DeserializeObject this)
{
    /* Insert implementation */
}
