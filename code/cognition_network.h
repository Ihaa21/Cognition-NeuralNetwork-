#if !defined(COGNITION_NETWORK_H)

struct hidden_layer
{
    f32 DropoutPercent;
    
    mat Synapse;
    mat Bias;
    mat Outputs;

#if BACKPROP_MOMENTUM
    mat MomentumSyn;
    mat MomentumBias;
#endif
};

inline mat Forward(hidden_layer* Layer, mat Inputs);

#define COGNITION_NETWORK_H
#endif
