#include area.h

static const char *form(void){
    return "triangle";
}

const struct area_vtable_ TRIANGLE[] = { { form } };

