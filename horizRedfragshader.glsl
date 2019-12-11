
varying vec2 tcoord;
uniform sampler2D tex;
uniform float screenWidth;
uniform float screenHeight;

void main(void) 
{
	gl_FragColor = texture2D(tex,tcoord);

/*	highp float col = gl_FragCoord.x; 
	highp float row = gl_FragCoord.y;
	

	//int stride =4;

	vec4 sum = vec4(0,0,0,0);
	int c=0;

	highp float x; //[4];
	highp float y= row/512.0;

	for (int i=0; i< 4; i++)
	{
            //x[i] =  (col *float(stride)/4.0+ float(i))/256.0;
	    x =  (col+ float(i)) /256.0;
	    

   	    vec4 temp = texture2D(tex, vec2( x,y) );

             if (temp.r !=  0.0){
		 c++;
		 sum+= temp; 
	     }
	}

 	gl_FragColor = vec4(float(c)/256.0,sum.g/float(c),sum.b/float(c),x);  //float(256)
        //gl_FragColor = vec4(x[0], x[1], x[2], y);

*/			
}

//stride as argument
//coelescing