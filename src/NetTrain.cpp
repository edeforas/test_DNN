#include "NetTrain.h"

#include "Net.h"
#include "Layer.h"
#include "Matrix.h"
#include "MatrixUtil.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
NetTrain::NetTrain()
{ }
/////////////////////////////////////////////////////////////////////////////////////////////////
NetTrain::~NetTrain()
{ }
/////////////////////////////////////////////////////////////////////////////////////////////////
double NetTrain::compute_loss(const Net& net, const MatrixFloat &mSamples, const MatrixFloat &mTruth)
{
    if(net.layers().size()==0)
        return -1.;

    int iNbSamples=mSamples.rows();
    MatrixFloat mOut,mError;

    for(int i=0;i<iNbSamples;i++)
    {
        net.forward(mSamples.row(i),mOut);

        if(i==0)
            mError=(mOut-mTruth.row(i)).cwiseAbs2();
        else
            mError+=(mOut-mTruth.row(i)).cwiseAbs2();
    }

    return mError.sum()/(double)iNbSamples;
}
/////////////////////////////////////////////////////////////////////////////////////////////
