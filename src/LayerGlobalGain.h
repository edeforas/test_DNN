/*
    Copyright (c) 2019, Etienne de Foras and the respective contributors
    All rights reserved.

    Use of this source code is governed by a MIT-style license that can be found
    in the LICENSE.txt file.
*/

#ifndef LayerGlobalGain_
#define LayerGlobalGain_

#include "Layer.h"
#include "Matrix.h"

class LayerGlobalGain : public Layer
{
public:
    LayerGlobalGain(int iInSize,float fGlobalGain);
    virtual ~LayerGlobalGain();

    virtual Layer* clone() const override;

    virtual void init() override;
    virtual void forward(const MatrixFloat& mMatin, MatrixFloat &mMatOut) const override;

    virtual void backpropagation(const MatrixFloat &mInput,const MatrixFloat &mDelta, MatrixFloat &mNewDelta) override;

    float gain() const;
    bool is_learned() const;

    virtual bool has_weight() override;
    virtual MatrixFloat& weights() override;
    virtual MatrixFloat& gradient_weights() override;

private:
    MatrixFloat _weight,_mDeltaWeight;
    bool _bLearnable;
};

#endif