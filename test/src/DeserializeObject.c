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
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_bool_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(*dst == true);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
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
    test_assert(dst != NULL);

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
    test_assert(dst != NULL);

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
    test_assert(dst != NULL);

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
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"int32\","
        "   \"value\": 10,"
        "   \"scope\":["
        "      {\"id\":\"child\", \"type\": \"int32\", \"value\": 20}"
        "   ]"
        "}";

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assertint(*dst, 10);
    test_assert(corto_check_state(dst, CORTO_VALID));

    /* Lookup child */
    int32_t *child = corto_lookup(dst, "child");
    test_assert(child != NULL);

    test_assertstr(corto_idof(child), "child");
    test_assert(corto_typeof(child) == (corto_type)corto_int32_o);
    test_assert(corto_parentof(child) == dst);
    test_assertint(*child, 20);
    test_assert(corto_check_state(child, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserMultipleChildren(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"int32\","
        "   \"value\": 10,"
        "   \"scope\":["
        "      {\"id\":\"child1\", \"type\": \"int32\", \"value\": 20},"
        "      {\"id\":\"child2\", \"type\": \"float32\", \"value\": 30}"
        "   ]"
        "}";

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assertint(*dst, 10);
    test_assert(corto_check_state(dst, CORTO_VALID));

    /* Lookup child1 */
    int32_t *child1 = corto_lookup(dst, "child1");
    test_assert(child1 != NULL);

    test_assertstr(corto_idof(child1), "child1");
    test_assert(corto_typeof(child1) == (corto_type)corto_int32_o);
    test_assert(corto_parentof(child1) == dst);
    test_assertint(*child1, 20);
    test_assert(corto_check_state(child1, CORTO_VALID));

    /* Lookup child2 */
    float *child2 = corto_lookup(dst, "child2");
    test_assert(child2 != NULL);

    test_assertstr(corto_idof(child2), "child2");
    test_assert(corto_typeof(child2) == (corto_type)corto_float32_o);
    test_assert(corto_parentof(child2) == dst);
    test_assertflt(*child2, 30.0);
    test_assert(corto_check_state(child2, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
    corto_release(child2);
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
    test_assert(dst != NULL);

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
    const char *json =
        "{"
        "   \"id\":\"data/Point\","
        "   \"type\":\"struct\","
        "   \"scope\":["
        "      {\"id\":\"x\", \"value\":{\"type\":\"int32\"}},"
        "      {\"id\":\"y\", \"value\":{\"type\":\"float32\"}}"
        "   ]"
        "}";

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");

    test_assertstr(corto_idof(dst), "Point");
    test_assert(corto_typeof(dst) == (corto_type)corto_struct_o);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_check_state(dst, CORTO_VALID));

    /* Lookup x */
    corto_member x = corto_lookup(dst, "x");
    test_assert(x != NULL);

    test_assertstr(corto_idof(x), "x");
    test_assert(corto_typeof(x) == (corto_type)corto_member_o);
    test_assert(corto_parentof(x) == dst);
    test_assert(x->type == (corto_type)corto_int32_o);
    test_assert(corto_check_state(x, CORTO_VALID));

    /* Lookup y */
    corto_member y = corto_lookup(dst, "y");
    test_assert(y != NULL);

    test_assertstr(corto_idof(y), "y");
    test_assert(corto_typeof(y) == (corto_type)corto_member_o);
    test_assert(corto_parentof(y) == dst);
    test_assert(y->type == (corto_type)corto_float32_o);
    test_assert(corto_check_state(y, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
    corto_release(x);
    corto_release(y);
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
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");
    corto_type type = corto_typeof(dst);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(type->kind == CORTO_COLLECTION);
    test_assert(corto_collection(type)->kind == CORTO_LIST);
    test_assert(corto_collection(type)->element_type == (corto_type)corto_int32_o);
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
    test_assert(dst != NULL);

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
    test_assert(dst != NULL);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(*dst == 10);
    test_assert(corto_parentof(dst) == parent);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(parent) == 0);
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
    test_assert(dst != NULL);

    corto_object data_o = corto_lookup(NULL, "data");
    corto_type type = corto_typeof(dst);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(type->kind == CORTO_COLLECTION);
    test_assert(corto_collection(type)->kind == CORTO_LIST);
    test_assert(corto_collection(type)->element_type == (corto_type)corto_int32_o);
    test_assert(corto_ll_count(*dst) == 0);
    test_assert(corto_parentof(dst) == data_o);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(dst) == 0);
    corto_release(data_o);
}

void test_DeserializeObject_tc_deserDefaultTypeAndValue(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent/obj\""
        "}";

    corto_object parent = corto_create(
        root_o, "data/parent", test_DefaultType_o);
    test_assert(parent != NULL);

    int32_t *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    test_assert(ret == 0);
    test_assert(dst != NULL);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(*dst == 0);
    test_assert(corto_parentof(dst) == parent);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(parent) == 0);
}

void test_DeserializeObject_tc_deserChildDefaultType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent\","
        "   \"type\":\"test/DefaultType\","
        "   \"scope\":["
        "       {\"id\": \"obj\", \"value\": 10}"
        "   ]"
        "}";

    corto_object data_o = corto_lookup(NULL, "data");
    test_assert(data_o != NULL);

    test_DefaultType *parent = NULL;
    int16_t ret = corto_deserialize(&parent, "text/json", json);
    test_assert(ret == 0);
    test_assert(parent != NULL);

    test_assertstr(corto_idof(parent), "parent");
    test_assert(corto_typeof(parent) == (corto_type)test_DefaultType_o);
    test_assert(corto_parentof(parent) == data_o);
    test_assert(corto_check_state(parent, CORTO_VALID));

    int32_t *dst = corto_lookup(NULL, "data/parent/obj");
    test_assert(dst != NULL);

    test_assertstr(corto_idof(dst), "obj");
    test_assert(corto_typeof(dst) == (corto_type)corto_int32_o);
    test_assert(*dst == 10);
    test_assert(corto_parentof(dst) == parent);
    test_assert(corto_check_state(dst, CORTO_VALID));

    test_assert(corto_delete(parent) == 0);
    corto_release(dst);
}

void test_DeserializeObject_tc_deserNotAType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"/data\""
        "}";

    corto_ll *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    corto_catch();

    test_assert(ret != 0);
    test_assert(dst == NULL);
}

void test_DeserializeObject_tc_deserUnresolvedChildType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent\","
        "   \"type\":\"int32\","
        "   \"scope\":["
        "       {\"id\": \"obj\", \"type\":\"data/does_not_exist\"}"
        "   ]"
        "}";

    corto_object int32_o = corto_resolve(NULL, "int32");
    int32_t prev_count = corto_countof(int32_o);

    test_DefaultType *parent = NULL;
    int16_t ret = corto_deserialize(&parent, "text/json", json);
    corto_catch();

    test_assert(ret != 0);
    test_assert(parent == NULL);

    /* Test if resources are cleaned up by checking refcount of type */
    test_assert(corto_countof(int32_o) == prev_count);
}

void test_DeserializeObject_tc_deserUnresolvedType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/obj\","
        "   \"type\":\"/data/does_not_exist\""
        "}";

    corto_ll *dst = NULL;
    int16_t ret = corto_deserialize(&dst, "text/json", json);
    corto_catch();

    test_assert(ret != 0);
    test_assert(dst == NULL);
}

void test_DeserializeObject_tc_deserMismatchParentType(
    test_DeserializeObject this)
{
    const char *json =
        "{"
        "   \"id\":\"data/parent\","
        "   \"type\":\"float32\","
        "   \"scope\":["
        "       {\"id\": \"obj\", \"type\":\"test/ParentType\"}"
        "   ]"
        "}";

    int32_t prev_count = corto_countof(test_DefaultType_o);

    test_DefaultType *parent = NULL;
    int16_t ret = corto_deserialize(&parent, "text/json", json);
    corto_catch();

    test_assert(ret != 0);
    test_assert(parent == NULL);

    /* Test if resources are cleaned up by checking refcount of type */
    test_assert(corto_countof(test_DefaultType_o) == prev_count);
}
