//
//  TAL source code is free for non-commercial use. It may be copied,
//  modified, and redistributed provided that credit is given to the author
//  somewhere in your program documentation. TAL source code is provided
//  'as is' without any warranty, implied or expressed. TAL website:
//  http://www.agri.upm.edu.my/~chris/tal
//
//  File     : futil.cpp
//  Author   : Christopher Teh Boon Sung (chris@agri.upm.edu.my)
//  Created  : Feb. 2001
//  Updated  : Aug. 2002
//  Version  : 1.2
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
namespace std {
#endif

#include <cmath>

#ifdef _MSC_VER
}
#endif

#include <stdexcept>
#include "futil.h"

bool futil::eq(double dLhs, double dRhs)
{
    return ((dLhs > dRhs-TOL) && (dLhs < dRhs+TOL));
}

bool futil::neq(double dLhs, double dRhs)
{
    return !eq(dLhs, dRhs);
}

bool futil::gt(double dLhs, double dRhs)
{
    return ((dLhs > dRhs) && neq(dLhs, dRhs));
}

bool futil::gte(double dLhs, double dRhs)
{
    return ((dLhs > dRhs) || eq(dLhs, dRhs));
}

bool futil::lt(double dLhs, double dRhs)
{
    return ((dLhs < dRhs) && neq(dLhs, dRhs));
}

bool futil::lte(double dLhs, double dRhs)
{
    return ((dLhs < dRhs) || eq(dLhs, dRhs));
}

// Round a value to a given decimal place
void futil::round(double &dVal, int nDecimal)
{
    if (nDecimal < 0)
        throw std::invalid_argument("futil::round");

    // move the decimal place nDecimal number of times
    dVal *= std::pow(10, nDecimal);

    double dDiff;
    if (dVal > 0)       // check for -ve or +ve
    {
        dDiff = dVal - std::floor(dVal);
        if (gte(dDiff, 0.5))
            dVal = std::ceil(dVal);      // e.g. 2.6 -> 3.0
        else
            dVal = std::floor(dVal);     // e.g. 2.2 -> 2.0
    }
    else
    {
        dDiff = std::fabs(dVal) - std::floor(std::fabs(dVal));
        if (gte(dDiff, 0.5))
            dVal = std::floor(dVal);    // e.g. -2.6 -> -3.0
        else
            dVal = std::ceil(dVal);     // e.g. -2.2 -> -2.0
    }

    // move back the decimal place to the original position
    dVal /= std::pow(10, nDecimal);
}

// Sign of value: 1 for +ve, and -1 for -ve
int futil::nsgn(double dVal)
{
    int sgn = 0;
    if (dVal > TOL)
        sgn = 1;
    else if (dVal < -TOL)
        sgn = -1;
    return sgn;
}
