#define SLANG
#include "../../source/Shaders/ShaderCommon.h"

#define fov 90.
#define itfov 1.0 / (tan(3.141592654 * fov / (180. * 2.)))

#define CAMERA_FOV 1.04

#define EPS 0.001
#define MAX_DIST 10.
#define MAX_STEPS 200

static const float W_0 = 30.0;

[[vk::push_constant]]
NeuralSdfConstants ctx;


float forward(float3 p) {

    float activations[512];

    // zero out;
    for (int i = 0; i < 2 * ctx.layerSize; ++i) {
        activations[i] = 0.0f;
    }

    activations[0] = p.x;
    activations[1] = p.y;
    activations[2] = p.z;
    int half_part = 0;

    // ===== Layer 0 =====
    // z = Wx + b
    for (int i = 0; i < ctx.layerSize; ++i) {
        for (int j = 0; j < 3; ++j) {
            activations[ctx.layerSize * ((half_part + 1) % 2) + i] += w_b[i * 3 + j] * activations[ctx.layerSize * ((half_part) % 2) + j];
        }
    }

    // + b
    for (int i = 0; i < ctx.layerSize; ++i) {
        activations[ctx.layerSize * ((half_part + 1) % 2) + i] += w_b[ctx.layerSize * 3 + i];
    }

    // sin
    for (int i = 0; i < ctx.layerSize; ++i) {
        activations[ctx.layerSize * ((half_part + 1) % 2) + i] = sin(W_0 * activations[ctx.layerSize * ((half_part + 1) % 2) + i]);
    }

    // ===== Hidden layers =====
    int w_b_pos = 4 * ctx.layerSize;
    for (int l = 0; l < ctx.numLayers; ++l) {
        // zero
        for (int i = 0; i < ctx.layerSize; ++i) {
            activations[(ctx.layerSize * (half_part % 2)) + i] = 0.0f;
        }
        // z = Wx + b
        for (int i = 0; i < ctx.layerSize; ++i) {
            for (int j = 0; j < ctx.layerSize; ++j) {
                activations[(ctx.layerSize * (half_part % 2)) + i] += activations[(ctx.layerSize * ((half_part + 1) % 2)) + j] * w_b[w_b_pos + i * ctx.layerSize + j];
            }
        }
        // + b
        for (int i = 0; i < ctx.layerSize; ++i) {
            activations[(ctx.layerSize * (half_part % 2)) + i] += w_b[w_b_pos + ctx.layerSize * ctx.layerSize + i];
        }
        // sin
        for (int i = 0; i < ctx.layerSize; ++i) {
            activations[(ctx.layerSize * (half_part % 2)) + i] = sin(W_0 * activations[(ctx.layerSize * (half_part % 2)) + i]);
        }
        w_b_pos += (ctx.layerSize + 1) * ctx.layerSize;
        half_part = (half_part + 1) % 2;
    }

    // ===== Output layer =====
    // zero
    activations[(ctx.layerSize * (half_part % 2))] = 0.0f;
    // z = Wx + b
    for (int j = 0; j < ctx.layerSize; ++j) {
        activations[ctx.layerSize * (half_part % 2)] += activations[ctx.layerSize * ((half_part + 1) % 2) + j] * w_b[w_b_pos + j];
    }
    // + b
    activations[ctx.layerSize * (half_part % 2)] += w_b[w_b_pos + ctx.layerSize];

    return activations[ctx.layerSize * (half_part % 2)];
}

float sdSphere(float3 p, float s) {
    return length(p) - s;
}

float2 RayBoxIntersection(float3 ray_pos, float3 ray_dir, float3 boxMin, float3 boxMax) {
    ray_dir.x = 1.0 / ray_dir.x; // may precompute if intersect many boxes
    ray_dir.y = 1.0 / ray_dir.y; // may precompute if intersect many boxes
    ray_dir.z = 1.0 / ray_dir.z; // may precompute if intersect many boxes

    float lo = ray_dir.x * (boxMin.x - ray_pos.x);
    float hi = ray_dir.x * (boxMax.x - ray_pos.x);

    float tmin = min(lo, hi);

    float tmax = max(lo, hi);

    float lo1 = ray_dir.y * (boxMin.y - ray_pos.y);
    float hi1 = ray_dir.y * (boxMax.y - ray_pos.y);

    tmin = max(tmin, min(lo1, hi1));
    tmax = min(tmax, max(lo1, hi1));

    float lo2 = ray_dir.z * (boxMin.z - ray_pos.z);
    float hi2 = ray_dir.z * (boxMax.z - ray_pos.z);

    tmin = max(tmin, min(lo2, hi2));
    tmax = min(tmax, max(lo2, hi2));

    return float2(tmin, tmax);
}

float3x3 lookAt(float3 cameraPos, float3 lookAtPoint, float3 up) {
    float3 cd = normalize(lookAtPoint - cameraPos); // camera direction
    float3 cr = normalize(cross(up, cd));           // camera right
    float3 cu = normalize(cross(cd, cr));           // camera up

    return float3x3(-cr, cu, -cd);
}

//////////////////////////////////////////////////////////////////////////////////
float map(float3 p) {
    float model_dist;

    model_dist = forward(p);

    return model_dist;
}

float rayMarch(float3 ro, float3 rd, float2 tNearAndFar) {
    float hit;
    float pos = tNearAndFar.x;
    for (int i = 0; i < MAX_STEPS; i++) {
        float3 p = ro + pos * rd;
        hit = map(p);
        pos += hit;
        if (abs(hit) < 0.0001)
            break;
        if (pos > tNearAndFar.y) {
            break;
        }
    }
    return pos;
}

float3 getNormal(float3 p) {
    float2 e = float2(EPS * 5, 0.0);
    float3 normal = float3(map(p)) - float3(map(p - e.xyy), map(p - e.yxy), map(p - e.yyx));
    return normalize(normal);
}

float3 getLight(float3 p, float3 rd, float3 color) {
    // Источник света (позиция)
    float3 lightSource = float3(20.0, 40.0, 30.0);

    // Вектор от точки к свету - L
    // float3 L = normalize(lightSource - p);
    float3 L = normalize(float3(0.707, 0.0, 0.707));
    // Нормаль точки p - N
    float3 N = getNormal(p);
    // Вектор из p в камеру - V
    float3 V = -rd;
    // Отраженный свет - R
    float3 R = reflect(-L, N);

    // return N;
    //  float3 specColor = float3(0.5);
    //  float3 specular = specColor * pow(clamp(dot(R, V), 0.0, 1.0), 10.0);
    float3 diffuse = color * clamp(dot(L, N), 0.0, 1.0);
    // float3 diffuse = color * clamp(dot(L, N), 0.0, 1.0);
    float3 ambient = color * 0.05;
    float3 fresnel = 0.10 * color * pow(1.0 + dot(rd, N), 3.0);
    // return diffuse;
    // return ambient;
    // return fresnel;
    return diffuse + ambient + fresnel;
}
float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

[shader("compute")]
[numthreads(WORKGROUP_SIZE, WORKGROUP_SIZE, 1)]
void main(uint3 tid: SV_DispatchThreadID) {
    if (tid.x < ctx.width && tid.y < ctx.height) {
		float2 iResolution = float2(float(ctx.width), float(ctx.height));
		float2 uv = ((tid.xy + 0.5) - 0.5 * iResolution.xy) / iResolution.y;
		uv.y = -uv.y;

		float3 up = float3(0.0, 1.0, 0.0);
		float3 lookAtPoint = float3(0., 0., 0.);
		float3 ro = float3(0., 0., 3.0);

		// float3 rd = lookAt(ro, lookAtPoint, up) * normalize(float3(uv, -1)); // ray direction

		float3x3 lookAt = lookAt(ro, lookAtPoint, up);
		float3 rd = mul(lookAt, normalize(float3(uv, -CAMERA_FOV)));

		float3 col = float3(0.0, 0.0, 0.0);

		float2 tNearAndFar = RayBoxIntersection(ro, rd, float3(-1.0, -1.0, -1.0), float3(1.0, 1.0, 1.0));
		if (tNearAndFar.x < tNearAndFar.y && tNearAndFar.x > 0.0) {
			float dist = rayMarch(ro, rd, tNearAndFar);
			if (dist < tNearAndFar.y) {
				col = getLight(ro + rd * dist, rd, float3(0.951, 0.9686, 1.0));
			} else
				col = float3(0.49, 0.73, 0.03);
		}

		uint pos = tid.y * ctx.width + tid.x;

		float4 color = float4(col, 1.0);

		color = clamp(color, 0.0, 1.0);
		outputImage[pos].value = color;
    }
}
