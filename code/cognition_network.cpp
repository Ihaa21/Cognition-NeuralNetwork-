inline hidden_layer HiddenLayer(u32 In, u32 Out, f32 DropoutPercent = 0.0f)
{
    hidden_layer Result = {};
    Result.Synapse = Rand(In, Out);
    Result.Bias = Rand(1, Out);
    Result.DropoutPercent = DropoutPercent;

#if BACKPROP_MOMENTUM
    Result.MomentumSyn = Clear(T2(In, Out), 0.0f);
    Result.MomentumBias = Clear(T2(1, Out), 0.0f);
#endif
    
    return Result;
}

inline mat Forward(hidden_layer* Layer, mat Inputs)
{
    Layer->Outputs = Sigmoid((Inputs*Layer->Synapse) + Layer->Bias);

    if (Layer->DropoutPercent)
    {
        Layer->Outputs = ElemMul(Layer->Outputs, RandDropout(Layer->Outputs.Rows, Layer->Outputs.Cols, Layer->DropoutPercent));
    }
    
    return Layer->Outputs;
}

inline mat BackProp(hidden_layer* Layer, mat Error, mat PrevOutputs)
{
#if BACKPROP_MOMENTUM
    mat Delta = ElemMul(Error, SigmoidDeriv(Layer->Outputs));
    mat NewError = Delta*Transpose(Layer->Synapse);

    mat MovementSyn = LearningRate*Transpose(PrevOutputs)*Delta + MomentumRate*Layer->MomentumSyn;
    Layer->Synapse += MovementSyn;
    mat MovementBias = LearningRate*Delta + MomentumRate*Layer->MomentumBias;
    Layer->Bias += MovementBias;

    Copy(&Layer->MomentumSyn, &MovementSyn);
    Copy(&Layer->MomentumBias, &MovementBias);
#else
    mat Delta = ElemMul(Error, SigmoidDeriv(Layer->Outputs));
    mat NewError = Delta*Transpose(Layer->Synapse);
    Layer->Synapse += LearningRate*Transpose(PrevOutputs)*Delta;
    Layer->Bias += LearningRate*Delta;
#endif
    
    return NewError;
}
