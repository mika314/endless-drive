$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(deferrdBaseColor, 0);
SAMPLER2D(lightBuffer, 1);
SAMPLER2D(depth, 2);
SAMPLER2D(emissionBuffer, 3);
SAMPLER2D(normalsCombine, 4);
uniform vec4 ambient;
uniform mat4 mtx;
uniform mat4 projViewCombine;
uniform vec4 time;
static const float ssaoRadius = .25f;
static const float bias = 0.01f;

vec3 decodeNormalUint(vec4 _encodedNormal)
{
  return normalize(_encodedNormal.xyz * 2.0 - 1.0);
}

uint PCGHash(inout uint rng_state)
{
  rng_state = rng_state * 747796405U + 2891336453U;
  uint state = rng_state;
  uint word = ((state >> ((state >> 28U) + 4U)) ^ state) * 277803737U;
  return (word >> 22U) ^ word;
}

vec3 getOffset(vec3 N, inout uint rng_state)
{
  float u = PCGHash(rng_state) / float(0xffffffffU);
  float v = PCGHash(rng_state) / float(0xffffffffU);
  float w = PCGHash(rng_state) / float(0xffffffffU);
  float r = ssaoRadius * u;
  float phi = 2 * 3.141592654f * v;
  float theta = acos(w);

  // 1. Create a helper vector to find a tangent
  // We use an arbitrary axis that isn't the normal
  vec3 helper = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
  vec3 T = normalize(cross(helper, N));
  vec3 B = cross(N, T);

  // 2. This IS your matrix.
  // It maps the 'Z-up' hemisphere to your 'Normal-up' hemisphere.
  mat3 transitionMatrix = mtxFromCols(T, B, N);

  // 3. Calculate the local offset in "Normal-Space"
  // Using your specific variables
  float sinTheta = sin(theta);
  vec3 localOffset = vec3(r * sinTheta * cos(phi), r * sinTheta * sin(phi), r * cos(theta) + bias);

  // 4. Multiply Matrix by Local Offset to get World/View Space Offset
  return mul(transitionMatrix, localOffset);
}

float toClipSpaceDepth(float _depthTextureZ)
{
#if BGFX_SHADER_LANGUAGE_GLSL
  return _depthTextureZ * 2.0 - 1.0;
#else
  return _depthTextureZ;
#endif // BGFX_SHADER_LANGUAGE_GLSL
}

#if BGFX_SHADER_LANGUAGE_GLSL
float getDepth(vec2 uv)
{
  float deviceDepth = texture2D(depth, uv).x;
  return toClipSpaceDepth(deviceDepth);
}
#else
float getDepth(vec2 uv)
{
  float deviceDepth = texture2D(depth, vec2(uv.x, -uv.y)).x;
  return toClipSpaceDepth(deviceDepth);
}
#endif

vec3 clipToWorld(mat4 _invViewProj, vec3 _clipPos)
{
  vec4 wpos = mul(_invViewProj, vec4(_clipPos, 1.0));
  return wpos.xyz / wpos.w;
}

void main()
{
  uint rng_state = uint(v_uv.y * 5000.f * 5000.f + v_uv.x * 5000.f + time.x);

  vec4 base = texture2D(deferrdBaseColor, v_uv);
  vec4 light = texture2D(lightBuffer, v_uv);
  vec4 emission = texture2D(emissionBuffer, v_uv);

  // Reconstruct world position from depth
  float deviceDepth = texture2D(depth, v_uv).x;
  float depth = toClipSpaceDepth(deviceDepth);

  vec3 clip = vec3(v_uv * 2.0 - 1.0, depth);
#if !BGFX_SHADER_LANGUAGE_GLSL
  clip.y = -clip.y;
#endif

  const vec3 worldPos = clipToWorld(mtx, clip);

#if 0
  vec4 tmp = mul(projViewCombine, vec4(worldPos, 1.f));
  vec3 backToScreen = tmp.xyz / tmp.w;
  float k = .0025f;
  gl_FragColor = vec4(1.f / k * (clip - backToScreen + vec3(k / 2.f, k / 2.f, k / 2.f)), 1.f);
  return;
#endif

  vec3 norm = decodeNormalUint(texture2D(normalsCombine, v_uv));
#if 0
  gl_FragColor = vec4(0.5 * (norm + vec3_splat(1.f)), 1.f);
  return;
#endif

  float occlusion = 0.0f;
  const int Samples = 16;

#if 0
  gl_FragColor = vec4(0.5 * (getOffset(norm, rng_state) + vec3_splat(1.f)), 1.f);
  return;
#endif

  for (int i = 0; i < Samples; ++i)
  {
    vec3 worldSpaceOffset = getOffset(norm, rng_state);
    vec3 worldOffseted = worldPos + worldSpaceOffset;
    vec4 screenOffseted = mul(projViewCombine, vec4(worldOffseted, 1.f));
    screenOffseted = vec4(screenOffseted.xyz / screenOffseted.w, 1.f);

    float dz = screenOffseted.z - getDepth((screenOffseted.xy + 1.f) / 2.f);
    float th = max(0.0001f, .125f * length(screenOffseted.xyz - clip));
    float intensity = smoothstep(0.0f, th * .1f, dz) * (1.f - smoothstep(th * .9f, th, dz));
    occlusion += intensity;
  }
  occlusion = 1.f - occlusion / Samples;

  // gl_FragColor =
  //   .5f * (projViewCombine * vec4(vec3(decodeNormalUint(texture2D(normalsCombine, v_uv))), 0.0f) +
  //          vec4(1.f, 1.f, 1.f, 0.0f));

  // gl_FragColor = vec4(v_uv, 0.f, 1.f);

  // gl_FragColor = (screenOffseted + 1.0) / 2.0;

  // gl_FragColor = light + ambient * occlusion;

  gl_FragColor = (base * (light + ambient * occlusion) + emission) * (1 - pow(clip.z, 150.f)) +
                 vec4_splat(0.1f) * pow(clip.z, 150.f);
}
