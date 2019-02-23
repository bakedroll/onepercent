uniform sampler2D distancemap;

uniform vec4  overlayColor;
uniform vec4  takeoverColor;
uniform float takeover;
uniform float takeoverScale;

const float takeoverEdgeWidth = 0.05;

void main(void) 
{
  float takeover_invers = (1.0 - takeover) * (1.0 + takeoverEdgeWidth * 2.0) - takeoverEdgeWidth;

  float distance        = texture2D(distancemap, gl_TexCoord[0].st).r;
  float intensity       = distance * 0.5 + 0.5;
  vec4  backgroundColor = vec4(overlayColor.rgb * intensity, overlayColor.a);

  float noiseVal = clamp((snoise(gl_TexCoord[1].xyz * takeoverScale) + 1.0) / 2.0, 0.0, 1.0);
  float val      = smoothstep(
    takeover_invers - takeoverEdgeWidth,
    takeover_invers + takeoverEdgeWidth,
    noiseVal);

  vec4 takeoverLayerColor = vec4(takeoverColor.rgb, takeoverColor.a * val);

  takeoverLayerColor.rgb *= takeoverLayerColor.a;
  backgroundColor.rgb    *= backgroundColor.a;

  gl_FragColor.rgb = takeoverLayerColor.rgb + (backgroundColor.rgb * (1.0 - takeoverLayerColor.a));       
  gl_FragColor.a   = takeoverLayerColor.a   + (backgroundColor.a   * (1.0 - takeoverLayerColor.a));
}
