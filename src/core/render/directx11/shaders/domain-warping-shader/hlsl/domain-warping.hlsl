float random(in float2 uv) { return frac(sin(dot(uv.xy, float2(12.9898f, 78.233f))) * 43758.5453123f); }
float noise(in float2 uv) {
    float2 i = floor(uv);
    float2 f = frac(uv);
    float a = random(i);
    float b = random(i + float2(1.f, 0.f));
    float c = random(i + float2(0.f, 1.f));
    float d = random(i + float2(1.f, 1.f));
    float2 u = f * f * (3.f - 2.f * f);
    return lerp(a, b, u.x) + (c - a) * u.y * (1.f - u.x) + (d - b) * u.x * u.y;
    
}
float fbm(in float2 uv, int lp) {
    float v = 0.f;
    float f = 1.f;
    float a = 0.5f;
    for(int i = 0; i != lp; i++) {
        float2x2 R = float2x2(cos(.5f), sin(.5f), -sin(.5f), cos(.5f));
        v += noise(uv * f) * a;
        uv = mul(R, uv) * 2.f + 100.f;
        f *= 2.f;
        a *= 0.5f;
    }
    return v;
}

float2 fbm_vec(float2 uv_x, float2 uv_y, int2 detailing) {
    return float2(fbm(uv_x, detailing.x), fbm(uv_y, detailing.y));
}

cbuffer constant_buffer : register(b0) {
	int4 first_color, second_color;
	float2 speed, x_offset, y_offset;
	int2 detailing;
	float aspect, time;
	int mix_detailing;
};

struct ps_input_t {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(ps_input_t input) : SV_Target {
    float2 uv = input.uv * aspect;
    float2 r_uvs = uv + 1.f * fbm_vec(uv, uv + float2(1.f, 1.f), detailing);
    float2 r = fbm_vec(r_uvs + x_offset + speed.x * time, r_uvs + y_offset + speed.y * time, int2(2, 4));
    
    return lerp(first_color / 255.f, second_color / 255.f, fbm(uv + r, mix_detailing)) * input.color;
}