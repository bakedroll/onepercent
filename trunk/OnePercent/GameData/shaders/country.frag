uniform sampler2D distancemap;

uniform float alpha;
uniform vec3  overlayColor;
uniform vec3  overlayBlendColor;
uniform float takeover;
uniform float takeoverScale;
uniform vec4  takeoverColor;

const float takeoverEdgeWidth = 0.05;

void main (void) 
{
  float takeover_invers = (1.0 - takeover) * (1.0 + takeoverEdgeWidth * 2.0) - takeoverEdgeWidth;
  float distance        = texture2D(distancemap, gl_TexCoord[0].st).r;
  float intensity       = distance * 0.5 + 0.5;
  vec3  color           = overlayColor * intensity;
  vec4  backgroundColor = vec4(color, alpha);

  float noiseVal = clamp((snoise(gl_TexCoord[1].xyz * takeoverScale) + 1.0) / 2.0, 0.0, 1.0);
  float val      = smoothstep(
    takeover_invers - takeoverEdgeWidth,
    takeover_invers + takeoverEdgeWidth,
    noiseVal);

  vec4 takeoverLayerColor = vec4(takeoverColor.rgb, takeoverColor.a * val);
  vec3 baseColor = 
    backgroundColor.rgb * (1.0 - takeoverLayerColor.a) +
    takeoverLayerColor.rgb * takeoverLayerColor.a;

  gl_FragColor = vec4(baseColor * overlayBlendColor, backgroundColor.a);
}
