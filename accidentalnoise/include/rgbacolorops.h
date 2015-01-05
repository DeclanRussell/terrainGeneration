#ifndef RGBA_COLOR_OPS_H
#define RGBA_COLOR_OPS_H
#include "rgbamodulebase.h"

namespace anl
{
    enum EColorOperations
    {
        COLORMULTIPLY,
        COLORADD,
        SCREEN,
        OVERLAY,
        SOFTLIGHT,
        HARDLIGHT,
        DODGE,
        BURN,
        LINEARDODGE,
        LINEARBURN
    };
    class CRGBAColorOps : public CRGBAModuleBase
    {
        public:
        CRGBAColorOps();
        CRGBAColorOps(int op);
        ~CRGBAColorOps();

        void setOperation(int op);

        void setSource1(float r, float g, float b, float a);
        void setSource1(CRGBAModuleBase *m);
        void setSource2(float r, float g, float b, float a);
        void setSource2(CRGBAModuleBase *m);

        SRGBA get(double x, double y);
        SRGBA get(double x, double y, double z);
        SRGBA get(double x, double y, double z, double w);
        SRGBA get(double x, double y, double z, double w, double u, double v);

        protected:
        CRGBAParameter m_source1, m_source2;
        int m_op;

        SRGBA multiply(SRGBA &s1, SRGBA &s2);
        SRGBA add(SRGBA &s1, SRGBA &s2);
        SRGBA screen(SRGBA &s1, SRGBA &s2);
        SRGBA overlay(SRGBA &s1, SRGBA &s2);
        SRGBA softLight(SRGBA &s1, SRGBA &s2);
        SRGBA hardLight(SRGBA &s1, SRGBA &s2);
        SRGBA dodge(SRGBA &s1, SRGBA &s2);
        SRGBA burn(SRGBA &s1, SRGBA &s2);
        SRGBA linearDodge(SRGBA &s1, SRGBA &s2);
        SRGBA linearBurn(SRGBA &s1, SRGBA &s2);
    };
};

#endif
