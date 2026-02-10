$input a_position, a_texcoord0
$output v_uv

#include <bgfx_shader.sh>

void main()
{
  gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
  v_uv = a_texcoord0;
}
