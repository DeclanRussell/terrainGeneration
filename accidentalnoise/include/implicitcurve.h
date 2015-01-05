#ifndef IMPLICIT_CURVE_H
#define IMPLICIT_CURVE_H
#include "implicitmodulebase.h"
#include "implicitbasisfunction.h"
#include "templates/tcurve.h"
namespace anl
{
    class CImplicitCurve : public CImplicitModuleBase
    {
        public:
        CImplicitCurve();
        ~CImplicitCurve();

        void pushPoint(double t, double v);
        void clearCurve();
        void setSource(double t);
        void setSource(CImplicitModuleBase *m);
        void setInterpType(int type);

        double get(double x, double y);
        double get(double x, double y, double z);
        double get(double x, double y, double z, double w);
        double get(double x, double y, double z, double w, double u, double v);

        protected:
        TCurve<double> m_curve;
        CScalarParameter m_source;
        int m_type;
    };
};

#endif
