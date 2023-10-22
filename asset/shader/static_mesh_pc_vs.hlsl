#include "static_mesh_pc.hlsli"

VS_OUT main(VS_IN vin)
{
    return static_mesh_pc_vs(vin);
}
