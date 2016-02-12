/* json/_interface.h
 *
 * This file contains generated code. Do not modify!
 */

#if BUILDING_JSON && defined _MSC_VER
#define JSON_DLL_EXPORTED __declspec(dllexport)
#elif BUILDING_JSON
#define JSON_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define JSON_EXPORT __declspec(dllimport)
#else
#define JSON_EXPORT
#endif

