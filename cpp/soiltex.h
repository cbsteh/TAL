//
//  TAL source code is free for non-commercial use. It may be copied,
//  modified, and redistributed provided that credit is given to the author
//  somewhere in your program documentation. TAL source code is provided
//  'as is' without any warranty, implied or expressed. TAL website:
//  http://www.agri.upm.edu.my/~chris/tal
//
//  File     : soiltex.h
//  Author   : Christopher Teh Boon Sung (chris@agri.upm.edu.my)
//  Created  : Feb. 2001
//  Updated  : Aug. 2002
//  Version  : 1.2
//
/////////////////////////////////////////////////////////////////////////////

#ifndef TAL_SOILTEX_H
#define TAL_SOILTEX_H

#include <string>
#include <iosfwd>
#include "polygon.h"

namespace tal
{
    // Range of two values
    typedef struct RANGEtag
    {
        double mMin, mMax;
        RANGEtag(double dMin=0.0, double dMax=100.0)
            : mMin(dMin), mMax(dMax) {}
    } RANGE;

    // Definition of a texture class
    typedef struct TEXTUREtag
    {
        std::string mName;  // name
        int mCode;          // code
        polygon mPolygon;   // polygon coordinates

        TEXTUREtag() : mName(""), mCode(-1) {}
        TEXTUREtag(const TEXTUREtag &rhs)
            : mName(rhs.mName), mCode(rhs.mCode), mPolygon(rhs.mPolygon) {}
        TEXTUREtag &operator=(const TEXTUREtag &rhs)
        {
            if (this != &rhs)
            {
                mName = rhs.mName;
                mCode = rhs.mCode;
                mPolygon = rhs.mPolygon;
            }
            return *this;
        }
    } TEXTURE;

    // Modified texture class border
    typedef struct MODBODtag
    {
        std::vector<int> mFrom, mTo;

        MODBODtag() {}
        MODBODtag(const MODBODtag &rhs) : mFrom(rhs.mFrom), mTo(rhs.mTo) {}
        MODBODtag &operator=(const MODBODtag &rhs)
        {
            if (this != &rhs)
            {
                mFrom = rhs.mFrom;
                mTo = rhs.mTo;
            }
            return *this;
        }
    } MODBOD;

    // Soil texture
    class soiltex
    {
    private:
        std::string mSchemeName;      // name of scheme
        std::string mAxX, mAxY, mAxZ; // names of each axis
        std::vector<TEXTURE> mTex;    // definitions of all texture classes
        std::vector<MODBOD> mBod;     // definitions of all modified borders

    protected:
        // checks for modified borders
        void AdjustForBorder(std::vector<TEXTURE> *ptc) const;

        // removes copies of the same texture class names
        void RemoveCopies(std::vector<TEXTURE> *ptc) const;

    public:
        // Constructors and destructor:
        soiltex();
        soiltex(const soiltex &rhs);
        virtual ~soiltex();

        soiltex &operator=(const soiltex &rhs);

        // Classification scheme:
        const std::string SchemeName() const;
        void SetSchemeName(const char *pszName);
        void ClearScheme();
        virtual void LoadScheme(std::fstream &f);
        virtual void SaveScheme(std::fstream &f);
        virtual bool IsSchemeValid() const;

        // Axes names:
        const std::string AxisX() const;
        const std::string AxisY() const;
        const std::string AxisZ() const;
        void SetAxisX(const char *pszX);
        void SetAxisY(const char *pszY);
        void SetAxisZ(const char *pszZ);
        
        // Texture classes:
        int TextureCount() const;
        const std::vector<TEXTURE> Texture() const;
        const TEXTURE Texture(int nIdx) const;
        void SetTexture(const std::vector<TEXTURE> &vtc);
        void SetTexture(int nIdx, const TEXTURE &tc);
        void AddTexture(const TEXTURE &tc);
        void DelTexture(int nIdx);

        // Modified borders:
        int ModBorderCount() const;
        const std::vector<MODBOD> ModBorder() const;
        const MODBOD ModBorder(int nIdx) const;
        void SetModBorder(const std::vector<MODBOD> &vmb);
        void SetModBorder(int nIdx, const MODBOD &mb);
        void AddModBorder(const MODBOD &mb);
        void DelModBorder(int nIdx);

        // Converts a range of values to a polygon
        const polygon RangeToPolygon(RANGE rgx, RANGE rgy, RANGE rgz=RANGE()) const;

        // Texture lookup:
        virtual const std::vector<TEXTURE> PointLookup(double dX, double dY, double dZ=-1) const;
        virtual const std::vector<TEXTURE> RangeLookup(RANGE rgx, RANGE rgy, RANGE rgz=RANGE()) const;
    };
}

#endif
