/* $CORTO_GENERATED
 *
 * Deserializer.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <test.h>

corto_void _test_Deserializer_tc_deserComposite(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserComposite) */
    test_Point *p = corto_create(test_Point_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point_o));

    corto_int16 ret = json_toCorto(p, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    corto_delete(p);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeList(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeList) */
    test_PointList *l = corto_create(test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    corto_int16 ret = json_toCorto(l, "[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}]");
    test_assert(ret == 0);
    test_assertint(corto_llSize(*l), 2);

    test_Point *p = corto_llGet(*l, 0);
    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    p = corto_llGet(*l, 1);
    test_assertint(p->x, 30);
    test_assertint(p->y, 40);

    corto_delete(l);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeNested(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeNested) */
    test_Line *l = corto_create(test_Line_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_Line_o));

    corto_int16 ret = json_toCorto(l, "{\"start\":{\"x\":10, \"y\":20}, \"stop\":{\"x\":30, \"y\":40}}");
    test_assert(ret == 0);

    test_assertint(l->start.x, 10);
    test_assertint(l->start.y, 20);
    test_assertint(l->stop.x, 30);
    test_assertint(l->stop.y, 40);

    corto_delete(l);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeObservable(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeObservable) */
    test_ObservableType *o = corto_create(test_ObservableType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_ObservableType_o));

    corto_int16 ret = json_toCorto(o, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeOptional(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeOptional) */
    test_OptionalType *o = corto_create(test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_int16 ret = json_toCorto(o, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeOptionalNotset(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeOptionalNotset) */
    test_OptionalType *o = corto_create(test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_int16 ret = json_toCorto(o, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z == NULL);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeTarget(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeTarget) */
    test_TargetType *o = corto_create(test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_int16 ret = json_toCorto(o, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z->target == 0);
    test_assert(o->z->actual == 40);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserCompositeTargetNotOwned(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserCompositeTargetNotOwned) */
    test_TargetType *o = corto_create(test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_mount m = corto_create(corto_mount_o);
    test_assert(m != NULL);
    corto_setOwner(m);

    corto_int16 ret = json_toCorto(o, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
    test_assert(ret == 0);

    corto_setOwner(NULL);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z->target == 30);
    test_assert(o->z->actual == 0);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserInheritance(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserInheritance) */
    test_Point3D *p = corto_create(test_Point3D_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point3D_o));

    corto_int16 ret = json_toCorto(p, "{\"super\":{\"x\":10, \"y\":20}, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(p->_parent.x, 10);
    test_assertint(p->_parent.y, 20);
    test_assertint(p->z, 30);

    corto_delete(p);

/* $end */
}

corto_void _test_Deserializer_tc_deserList(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserList) */
    corto_int32List *l = corto_create(corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_int16 ret = json_toCorto(l, "[10, 20, 30]");
    test_assert(ret == 0);
    test_assertint(corto_llSize(*l), 3);
    test_assertint((corto_int32)(corto_word)corto_llGet(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)corto_llGet(*l, 1), 20);
    test_assertint((corto_int32)(corto_word)corto_llGet(*l, 2), 30);

    corto_delete(l);

/* $end */
}

corto_void _test_Deserializer_tc_deserObject(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserObject) */
    corto_object o = NULL;
    corto_int16 r = json_toObject(&o, "{\"id\": \"a\", \"type\": \"int8\", \"value\": 8}");
    test_assertint(r, 0);
    test_assert(o != NULL);
    test_assert(corto_fullpath(NULL, o) != NULL);
    test_assertstr(corto_fullpath(NULL, o), "/a");
    test_assert(corto_typeof(o) == (corto_type)corto_int8_o);
    test_assertint(*(corto_int8*)o, 8);
    corto_delete(o);
/* $end */
}

corto_void _test_Deserializer_tc_deserObjectErrorParsing(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserObjectErrorParsing) */
    corto_object o = NULL;
    corto_int16 r = json_toObject(&o, "{\"id\", \"/a/b/c\", \"value\": 9, \"type\", \"int16\"}");
    test_assert(r != 0);
    char* lasterr = corto_lasterr();
    test_assertstr(lasterr, "json: error parsing '{\"id\", \"/a/b/c\", \"value\": 9, \"type\", \"int16\"}'");
    corto_seterr(NULL);
/* $end */
}

corto_void _test_Deserializer_tc_deserObjectNonFullyScopedName(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserObjectNonFullyScopedName) */
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
/* $end */
}

corto_void _test_Deserializer_tc_deserObjectScoped(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserObjectScoped) */
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
/* $end */
}

corto_void _test_Deserializer_tc_deserReference(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReference) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":\"/corto/core/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserReferenceAnonymous(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReferenceAnonymous) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":{\"type\":\"int32\",\"value\":10}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(corto_int32_o));
    test_assertint(*(corto_int32*)o->r, 10);
    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserReferenceAnonymousCollection(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReferenceAnonymousCollection) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":{\"type\":\"/test/myList\",\"value\":[10,20,30]}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_myList_o));
    test_assertint(corto_llSize(*(corto_ll*)o->r), 3);

    test_assertint((corto_int32)(corto_word)corto_llGet(*(corto_ll*)o->r, 0), 10);
    test_assertint((corto_int32)(corto_word)corto_llGet(*(corto_ll*)o->r, 1), 20);
    test_assertint((corto_int32)(corto_word)corto_llGet(*(corto_ll*)o->r, 2), 30);

    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserReferenceAnonymousComplex(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReferenceAnonymousComplex) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_Point_o));
    test_assertint(test_Point(o->r)->x, 10);
    test_assertint(test_Point(o->r)->y, 20);
    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserReferenceFromLang(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReferenceFromLang) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":\"/corto/core/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

/* $end */
}

corto_void _test_Deserializer_tc_deserReferenceNull(
    test_Deserializer this)
{
/* $begin(test/Deserializer/tc_deserReferenceNull) */
    test_ReferenceType *o = corto_create(test_ReferenceType_o);
    corto_int16 ret = json_toCorto(o, "{\"r\":\"/corto/core/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);

    ret = json_toCorto(o, "{\"r\":null}");
    test_assert(o->r == NULL);
    corto_delete(o);

/* $end */
}
