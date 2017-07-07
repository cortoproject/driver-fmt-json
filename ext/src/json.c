/* $CORTO_GENERATED
 *
 * json.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include <corto/ext/json/json.h>

static int json_loadFile(corto_string file, int argc, char* argv[], void* ctx) {
    char *json = corto_fileLoad(file);
    if (!json) {
        corto_seterr("json: cannot find file '%s'", file);
        goto error;
    }
    int result = json_toObject(NULL, json);
    corto_dealloc(json);
    return result;
error:
    return -1;
}

int jsonMain(int argc, char *argv[]) {

    corto_loaderRegister("json", json_loadFile, NULL);

    return 0;
}
