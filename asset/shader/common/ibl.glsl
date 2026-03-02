// IBL (Image-Based Lighting) ambient contribution
// Split-sum approximation (Karis 2013)

#include <common/brdf.glsl>

// Calculate IBL ambient contribution (diffuse + specular)
// irradianceMap  : convolved diffuse irradiance cubemap
// prefilteredMap : prefiltered specular cubemap (mips = roughness levels)
// brdfLUT        : precomputed split-sum BRDF LUT (R=scale, G=bias)
vec3 calculateIBLLighting(
    vec3 N,
    vec3 V,
    vec3 albedo,
    float metallic,
    float roughness,
    float ao,
    samplerCube irradianceMap,
    samplerCube prefilteredMap,
    sampler2D brdfLUT
)
{
    float NdotV = max(dot(N, V), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F   = fresnelSchlick(NdotV, F0, roughness);

    vec3 kD = (1.0 - F) * (1.0 - metallic);

    // Diffuse IBL
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = kD * irradiance * albedo;

    // Specular IBL (split-sum)
    vec3  R                = reflect(-V, N);
    const float MAX_LOD    = 4.0; // must match prefilter mip count
    vec3  prefilteredColor = textureLod(prefilteredMap, R, roughness * MAX_LOD).rgb;
    vec2  brdf             = texture(brdfLUT, vec2(NdotV, roughness)).rg;
    vec3  specular         = prefilteredColor * (F * brdf.x + brdf.y);

    return (diffuse + specular) * ao;
}
