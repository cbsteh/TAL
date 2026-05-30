//
//  TAL source code is free for non-commercial use. It may be copied,
//  modified, and redistributed provided that credit is given to the author
//  somewhere in your program documentation. TAL source code is provided
//  'as is' without any warranty, implied or expressed. TAL website:
//  http://www.agri.upm.edu.my/~chris/tal
//
//  File     : soiltex.cpp
//  Author   : Christopher Teh Boon Sung (chris@agri.upm.edu.my)
//  Created  : Feb. 2001
//  Updated  : Aug. 2002
//  Version  : 1.2
//
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <fstream>
#include "soiltex.h"
#include "futil.h"

using namespace tal;

namespace
{
    const char TAG_TEXTURE[] = "[texture]";
    const char TAG_BORDER[] = "[border]";
    const char TAG_END[] = "#";
    const char TAG_EQUAL[] = "=";
}

///////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
///////////////////////////////////////////////////////////////////////////////

soiltex::soiltex()
   : mAxX("x"), mAxY("y"), mAxZ("z")      // default axes names
{}

soiltex::soiltex(const soiltex &rhs)
    : mSchemeName(rhs.mSchemeName),
      mAxX(rhs.mAxX), mAxY(rhs.mAxY), mAxZ(rhs.mAxZ),
      mTex(rhs.mTex),
      mBod(rhs.mBod)
{}

soiltex::~soiltex()
{}

soiltex &soiltex::operator=(const soiltex &rhs)
{
    if (this != &rhs)
    {
        ClearScheme();
        mSchemeName = rhs.mSchemeName;
        mAxX = rhs.mAxX; mAxY = rhs.mAxY; mAxZ = rhs.mAxZ;
        mTex = rhs.mTex;
        mBod = rhs.mBod;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Implementation (protected member functions)
///////////////////////////////////////////////////////////////////////////////

// Texture classes (as pointed by ptc) will be checked against
// all modified borders and adjusted if needed
void soiltex::AdjustForBorder(std::vector<TEXTURE> *ptc) const
{
    if (!ptc) throw std::invalid_argument("soiltex::AdjustForBorder");

    bool bFound = false;
    int cBod = static_cast<int>(mBod.size());
    int n = static_cast<int>(ptc->size());
    for (int i=0; i<cBod && !bFound; ++i)
    {
        // determine if all the mCode in *ptc container matches
        // the modified border codes (mFrom):
        const std::vector<int> *pBorder = &(mBod[i].mFrom);
        int cBorder = static_cast<int>(pBorder->size());
        if (cBorder != n) continue;     // no match if both sizes are different

        int cMatch = 0;
        for (int j=0; j<cBorder; ++j)
        {
            int iCode = pBorder->at(j);
            for (int k=0; k<n; ++k)
            {
                if (ptc->at(k).mCode == iCode)
                {
                    ++cMatch;   // count the no. of matches
                    break;
                }
            }
        }

        // match if cMatch equals modified border count
        bFound = (cBorder!=0 && cMatch==cBorder);
        if (bFound)
        {
            // delete mCodes in *ptc that doesn't match the codes
            // of modified border (mTo):
            const std::vector<int> *pResult = &(mBod[i].mTo);
            int cResult = static_cast<int>(pResult->size());
            int nLoc = 0, nn = static_cast<int>(ptc->size());
            while (nLoc < nn)
            {
                int iCode = ptc->at(nLoc).mCode;
                bool bLoc = false;
                for (int m=0; !bLoc && m<cResult; ++m)
                    bLoc = (pResult->at(m) == iCode);   // check for match

                if (!bLoc)
                {
                    // delete the texture class with the unmatching code
                    ptc->erase(ptc->begin()+nLoc);
                    // update count because of erasure
                    nn = static_cast<int>(ptc->size());
                }
                else
                    ++nLoc;             // move index for next check
            }
        }
    }
}

// Removes any copies of texture classes, so for every texture class
// type in the *ptc container, there is only a single copy of it.
// The single copy to keep is the one with the smallest index.
void soiltex::RemoveCopies(std::vector<TEXTURE> *ptc) const
{
    if (!ptc) throw std::invalid_argument("soiltex::RemoveCopies");

    std::vector<TEXTURE>::size_type nLoc = 0;
    while (nLoc < ptc->size())
    {
        std::string name = ptc->at(nLoc).mName;
        std::vector<TEXTURE>::size_type nLoc2 = nLoc + 1;
        while (nLoc2 < ptc->size())
        {
            std::string name2 = ptc->at(nLoc2).mName;
            if (name2==name)
                ptc->erase(ptc->begin() + nLoc2);
            else
                ++nLoc2;
        }
        ++nLoc;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Classification scheme
///////////////////////////////////////////////////////////////////////////////

const std::string soiltex::SchemeName() const
{
    return mSchemeName;
}

void soiltex::SetSchemeName(const char *pszName)
{
    if (!pszName) throw std::invalid_argument("soiltex::SetSchemeName");
    mSchemeName = pszName;
}

// Removes all scheme and modified borders definitions
// and resets names of axes and scheme
void soiltex::ClearScheme()
{
    mSchemeName = "";
    mAxX = "x"; mAxY = "y"; mAxZ = "z";
    mTex.clear();
    int n = static_cast<int>(mBod.size());
    for (int i=0; i<n; ++i)
    {
        mBod[i].mFrom.clear();
        mBod[i].mTo.clear();
    }
    mBod.clear();
}

// Reads in the classification scheme from file
void soiltex::LoadScheme(std::fstream &f)
{
    if (!f) throw(std::ios_base::failure("soiltex::LoadScheme"));

    // find tag in file
    if (std::find(std::istream_iterator<std::string>(f),
                  std::istream_iterator<std::string>(),
                  std::string(TAG_TEXTURE))
        == std::istream_iterator<std::string>())
        throw(std::domain_error("soiltex::LoadScheme"));

    ClearScheme();              // tag found, so clear current scheme

    int nItem = 0;
    bool bX = true, bScheme = true;
    POINT pt;
    TEXTURE tc;
    std::string str;
    while (bScheme)
    {
        if (nItem < 5)
        {
            if (!std::getline(f, str)) return;  // allow spaces to be read
            if (str.length() == 0) continue;    // blank, so read next line
        }
        else
        {
            if (!(f >> str)) return;            // space as a delimiter
        }

        if (str == TAG_BORDER) bScheme = false; // break if border tag found

        if (bScheme)
        {
            if (nItem == 0)         // nItem will only be 0 to 3 once
                mSchemeName = str;
            else if (nItem == 1)
                mAxX = str;
            else if (nItem == 2)
                mAxY = str;
            else if (nItem == 3)
                mAxZ = str;
            else if (nItem == 4)    // set texture class name
                tc.mName = str;
            else if (nItem == 5)    // set texture class code
                tc.mCode = atoi(str.c_str());
            else if (str != TAG_END)
            {
                if (bX)
                    pt.x = atof(str.c_str());   // read x
                else
                {
                    pt.y = atof(str.c_str());   // complete it by reading y
                    tc.mPolygon.Add(pt);        // (x, y) read, so add now
                }
                bX = !bX;   // toggle between x and y coordinates
            }
            else
            {
                mTex.push_back(tc);     // add texture class
                // reset all for reading next texture class
                nItem = 3;              // set for next texture class
                bX = true;
                tc.mPolygon.Clear();
                pt.x = pt.y = 0.0;
            }

            ++nItem;
        }
    }

    int nBorder = -1;
    bool bAddBorder = true, bEqual = false;
    while (f >> str)
    {
        // e.g., 2 3 4 = 2 3  => LHS is 2 3 4, and RHS is 2 3
        if (str == TAG_END)         // end tag found, so will read next border
        {
            bAddBorder = true;      // prepare to add a new border
            bEqual = false;         // prepare to read LHS (left hand side)
            continue;               // next read
        }
        else if (str == TAG_EQUAL)  // equal sign found, so will read RHS
        {
            bEqual = true;          // prepare to read RHS (right hand side)
            continue;               // next read
        }
        else if (bAddBorder)        // true if must add a new border
        {
            mBod.push_back(MODBOD());
            ++nBorder;              // increment border index
            bAddBorder = false;     // new border already added
        }

        if (!bEqual)    // false = LHS; true = RHS;
            mBod.at(nBorder).mFrom.push_back(atoi(str.c_str()));
        else
            mBod.at(nBorder).mTo.push_back(atoi(str.c_str()));
    }
}

// Saves the classification scheme to a file
void soiltex::SaveScheme(std::fstream &f)
{
    if (!f) throw(std::ios_base::failure("soiltex::SaveScheme"));

    f << TAG_TEXTURE << "\n" << mSchemeName << "\n";
    f << mAxX << "\n" << mAxY << "\n" << mAxZ << "\n\n";

    int n = static_cast<int>(mTex.size());
    int i = 0;
    for (; i<n; ++i)
    {
        TEXTURE tc = mTex[i];
        f << tc.mName << "\n" << tc.mCode << "\n";
        int m = tc.mPolygon.Count();
        for (int j=0; j<m; ++j)
        {
            POINT pt = tc.mPolygon.At(j);
            f << pt.x << " " << pt.y << "\n";
        }
        f << TAG_END << "\n";
    }

    int nn = static_cast<int>(mBod.size());
    if (nn > 0)
    {
        f << TAG_BORDER << "\n";
        for (i=0; i<nn; ++i)
        {
            int qn = static_cast<int>(mBod[i].mFrom.size());
            for (int j=0; j<qn; ++j) f << mBod[i].mFrom[j] << " ";
            f << TAG_EQUAL << " ";
            int qm = static_cast<int>(mBod[i].mTo.size());
            for (int k=0; k<qm; ++k)
            {
                f << mBod[i].mTo[k];
                if (k < (qm-1)) f << " ";
            }
            f << "\n" << TAG_END << "\n";
        }
    }
}

// Checks the validity of the classification scheme
bool soiltex::IsSchemeValid() const
{
    // 1. must have at least one texture class defined
    int n = static_cast<int>(mTex.size());
    if (n == 0) return false;

    // 2. texture polygons must not overlap
    // 3. sum the area of all texture polygons to determine if
    //    they occupy a triangle area of 5000, i.e., (100% * 100%) / 2
    double dArea = 0.0;
    for (int i=0; i<n; ++i)
    {
        polygon subject = mTex[i].mPolygon;
        for (int j=i+1; j<n; ++j)
        {
            // no overlap of polygons if Count() is 0
            if (subject.Clip(mTex[j].mPolygon).Count() != 0)
                return false;
        }
        dArea += subject.Area();
    }

    bool bValid = futil::eq(dArea, 5000.0);     // final check
    return bValid;
}

///////////////////////////////////////////////////////////////////////////////
// Axes names
///////////////////////////////////////////////////////////////////////////////

const std::string soiltex::AxisX() const
{
    return mAxX;
}

const std::string soiltex::AxisY() const
{
    return mAxY;
}

const std::string soiltex::AxisZ() const
{
    return mAxZ;
}

void soiltex::SetAxisX(const char *pszX)
{
    if (!pszX) throw std::invalid_argument("soiltex::SetAxisX");
    mAxX = pszX;
}

void soiltex::SetAxisY(const char *pszY)
{
    if (!pszY) throw std::invalid_argument("soiltex::SetAxisY");
    mAxY = pszY;
}

void soiltex::SetAxisZ(const char *pszZ)
{
    if (!pszZ) throw std::invalid_argument("soiltex::SetAxisZ");
    mAxZ = pszZ;
}

///////////////////////////////////////////////////////////////////////////////
// Texture classes
///////////////////////////////////////////////////////////////////////////////

// Number of soil texture classes stored in scheme
int soiltex::TextureCount() const
{
    return static_cast<int>(mTex.size());
}

// Returns a copy of all of the texture classes
const std::vector<TEXTURE> soiltex::Texture() const
{
    return mTex;
}

// Retrieves a copy of the definition of a specified texture class
const TEXTURE soiltex::Texture(int nIdx) const
{
    return mTex.at(nIdx);
}

// Changes the definitions of all texture classes
void soiltex::SetTexture(const std::vector<TEXTURE> &vtc)
{
    mTex.clear();
    mTex = vtc;
}

// Changes the definition of a specified texture class
void soiltex::SetTexture(int nIdx, const TEXTURE &tc)
{
    mTex.at(nIdx) = tc;
}

// Adds a texture class to the classification scheme
void soiltex::AddTexture(const TEXTURE &tc)
{
    mTex.push_back(tc);
}

// Removes a specified texture class from the classification scheme
void soiltex::DelTexture(int nIdx)
{
    int n = static_cast<int>(mTex.size());
    if (nIdx<0 || nIdx>=n) throw std::out_of_range("soiltex::DelTexture");
    mTex.erase(mTex.begin() + nIdx);
}

///////////////////////////////////////////////////////////////////////////////
// Modified borders
///////////////////////////////////////////////////////////////////////////////

// Number of modified borders in scheme
int soiltex::ModBorderCount() const
{
    return static_cast<int>(mBod.size());
}

// Returns a copy of all of the modified borders
const std::vector<MODBOD> soiltex::ModBorder() const
{
    return mBod;
}

// Retrieves a copy of the specified modified border
const MODBOD soiltex::ModBorder(int nIdx) const
{
    return mBod.at(nIdx);
}

// Changes all modified borders
void soiltex::SetModBorder(const std::vector<MODBOD> &vmb)
{
    mBod.clear();
    mBod = vmb;
}

// Changes the definition of a specified modified border
void soiltex::SetModBorder(int nIdx, const MODBOD &mb)
{
    mBod.at(nIdx) = mb;
}

// Adds a modified border to the classification scheme
void soiltex::AddModBorder(const MODBOD &mb)
{
    mBod.push_back(mb);
}

// Removes a specified modified border from the classification scheme
void soiltex::DelModBorder(int nIdx)
{
    int n = static_cast<int>(mBod.size());
    if (nIdx<0 || nIdx>=n) throw std::out_of_range("soiltex::DelModBorder");
    mBod.erase(mBod.begin() + nIdx);
}

// Converts a range of values to a polygon
const polygon soiltex::RangeToPolygon(RANGE rgx, RANGE rgy, RANGE rgz) const
{
    // check validity of range
    bool bValid = true;
    RANGE range[3] = {rgx, rgy, rgz};
    // values must be within 0 and 100 inclusive, and mMin < mMax
    for (int i=0; i<3 && bValid; ++i)
        bValid = (futil::lt(range[i].mMin, range[i].mMax) &&
                  futil::gte(range[i].mMin, 0.0) &&
                  futil::lte(range[i].mMax, 100.0));

    polygon result;
    if (!bValid || !IsSchemeValid()) return result;

    polygon subject;
    subject.Add(POINT(rgx.mMin, rgy.mMin));
    subject.Add(POINT(rgx.mMax, rgy.mMin));
    subject.Add(POINT(rgx.mMax, rgy.mMax));
    subject.Add(POINT(rgx.mMin, rgy.mMax));

    polygon clip;
    double dXA = 100.0 - (rgy.mMin + rgz.mMin);
    double dXB = 100.0 - (rgy.mMax + rgz.mMin);
    double dXC = 100.0 - (rgy.mMin + rgz.mMax);
    double dXD = 100.0 - (rgy.mMax + rgz.mMax);
    clip.Add(POINT(dXD, rgy.mMax));
    clip.Add(POINT(dXC, rgy.mMin));
    clip.Add(POINT(dXA, rgy.mMin));
    clip.Add(POINT(dXB, rgy.mMax));

    result = subject.Clip(clip);
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Texture lookup
///////////////////////////////////////////////////////////////////////////////

// Lookup soil texture class; assign -1 to the argument to be ignored
const std::vector<TEXTURE> soiltex::PointLookup(double dX, double dY, double dZ) const
{
    std::vector<TEXTURE> result;
    double vals[3] = {dX, dY, dZ};

    // check if there is more than one ignored variable
    if ((vals[0]<0 && vals[1]<0) ||
        (vals[0]<0 && vals[2]<0) || (vals[0]<0 && vals[2]<0))
        return result;

    // check which variable is to be ignored, and give it a correct value
    if (vals[0] < 0)
        vals[0] = 100.0 - (vals[1] + vals[2]);
    else if (vals[1] < 0)
        vals[1] = 100.0 - (vals[0] + vals[2]);
    else if (vals[2] < 0)
        vals[2] = 100.0 - (vals[0] + vals[1]);

    // sum of variables must equal 100.0
    double dSum = vals[0] + vals[1] + vals[2];
    bool bValid = futil::eq(dSum, 100.0);

    // all values must be within 0 and 100 inclusive
    int i = 0;
    for (; i<3 && bValid; ++i)
        bValid = (futil::gte(vals[i], 0.0) && futil::lte(vals[i], 100.0));

    if (!bValid || !IsSchemeValid()) return result;

    // every texture class definition in the classification scheme
    POINT pt(vals[0], vals[1]);     // only use (x, y) coordinates
    int n = static_cast<int>(mTex.size());
    for (i=0; i<n; ++i)
    {
        const TEXTURE *ptc = &(mTex[i]);
        POSITION pos = ptc->mPolygon.Find(pt);  // find the point in polygon
        if (pos.mPos!=POSITION::NONE && pos.mPos!=POSITION::EXTERIOR)
            result.push_back(*ptc);
    }

    AdjustForBorder(&result);    // check against any modified borders
    return result;
}

// Lookup a range of soil texture classes
const std::vector<TEXTURE> soiltex::RangeLookup(RANGE rgx, RANGE rgy, RANGE rgz) const
{
    std::vector<TEXTURE> result;
    polygon rg = RangeToPolygon(rgx, rgy, rgz);
    if (rg.Count() == 0) return result;

    // every texture class definition in the classification scheme
    int n = static_cast<int>(mTex.size());
    for (int i=0; i<n; ++i)
    {
        const TEXTURE *ptc = &(mTex[i]);
        if (ptc->mPolygon.Clip(rg).Count() > 0)
            result.push_back(*ptc);
    }

    RemoveCopies(&result); // don't want copies of texture names
    return result;
}
