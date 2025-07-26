#include "guardar_en_disco.h"

void guardar_en_disco::init(double t, ...)
{
    char *FName;
    va_list parameters;
    va_start(parameters, t);
    FName = va_arg(parameters, char *);
    Sigma = 10e10;
    FOutput = PDFileOpen(FName, 'w');
}

double guardar_en_disco::ta(double t)
{
    return Sigma;
}

void guardar_en_disco::dint(double t)
{
    Sigma = 10e10;
}

void guardar_en_disco::dext(Event x, double t)
{
    char buf[1024];
    double *Aux;
    if (x.port >= 0)
    {
        Aux = (double *)(x.value);
        sprintf(buf, "%g, %g\n", t, *Aux);
        PDFileWrite(FOutput, buf, strlen(buf));
    }
}

Event guardar_en_disco::lambda(double t)
{
    return Event(0, 0);
}

void guardar_en_disco::exit()
{
    PDFileClose(FOutput);
}