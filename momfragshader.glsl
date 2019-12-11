varying vec2 tcoord;
uniform sampler2D tex;
uniform float screenWidth;
uniform float screenHeight;

void main(void) 
{
   vec4 temp = texture2D(tex,tcoord);
   float intensity  = max( max (temp.r , temp.g), temp.b) ;
   
  // gl_FragColor =  vec4( intensity , (gl_FragCoord.x/float(screenWidth)), (gl_FragCoord.y/float(screenHeight)), 1);		
   gl_FragColor =  (intensity == 1.0)? vec4( intensity , (gl_FragCoord.x/512.0), (gl_FragCoord.y/512.0), 0.25) : vec4(0,0,0,64);			
}
