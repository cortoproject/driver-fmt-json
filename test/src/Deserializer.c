/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
void test_Deserializer_tc_deserBinary(
    test_Deserializer this)
{
    corto_word *o = corto_create(NULL, NULL, corto_word_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserBitmask(
    test_Deserializer this)
{
    corto_word *o = corto_create(NULL, NULL, test_Sandwich_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"Beef|Lettuce|Tomato\"");
    test_assert(ret == 0);
    test_assertint(*o, Test_Beef | Test_Lettuce | Test_Tomato);
    corto_delete(o);

}

void test_Deserializer_tc_deserBoolFalse(
    test_Deserializer this)
{
    corto_bool *o = corto_create(NULL, NULL, corto_bool_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "false");
    test_assert(ret == 0);
    test_assertint(*o, FALSE);
    corto_delete(o);

}

void test_Deserializer_tc_deserBoolTrue(
    test_Deserializer this)
{
    corto_bool *o = corto_create(NULL, NULL, corto_bool_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "true");
    test_assert(ret == 0);
    test_assertint(*o, TRUE);
    corto_delete(o);

}

void test_Deserializer_tc_deserChar(
    test_Deserializer this)
{
    corto_char *o = corto_create(NULL, NULL, corto_char_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"a\"");
    test_assert(ret == 0);
    test_assertint(*o, 'a');
    corto_delete(o);

}

void test_Deserializer_tc_deserCharEscape(
    test_Deserializer this)
{
    corto_char *o = corto_create(NULL, NULL, corto_char_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"\\n\"");
    test_assert(ret == 0);
    test_assertint(*o, '\n');
    corto_delete(o);

}

void test_Deserializer_tc_deserComposite(
    test_Deserializer this)
{
    test_Point *p = corto_create(NULL, NULL, test_Point_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    corto_delete(p);
}

void test_Deserializer_tc_deserCompositeList(
    test_Deserializer this)
{
    test_PointList *l = corto_create(NULL, NULL, test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}]");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 2);

    test_Point *p = ut_ll_get(*l, 0);
    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    p = ut_ll_get(*l, 1);
    test_assertint(p->x, 30);
    test_assertint(p->y, 40);

    corto_delete(l);

}

void test_Deserializer_tc_deserCompositeNested(
    test_Deserializer this)
{
    test_Line *l = corto_create(NULL, NULL, test_Line_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_Line_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"start\":{\"x\":10, \"y\":20}, \"stop\":{\"x\":30, \"y\":40}}");
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
    test_LineRef l = corto_create(NULL, NULL, test_LineRef_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_LineRef_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(
      NULL,
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
    test_ObservableType *o = corto_create(NULL, NULL, test_ObservableType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_ObservableType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeOptional(
    test_Deserializer this)
{
    test_OptionalType *o = corto_create(NULL, NULL, test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assertint(*o->z, 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeOptionalNotset(
    test_Deserializer this)
{
    test_OptionalType *o = corto_create(NULL, NULL, test_OptionalType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_OptionalType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z == NULL);

    corto_delete(o);

}

void test_Deserializer_tc_deserCompositeRef(
    test_Deserializer this)
{
    test_Point *p = corto_create(NULL, NULL, test_PointRef_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_PointRef_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20}");
    test_assert(ret == 0);

    test_assertint(p->x, 10);
    test_assertint(p->y, 20);

    corto_delete(p);

}

void test_Deserializer_tc_deserCompositeTarget(
    test_Deserializer this)
{
    test_TargetType *o = corto_create(NULL, NULL, test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
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
    test_TargetType *o = corto_create(NULL, NULL, test_TargetType_o);
    test_assert(o != NULL);
    test_assert(corto_typeof(o) == corto_type(test_TargetType_o));

    corto_mount m = corto_create(NULL, NULL, corto_mount_o);
    test_assert(m != NULL);
    corto_set_source(m);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"x\":10, \"y\":20, \"z\":{\"target\":30, \"actual\":40}}");
    test_assert(ret == 0);

    corto_set_source(NULL);

    test_assertint(o->x, 10);
    test_assertint(o->y, 20);
    test_assert(o->z->target == 30);
    test_assert(o->z->actual == 0);

    corto_delete(o);

}

void test_Deserializer_tc_deserEnum(
    test_Deserializer this)
{
    test_Color *o = corto_create(NULL, NULL, test_Color_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"Yellow\"");
    test_assert(ret == 0);
    test_assertint(*o, Test_Yellow);
    corto_delete(o);

}

void test_Deserializer_tc_deserInheritance(
    test_Deserializer this)
{
    test_Point3D *p = corto_create(NULL, NULL, test_Point3D_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point3D_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"super\":{\"x\":10, \"y\":20}, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(p->super.x, 10);
    test_assertint(p->super.y, 20);
    test_assertint(p->z, 30);

    corto_delete(p);

}

void test_Deserializer_tc_deserInheritanceRef(
    test_Deserializer this)
{
    test_Point3D *p = corto_create(NULL, NULL, test_Point3D_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point3D_o));

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"super\":{\"x\":10, \"y\":20}, \"z\":30}");
    test_assert(ret == 0);

    test_assertint(p->super.x, 10);
    test_assertint(p->super.y, 20);
    test_assertint(p->z, 30);

    corto_delete(p);

}

void test_Deserializer_tc_deserInt16(
    test_Deserializer this)
{
    corto_int16 *o = corto_create(NULL, NULL, corto_int16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt16Minus(
    test_Deserializer this)
{
    corto_int16 *o = corto_create(NULL, NULL, corto_int16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt32(
    test_Deserializer this)
{
    corto_int32 *o = corto_create(NULL, NULL, corto_int32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt32Minus(
    test_Deserializer this)
{
    corto_int32 *o = corto_create(NULL, NULL, corto_int32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt64(
    test_Deserializer this)
{
    corto_int64 *o = corto_create(NULL, NULL, corto_int64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt64Minus(
    test_Deserializer this)
{
    corto_int64 *o = corto_create(NULL, NULL, corto_int64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt8(
    test_Deserializer this)
{
    corto_int8 *o = corto_create(NULL, NULL, corto_int8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserInt8Minus(
    test_Deserializer this)
{
    corto_int8 *o = corto_create(NULL, NULL, corto_int8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "-10");
    test_assert(ret == 0);
    test_assertint(*o, -10);
    corto_delete(o);

}

void test_Deserializer_tc_deserList(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "[10, 20, 30]");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 3);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 30);

    corto_delete(l);
}

void test_Deserializer_tc_deserListTwice(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "[10, 20, 30]");
    test_assert(ret == 0);
    test_assert(corto_value_ptrof(&v) == l);
    test_assertint(ut_ll_count(*l), 3);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 30);

    ret = json_toValue(NULL, &v, "[20, 30, 40]");
    test_assert(ret == 0);
    test_assert(corto_value_ptrof(&v) == l);
    test_assertint(ut_ll_count(*l), 3);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 30);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 40);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_append(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_int32List__append(*l, 10);
    corto_int32List__append(*l, 20);
    corto_int32List__append(*l, 30);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$append\":40}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 4);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 30);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 3), 40);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_append_composite(
    test_Deserializer this)
{
    test_PointList *l = corto_create(NULL, NULL, test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    test_Point p = {10, 20};
    test_PointList__append(*l, &p);
    p = (test_Point){30, 40};
    test_PointList__append(*l, &p);
    p = (test_Point){50, 60};
    test_PointList__append(*l, &p);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$append\":{\"x\":70,\"y\":80}}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 4);
    test_Point *e = ut_ll_get(*l, 0);
    test_assertint(e->x, 10);
    test_assertint(e->y, 20);
    e = ut_ll_get(*l, 1);
    test_assertint(e->x, 30);
    test_assertint(e->y, 40);
    e = ut_ll_get(*l, 2);
    test_assertint(e->x, 50);
    test_assertint(e->y, 60);
    e = ut_ll_get(*l, 3);
    test_assertint(e->x, 70);
    test_assertint(e->y, 80);
}

void test_Deserializer_tc_deserList_append_ref(
    test_Deserializer this)
{
    corto_classList *l = corto_create(NULL, NULL, corto_classList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_classList_o));

    corto_classList__append(*l, corto_type_o);
    corto_classList__append(*l, corto_struct_o);
    corto_classList__append(*l, corto_class_o);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$append\":\"collection\"}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 4);
    test_assert(corto_classList__get(*l, 0) == corto_type_o);
    test_assert(corto_classList__get(*l, 1) == corto_struct_o);
    test_assert(corto_classList__get(*l, 2) == corto_class_o);
    test_assert(corto_classList__get(*l, 3) == corto_collection_o);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_dec(
    test_Deserializer this)
{
  test_PointList *l = corto_create(NULL, NULL, test_PointList_o);
  test_assert(l != NULL);
  test_assert(corto_typeof(l) == corto_type(test_PointList_o));

  test_Point p = {10, 20};
  test_PointList__append(*l, &p);
  p = (test_Point){30, 40};
  test_PointList__append(*l, &p);
  p = (test_Point){50, 60};
  test_PointList__append(*l, &p);

  corto_value v = corto_value_object(l, NULL);
  corto_int16 ret = json_toValue(NULL, &v, "{\"$dec\":{\"[1].x\": 2}}");
  test_assert(ret == 0);

  test_assertint(ut_ll_count(*l), 3);
  test_Point *e = ut_ll_get(*l, 0);
  test_assertint(e->x, 10);
  test_assertint(e->y, 20);
  e = ut_ll_get(*l, 1);
  test_assertint(e->x, 28);
  test_assertint(e->y, 40);
  e = ut_ll_get(*l, 2);
  test_assertint(e->x, 50);
  test_assertint(e->y, 60);

  corto_delete(l);
}

void test_Deserializer_tc_deserList_inc(
    test_Deserializer this)
{
    test_PointList *l = corto_create(NULL, NULL, test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    test_Point p = {10, 20};
    test_PointList__append(*l, &p);
    p = (test_Point){30, 40};
    test_PointList__append(*l, &p);
    p = (test_Point){50, 60};
    test_PointList__append(*l, &p);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$inc\":{\"[1].x\": 2}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(*l), 3);
    test_Point *e = ut_ll_get(*l, 0);
    test_assertint(e->x, 10);
    test_assertint(e->y, 20);
    e = ut_ll_get(*l, 1);
    test_assertint(e->x, 32);
    test_assertint(e->y, 40);
    e = ut_ll_get(*l, 2);
    test_assertint(e->x, 50);
    test_assertint(e->y, 60);

    corto_delete(l);
}

void test_Deserializer_tc_deserCompositeListAfterComposite_dec(
    test_Deserializer this)
{
    test_CompositeListMember *l = corto_create(NULL, NULL, test_CompositeListMember_o);
    test_assert(l != NULL);

    test_Point p = {10, 20};
    test_PointList__append(l->m, &p);
    p = (test_Point){30, 40};
    test_PointList__append(l->m, &p);
    p = (test_Point){50, 60};
    test_PointList__append(l->m, &p);
    test_assertint(ut_ll_count(l->m), 3);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$dec\":{\"m[1].x\": 2}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(l->m), 3);
    test_Point *e = ut_ll_get(l->m, 0);
    test_assertint(e->x, 10);
    test_assertint(e->y, 20);
    e = ut_ll_get(l->m, 1);
    test_assertint(e->x, 28);
    test_assertint(e->y, 40);
    e = ut_ll_get(l->m, 2);
    test_assertint(e->x, 50);
    test_assertint(e->y, 60);
}

void test_Deserializer_tc_deserCompositeListAfterComposite_inc(
    test_Deserializer this)
{
    test_CompositeListMember *l = corto_create(NULL, NULL, test_CompositeListMember_o);
    test_assert(l != NULL);

    test_Point p = {10, 20};
    test_PointList__append(l->m, &p);
    p = (test_Point){30, 40};
    test_PointList__append(l->m, &p);
    p = (test_Point){50, 60};
    test_PointList__append(l->m, &p);
    test_assertint(ut_ll_count(l->m), 3);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$inc\":{\"m[1].x\": 2}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(l->m), 3);
    test_Point *e = ut_ll_get(l->m, 0);
    test_assertint(e->x, 10);
    test_assertint(e->y, 20);
    e = ut_ll_get(l->m, 1);
    test_assertint(e->x, 32);
    test_assertint(e->y, 40);
    e = ut_ll_get(l->m, 2);
    test_assertint(e->x, 50);
    test_assertint(e->y, 60);
}

void test_Deserializer_tc_deserIntListAfterComposite_dec(
    test_Deserializer this)
{
    test_IntListMember *l = corto_create(NULL, NULL, test_IntListMember_o);
    test_assert(l != NULL);

    corto_int32List__append(l->m, 10);
    corto_int32List__append(l->m, 20);
    corto_int32List__append(l->m, 30);
    test_assertint(ut_ll_count(l->m), 3);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$dec\":{\"m[1]\": 2}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(l->m), 3);
    test_assertint(corto_int32List__get(l->m, 0), 10);
    test_assertint(corto_int32List__get(l->m, 1), 18);
    test_assertint(corto_int32List__get(l->m, 2), 30);
}

void test_Deserializer_tc_deserIntListAfterComposite_inc(
    test_Deserializer this)
{
    test_IntListMember *l = corto_create(NULL, NULL, test_IntListMember_o);
    test_assert(l != NULL);

    corto_int32List__append(l->m, 10);
    corto_int32List__append(l->m, 20);
    corto_int32List__append(l->m, 30);
    test_assertint(ut_ll_count(l->m), 3);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$inc\":{\"m[1]\": 2}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(l->m), 3);
    test_assertint(corto_int32List__get(l->m, 0), 10);
    test_assertint(corto_int32List__get(l->m, 1), 22);
    test_assertint(corto_int32List__get(l->m, 2), 30);
}

void test_Deserializer_tc_deserList_remove(
    test_Deserializer this)
{
  corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
  test_assert(l != NULL);
  test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

  corto_int32List__append(*l, 10);
  corto_int32List__append(*l, 20);
  corto_int32List__append(*l, 30);

  corto_value v = corto_value_object(l, NULL);
  corto_int16 ret = json_toValue(NULL, &v, "{\"$remove\": [1]}");
  test_assert(ret == 0);
  test_assertint(ut_ll_count(*l), 2);
  test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);
  test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 30);

  corto_delete(l);
}

void test_Deserializer_tc_deserList_remove_multiple(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_int32List__append(*l, 10);
    corto_int32List__append(*l, 20);
    corto_int32List__append(*l, 30);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$remove\": [1, 2]}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 1);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_set(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_int32List__append(*l, 10);
    corto_int32List__append(*l, 20);
    corto_int32List__append(*l, 30);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$set\":{\"1\": 25}}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 3);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 25);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 30);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_set_multiple(
    test_Deserializer this)
{
    corto_int32List *l = corto_create(NULL, NULL, corto_int32List_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_int32List_o));

    corto_int32List__append(*l, 10);
    corto_int32List__append(*l, 20);
    corto_int32List__append(*l, 30);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$set\":{\"1\": 25, \"0\": 15}}");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 3);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 0), 15);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 1), 25);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*l, 2), 30);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_set_composite(
    test_Deserializer this)
{
    test_PointList *l = corto_create(NULL, NULL, test_PointList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_PointList_o));

    test_Point p = {10, 20};
    test_PointList__append(*l, &p);
    p = (test_Point){30, 40};
    test_PointList__append(*l, &p);
    p = (test_Point){50, 60};
    test_PointList__append(*l, &p);

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$set\":{\"1\": {\"x\":35,\"y\":45}}}");
    test_assert(ret == 0);

    test_assertint(ut_ll_count(*l), 3);
    test_Point *e = ut_ll_get(*l, 0);
    test_assertint(e->x, 10);
    test_assertint(e->y, 20);
    e = ut_ll_get(*l, 1);
    test_assertint(e->x, 35);
    test_assertint(e->y, 45);
    e = ut_ll_get(*l, 2);
    test_assertint(e->x, 50);
    test_assertint(e->y, 60);

    corto_delete(l);
}

void test_Deserializer_tc_deserList_set_ref(
    test_Deserializer this)
{
  corto_classList *l = corto_create(NULL, NULL, corto_classList_o);
  test_assert(l != NULL);
  test_assert(corto_typeof(l) == corto_type(corto_classList_o));

  corto_classList__append(*l, corto_type_o);
  corto_classList__append(*l, corto_struct_o);
  corto_classList__append(*l, corto_class_o);

  corto_value v = corto_value_object(l, NULL);
  corto_int16 ret = json_toValue(NULL, &v, "{\"$set\":{\"1\":\"collection\"}}");
  test_assert(ret == 0);
  test_assertint(ut_ll_count(*l), 3);
  test_assert(corto_classList__get(*l, 0) == corto_type_o);
  test_assert(corto_classList__get(*l, 1) == corto_collection_o);
  test_assert(corto_classList__get(*l, 2) == corto_class_o);

  corto_delete(l);
}

void test_Deserializer_tc_deserNaN(
    test_Deserializer this)
{
    corto_float64 *o = corto_create(NULL, NULL, corto_float64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "null");
    test_assert(ret == 0);
    test_assert(CORTO_ISNAN(*o));
    corto_delete(o);

}

void test_Deserializer_tc_deserObject(
    test_Deserializer this)
{
    corto_object o = NULL;

    corto_int16 r = json_toObject(NULL, &o, "{\"id\": \"a\", \"type\": \"int8\", \"value\": 8}");
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
    corto_int16 r = json_toObject(NULL, &o, "{\"id\", \"/a/b/c\", \"value\": 9, \"type\", \"int16\"}");
    test_assert(r != 0);
    test_assert(ut_catch());
}

void test_Deserializer_tc_deserObjectNonFullyScopedName(
    test_Deserializer this)
{
    corto_object a = corto_create(root_o, "a", corto_int8_o);
    corto_object b = corto_create(a, "b", corto_int8_o);
    test_assert(a != NULL);
    test_assert(b != NULL);
    corto_object c = NULL;
    corto_int16 r = json_toObject(NULL, &c, "{\"id\": \"a/b/c\", \"value\": 9, \"type\": \"int16\"}");
    test_assert(r == 0);
    test_assert(corto_fullpath(NULL, c) != NULL);
    test_assertstr(corto_fullpath(NULL, c), "/a/b/c");
    test_assert(corto_typeof(c) == (corto_type)corto_int16_o);
    test_assert(*(corto_int16*)c == 9);
}

void test_Deserializer_tc_deserObjectScoped(
    test_Deserializer this)
{
    corto_object a = corto_create(root_o, "a", corto_int8_o);
    corto_object b = corto_create(a, "b", corto_int8_o);
    test_assert(a != NULL);
    test_assert(b != NULL);
    corto_object c = NULL;
    corto_int16 r = json_toObject(NULL, &c, "{\"id\": \"/a/b/c\", \"value\": 9, \"type\": \"int16\"}");
    test_assert(r == 0);
    test_assert(corto_fullpath(NULL, c) != NULL);
    test_assertstr(corto_fullpath(NULL, c), "/a/b/c");
    test_assert(corto_typeof(c) == (corto_type)corto_int16_o);
    test_assert(*(corto_int16*)c == 9);
}

void test_Deserializer_tc_deserReference(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":\"/corto/lang/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymous(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"int32\",\"value\":10}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(corto_int32_o));
    test_assertint(*(corto_int32*)o->r, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousCollection(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"/test/myList\",\"value\":[10,20,30]}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    test_assert(corto_typeof(o->r) == corto_type(test_myList_o));
    test_assertint(ut_ll_count(*(ut_ll*)o->r), 3);

    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 1), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 2), 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousCollectionAnonymousType(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"list{int32}\",\"value\":[10,20,30]}}");
    test_assert(ret == 0);
    test_assert(o->r != NULL);

    corto_type t = corto_typeof(o->r);
    test_assert(t->kind == CORTO_COLLECTION);
    test_assert(corto_collection(t)->kind == CORTO_LIST);
    test_assert(corto_collection(t)->element_type == corto_type(corto_int32_o));
    test_assertint(ut_ll_count(*(ut_ll*)o->r), 3);

    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 0), 10);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 1), 20);
    test_assertint((corto_int32)(corto_word)ut_ll_get(*(ut_ll*)o->r, 2), 30);

    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceAnonymousComplex(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}}");
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
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"Hello\",\"s2\":\"World\"}}}");
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
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"\\\"Hello\\\"\",\"s2\":\"\\\"World\\\"\"}}}");
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
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":\"corto/lang/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);
    corto_delete(o);

}

void test_Deserializer_tc_deserReferenceNull(
    test_Deserializer this)
{
    test_ReferenceType *o = corto_create(NULL, NULL, test_ReferenceType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"r\":\"corto/lang/package\"}");
    test_assert(ret == 0);
    test_assert(o->r == corto_package_o);

    ret = json_toValue(NULL, &v, "{\"r\":null}");
    test_assert(o->r == NULL);
    corto_delete(o);

}

void test_Deserializer_tc_deserString(
    test_Deserializer this)
{
    corto_string *o = corto_create(NULL, NULL, corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"Hello World\"");
    test_assert(ret == 0);
    test_assertstr(*o, "Hello World");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringEmpty(
    test_Deserializer this)
{
    corto_string *o = corto_create(NULL, NULL, corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"\"");
    test_assert(ret == 0);
    test_assertstr(*o, "");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringEscape(
    test_Deserializer this)
{
    corto_string *o = corto_create(NULL, NULL, corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"Hello World\\n\"");
    test_assert(ret == 0);
    test_assertstr(*o, "Hello World\n");
    corto_delete(o);

}

void test_Deserializer_tc_deserStringNull(
    test_Deserializer this)
{
    corto_string *o = corto_create(NULL, NULL, corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "null");
    test_assert(ret == 0);
    test_assert(*o == NULL);
    corto_delete(o);

}

void test_Deserializer_tc_deserStringWhitespace(
    test_Deserializer this)
{
    corto_string *o = corto_create(NULL, NULL, corto_string_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "\"  Hello World  \"");
    test_assert(ret == 0);
    test_assertstr(*o, "  Hello World  ");
    corto_delete(o);

}

void test_Deserializer_tc_deserUint16(
    test_Deserializer this)
{
    corto_uint16 *o = corto_create(NULL, NULL, corto_uint16_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint32(
    test_Deserializer this)
{
    corto_uint32 *o = corto_create(NULL, NULL, corto_uint32_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint64(
    test_Deserializer this)
{
    corto_uint64 *o = corto_create(NULL, NULL, corto_uint64_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUint8(
    test_Deserializer this)
{
    corto_uint8 *o = corto_create(NULL, NULL, corto_uint8_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "10");
    test_assert(ret == 0);
    test_assertint(*o, 10);
    corto_delete(o);

}

void test_Deserializer_tc_deserUnion(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(NULL, NULL, test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"_d\":1,\"flt\":10.5}");
    test_assert(ret == 0);
    test_assertint(o->d, 1);
    test_assertflt(o->is.flt, 10.5);

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionChangeDiscriminator(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(NULL, NULL, test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"_d\":1,\"flt\":10.5}");
    test_assert(ret == 0);
    test_assertint(o->d, 1);
    test_assertflt(o->is.flt, 10.5);

    ret = json_toValue(NULL, &v, "{\"_d\":2,\"str\":\"foo\"}");
    test_assert(ret == 0);
    test_assertint(o->d, 2);
    test_assertstr(o->is.str, "foo");

    ret = json_toValue(NULL, &v, "{\"_d\":0,\"num\":20}");
    test_assert(ret == 0);
    test_assertint(o->d, 0);
    test_assertint(o->is.num, 20);

    ret = json_toValue(NULL, &v, "{\"_d\":4,\"pt\":{\"x\":10,\"y\":20}}");
    test_assert(ret == 0);
    test_assertint(o->d, 4);
    test_assertint(o->is.pt.x, 10);
    test_assertint(o->is.pt.y, 20);

    ret = json_toValue(NULL, &v, "{\"_d\":3,\"str\":\"bar\"}");
    test_assert(ret == 0);
    test_assertint(o->d, 3);
    test_assertstr(o->is.str, "bar");

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionComplex(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(NULL, NULL, test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"_d\":4,\"pt\":{\"x\":10,\"y\":20}}");
    test_assert(ret == 0);
    test_assertint(o->d, 4);
    test_assertflt(o->is.pt.x, 10);
    test_assertflt(o->is.pt.y, 20);

    corto_delete(o);

}

void test_Deserializer_tc_deserUnionDefault(
    test_Deserializer this)
{
    test_UnionType *o = corto_create(NULL, NULL, test_UnionType_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"_d\":5,\"other\":30}");
    test_assert(ret == 0);
    test_assertint(o->d, 5);
    test_assertflt(o->is.other, 30);

    ret = json_toValue(NULL, &v, "{\"_d\":6,\"other\":40}");
    test_assert(ret == 0);
    test_assertint(o->d, 6);
    test_assertflt(o->is.other, 40);

    corto_delete(o);

}

void test_Deserializer_tc_deserVoid(
    test_Deserializer this)
{
    corto_void *o = corto_create(NULL, NULL, corto_void_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{}");
    test_assert(ret == 0);
    corto_delete(o);

}

void test_Deserializer_tc_serAnyCollection(
    test_Deserializer this)
{
    corto_any *o = corto_create(NULL, NULL, corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"type\":\"list[int32]\",\"value\":[10,20,30]}");
    test_assert(ret == 0);
    test_assertstr(corto_fullpath(NULL, o->type), "list[int32,0]");
    test_assert(o->value != NULL);
    test_assert(o->owner == TRUE);

    ut_ll list = *(ut_ll*)o->value;
    test_assertint(ut_ll_count(list), 3);
    test_assertint((corto_word)ut_ll_get(list, 0), 10);
    test_assertint((corto_word)ut_ll_get(list, 1), 20);
    test_assertint((corto_word)ut_ll_get(list, 2), 30);

    test_assert(corto_delete(o) == 0);

}

void test_Deserializer_tc_serAnyComposite(
    test_Deserializer this)
{
    corto_any *o = corto_create(NULL, NULL, corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}");
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
    corto_any *o = corto_create(NULL, NULL, corto_any_o);

    corto_value v = corto_value_object(o, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"type\":\"int32\",\"value\":10}");
    test_assert(ret == 0);
    test_assert(corto_type(corto_int32_o) == o->type);
    test_assert(o->value != NULL);
    test_assert(o->owner == TRUE);

    test_assertint(*(corto_int32*)o->value, 10);

    test_assert(corto_delete(o) == 0);

}

void test_Deserializer_tc_deserCompositeReferenceList(
    test_Deserializer this)
{
    corto_classList *l = corto_create(NULL, NULL, corto_classList_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(corto_classList_o));

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "[\"class\", \"type\", \"interface\"]");
    test_assert(ret == 0);
    test_assertint(ut_ll_count(*l), 3);
    test_assert(ut_ll_get(*l, 0) == corto_class_o);
    test_assert(ut_ll_get(*l, 1) == corto_type_o);
    test_assert(ut_ll_get(*l, 2) == corto_interface_o);

    corto_delete(l);
}

void test_Deserializer_tc_deserComposite_dec(
    test_Deserializer this)
{
    test_Point *p = corto_create(NULL, NULL, test_Point_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point_o));

    p->x = 10;
    p->y = 20;

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$dec\":{\"x\": 2}}");
    test_assert(ret == 0);

    test_assertint(p->x, 8);
    test_assertint(p->y, 20);

    corto_delete(p);
}

void test_Deserializer_tc_deserComposite_inc(
    test_Deserializer this)
{
    test_Point *p = corto_create(NULL, NULL, test_Point_o);
    test_assert(p != NULL);
    test_assert(corto_typeof(p) == corto_type(test_Point_o));

    p->x = 10;
    p->y = 20;

    corto_value v = corto_value_object(p, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$inc\":{\"x\": 2}}");
    test_assert(ret == 0);

    test_assertint(p->x, 12);
    test_assertint(p->y, 20);

    corto_delete(p);
}

void test_Deserializer_tc_deserCompositeNested_dec(
    test_Deserializer this)
{
    test_Line *l = corto_create(NULL, NULL, test_Line_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_Line_o));

    l->start.x = 10;
    l->start.y = 20;
    l->stop.x = 30;
    l->stop.y = 40;

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$dec\":{\"stop.x\": 2}}");
    test_assert(ret == 0);

    test_assertint(l->start.x, 10);
    test_assertint(l->start.y, 20);
    test_assertint(l->stop.x, 28);
    test_assertint(l->stop.y, 40);

    corto_delete(l);
}

void test_Deserializer_tc_deserCompositeNested_inc(
    test_Deserializer this)
{
    test_Line *l = corto_create(NULL, NULL, test_Line_o);
    test_assert(l != NULL);
    test_assert(corto_typeof(l) == corto_type(test_Line_o));

    l->start.x = 10;
    l->start.y = 20;
    l->stop.x = 30;
    l->stop.y = 40;

    corto_value v = corto_value_object(l, NULL);
    corto_int16 ret = json_toValue(NULL, &v, "{\"$inc\":{\"stop.x\": 2}}");
    test_assert(ret == 0);

    test_assertint(l->start.x, 10);
    test_assertint(l->start.y, 20);
    test_assertint(l->stop.x, 32);
    test_assertint(l->stop.y, 40);

    corto_delete(l);
}
