$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(deferrdBaseColor, 0);
SAMPLER2D(lightBuffer, 1);
SAMPLER2D(emissionBuffer, 2);

void main()
{
  vec4 base = texture2D(deferrdBaseColor, v_uv);
  vec4 light = texture2D(lightBuffer, v_uv);
  vec4 emission = texture2D(emissionBuffer, v_uv);
  gl_FragColor = base * (light + vec4(.1f)) + emission;
}
