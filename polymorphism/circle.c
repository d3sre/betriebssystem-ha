#include area.h

static const char *form(void)
{
    return "round";
}

const struct area_vtable_ CIRCLE[] = { { form } };
