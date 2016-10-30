/* $CORTO_GENERATED
 *
 * Deserializer.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <test.h>

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
