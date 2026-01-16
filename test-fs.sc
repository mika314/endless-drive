$input v_norm, v_frag_pos, v_uv

#include <bgfx_shader.sh>

SAMPLER2D(baseColorTex, 0);
SAMPLER2D(metalicTex, 1);
SAMPLER2D(roughnessTex, 2);
uniform vec4 settings;
uniform vec4 baseColor;
uniform vec4 metalic;
uniform vec4 roughness;

void main()
{
  gl_FragColor = texture2D(baseColorTex, v_uv);
//  gl_FragColor = vec4(v_uv, 0.f, 1.f);
}
