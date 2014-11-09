#ifndef STRUCTUREFUNCTIONS_H_
#define STRUCTUREFUNCTIONS_H_

double area(struct shape *shape)
{
    const struct area_vtable_ *vtable;
    const char *name;
};

double circumference(struct shape *shape)
{
    const char *(*form)(void);
};

void output(struct shape *shape)
{
    return area->vtable_->form();
}

extern const struct area_vtable_ CIRCLE[], QUADRAT[], TRIANGLE[];

#endif
