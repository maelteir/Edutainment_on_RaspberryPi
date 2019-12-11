varying vec2 tcoord;
uniform sampler2D tex;
void main(void) 
{
    //float r = texture2D(tex,tcoord).r ;
    //float g = texture2D(tex,tcoord).g ;
    //float b = texture2D(tex,tcoord).b ;

    vec4 t = texture2D(tex,tcoord);

    float gray = 0.299 * t.r + 0.587 * t.g + 0.114 *t.b;

    gl_FragColor =  vec4( gray,gray,gray,1); 
}
