#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

#include "Net.h"

#include "NetTrainSGD.h"

int main()
{
    //build net
    Net net;
    net.add_dense_layer(1,10,true);
    net.add_activation_layer("Tanh");
    net.add_dense_layer(10,1,true);

    //set train data
    MatrixFloat mTruth(64,1);
    MatrixFloat mSamples(64,1);
    for(int i=0;i<64;i++)
    {
        float x=i/10.f;
        mTruth(i,0)=sin(x);
        mSamples(i,0)=x;
    }

    //learn
    cout << "Learning..." << endl;
    TrainOption tOpt;
    tOpt.learningRate=0.1f;
    NetTrainSGD netfit;
    netfit.fit(net,mSamples,mTruth,tOpt);

    //show results
    MatrixFloat mOnePredict(1,1), mOneSample(1,1), mOneTruth(1,1);
    for(int i=0;i<mSamples.size();i+=4) //show 16 samples
    {
        mOneSample(0,0)=mSamples(i,0);
        mOneTruth(0,0)=mTruth(i,0);
        net.forward(mOneSample,mOnePredict);
        cout << std::setprecision(4) << "x=" << mOneSample(0,0) << "\ttruth=" << mOneTruth(0,0) << "\tpredict=" << mOnePredict(0,0) <<endl;
    }

    //compute loss
    double dLoss=netfit.compute_loss(net,mSamples,mTruth);
    cout << "Loss=" << dLoss << endl;

    return 0;
}
