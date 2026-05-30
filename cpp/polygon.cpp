//
//  TAL source code is free for non-commercial use. It may be copied,
//  modified, and redistributed provided that credit is given to the author
//  somewhere in your program documentation. TAL source code is provided
//  'as is' without any warranty, implied or expressed. TAL website:
//  http://www.agri.upm.edu.my/~chris/tal
//
//  File     : polygon.cpp
//  Author   : Christopher Teh Boon Sung (chris@agri.upm.edu.my)
//  Created  : Feb. 2001
//  Updated  : Aug. 2002
//  Version  : 1.2
//
/////////////////////////////////////////////////////////////////////////////

#include "polygon.h"
#include "futil.h"

using namespace tal;

///////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
///////////////////////////////////////////////////////////////////////////////

polygon::polygon()
{}

polygon::polygon(const polygon &rhs)
    : mVertex(rhs.mVertex)
{}

polygon::~polygon()
{}

polygon &polygon::operator=(const polygon &rhs)
{
    if (this != &rhs)
    {
        mVertex.clear();
        mVertex = rhs.mVertex;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

POINT &polygon::operator[](int nIdx)
{
    if (nIdx < 0) throw std::out_of_range("polygon::operator[]");

    int n = static_cast<int>(mVertex.size());
    // add points if nIdx exceeds n-1
    for (int i=n-1; i<nIdx; ++i) mVertex.push_back(POINT());
    return mVertex.at(nIdx);
}

const POINT polygon::At(int nIdx) const
{
    return mVertex.at(nIdx);
}

void polygon::Set(int nIdx, POINT pt)
{
    mVertex.at(nIdx) = pt;
}

void polygon::Add(POINT pt)
{
    mVertex.push_back(pt);
}

void polygon::Del(int nIdx)
{
    int n = static_cast<int>(mVertex.size());
    if (nIdx<0 || nIdx>=n) throw std::out_of_range("polygon::Del");
    mVertex.erase(mVertex.begin() + nIdx);
}

void polygon::Clear()
{
    mVertex.clear();
}

int polygon::Count() const
{
    return static_cast<int>(mVertex.size());
}

///////////////////////////////////////////////////////////////////////////////
// Conversion to and fro from GPC
///////////////////////////////////////////////////////////////////////////////

// Converts the vertices stored in this class to GPC so you can easily use
// the GPC functions. NOTE: ensure the pointer pGPC points to an object
// that is newly instantiated and not used in anyway before this member
// function is called.
void polygon::ToGPC(gpc::gpc_polygon *pGPC) const
{
    if (!pGPC) throw std::invalid_argument("polygon::ToGPC");
    // memory allocation ala C style because GPC frees memory ala C style
    pGPC->num_contours = 1;
    pGPC->hole = static_cast<int*>(malloc(pGPC->num_contours*sizeof(int)));
    pGPC->contour = static_cast<gpc::gpc_vertex_list*>
                    (malloc(pGPC->num_contours*sizeof(gpc::gpc_vertex_list)));
    pGPC->contour[0].num_vertices = static_cast<int>(mVertex.size());
    pGPC->hole[0] = 0;  // FALSE; no holes in polygon
    pGPC->contour[0].vertex = static_cast<gpc::gpc_vertex*>
                              (malloc(pGPC->contour[0].num_vertices*sizeof(gpc::gpc_vertex)));
    for (int v = 0; v < pGPC->contour[0].num_vertices; ++v)
    {
        pGPC->contour[0].vertex[v].x = mVertex[v].x;
        pGPC->contour[0].vertex[v].y = mVertex[v].y;
    }
}

// Stores in this class the vertices in GPC
void polygon::FromGPC(const gpc::gpc_polygon *pGPC)
{
    if (!pGPC) throw std::invalid_argument("polygon::FromGPC");
    mVertex.clear();    // reset
    if (pGPC->num_contours > 0)
    {
        for (int v = 0; v < pGPC->contour[0].num_vertices; ++v)
            mVertex.push_back(POINT(pGPC->contour[0].vertex[v].x,
                                    pGPC->contour[0].vertex[v].y));
    }
}

// GPC Clipping operations (calls GPC function)
const polygon polygon::GPCOperation(const polygon &pg, gpc::gpc_op op) const
{
    // IMPORTANT: subject object is newly instantiated and not used in
    // anyway. After the call to ToGPC, subject will be properly constructed
    gpc::gpc_polygon subject;
    ToGPC(&subject);

    // IMPORTANT: clip object is newly instantiated and not used in
    // anyway. After the call to ToGPC, clip will be properly constructed
    gpc::gpc_polygon clip;
    pg.ToGPC(&clip);

    // result object will contain the clipping results. Ensure the
    // result object is newly instantiated and not used before passing
    // it into the gpc_polygon_clip function
    gpc::gpc_polygon result;
    gpc::gpc_polygon_clip(op, &subject, &clip, &result);

    polygon ar;
    ar.FromGPC(&result);    // make the result object compatible to ours

    // IMPORTANT: there is no explicit destructor for GPC objects,
    // so must do manual cleanup here (free memory)
    gpc::gpc_free_polygon(&subject);
    gpc::gpc_free_polygon(&clip);
    gpc::gpc_free_polygon(&result);

    return ar;
}

///////////////////////////////////////////////////////////////////////////////
// Services
///////////////////////////////////////////////////////////////////////////////

// Is polygon valid?
bool polygon::IsValid() const
{
    return (mVertex.size()>=3 && Orientation()!=0);
}

// Orientation of polygon: 1 for CCW, -1 for CW, 0 for error
int polygon::Orientation() const
{
    int n = static_cast<int>(mVertex.size());
    if (n < 3) throw std::domain_error("polygon::Orientation");

    double minn[2] = {mVertex[0].x, mVertex[0].y};
    int m = 0;
    for(int i=0; i < n; ++i)
    {
        if (mVertex[i].y < minn[1] ||
           (futil::eq(mVertex[i].y, minn[1]) && mVertex[i].x > minn[0]))
        {
            m = i;
            minn[0] = mVertex[m].x;
            minn[1] = mVertex[m].y;
        }
    }

    int m1 = (m + (n-1)) % n;
    double a[2] = {mVertex[m1].x, mVertex[m1].y};
    double b[2] = {mVertex[m].x, mVertex[m].y};
    double c[2] = {mVertex[(m+1)%n].x, mVertex[(m+1)%n].y};
    double area2 = a[0] * b[1] - a[1] * b[0] +
                   a[1] * c[0] - a[0] * c[1] +
                   b[0] * c[1] - c[0] * b[1];

    int iOrient = 0;
    if (area2 > 0)
        iOrient =  1;       // counter-clockwise
    else if (area2 < 0)
        iOrient = -1;       // clockwise

    return iOrient;
}

// Area of polygon
double polygon::Area() const
{
    int n = static_cast<int>(mVertex.size());
    if (n < 3) throw std::domain_error("polygon::Area");

    double dArea = 0.0;
    for (int i=0; i<n; ++i)
    {
        int j = i + 1;
        if (j == n) j = 0;
        dArea += mVertex[i].x * mVertex[j].y - mVertex[i].y * mVertex[j].x;
    }

    dArea /= 2;
    if (dArea < 0) dArea *= -1;     // area may be negative
    return dArea;
}

// Position of a given point in a polygon
const POSITION polygon::Find(POINT pt) const
{
    std::vector<POINT> P = mVertex;
    int n = static_cast<int>(P.size());
    if (n < 3) throw std::domain_error("polygon::Find");

    // shift so that pt is the origin
    int i = 0;
    for (; i < n; i++)
    {
        P[i].x -= pt.x;
        P[i].y -= pt.y;
    }

    int Rcross = 0;     // number of right edge/ray crossings
    int Lcross = 0;     // number of left edge/ray crossings
    POSITION pos;

    // for each edge e = (i - 1, i), see if crosses ray
    for (i = 0; i < n; i++)
    {
        // first, see if pt = (0, 0) is a vertex
        if (futil::eq(P[i].x, 0) && futil::eq(P[i].y, 0))
        {
            pos.mPos = POSITION::VERTEX;
            return pos;
        }

        // check if e "straddles" the x - axis
        int i1 = (i + n - 1) % n;
        if ((P[i].y > 0) != (P[i1].y > 0))
        {
            // e straddles ray, so compute intersection with ray
            double x = (P[i].x * P[i1].y - P[i1].x * P[i].y)
                       / (P[i1].y - P[i].y);
            // crosses ray if strictly positive intersection
            if (x > 0) ++Rcross;
        }

        // check if e straddles the x - axis when reversed
        if ((P[i].y < 0) != (P[i1].y < 0))
        {
            // e straddles ray, so compute intersection with ray
            double x = (P[i].x * P[i1].y - P[i1].x * P[i].y)
                       / (P[i1].y - P[i].y);
            // crosses ray if strictly positive intersection
            if (x < 0) ++Lcross;
        }
    }

    if ((Rcross % 2) != (Lcross % 2))
        // is edge if left and right cross are not the same parity
        pos.mPos = POSITION::EDGE;
    else if ((Rcross % 2) == 1)
        // is inside if an odd number of crossings
        pos.mPos = POSITION::INTERIOR;
    else
        // otherwise, completely outside
        pos.mPos = POSITION::EXTERIOR;

    return pos;
}

// Centroid (centre of gravity/mass) of the polygon
const POINT polygon::Centroid() const
{
    int n = static_cast<int>(mVertex.size());
    if (n < 3) throw std::domain_error("polygon::Centroid");

    POINT pt;
    double dA = 0.0;
    for (int i=0; i<n; ++i)
    {
        int j = i + 1;
        if (j == n) j = 0;
        double dT = mVertex[i].x * mVertex[j].y - mVertex[j].x * mVertex[i].y;
        pt.x += (mVertex[i].x + mVertex[j].x) * dT;
        pt.y += (mVertex[i].y + mVertex[j].y) * dT;
        dA += dT;
    }

    double dN = 3 * dA;
    pt.x /= dN;
    pt.y /= dN;
    return pt;
}

///////////////////////////////////////////////////////////////////////////////
// Polygon clipping operations
///////////////////////////////////////////////////////////////////////////////

// Intersection (overlap) of two polygons
const polygon polygon::Clip(const polygon &pg) const
{
    return GPCOperation(pg, gpc::GPC_INT);
}

// Difference of two polygons
const polygon polygon::Difference(const polygon &pg) const
{
    return GPCOperation(pg, gpc::GPC_DIFF);
}

// Exclusive-OR of two polygons
const polygon polygon::XOR(const polygon &pg) const
{
    return GPCOperation(pg, gpc::GPC_XOR);
}

// Union of two polygons
const polygon polygon::Union(const polygon &pg) const
{
    return GPCOperation(pg, gpc::GPC_UNION);
}
