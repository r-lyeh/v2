#ifndef LIGHT_GLSL
#define LIGHT_GLSL

#ifdef FS_PASS
#include "brdf.glsl"
#include "parallax.glsl"
#endif

struct light_t {
    vec4 diffuse;
    vec4 pos;
    vec4 dir;
    float power;
    float innerCone;
    float outerCone;
    float shadow_bias;
    float normal_bias;
    float min_variance;
    float variance_transition;
    float shadow_softness;
    float penumbra_size;
    int type;
    bool processed_shadows;
    bool hard_shadows;
};

layout(std140) uniform LightBuffer {
    light_t lights[MAX_LIGHTS];
} lightBuffer;

const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;

uniform int u_num_lights;

#ifdef FS_PASS
#include "shadowmap.glsl"
#endif

struct material_t {
    vec3 albedo;
    vec3 normal;
    vec3 F0;
    float roughness;
    float metallic;
    float alpha;
    vec3 tangent_view_dir;
    vec2 texcoords;
};

vec3 shading_light(light_t l, material_t m) {
    vec3 lightDir;
    float attenuation = 1.0;
    float distance = 0.0;
    vec3 radiance = vec3(0.0);

    if (l.type == LIGHT_DIRECTIONAL) {
        lightDir = normalize(-l.dir.xyz);
        distance = 0.0;
        radiance = l.diffuse.rgb * l.power / PI / 10.0;
    } else if (l.type == LIGHT_POINT || l.type == LIGHT_SPOT) {
        vec3 toLight = l.pos.xyz - v_position_ws;
        lightDir = normalize(toLight);
        distance = length(toLight);
        
        attenuation = 1.0 / max(distance * distance, 0.0001);

        if (l.type == LIGHT_SPOT) {
            float angle = dot(l.dir.xyz, -lightDir);
            if (angle > l.innerCone) {
                float intensity = (angle-l.innerCone)/(l.outerCone-l.innerCone);
                attenuation *= clamp(intensity, 0.0, 1.0);
            } else {
                attenuation = 0.0;
            }
        }

        float luminance = l.power * attenuation;
        if (luminance < 0.01) {
            return vec3(0.0);
        }

        radiance = l.diffuse.rgb * luminance / PI;
    }

#ifdef FS_PASS
    vec3 V = normalize( v_to_camera );
    vec3 N = m.normal;
    vec3 L = normalize( lightDir );
    vec3 H = normalize( V + L );

    vec3 F = fresnel_schlick( H, V, m.F0 );
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - m.metallic;

    // Premultiplied alpha applied to the diffuse component only
    kD *= m.alpha;

    float D = distribution_ggx( N, H, m.roughness );
    float G = geometry_smith( N, V, L, m.roughness );

    vec3 num = D * F * G;
    float denom = 4. * max( 0., dot( N, V ) ) * max( 0., dot( N, L ) ) + 0.0001;

    vec3 specular = kS * (num / max( 0.001, denom ));

    float NdotL = max( 0., dot( N, L ) );

    float self_shadow = 0.0;
    if (map_parallax.has_tex) {
        vec3 ldir = normalize(v_tbn * lightDir);
        self_shadow = parallax_shadowing(N, ldir, m.texcoords);
    }

    return ( pow((1.0 - self_shadow), parallax_shadow_power) * kD * ( m.albedo / PI ) + specular ) * radiance * NdotL;
#else
    vec3 n = normalize(v_normal_ws);

    float diffuse = max(dot(n, lightDir), 0.0);

    vec3 halfVec = normalize(lightDir + u_cam_dir);
    float specular = pow(max(dot(n, halfVec), 0.0), l.power);

    return (diffuse*attenuation*l.diffuse.rgb);
#endif
}

vec3 lighting(material_t m) {
    vec3 lit = vec3(0,0,0);
#if USE_LIGHTING
    for (int i=0; i<u_num_lights; i++) {
        vec3 lit_contrib = shading_light(lightBuffer.lights[i], m);

        vec3 result = lit_contrib;
#ifdef FS_PASS
#if USE_SHADOWS
        if (result != vec3(0))
            result *= shadowing(i).xyz;
#endif
#endif
        lit += result;
    }
#endif
    return lit;
}

#endif
