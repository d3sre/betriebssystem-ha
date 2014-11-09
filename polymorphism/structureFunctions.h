#ifndef STRUCTUREFUNCTIONS_H_
#define STRUCTUREFUNCTIONS_H_

int area(struct shape *shape)
{
    return shape->area(shape);
}

int circumference(struct shape *shape)
{
    return shape->circumference(shape);
};

void output(struct shape *shape)
{
    return shape->output(shape);
}



#endif
