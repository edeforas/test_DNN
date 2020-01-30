/*
    Copyright (c) 2019, Etienne de Foras and the respective contributors
    All rights reserved.

    Use of this source code is governed by a MIT-style license that can be found
    in the LICENSE.txt file.
*/

#include "NetUtil.h"
#include "Net.h"
#include "NetTrain.h"
#include "Layer.h"
#include "Matrix.h"

#include "LayerActivation.h"
#include "LayerBias.h"
#include "LayerChannelBias.h"
#include "LayerConvolution2D.h"
#include "LayerDense.h"
#include "LayerDropout.h"
#include "LayerGain.h"
#include "LayerGaussianNoise.h"
#include "LayerGlobalBias.h"
#include "LayerGlobalGain.h"
#include "LayerPoolMax2D.h"
#include "LayerPRelu.h"
#include "LayerSoftmax.h"
#include "LayerUniformNoise.h"

#include <sstream>
#include <fstream>
using namespace std;

namespace NetUtil {

/////////////////////////////////////////////////////////////////////////////////////////////////
void write(const Net& net,string & s)
{
    stringstream ss;

    auto layers=net.layers();
    ss << "Engine=BeeDNN" << endl;
    ss << "NbLayers=" << layers.size() << endl;

	string sProblem= net.is_classification_mode()?"Classification":"Regression";
	ss << "Problem=" << sProblem << endl;

    for(size_t i=0;i<layers.size();i++)
    {
        Layer* layer=layers[i];

        ss << endl;
        ss << "Layer" << i+1 << ".type=" << layer->type() << endl;
        if(layer->type()=="Dense")
        {
            LayerDense* l=static_cast<LayerDense*>(layer);
            ss << "Layer" << i+1 << ".hasBias=" << (l->has_bias()?1:0) << endl;
			ss << "Layer" << i + 1 << ".inputSize=" << l->input_size() << endl;
			ss << "Layer" << i + 1 << ".outputSize=" << l->output_size() << endl;
            ss << "Layer" << i+1 << ".weight=" << endl;
            ss << toString(layer->weights()) << endl;
        }

        else if(layer->type()=="GlobalGain")
        {
            LayerGlobalGain* l=static_cast<LayerGlobalGain*>(layer);
            ss << "Layer" << i+1 << ".globalGain=" << l->weights()(0) << endl;
        }

		else if (layer->type() == "Gain")
		{
			LayerGain* l = static_cast<LayerGain*>(layer);
			ss << "Layer" << i + 1 << ".gain=" << toString(l->weights()) << endl;
		}
		
		else if(layer->type()=="GlobalBias")
        {
            LayerGlobalBias* l=static_cast<LayerGlobalBias*>(layer);
            ss << "Layer" << i+1 << ".globalBias=" << l->weights()(0) << endl;
        }

		else if(layer->type()=="ChannelBias")
        {
            LayerChannelBias* l=static_cast<LayerChannelBias*>(layer);
            ss << "Layer" << i+1 << ".channelBias=" << toString(l->weights()) << endl;
        }

		else if (layer->type() == "Bias")
		{
			LayerBias* l = static_cast<LayerBias*>(layer);
			ss << "Layer" << i + 1 << ".bias=" << toString(l->weights()) << endl;
		}

        else if(layer->type()=="Dropout")
        {
            LayerDropout* l=static_cast<LayerDropout*>(layer);
            ss << "Layer" << i+1 << ".rate=" << l->get_rate() << endl;
        }

		else if (layer->type() == "PRelu")
		{
			ss << "Layer" << i + 1 << ".weight=" << endl;
			ss << toString(layer->weights()) << endl;
		}

        else if (layer->type() == "GaussianNoise")
        {
            LayerGaussianNoise* l = static_cast<LayerGaussianNoise*>(layer);
            ss << "Layer" << i+1 << ".stdNoise=" << l->get_std() << endl;
        }
		else if (layer->type() == "UniformNoise")
		{
			LayerUniformNoise* l = static_cast<LayerUniformNoise*>(layer);
			ss << "Layer" << i + 1 << ".noise=" << l->get_noise() << endl;
		}
		else if (layer->type() == "PoolMax2D")
		{
			LayerPoolMax2D* l = static_cast<LayerPoolMax2D*>(layer);

			Index inRows, inCols, iChannels, rowFactor, colFactor;
			l->get_params(inRows, inCols, iChannels, rowFactor, colFactor);
			ss << "Layer" << i + 1 << ".inRows=" << inRows << endl;
			ss << "Layer" << i + 1 << ".inCols=" << inCols << endl;
			ss << "Layer" << i + 1 << ".inChannels=" << iChannels << endl;
			ss << "Layer" << i + 1 << ".rowFactor=" << rowFactor << endl;
			ss << "Layer" << i + 1 << ".colFactor=" << colFactor << endl;
		}
		else if (layer->type() == "Convolution2D")
		{
			LayerConvolution2D* l = static_cast<LayerConvolution2D*>(layer);

			ss << "Layer" << i + 1 << ".weight=" << endl;
			ss << toString(l->weights()) << endl;

			Index inRows, inCols, inChannels, kernelRows, kernelCols, outChannels, rowStride, colStride;
			l->get_params(inRows, inCols, inChannels, kernelRows, kernelCols, outChannels,rowStride,colStride);

			ss << "Layer" << i + 1 << ".inRows=" << inRows << endl;
			ss << "Layer" << i + 1 << ".inCols=" << inCols << endl;
			ss << "Layer" << i + 1 << ".inChannels=" << inChannels << endl;
			ss << "Layer" << i + 1 << ".kernelRows=" << kernelRows << endl;
			ss << "Layer" << i + 1 << ".kernelCols=" << kernelCols << endl;
			ss << "Layer" << i + 1 << ".rowStride=" << rowStride << endl;
			ss << "Layer" << i + 1 << ".colStride=" << colStride << endl;
			ss << "Layer" << i + 1 << ".outChannels=" << outChannels << endl;
		}
	}

    s+=ss.str();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void read(const string& s,Net& net)
{
    net.clear();

    string sNbLayer=NetUtil::find_key(s,"NbLayers");
    int iNbLayers=stoi(sNbLayer);

    string sProblem = NetUtil::find_key(s, "Problem");
    bool bClassification = (sProblem == "Classification");
    net.set_classification_mode(bClassification);
 
    for(int i=0;i<iNbLayers;i++)
    {
        string sLayer="Layer"+to_string(i+1);

        string sType=find_key(s,sLayer+".type");

        if(sType=="Dense")
        {        
			string sInputSize=find_key(s,sLayer+".inputSize");
			string sOutputSize=find_key(s,sLayer+".outputSize");
            Index iInputSize=stoi(sInputSize); 
			Index iOutputSize=stoi(sOutputSize);
			
			string sHasBias=find_key(s,sLayer+".hasBias");
            bool bHasBias=sHasBias!="0";
            net.add(new LayerDense(iInputSize,iOutputSize,bHasBias));

            string sWeight=find_key(s,sLayer+".weight");
            MatrixFloat mf=fromString(sWeight);
            mf.resize(iInputSize+(bHasBias?1:0),iOutputSize);
            net.layer(net.size()-1).weights()=mf;
        }

        else if(sType=="GlobalGain")
        {
            float fGain= stof(find_key(s,sLayer+".globalGain"));
            net.add(new LayerGlobalGain());
			MatrixFloat mf(1, 1);
			mf(0) = fGain;
			net.layer(net.size() - 1).weights() = mf;
        }

		else if (sType == "Gain")
		{
			string sWeight = find_key(s, sLayer + ".gain");
			MatrixFloat mf = fromString(sWeight);
			net.add(new LayerGain());
			net.layer(net.size() - 1).weights() = mf;
		}
		
		else if(sType=="GlobalBias")
        {
            float fBias= stof(find_key(s,sLayer+".globalBias"));
            net.add(new LayerGlobalBias());
            MatrixFloat mf(1, 1);
            mf(0) = fBias;
            net.layer(net.size() - 1).weights() = mf;
        }

		else if (sType == "ChannelBias")
		{
			string sWeight = find_key(s, sLayer + ".channelBias");
			MatrixFloat mf = fromString(sWeight);
			net.add(new LayerBias());
			net.layer(net.size() - 1).weights() = mf;
		}
		
		else if (sType == "Bias")
		{
			string sWeight = find_key(s, sLayer + ".bias");
			MatrixFloat mf = fromString(sWeight);
			net.add(new LayerBias());
			net.layer(net.size() - 1).weights() = mf;
		}

        else if(sType=="Dropout")
        {
            string sRate=find_key(s,sLayer+".rate");
            net.add(new LayerDropout(stof(sRate)));
        }

		else if (sType == "PRelu")
		{
			net.add(new LayerPRelu());
			string sWeight = find_key(s, sLayer + ".weight");
			MatrixFloat mf = fromString(sWeight);
			mf.resize(1, mf.size());
			net.layer(net.size() - 1).weights() = mf;
		}

        else if (sType == "GaussianNoise")
        {
            string sNoise=find_key(s,sLayer+".stdNoise");
            net.add(new LayerGaussianNoise(stof(sNoise)));
        }

		else if (sType == "UniformNoise")
		{
			string sNoise = find_key(s, sLayer + ".noise");
			net.add(new LayerUniformNoise(stof(sNoise)));
		}

		else if (sType == "PoolMax2D")
		{
			Index inSizeX = stoi(find_key(s, sLayer + ".inRows"));
			Index inSizeY = stoi(find_key(s, sLayer + ".inCols"));
			Index inChannels = stoi(find_key(s, sLayer + ".inChannels"));
			Index factorX = stoi(find_key(s, sLayer + ".rowFactor"));
			Index factorY = stoi(find_key(s, sLayer + ".colFactor"));
			net.add(new LayerPoolMax2D(inSizeX, inSizeY, inChannels, factorX, factorY));
		}

		else if (sType == "Convolution2D")
		{
			Index inSizeX = stoi(find_key(s, sLayer + ".inRows"));
			Index inSizeY = stoi(find_key(s, sLayer + ".inCols"));
			Index inChannels = stoi(find_key(s, sLayer + ".inChannels"));
			Index kernelRows = stoi(find_key(s, sLayer + ".kernelRows"));
			Index kernelCols = stoi(find_key(s, sLayer + ".kernelCols"));
			Index rowStride = stoi(find_key(s, sLayer + ".rowStride"));
			Index colStride = stoi(find_key(s, sLayer + ".colStride"));
			Index outChannels = stoi(find_key(s, sLayer + ".outChannels"));
			net.add(new LayerConvolution2D(inSizeX, inSizeY, inChannels, kernelRows, kernelCols, outChannels, rowStride,colStride));

			string sWeight = find_key(s, sLayer + ".weight");
			MatrixFloat mf = fromString(sWeight);
			net.layer(net.size() - 1).weights() = mf;
		}

		else if (sType == "Softmax")
		{
			net.add(new LayerSoftmax());
		}

        else
        {
            //activation layer
            net.add(new LayerActivation(sType));
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void write(const NetTrain& train,string & s)
{
    stringstream ss;
    ss << "Epochs=" <<train.get_epochs() << endl;
    ss << "BatchSize=" <<train.get_batchsize() << endl;
    ss << "Loss=" << train.get_loss() << endl;
    ss << "KeepBest=" << train.get_keepbest() << endl;
    ss << "ReboostEveryEpochs=" << train.get_reboost_every_epochs() << endl;

	ss << "ClassBalancingWeightLoss=" << train.get_classbalancing() << endl;

    ss << "Optimizer=" << train.get_optimizer() << endl;
    ss << "LearningRate=" << train.get_learningrate() << endl;
    ss << "Decay=" << train.get_decay() << endl;
    ss << "Momentum=" << train.get_momentum() << endl;

    s += ss.str();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void read(const string& s,NetTrain& train)
{
    train.clear();

    train.set_epochs(stoi(find_key(s,"Epochs")));
    train.set_batchsize(stoi(find_key(s,"BatchSize")));
    train.set_loss(find_key(s,"Loss"));
    train.set_keepbest(stoi(find_key(s,"KeepBest"))!=0);
    train.set_reboost_every_epochs(stoi(find_key(s,"ReboostEveryEpochs")));

	if (!find_key(s, "ClassBalancingWeightLoss").empty())
		train.set_classbalancing(stoi(find_key(s, "ClassBalancingWeightLoss")) != 0);
	else
		train.set_classbalancing(true);

    string sOptimizer=find_key(s,"Optimizer");
    float fLearningRate=stof(find_key(s,"LearningRate"));
    float fDecay=stof(find_key(s,"Decay"));
    float fMomentum=stof(find_key(s,"Momentum"));

    train.set_optimizer(sOptimizer);
    train.set_learningrate(fLearningRate);
    train.set_decay(fDecay);
    train.set_momentum(fMomentum);
}
////////////////////////////////////////////////////////////////////////////////////////////////
string find_key(string s,string sKey)
{
    auto i = s.find(sKey+"=");

    if(i==string::npos)
        return "";

    i+=sKey.size()+1;

    auto i2=s.find("\n",i);

    if(i2==string::npos)
        i2=s.size();

    string s2=s.substr(i,i2-i);

    //trim right
    auto i3=s2.find_last_not_of(" \t\r\n");
    if(i3!=string::npos)
        return s2.substr(0,i3+1);
    else
        return s2;
}
////////////////////////////////////////////////////////////////////////////////////////////////
}
