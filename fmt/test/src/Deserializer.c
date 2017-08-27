/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

void test_Deserializer_tc_deserBinary(
    test_Deserializer this)
{
    corto_word *o = corto_create(corto_word_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserBitmask(
    test_Deserializer this)
{
    corto_word *o = corto_create(test_Sandwich_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"Beef|Lettuce|Tomato\"");
    test_assert(ret == 0);
    test_assertint(*o, Test_Beef | Test_Lettuce | Test_Tomato);
    corto_delete(o);

}

void test_Deserializer_tc_deserBoolFalse(
    test_Deserializer this)
{
    corto_bool *o = corto_create(corto_bool_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "false");
    test_assert(ret == 0);
    test_assertint(*o, FALSE);
    corto_delete(o);

}

void test_Deserializer_tc_deserBoolTrue(
    test_Deserializer this)
{
    corto_bool *o = corto_create(corto_bool_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "true");
    test_assert(ret == 0);
    test_assertint(*o, TRUE);
    corto_delete(o);

}

void test_Deserializer_tc_deserChar(
    test_Deserializer this)
{
    corto_char *o = corto_create(corto_char_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"a\"");
    test_assert(ret == 0);
    test_assertint(*o, 'a');
    corto_delete(o);

}

void test_Deserializer_tc_deserCharEscape(
    test_Deserializer this)
{
    corto_char *o = corto_create(corto_char_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"\\n\"");
    test_assert(ret == 0);
    test_assertint(*o, '\n');
    corto_delete(o);

}

void test_Deserializer_tc_deserComposite(
    test_Deserializer this)
{
    test_Point *p = corto_create(test_Point_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    corto_delete(p);

}

void test_Deserializer_tc_deserCompositeList(
    test_Deserializer this)
{
    test_PointList *l = corto_create(test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(&v, "[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}]");
    test_assert(ret == 0);
    test_assertint(corto_ll_size(*l), 2);

    test_Point *p = corto_ll_get(*l, 0);
    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    p = corto_ll_get(*l, 1);
    test_assertint(p->x, 30);
    test_assertint(p->y, 40);

    corto_delete(l);

}

void test_Deserializer_tc_deserCompositeNested(
    test_Deserializer this)
{
    test_Line *l = corto_create(test_Line_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_Line_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(&v, "{\"start\":{\"x\":10, \"y\":20}, \"stop\":{\"x\":30, \"y\":40}}");
    test_assert(ret == 0);

    test_assertint(l->start.x, 10);
    test_assertint(l->start.y, 20);
    test_assertint(l->stop.x, 30);
    test_assertint(l->stop.y, 40);

    corto_delete(l);

}

void test_Deserializer_tc_deserCompositeNestedRef(
    test_Deserializer this)
{
    test_LineRef l = corto_create(test_LineRef_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_LineRef_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(
      &v,
      "{\"start\":{\"type\":\"test/PointRef\",\"value\":{\"x\":10,\"y\":20}},\"stop\":{\"type\":\"test/PointRef\",\"value\":{\"x\":30,\"y\":40}}}"
    );
    test_assert(ret == 0);

    test_assertint(l->start->x, 10);
    test_assertint(l->start->y, 20);
    test_assertint(l->stop->x, 30);
    test_assertint(l->stop->y, 40);

    corto_delete(l);

}

void test_Deserializer_tc_deserCompositeObservable(
    test_Deserializer this)
{
    test_ObservableType *o = corto_create(test_ObservableType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_ObservableType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeOptional(
    test_Deserializer this)
{
    test_OptionalType *o = corto_create(test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeOptionalNotset(
    test_Deserializer this)
{
    test_OptionalType *o = corto_create(test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z == NULL);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeRef(
    test_Deserializer this)
{
    test_Point *p = corto_create(test_PointRef_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_PointRef_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    corto_delete(p);

}

void test_Deserializer_tc_deserCompositeTarget(
    test_Deserializer this)
{
    test_TargetType *o = corto_create(test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z->target == 0);
    test_assert(o->z->actual == 40);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeTargetNotOwned(
    test_Deserializer this)
{
    test_TargetType *o = corto_create(test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_mount m = corto_create(corto_mount_o);
    test_assert(m != NULL);
    corto_setOwner(m);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
    test_assert(ret == 0);

    corto_setOwner(NULL);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z->target == 30);
    test_assert(o->z->actual == 0);

    corto_delete(o);

}

void test_Deserializer_tc_deserEnum(
    test_Deserializer this)
{
    test_Color *o = corto_create(test_Color_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"Yellow\"");
    test_assert(ret == 0);
    test_assertint(*o, Test_Yellow);
    corto_delete(o);

}

void test_Deserializer_tc_deserInheritance(
    test_Deserializer this)
{
    test_Point3D *p = corto_create(test_Point3D_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point3D_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(&v, "{\"super\":{\"x\":10, \"y\":20}, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(p->super.x, 10);
    test_assertint(p->super.y, 20);
    test_assertint(p->z, 30);

    corto_delete(p);

}

void test_Deserializer_tc_deserInheritanceRef(
    test_Deserializer this)
{
    test_Point3D *p = corto_create(test_Point3D_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point3D_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(&v, "{\"super\":{\"x\":10, \"y\":20}, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(p->super.x, 10);
    test_assertint(p->super.y, 20);
    test_assertint(p->z, 30);

    corto_delete(p);

}

void test_Deserializer_tc_deserInt16(
    test_Deserializer this)
{
    corto_int16 *o = corto_create(corto_int16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt16Minus(
    test_Deserializer this)
{
    corto_int16 *o = corto_create(corto_int16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt32(
    test_Deserializer this)
{
    corto_int32 *o = corto_create(corto_int32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt32Minus(
    test_Deserializer this)
{
    corto_int32 *o = corto_create(corto_int32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt64(
    test_Deserializer this)
{
    corto_int64 *o = corto_create(corto_int64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt64Minus(
    test_Deserializer this)
{
    corto_int64 *o = corto_create(corto_int64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt8(
    test_Deserializer this)
{
    corto_int8 *o = corto_create(corto_int8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt8Minus(
    test_Deserializer this)
{
    corto_int8 *o = corto_create(corto_int8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserList(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(&v, "[10, 20, 30]");
    test_assert(ret == 0);
    test_assertint(corto_ll_size(*l), 3);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*l, 1), 20);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*l, 2), 30);

    corto_delete(l);

}

void test_Deserializer_tc_deserNaN(
    test_Deserializer this)
{
    corto_float64 *o = corto_create(corto_float64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "null");
    test_assert(ret == 0);
    test_assert(CORTO_ISNAN(*o));
    corto_delete(o);

}

void test_Deserializer_tc_deserObject(
    test_Deserializer this)
{
    corto_object o = NULL;

    corto_int16 r = json_toObject(&o, "{\"id\": \"a\", \"type\": \"int8\", \"value\": 8}");
    test_assertint(r, 0);
    test_assert(o != NULL);
    test_assert(corto_fullpath(NULL, o) != NULL);
    test_assertstr(corto_fullpath(NULL, o), "/a");
    test_assert(corto_typeof(o) == (corto_type)corto_int8_o);
    test_assertint(*(corto_int8*)o, 8);
    corto_delete(o);
}

void test_Deserializer_tc_deserObjectErrorParsing(
    test_Deserializer this)
{
    corto_object o = NULL;
    corto_int16 r = json_toObject(&o, "{\"id\", \"/a/b/c\", \"value\": 9, \"type\", \"int16\"}");
    test_assert(r != 0);
    char* lasterr = corto_lasterr();
    test_assertstr(lasterr, "json: error parsing '{\"id\", \"/a/b/c\", \"value\": 9, \"type\", \"int16\"}'");
    corto_seterr(NULL);
}

void test_Deserializer_tc_deserObjectNonFullyScopedName(
    test_Deserializer this)
{
corto_object a = corto_createChild(root_o, "a", corto_int8_o);
corto_object b = corto_createChild(a, "b", corto_int8_o);
test_assert(a != NULL);
test_assert(b != NULL);
corto_object c = NULL;
corto_int16 r = json_toObject(&c, "{\"id\": \"a/b/c\", \"value\": 9, \"type\": \"int16\"}");
test_assert(r == 0);
test_assert(corto_fullpath(NULL, c) != NULL);
test_assertstr(corto_fullpath(NULL, c), "/a/b/c");
test_assert(corto_typeof(c) == (corto_type)corto_int16_o);
test_assert(*(corto_int16*)c == 9);
}

void test_Deserializer_tc_deserObjectScoped(
    test_Deserializer this)
{
    corto_object a = corto_createChild(root_o, "a", corto_int8_o);
    corto_object b = corto_createChild(a, "b", corto_int8_o);
    test_assert(a != NULL);
    test_assert(b != NULL);
    corto_object c = NULL;
    corto_int16 r = json_toObject(&c, "{\"id\": \"/a/b/c\", \"value\": 9, \"type\": \"int16\"}");
    test_assert(r == 0);
    test_assert(corto_fullpath(NULL, c) != NULL);
    test_assertstr(corto_fullpath(NULL, c), "/a/b/c");
    test_assert(corto_typeof(c) == (corto_type)corto_int16_o);
    test_assert(*(corto_int16*)c == 9);
}

void test_Deserializer_tc_deserReference(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":\"/corto/vstore/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymous(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"int32\",\"value\":10}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(corto_int32_o));
    test_assertint(*(corto_int32*)o->r, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousCollection(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"/test/myList\",\"value\":[10,20,30]}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_myList_o));
    test_assertint(corto_ll_size(*(corto_ll*)o->r), 3);

    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 0), 10);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 1), 20);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 2), 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousCollectionAnonymousType(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"list{int32}\",\"value\":[10,20,30]}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    corto_type t = corto_typeof(o->r);
    test_assert(t->kind == CORTO_COLLECTION);
    test_assert(corto_collection(t)->kind == CORTO_LIST);
    test_assert(corto_collection(t)->elementType == corto_type(corto_int32_o));
    test_assertint(corto_ll_size(*(corto_ll*)o->r), 3);

    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 0), 10);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 1), 20);
    test_assertint((corto_int32)(corto_word)corto_ll_get(*(corto_ll*)o->r, 2), 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousComplex(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_Point_o));
    test_assertint(test_Point(o->r)->x, 10);
    test_assertint(test_Point(o->r)->y, 20);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousComplexWithString(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"Hello\",\"s2\":\"World\"}}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_StringType_o));
    test_assertstr(test_StringType(o->r)->s1, "Hello");
    test_assertstr(test_StringType(o->r)->s2, "World");
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousComplexWithStringEsc(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"\\\"Hello\\\"\",\"s2\":\"\\\"World\\\"\"}}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_StringType_o));
    test_assertstr(test_StringType(o->r)->s1, "\"Hello\"");
    test_assertstr(test_StringType(o->r)->s2, "\"World\"");
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceFromLang(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":\"/corto/vstore/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceNull(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"r\":\"/corto/vstore/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);

    ret = json_toValue(&v, "{\"r\":null}");
    test_assert(o->r == NULL);
    corto_delete(o);

}

void test_Deserializer_tc_deserString(
    test_Deserializer this)
{
    corto_string *o = corto_create(corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"Hello World\"");
    test_assert(ret == 0);
    test_assertstr(*o, "Hello World");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringEmpty(
    test_Deserializer this)
{
    corto_string *o = corto_create(corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"\"");
    test_assert(ret == 0);
    test_assertstr(*o, "");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringEscape(
    test_Deserializer this)
{
    corto_string *o = corto_create(corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"Hello World\\n\"");
    test_assert(ret == 0);
    test_assertstr(*o, "Hello World\n");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringNull(
    test_Deserializer this)
{
    corto_string *o = corto_create(corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "null");
    test_assert(ret == 0);
    test_assert(*o == NULL);
    corto_delete(o);

}

void test_Deserializer_tc_deserStringWhitespace(
    test_Deserializer this)
{
    corto_string *o = corto_create(corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "\"  Hello World  \"");
    test_assert(ret == 0);
    test_assertstr(*o, "  Hello World  ");
    corto_delete(o);

}

void test_Deserializer_tc_deserUint16(
    test_Deserializer this)
{
    corto_uint16 *o = corto_create(corto_uint16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint32(
    test_Deserializer this)
{
    corto_uint32 *o = corto_create(corto_uint32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint64(
    test_Deserializer this)
{
    corto_uint64 *o = corto_create(corto_uint64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint8(
    test_Deserializer this)
{
    corto_uint8 *o = corto_create(corto_uint8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUnion(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"_d\":1,\"flt\":10.5}");
    test_assert(ret == 0);
    test_assertint(o->d, 1);
    test_assertflt(o->is.flt, 10.5);

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionChangeDiscriminator(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"_d\":1,\"flt\":10.5}");
    test_assert(ret == 0);
    test_assertint(o->d, 1);
    test_assertflt(o->is.flt, 10.5);

    ret = json_toValue(&v, "{\"_d\":2,\"str\":\"foo\"}");
    test_assert(ret == 0);
    test_assertint(o->d, 2);
    test_assertstr(o->is.str, "foo");

    ret = json_toValue(&v, "{\"_d\":0,\"num\":20}");
    test_assert(ret == 0);
    test_assertint(o->d, 0);
    test_assertint(o->is.num, 20);

    ret = json_toValue(&v, "{\"_d\":4,\"pt\":{\"x\":10,\"y\":20}}");
    test_assert(ret == 0);
    test_assertint(o->d, 4);
    test_assertint(o->is.pt.x, 10);
    test_assertint(o->is.pt.y, 20);

    ret = json_toValue(&v, "{\"_d\":3,\"str\":\"bar\"}");
    test_assert(ret == 0);
    test_assertint(o->d, 3);
    test_assertstr(o->is.str, "bar");

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionComplex(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"_d\":4,\"pt\":{\"x\":10,\"y\":20}}");
    test_assert(ret == 0);
    test_assertint(o->d, 4);
    test_assertflt(o->is.pt.x, 10);
    test_assertflt(o->is.pt.y, 20);

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionDefault(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"_d\":5,\"other\":30}");
    test_assert(ret == 0);
    test_assertint(o->d, 5);
    test_assertflt(o->is.other, 30);

    ret = json_toValue(&v, "{\"_d\":6,\"other\":40}");
    test_assert(ret == 0);
    test_assertint(o->d, 6);
    test_assertflt(o->is.other, 40);

    corto_delete(o);

}

void test_Deserializer_tc_deserVoid(
    test_Deserializer this)
{
    corto_void *o = corto_create(corto_void_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{}");
    test_assert(ret == 0);
    corto_delete(o);

}

void test_Deserializer_tc_serAnyCollection(
    test_Deserializer this)
{
    corto_any *o = corto_create(corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"type\":\"list{int32}\",\"value\":[10,20,30]}");
    test_assert(ret == 0);
    test_assertstr(corto_fullpath(NULL, o->type), "list{int32,0}");
    test_assert(o->value != NULL);
    test_assert(o->owner == TRUE);

    corto_ll list = *(corto_ll*)o->value;
    test_assertint(corto_ll_size(list), 3);
    test_assertint((corto_word)corto_ll_get(list, 0), 10);
    test_assertint((corto_word)corto_ll_get(list, 1), 20);
    test_assertint((corto_word)corto_ll_get(list, 2), 30);

    test_assert(corto_delete(o) == 0);

}

void test_Deserializer_tc_serAnyComposite(
    test_Deserializer this)
{
    corto_any *o = corto_create(corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}");
    test_assert(ret == 0);
    test_assert(corto_type(test_Point_o) == o->type);
    test_assert(o->value != NULL);
    test_assert(o->owner == TRUE);

    test_Point *p = o->value;
    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    test_assert(corto_delete(o) == 0);

}

void test_Deserializer_tc_serAnyPrimitive(
    test_Deserializer this)
{
    corto_any *o = corto_create(corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(&v, "{\"type\":\"int32\",\"value\":10}");
    test_assert(ret == 0);
    test_assert(corto_type(corto_int32_o) == o->type);
    test_assert(o->value != NULL);
    test_assert(o->owner == TRUE);

    test_assertint(*(corto_int32*)o->value, 10);

    test_assert(corto_delete(o) == 0);

}

