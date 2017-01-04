/* $CORTO_GENERATED
 *
 * Serializer.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <test.h>

/* $header() */
static corto_string json_fromCorto(corto_object object) {
    corto_value v = corto_value_object(object, NULL);
    return json_fromValue(&v);
}
/* $end */

corto_void _test_Serializer_tc_serArray(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serArray) */

    corto_string s = json_fromCorto(test_array_o);
    test_assertstr(s, "[10,20,30]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serArrayComplex(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serArrayComplex) */

    corto_string s = json_fromCorto(test_arrayComplex_o);
    test_assertstr(s, "[{\"x\":10,\"y\":20},{\"x\":30,\"y\":40},{\"x\":50,\"y\":60}]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serBinary(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serBinary) */

    corto_string s = json_fromCorto(test_binary_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serBitmask(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serBitmask) */

    corto_string s = json_fromCorto(test_bitmask_o);
    test_assertstr(s, "\"Beef|Cheese|Lettuce|Tomato\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serBoolFalse(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serBoolFalse) */

    corto_string s = json_fromCorto(test_boolFalse_o);
    test_assertstr(s, "false");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serBoolTrue(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serBoolTrue) */

    corto_string s = json_fromCorto(test_boolTrue_o);
    test_assertstr(s, "true");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serChar(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serChar) */

    corto_string s = json_fromCorto(test_char_o);
    test_assertstr(s, "\"a\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCharEscape(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCharEscape) */

    corto_string s = json_fromCorto(test_charEscape_o);
    test_assertstr(s, "\"\\\\\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCharNull(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCharNull) */

    corto_string s = json_fromCorto(test_charNull_o);
    test_assertstr(s, "\"\"");
    corto_dealloc(s);


/* $end */
}

corto_void _test_Serializer_tc_serComposite(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serComposite) */

    corto_string s = json_fromCorto(test_p_o);
    test_assertstr(s, "{\"x\":10,\"y\":20}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeNested(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeNested) */

    corto_string s = json_fromCorto(test_l_o);
    test_assertstr(s, "{\"start\":{\"x\":10,\"y\":20},\"stop\":{\"x\":30,\"y\":40}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeNestedRef(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeNestedRef) */

    corto_string s = json_fromCorto(test_l_ref_o);
    test_assertstr(
      s,
      "{\"start\":{\"type\":\"/test/PointRef\",\"value\":{\"x\":10,\"y\":20}},\"stop\":{\"type\":\"/test/PointRef\",\"value\":{\"x\":30,\"y\":40}}}"
    );
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeObservable(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeObservable) */
    test_ObservableTypeCreate_auto(o, 10, 20, 30);
    test_assert(o != NULL);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"x\":10,\"y\":20,\"z\":30}");
    corto_dealloc(s);
    corto_delete(o);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeOptionalNotSet(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeOptionalNotSet) */
    test_OptionalTypeCreate_auto(o, 10, 20, NotSet);
    test_assert(o != NULL);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"x\":10,\"y\":20}");
    corto_dealloc(s);
    corto_delete(o);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeOptionalSet(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeOptionalSet) */
    test_OptionalTypeCreate_auto(o, 10, 20, Set(30));
    test_assert(o != NULL);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"x\":10,\"y\":20,\"z\":30}");
    corto_dealloc(s);
    corto_delete(o);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeRef(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeRef) */

    corto_string s = json_fromCorto(test_p_ref_o);
    test_assertstr(s, "{\"x\":10,\"y\":20}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeTarget(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeTarget) */
    test_TargetTypeCreate_auto(o, 10, 20, 30);
    test_assert(o != NULL);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"x\":10,\"y\":20,\"z\":{\"actual\":30,\"target\":0,\"objective\":0}}");
    corto_dealloc(s);
    corto_delete(o);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeUnion(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeUnion) */
    test_UnionTypeCreate_flt_auto(o, 1, 10.5);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"_d\":1,\"flt\":10.500000}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeUnionComplex(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeUnionComplex) */
    test_Point p = {10, 20};
    test_UnionTypeCreate_pt_auto(o, 4, &p);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"_d\":4,\"pt\":{\"x\":10,\"y\":20}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serCompositeUnionDefault(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serCompositeUnionDefault) */
    test_UnionTypeCreate_other_auto(o, 5, 10);
    corto_string s = json_fromCorto(o);
    test_assertstr(s, "{\"_d\":5,\"other\":10}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serEnum(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serEnum) */

    corto_string s = json_fromCorto(test_enum_o);
    test_assertstr(s, "\"Yellow\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInheritance(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInheritance) */

    corto_string s = json_fromCorto(test_p3d_o);
    test_assertstr(s, "{\"super\":{\"x\":10,\"y\":20},\"z\":30}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInheritanceRef(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInheritanceRef) */

    corto_string s = json_fromCorto(test_p3d_ref_o);
    test_assertstr(s, "{\"super\":{\"x\":10,\"y\":20},\"z\":30}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInt16(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt16) */

    corto_string s = json_fromCorto(test_int16_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInt16Minus(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt16Minus) */

    corto_string s = json_fromCorto(test_int16Minus_o);
    test_assertstr(s, "-10");
    corto_dealloc(s);
/* $end */
}

corto_void _test_Serializer_tc_serInt32(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt32) */

    corto_string s = json_fromCorto(test_int32_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInt32Minus(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt32Minus) */

    corto_string s = json_fromCorto(test_int32Minus_o);
    test_assertstr(s, "-10");
    corto_dealloc(s);
/* $end */
}

corto_void _test_Serializer_tc_serInt64(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt64) */

    corto_string s = json_fromCorto(test_int64_o);
    test_assertstr(s, "10");
    corto_dealloc(s);
/* $end */
}

corto_void _test_Serializer_tc_serInt64Minus(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt64Minus) */

    corto_string s = json_fromCorto(test_int64Minus_o);
    test_assertstr(s, "-10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInt8(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt8) */

    corto_string s = json_fromCorto(test_int8_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serInt8Minus(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serInt8Minus) */

    corto_string s = json_fromCorto(test_int8Minus_o);
    test_assertstr(s, "-10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serList(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serList) */

    corto_string s = json_fromCorto(test_list_o);
    test_assertstr(s, "[10,20,30]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serListComplex(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serListComplex) */

    corto_string s = json_fromCorto(test_listComplex_o);
    test_assertstr(s, "[{\"x\":10,\"y\":20},{\"x\":30,\"y\":40},{\"x\":50,\"y\":60}]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReference(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReference) */

    corto_string s = json_fromCorto(test_reference_o);
    test_assertstr(s, "{\"r\":\"/corto/core/package\"}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymous(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymous) */

    corto_string s = json_fromCorto(test_referenceAnonymous_o);
    test_assertstr(s, "{\"r\":{\"type\":\"int32\",\"value\":10}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymousCollection(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymousCollection) */

    corto_string s = json_fromCorto(test_referenceAnonymousCollection_o);
    test_assertstr(s, "{\"r\":{\"type\":\"/test/myList\",\"value\":[10,20]}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymousCollectionAnonymousType(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymousCollectionAnonymousType) */

    corto_string s = json_fromCorto(test_referenceAnonymousCollectionAnonymousType_o);
    test_assertstr(s, "{\"r\":{\"type\":\"array{int32,2}\",\"value\":[10,20]}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymousComplex(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymousComplex) */

    corto_string s = json_fromCorto(test_referenceAnonymousComplex_o);
    test_assertstr(s, "{\"r\":{\"type\":\"/test/Point\",\"value\":{\"x\":10,\"y\":20}}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymousComplexWithString(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymousComplexWithString) */

    corto_string s = json_fromCorto(test_referenceAnonymousComplexString_o);
    test_assertstr(s, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"Hello\",\"s2\":\"World\"}}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceAnonymousComplexWithStringEsc(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceAnonymousComplexWithStringEsc) */

    corto_string s = json_fromCorto(test_referenceAnonymousComplexStringEsc_o);
    test_assertstr(s, "{\"r\":{\"type\":\"/test/StringType\",\"value\":{\"s1\":\"\\\"Hello\\\"\",\"s2\":\"\\\"World\\\"\"}}}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceFromLang(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceFromLang) */

    corto_string s = json_fromCorto(test_referenceFromLang_o);
    test_assertstr(s, "{\"r\":\"type\"}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serReferenceNull(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serReferenceNull) */

    corto_string s = json_fromCorto(test_referenceNull_o);
    test_assertstr(s, "{\"r\":null}");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serSequence(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serSequence) */

    corto_string s = json_fromCorto(test_sequence_o);
    test_assertstr(s, "[10,20,30]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serSequenceComplex(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serSequenceComplex) */

    corto_string s = json_fromCorto(test_sequenceComplex_o);
    test_assertstr(s, "[{\"x\":10,\"y\":20},{\"x\":30,\"y\":40},{\"x\":50,\"y\":60}]");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serString(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serString) */

    corto_string s = json_fromCorto(test_string_o);
    test_assertstr(s, "\"Hello World\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serStringEmpty(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serStringEmpty) */

    corto_string s = json_fromCorto(test_stringEmpty_o);
    test_assertstr(s, "\"\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serStringEscape(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serStringEscape) */

    corto_string s = json_fromCorto(test_stringEscape_o);
    test_assertstr(s, "\"\\\\\"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serStringNull(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serStringNull) */

    corto_string s = json_fromCorto(test_stringNull_o);
    test_assertstr(s, "null");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serStringWhitespace(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serStringWhitespace) */

    corto_string s = json_fromCorto(test_stringWhitespace_o);
    test_assertstr(s, "\" \"");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serUint16(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serUint16) */

    corto_string s = json_fromCorto(test_uint16_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serUint32(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serUint32) */

    corto_string s = json_fromCorto(test_uint32_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serUint64(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serUint64) */

    corto_string s = json_fromCorto(test_uint64_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serUint8(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serUint8) */

    corto_string s = json_fromCorto(test_uint8_o);
    test_assertstr(s, "10");
    corto_dealloc(s);

/* $end */
}

corto_void _test_Serializer_tc_serVoid(
    test_Serializer this)
{
/* $begin(test/Serializer/tc_serVoid) */

    corto_string s = json_fromCorto(test_void_o);
    test_assertstr(s, "{}");
    corto_dealloc(s);

/* $end */
}
