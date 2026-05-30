//
//  TAL source code is free for non-commercial use. It may be copied,
//  modified, and redistributed provided that credit is given to the author
//  somewhere in your program documentation. TAL source code is provided
//  'as is' without any warranty, implied or expressed. TAL website:
//  http://www.agri.upm.edu.my/~chris/tal
//
//  File     : polygon.h
//  Author   : Christopher Teh Boon Sung (chris@agri.upm.edu.my)
//  Created  : Feb. 2001
//  Updated  : Aug. 2002
//  Version  : 1.2
//
/////////////////////////////////////////////////////////////////////////////

#ifndef TAL_POLYGON_H
#define TAL_POLYGON_H

#include <vector>

namespace gpc   // GPC library
{
    extern "C"
    {
#include "gpc.h"
    }
}

namespace tal
{
    // Cartesian coordinates of a point
    typedef struct POINTtag
    {
        double x, y;
        POINTtag(double dX=0.0, double dY=0.0) : x(dX), y(dY) {}
    } POINT;

    // Position of a single point in a polygon
    typedef struct POSITIONtag
    {
        int mPos;
        POSITIONtag() : mPos(NONE) {}
        enum {NONE=0, INTERIOR=1, EXTERIOR, VERTEX, EDGE};
    } POSITION;

    // Polygon
    class polygon
    {
    private:
        std::vector<POINT> mVertex;    // vertices

    protected:
        // convert to and from GPC:
        virtual void ToGPC(gpc::gpc_polygon *pGPC) const; 
        virtual void FromGPC(const gpc::gpc_polygon *pGPC);

        // GPC polygon operations (calls GPC clipping function)
        const polygon GPCOperation(const polygon &pg, gpc::gpc_op op) const;

    public:
        // Constructors and destructor:
        polygon();
        polygon(const polygon &rhs);
        virtual ~polygon();

        polygon &operator=(const polygon &rhs);

        // Accessors:
        POINT &operator[](int nIdx);
        const POINT At(int nIdx) const;
        void Set(int nIdx, POINT pt);
        void Add(POINT pt);
        void Del(int nIdx);
        void Clear();
        int Count() const;

        // Services:
        virtual bool IsValid() const;
        int Orientation() const;
        double Area() const;
        const POSITION Find(POINT pt) const;
        const POINT Centroid() const;

        // Polygon clipping operations:
        const polygon Clip(const polygon &pg) const;
        const polygon Difference(const polygon &pg) const;
        const polygon XOR(const polygon &pg) const;
        const polygon Union(const polygon &pg) const;
    };
}

#endif
