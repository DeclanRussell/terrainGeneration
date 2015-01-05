#ifndef IMPLICIT_TRIANGLE_H
#define IMPLICIT_TRIANGLE_H
#include "implicitmodulebase.h"

namespace anl
{
    class CImplicitTriangle : public CImplicitModuleBase
    {
        public:
        CImplicitTriangle(double period, double offset);
        ~CImplicitTriangle();

        void setSource(double s);
        void setSource(CImplicitModuleBase *s);
        void setPeriod(double p);
        void setPeriod(CImplicitModuleBase *p);
        void setOffset(double o);
        void setOffset(CImplicitModuleBase *o);

        double get(double x, double y);
        double get(double x, double y, double z);
        double get(double x, double y, double z, double w);
        double get(double x, double y, double z, double w, double u, double v);

        protected:
        CScalarParameter m_source, m_period, m_offset;
    };
};

#endif
