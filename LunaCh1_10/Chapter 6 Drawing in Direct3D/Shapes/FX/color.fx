//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

static float iGlobalTime;
float2 iResolution = float2(300, 300);

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	float globalTime;
};

struct VertexIn
{
	float3 Pos   : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;

	return vout;
}


//-----------------SETTINGS-----------------
//#define TIMES_DETAILED (sin(time*32.0)+1.0)
#define TIMES_DETAILED (1.0+.1*sin(time*PI*1.0))
#define SPIRAL_BLUR_SCALAR (1.0+.1*sin(time*PI*1.0))
//-----------------USEFUL-----------------

#define MOUSE_X (iMouse.x/iResolution.x)
#define MOUSE_Y (iMouse.y/iResolution.y)

#define PI 3.14159265359
#define E 2.7182818284
#define GR 1.61803398875
#define EPS .001

#define time ((saw(float(__LINE__))+1.0)*(iGlobalTime+12345.12345)/PI/2.0)
#define saw(x) (acos(cos(x))/PI)
#define sphereN(uv) (normalize(float3((uv).xy, sqrt(clamp(1.0-length((uv)), 0.0, 1.0)))))
#define rotatePoint(p,n,theta) (p*cos(theta)+cross(n,p)*sin(theta)+n*dot(p,n) *(1.0-cos(theta)))
//-----------------IMAGINARY-----------------

float2 cmul(float2 v1, float2 v2) {
	return float2(v1.x * v2.x - v1.y * v2.y, v1.y * v2.x + v1.x * v2.y);
}

float2 cdiv(float2 v1, float2 v2) {
	return float2(v1.x * v2.x + v1.y * v2.y, v1.y * v2.x - v1.x * v2.y) / dot(v2, v2);
}

//-----------------RENDERING-----------------

static float seedling;

float2 mobius(float2 uv)
{
	float2 a = sin(seedling + float2(time, time*GR / E))*10.0;
	float2 b = sin(seedling + float2(time, time*GR / E))*10.0;
	float2 c = sin(seedling + float2(time, time*GR / E))*10.0;
	float2 d = sin(seedling + float2(time, time*GR / E))*10.0;
	return cdiv(cmul(uv, a) + b, cmul(uv, c) + d);
}

float2 map(float2 uv)
{
	float zoom = (2.5 + 2.0*sin(time));;
	float2 mob = mobius(zoom*(uv*2.0 - 1.0));

	return (mob*2.0*PI);
}

float2 spiral(float2 uv)
{
	return (1.0 - saw(PI*(uv*.5 + .5)));
}

float2 iterate(float2 uv, float2 dxdy, out float magnification)
{
	float2 a = uv + float2(0.0, 0.0);
	float2 b = uv + float2(dxdy.x, 0.0);
	float2 c = uv + float2(dxdy.x, dxdy.y);
	float2 d = uv + float2(0.0, dxdy.y);//((fragCoord.xy + float2(0.0, 1.0)) / iResolution.xy * 2.0 - 1.0) * aspect;

	float2 ma = map(a);
	float2 mb = map(b);
	float2 mc = map(c);
	float2 md = map(d);

	float da = length(mb - ma);
	float db = length(mc - mb);
	float dc = length(md - mc);
	float dd = length(ma - md);

	float stretch = max(max(max(da / dxdy.x, db / dxdy.y), dc / dxdy.x), dd / dxdy.y);

	magnification = stretch;

	return map(uv);
}


float4 PS(VertexOut pin) : SV_Target
{
	iGlobalTime = globalTime;
	float aspect = iResolution.y / iResolution.x;

	float2 uv = pin.PosH / iResolution.xy;




	const int max_i = 1;
	float stretch = 1.0;
	float ifs = 1.0;
	float ifsSqrt = 1.0;
	float depth = 0.0;
	float magnification;

	for (int i = 0; i < max_i; i++)
	{
		seedling += frac(float(i)*123456.123456);

		float2 next = iterate(uv, .5 / iResolution.xy, magnification);

		float weight = ifs;

		ifs *= smoothstep(0.0, 1.0 / TIMES_DETAILED, sqrt(1.0 / (1.0 + magnification)));

		ifsSqrt = sqrt(ifs);

		uv = next*weight + uv*(1.0 - weight);

		float delta = sphereN(uv*2.0 - 1.0).z*ifsSqrt;
		depth += (1.0 - delta)*ifs;

		if (ifs != 0.0)
		{
			uv = spiral(uv*2.0 - 1.0);//*clamp(pow(delta, SPIRAL_BLUR_SCALAR)*2.0, 0.0, 1.0);
		}

		ifs = ifsSqrt;

		//if(mod(iGlobalTime, float(max_i))-float(i) < 0.0) break;
	}


	float4 fragColor = float4(uv, 0.0, 1.0);

	//depth /= float(max_i);
	float shift = time;

	float stripes = depth*PI*5.0*ifs*(sin(time) + 2.0);
	float black = saw(atan2(uv.y*2.0 - 1.0, uv.x*2.0 - 1.0));//sqrt(smoothstep(0.0, .25, saw(stripes)));
	float white = smoothstep(0.75, 1.0, saw(stripes));


	float3 final = (
		float3(saw(depth*PI*2.0 + shift),
			saw(4.0*PI / 3.0 + depth*PI*2.0 + shift),
			saw(2.0*PI / 3.0 + depth*PI*2.0 + shift)
			)
		)*black
		+ white;

	fragColor = float4(float3(ifs, ifs, ifs), 1.0);

	fragColor = float4(saw((depth)), saw((depth)), saw((depth)), saw((depth)));
	fragColor = float4(final, 1.0);

	return fragColor;
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

