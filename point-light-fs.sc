$input v_uv

#include <bgfx_shader.sh>

uniform mat4 mtx;
uniform vec4 camPos;
uniform vec4 lightPos;
uniform vec4 lightColor;

SAMPLER2D(normals, 0);
SAMPLER2D(metallicRoughness, 1);
SAMPLER2D(depth, 2);

const float PI = 3.141592654;

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

vec3 decodeNormalUint(vec4 _encodedNormal)
{
  return normalize(_encodedNormal.xyz * 2.0 - 1.0);
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

float toClipSpaceDepth(float _depthTextureZ)
{
#if BGFX_SHADER_LANGUAGE_GLSL
  return _depthTextureZ * 2.0 - 1.0;
#else
  return _depthTextureZ;
#endif // BGFX_SHADER_LANGUAGE_GLSL
}

vec3 clipToWorld(mat4 _invViewProj, vec3 _clipPos)
{
  vec4 wpos = mul(_invViewProj, vec4(_clipPos, 1.0));
  return wpos.xyz / wpos.w;
}

void main()
{
  vec3 lBaseColor = vec3(1.f);
  float lMetallic = texture2D(metallicRoughness, v_uv).b;
  float lRoughness = texture2D(metallicRoughness, v_uv).g;
  vec3 normal = decodeNormalUint(texture2D(normals, v_uv));

  vec3 totColor = vec3(0.0);

  // Reconstruct world position from depth
  float deviceDepth = texture2D(depth, v_uv).x;
  float depth = toClipSpaceDepth(deviceDepth);

  vec3 clip = vec3(v_uv * 2.0 - 1.0, depth);
#if !BGFX_SHADER_LANGUAGE_GLSL
  clip.y = -clip.y;
#endif
  vec3 worldPos = clipToWorld(mtx, clip);

  vec3 N = normal;
  vec3 V = normalize(camPos.xyz - worldPos);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, lBaseColor, lMetallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // calculate per-light radiance
  vec3 L = normalize(lightPos.xyz - worldPos);
  vec3 H = normalize(V + L);
  float distance = length(lightPos.xyz - worldPos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightColor.rgb * attenuation;

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

  vec3 color = Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));

  gl_FragColor = vec4(color, 1.0);
}
