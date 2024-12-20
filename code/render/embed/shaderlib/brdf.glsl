#ifndef BRDF_GLSL
#define BRDF_GLSL

#include "utils.glsl"

#define USE_BRUTEFORCE_IRRADIANCE false         // Samples irradiance from tex_skysphere when enabled.
#define USE_WRAPAROUND_SPECULAR true            // Makes silhouettes more reflective to avoid black pixels.
#define _USE_SPECULAR_AO_ATTENUATION true        // Dampens IBL specular ambient with AO if enabled.
#define USE_NORMAL_VARIATION_TO_ROUGHNESS true  // Increases roughness if normal map has variation and was minified.
#define BOOST_LIGHTING  100.00

uniform float skysphere_mip_count;
uniform sampler2D tex_skycube;
uniform sampler2D tex_skysphere;
uniform sampler2D tex_skyenv;
uniform samplerCube tex_skyprobe;
uniform sampler2D tex_brdf_lut;

uniform bool has_tex_skycube;
uniform bool has_tex_skysphere;
uniform bool has_tex_skyenv;
uniform bool has_tex_skyprobe;

vec3 fresnel_schlick( vec3 H, vec3 V, vec3 F0 )
{
    float cosTheta = clamp( dot( H, V ), 0., 1. );
    return F0 + ( vec3( 1.0 ) - F0 ) * pow( 1. - cosTheta, 5.0 );
}

// A Fresnel term that dampens rough specular reflections.
// https://seblagarde.wordpress.com/2011/08/17/hello-world/
vec3 fresnel_schlick_roughness( vec3 H, vec3 V, vec3 F0, float roughness )
{
    float cosTheta = clamp( dot( H, V ), 0., 1. );
    return F0 + ( max( vec3( 1.0 - roughness ), F0 ) - F0 ) * pow( 1. - cosTheta, 5.0 );
}

float distribution_ggx( vec3 N, vec3 H, float roughness )
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max( 0., dot( N, H ) );
    float factor = NdotH * NdotH * ( a2 - 1. ) + 1.;

    return a2 / ( PI * factor * factor );
}

float geometry_schlick_ggx( vec3 N, vec3 V, float k )
{
    float NdotV = max( 0., dot( N, V ) );
    return NdotV / (NdotV * ( 1. - k ) + k );
}

float geometry_smith( vec3 N, vec3 V, vec3 L, float roughness )
{
#if 1 // original
    float r = roughness + 1.;
    float k = (r * r) / 8.;
#elif 0 // vries
    float a = roughness;
    float k = (a * a) / 2.0;
#elif 0 // vries improved?
    float a = roughness * roughness;
    float k = a / 2.0;
#endif
    return geometry_schlick_ggx( N, V, k ) * geometry_schlick_ggx( N, L, k );
}

vec2 sphere_to_polar( vec3 normal ) {
    normal = normalize( normal );
    return vec2( atan( -normal.x, normal.z ) / PI / 2.0 + 0.5 , acos( normal.y ) / PI  );
}

vec3 sample_sky( vec3 normal )
{
    vec2 polar = sphere_to_polar( normal );
    return texture( tex_skysphere, polar ).rgb;
}

// Takes samples around the hemisphere, converts them to radiances via weighting and
// returns a normalized sum.
vec3 sample_irradiance_slow( vec3 normal, vec3 vertex_tangent )
{
    float delta = 0.10;

    vec3 up = abs( normal.y ) < 0.999 ? vec3( 0., 1., 0. ) : vec3( 0., 0., 1. );
    vec3 tangent_x = normalize( cross( up, normal ) );
    vec3 tangent_y = cross( normal, tangent_x );

    int numIrradianceSamples = 0;

    vec3 irradiance = vec3(0.);

    for ( float phi = 0.; phi < 2. * PI ; phi += delta )
    {
        for ( float theta = 0.; theta < 0.5 * PI; theta += delta )
        {
            vec3 tangent_space = vec3(
                sin( theta ) * cos( phi ),
                sin( theta ) * sin( phi ),
                cos( theta ) );

            vec3 world_space = tangent_space.x * tangent_x + tangent_space.y + tangent_y + tangent_space.z * normal;

            vec3 color = sample_sky( world_space );
            irradiance += color * cos( theta ) * sin( theta );
            numIrradianceSamples++;
        }
    }

    irradiance = PI * irradiance / float( numIrradianceSamples );
    return irradiance;
}

vec3 sample_irradiance_fast( vec3 normal, vec3 vertex_tangent )
{
    // Sample the irradiance map if it exists, otherwise fall back to blurred reflection map.
    vec2 polar = sphere_to_polar( normal );
    if ( has_tex_skyenv )
    {
        return textureLod( tex_skyenv, polar, 0.0 ).rgb;
    }
    else if (has_tex_skysphere)
    {
        return textureLod( tex_skysphere, polar, 0.80 * skysphere_mip_count ).rgb;
    }
    else if (has_tex_skyprobe)
    {
        return textureLod( tex_skyprobe, normal, 0.80 * skysphere_mip_count).rgb;
    }
    else if (has_tex_skycube)
    {
        vec3 color = textureLod( tex_skycube, polar, 0.0 ).rgb;
        color = pow(color, vec3(1.0/2.2));
        return color;
    }
}

vec3 specular_ibl( vec3 V, vec3 N, float roughness, vec3 fresnel, float metallic )
{
    // What we'd like to do here is take a LOT of skybox samples around the reflection
    // vector R according to the BRDF lobe.
    //
    // Unfortunately it's not possible in real time so we use the following UE4 style approximations:
    // 1. Integrate incoming light and BRDF separately ("split sum approximation")
    // 2. Assume V = R = N so that we can just blur the skybox and sample that.
    // 3. Bake the BRDF integral into a lookup texture so that it can be computed in constant time.
    //
    // Here we also simplify approximation #2 by using bilinear mipmaps with a magic formula instead
    // of properly convolving it with a GGX lobe.
    //
    // For details, see Brian Karis, "Real Shading in Unreal Engine 4", 2013.

    vec3 R = 2. * dot( V, N ) * N - V;
    vec2 polar = sphere_to_polar( R );

    // Map roughness from range [0, 1] into a mip LOD [0, skysphere_mip_count].
    // The magic numbers were chosen empirically.

    float mip = 0.9 * skysphere_mip_count * pow(roughness, 0.25);

    vec3 prefiltered = vec3(0.0);
    if (has_tex_skysphere)
    {
        prefiltered = textureLod( tex_skysphere, polar, mip ).rgb;
    }
    else if (has_tex_skyprobe)
    {
        prefiltered = textureLod( tex_skyprobe, R, mip ).rgb;
    }
    else if (has_tex_skycube)
    {
        prefiltered = textureLod( tex_skycube, polar, mip ).rgb;
        prefiltered = pow(prefiltered, vec3(1.0/2.2));
    }

    float NdotV = dot( N, V );
    // dot( N, V ) seems to produce negative values so we can try to stretch it a bit behind the silhouette
    // to avoid black pixels.
    if (USE_WRAPAROUND_SPECULAR)
    {
        NdotV = NdotV * 0.9 + 0.1;
    }

    NdotV = min(0.99, max(0.01, NdotV));

    // A precomputed lookup table contains a scale and a bias term for specular intensity (called "fresnel" here).
    // See equation (8) in Karis' course notes mentioned above.
    vec2 envBRDF = texture( tex_brdf_lut, vec2(NdotV, roughness) ).xy; // (NdotV,1-roughtness) for green top-left (NdotV,roughness) for green bottom-left
    vec3 specular = prefiltered * (fresnel * envBRDF.x + vec3(envBRDF.y));

    return specular;
}
#endif // BRDF_GLSL
