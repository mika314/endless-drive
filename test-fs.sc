$input v_norm, v_frag_pos, v_uv

#include <bgfx_shader.sh>

SAMPLER2D(baseColorTex, 0);
SAMPLER2D(metallicTex, 1);
SAMPLER2D(roughnessTex, 2);
uniform vec4 settings;
uniform vec4 baseColor;
uniform vec4 metallic;
uniform vec4 roughness;
uniform vec4 camPos;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness)
{
  float a2 = roughness * roughness * roughness * roughness;
  float NdotH = max(dot(N, H), 0.0);
  float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
  return a2 / (PI * denom * denom);
}

float geometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;
  return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  return geometrySchlickGGX(max(dot(N, L), 0.0), roughness) *
         geometrySchlickGGX(max(dot(N, V), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// https://www.shadertoy.com/view/4d2XWV by Inigo Quilez
float sphereIntersect(vec3 ro, vec3 rd, vec4 sph)
{
  vec3 oc = ro - sph.xyz;
  float b = dot(oc, rd);
  float c = dot(oc, oc) - sph.w * sph.w;
  float h = b * b - c;
  if (h < 0.0)
    return -1.0;
  return -b - sqrt(h);
}

void main()
{
  vec3 lBaseColor = settings.x > .5f ? texture2D(baseColorTex, v_uv).rgb : baseColor.rgb;
  float lMetallic = settings.y > .5f ? texture2D(metallicTex, v_uv).b : metallic.r;
  float lRoughness = settings.z > .5f ? texture2D(roughnessTex, v_uv).g : roughness.r;

  vec3 lightPos = vec3(-1.25, 1.0, 2);
  vec3 lightColor = vec3(4.0);
  vec3 totColor = vec3(0.0);

  vec3 N = normalize(v_norm.xyz);
  vec3 V = normalize(camPos.xyz - v_frag_pos.xyz);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, lBaseColor, lMetallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // calculate per-light radiance
  vec3 L = normalize(lightPos - v_frag_pos.xyz);
  vec3 H = normalize(V + L);
  float distance = length(lightPos - v_frag_pos.xyz);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightColor * attenuation;

  // cook-torrance brdf
  float NDF = distributionGGX(N, H, lRoughness);
  float G = geometrySmith(N, V, L, lRoughness);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - lMetallic;

  vec3 numerator = NDF * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
  vec3 specular = numerator / denominator;

  // add to outgoing radiance Lo
  float NdotL = max(dot(N, L), 0.0);
  Lo += (kD * lBaseColor / PI + specular) * radiance * NdotL;

  vec3 ambient = vec3(0.03) * lBaseColor; // * ao;
  vec3 color = ambient + Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));

  gl_FragColor = vec4(color, 1.0);
}
