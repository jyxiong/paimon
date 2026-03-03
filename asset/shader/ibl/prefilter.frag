#version 460 core

#include <common/brdf.glsl>
#include <common/sampling.glsl>

layout(location = 0) in vec3 v_localPos;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_environmentMap;

layout(binding = 1, std140) uniform PrefilteredParams {
    float u_roughness;
    float u_envMapResolution;
};

void main() {
    vec3 N = normalize(v_localPos);
    vec3 R = N;
    vec3 V = R;
    
    // Debug: test if we can sample the environment map at all
    vec3 testSample = textureLod(u_environmentMap, N, 0.0).rgb;
    if (u_roughness == 0.0) {
        // For mip 0, just return direct sample (no prefiltering needed)
        FragColor = vec4(testSample, 1.0);
        return;
    }
    
    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);
    
    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0) {
            // Sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H, u_roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;
            
            float resolution = u_envMapResolution; // Resolution of source cubemap (per face)
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            
            float mipLevel = u_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            
            prefilteredColor += textureLod(u_environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    
    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}
