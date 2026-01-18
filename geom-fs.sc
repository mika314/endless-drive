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

void main()
{
  vec3 lBaseColor = settings.x > .5f ? texture2D(baseColorTex, v_uv).rgb : baseColor.rgb;
  float lMetallic = settings.y > .5f ? texture2D(metallicTex, v_uv).b : metallic.r;
  float lRoughness = settings.z > .5f ? texture2D(roughnessTex, v_uv).g : roughness.r;
  vec3 N = normalize(v_norm.xyz);

  gl_FragData[0] = vec4(lBaseColor, 1.f);
  gl_FragData[1] = vec4(1.f, lRoughness, lMetallic, 1.f);
  gl_FragData[2] = vec4(N, 1.f);
}
