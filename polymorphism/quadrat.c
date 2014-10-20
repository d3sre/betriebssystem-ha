#include area.h

static const char *form(void) {
    return "quadrat";
}

const struct area_vtable_ QUADRAT[] = { { form } };
