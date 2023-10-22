struct VS_IN
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VS_OUT static_mesh_pc_vs(VS_IN vin)
{
    VS_OUT vout;
    vout.position = float4(vin.position, 1.0f);
    vout.color = vin.color;
    return vout;
}

float4 static_mesh_pc_ps(VS_OUT pin)
{
    return pin.color;
}
