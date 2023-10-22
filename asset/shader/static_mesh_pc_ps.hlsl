#include "static_mesh_pc.hlsli"

float4 main(VS_OUT pin) : SV_Target
{
    return static_mesh_pc_ps(pin);
}
