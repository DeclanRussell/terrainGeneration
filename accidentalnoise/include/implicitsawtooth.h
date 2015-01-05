#ifndef IMPLICIT_SAWTOOTH_H
#define IMPLICIT_SAWTOOTH_H
#include "implicitmodulebase.h"

namespace anl
{
class CImplicitSawtooth : public CImplicitModuleBase
{
    public:
    CImplicitSawtooth(double period);
    ~CImplicitSawtooth();

    void setSource(CImplicitModuleBase *s);
    void setSource(double s);
    void setPeriod(CImplicitModuleBase *p);
    void setPeriod(double p);

    double get(double x, double y);
    double get(double x, double y, double z);
    double get(double x, double y, double z, double w);
    double get(double x, double y, double z, double w, double u, double v);

    protected:
    CScalarParameter m_source;
    CScalarParameter m_period;

};
};
#endif
