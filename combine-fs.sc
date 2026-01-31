$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(deferrdBaseColor, 0);
SAMPLER2D(lightBuffer, 1);
SAMPLER2D(depth, 2);
SAMPLER2D(emissionBuffer, 3);
SAMPLER2D(normalsCombine, 4);
uniform vec4 ambient;

#define KERNEL_SIZE 64
const float u_ssaoRadius = 0.005;
const float u_ssaoBias = 0.;

vec3 get_ssao_kernel_sample(int index)
{
  if (index == 0)
    return vec3(0.057390, 0.449755, 0.891285);
  if (index == 1)
    return vec3(0.407981, -0.669666, 0.620023);
  if (index == 2)
    return vec3(-0.354148, -0.916892, 0.198305);
  if (index == 3)
    return vec3(0.850785, 0.490714, 0.187313);
  if (index == 4)
    return vec3(-0.061765, 0.655883, 0.752391);
  if (index == 5)
    return vec3(0.126487, 0.985955, 0.113038);
  if (index == 6)
    return vec3(0.672023, 0.738153, 0.024222);
  if (index == 7)
    return vec3(-0.957591, 0.287232, 0.043743);
  if (index == 8)
    return vec3(0.128704, 0.991666, 0.007609);
  if (index == 9)
    return vec3(-0.930965, 0.364955, 0.008436);
  if (index == 10)
    return vec3(-0.435728, -0.899732, 0.015099);
  if (index == 11)
    return vec3(0.984021, -0.174154, 0.038166);
  if (index == 12)
    return vec3(0.793744, -0.608035, 0.038507);
  if (index == 13)
    return vec3(-0.028905, -0.999201, 0.027042);
  if (index == 14)
    return vec3(-0.640951, 0.767475, 0.004453);
  if (index == 15)
    return vec3(0.999863, -0.016599, 0.000302);
  return vec3(0.0, 0.0, 0.0);
}

vec3 decodeNormalUint(vec4 _encodedNormal)
{
  return normalize(_encodedNormal.xyz * 2.0 - 1.0);
}

uint rng_state;

uint PCGHash()
{
  rng_state = rng_state * 747796405U + 2891336453U;
  uint state = rng_state;
  uint word = ((state >> ((state >> 28U) + 4U)) ^ state) * 277803737U;
  return (word >> 22U) ^ word;
}

float toClipSpaceDepth(float _depthTextureZ)
{
#if BGFX_SHADER_LANGUAGE_GLSL
  return _depthTextureZ * 2.0 - 1.0;
#else
  return _depthTextureZ;
#endif // BGFX_SHADER_LANGUAGE_GLSL
}

float getDepth(vec2 uv)
{
  float deviceDepth = texture2D(depth, uv).x;
  return toClipSpaceDepth(deviceDepth);
}

void main()
{
  rng_state = uint(v_uv.y * 5000.f * 5000.f + v_uv.x * 5000.f);

  vec4 base = texture2D(deferrdBaseColor, v_uv);
  vec4 light = texture2D(lightBuffer, v_uv);
  vec4 emission = texture2D(emissionBuffer, v_uv);

  float occlusion = 0.0;
  float z = getDepth(v_uv);
  for (int i = 0; i < KERNEL_SIZE; ++i)
  {
    vec2 offset =
      vec2(PCGHash() / float(0xFFFFFFFFU) - .5, PCGHash() / float(0xFFFFFFFFU) - .5) * u_ssaoRadius;
    if (z < getDepth(v_uv + offset))
      ++occlusion;
  }

  occlusion /= KERNEL_SIZE;

  gl_FragColor = base * (light + ambient * occlusion) + emission;

  /*




    vec3 normal_world = decodeNormalUint(texture2D(normalsCombine, v_uv));

    float z = getDepth(v_uv);
    vec4 clip_pos = vec4(v_uv.x * 2.0 - 1.0, (1.0 - v_uv.y) * 2.0 - 1.0, z, 1.0);
    vec4 view_pos_h = mul(u_invProj, clip_pos);
    vec3 view_pos = view_pos_h.xyz / view_pos_h.w;

    vec3 view_normal = normalize(mul(u_view, vec4(normal_world, 0.0)).xyz);

    vec3 random_vec = normalize(
      vec3(PCGHash() / float(0xFFFFFFFFU) * 2.0 - 1.0, PCGHash() / float(0xFFFFFFFFU) * 2.0 - 1.0, 0.0));
    vec3 tangent = normalize(random_vec - view_normal * dot(random_vec, view_normal));
    vec3 bitangent = cross(view_normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, view_normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; ++i)
    {
      vec3 sample_pos_tangent = get_ssao_kernel_sample(i);
      vec3 sample_pos_view = tbn * sample_pos_tangent;
      sample_pos_view = view_pos + sample_pos_view * u_ssaoRadius;

      vec4 offset = vec4(sample_pos_view, 1.0);
      offset = mul(u_proj, offset);
      offset.xyz /= offset.w;
      offset.xy = offset.xy * 0.5 + 0.5;

      float sample_depth = getDepth(offset.xy);

      float range_check = smoothstep(0.0, 1.0, u_ssaoRadius / abs(view_pos.z - sample_depth));
      occlusion += (sample_depth >= sample_pos_view.z + u_ssaoBias ? 1.0 : 0.0) * range_check;
    }

    occlusion = 1.0 - (occlusion / float(KERNEL_SIZE));

    // gl_FragColor = base * (light + ambient * occlusion) + emission;
    gl_FragColor = vec4(1.) * occlusion;
  */
}
