#include <cassert>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "MatrixUtil.h"
///////////////////////////////////////////////////////////////////////////
MatrixFloat rand_perm(unsigned int iSize) //create a vector of index shuffled
{
    MatrixFloat m(iSize);

    //create ordered vector
    for(unsigned int i=0;i<iSize;i++)
        m(i)=(float)i;

    //now bubble shuffle
    for(unsigned int i=0;i<iSize;i++)
    {
        unsigned int iNewPos=rand()%iSize;
        double dVal=m(iNewPos);
        m(iNewPos)=m(i);
        m(i)=(float)dVal;
    }

    return m;
}
///////////////////////////////////////////////////////////////////////////
MatrixFloat index_to_position(const MatrixFloat& mIndex, unsigned int uiMaxPosition)
{
    unsigned int uiNbRows=mIndex.rows();
    MatrixFloat mPos(uiNbRows,uiMaxPosition);
    mPos.setZero();

    for(unsigned int i=0;i<uiNbRows;i++)
    {
        mPos(i,(unsigned int)mIndex(i))=1;
    }

    return mPos;
}
///////////////////////////////////////////////////////////////////////////
MatrixFloat argmax(const MatrixFloat& m)
{
    if(m.cols()==0)
        return m;

    MatrixFloat mResult(m.rows(),1);

    for(unsigned int iR=0;iR<m.rows();iR++)
    {
        double d=m(iR,0);
        unsigned int iIndex=0;

        for(unsigned int iC=1;iC<m.cols();iC++)
        {
            if(m(iR,iC)>d)
            {
                d=m(iR,iC);
                iIndex=iC;
            }
        }
        mResult(iR)=(float)iIndex;
    }

    return mResult;
}
///////////////////////////////////////////////////////////////////////////
MatrixFloat decimate(const MatrixFloat& m, unsigned int iRatio)
{
	unsigned int iNewSize=m.rows()/iRatio;
	
    MatrixFloat mDecimated(iNewSize,m.cols());

    for(unsigned int i=0;i<iNewSize;i++)
		mDecimated.row(i)=m.row(i*iRatio);
	
	return mDecimated;
}
///////////////////////////////////////////////////////////////////////////
string to_string(const MatrixFloat& m)
{
    stringstream ss; ss << setprecision(4);
    for(unsigned int iL=0;iL<m.rows();iL++)
    {
        for(unsigned int iR=0;iR<m.cols();iR++)
            ss << setw(10) << m(iL,iR);
        ss << endl;
    }

    return ss.str();
}
///////////////////////////////////////////////////////////////////////////
