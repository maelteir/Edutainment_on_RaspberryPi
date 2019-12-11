varying vec2 tcoord;
uniform sampler2D tex;
uniform sampler2D tex1;
void main(void) 
{
   float diff= abs(texture2D(tex1,tcoord).r-texture2D(tex,tcoord).r);
   gl_FragColor = vec4( diff,diff,diff, 1 ); 
}
