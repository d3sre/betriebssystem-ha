#ifndef AREA_H_
#define AREA_H_

struct area
{
    const struct area_vtable_ *vtable;
    const char *name;
};

struct area_vtable_
{
    const char *(*form)(void);
};

static inline const char *area_form(struct area *area)
{
    return area->vtable_->form();
}

extern const struct area_vtable_ CIRCLE[], QUADRAT[], TRIANGLE[];

#endif
