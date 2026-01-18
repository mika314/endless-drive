$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(baseColor, 0);
SAMPLER2D(lightBuffer, 1);

void main()
{
  gl_FragColor = texture2D(baseColor, v_uv) * (texture2D(lightBuffer, v_uv) + vec4(.1f));
}
