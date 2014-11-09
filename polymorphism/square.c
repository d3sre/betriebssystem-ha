#include "structureFunctions.h"

static const char *form(void) {
    return "square";
}

const struct area_vtable_ QUADRAT[] = { { form } };
