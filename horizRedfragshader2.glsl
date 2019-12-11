varying vec2 tcoord;
uniform sampler2D tex;
uniform float screenWidth;
uniform float screenHeight;

void main(void) 
{	gl_FragColor = texture2D(tex,tcoord);

	highp float col = (gl_FragCoord.x); 
	highp float row = (gl_FragCoord.y);
	

	//int stride = 4;

   	vec4 sum = vec4(0,0,0,0);
	int c=0;

	highp float x;//[4];
	highp float y = row/512.0;
	
	for (int i=0; i< 4; i++)
	{
            //x[i] =  (col * float(stride)/4.0+ float(i))/ 256.0 +0.25 ;
            x =  (col + float(i))/ 256.0 +0.25 ;

	    vec4 temp = texture2D(tex, vec2( x,y) );
	    if (temp.r !=  0.0){
		 c++;
		 float c2 = sum.r + temp.r;
		 //calculate weighted average
		 sum = (c > 1) ? vec4 ( c2, ((sum.r*sum.g)+(temp.r*temp.g))/c2, ((sum.r*sum.b)+(temp.r*temp.b))/c2, 0) : sum+temp;  //if (c > 1){ 
	     }

	}

	gl_FragColor = vec4(sum.r,sum.g, sum.b,x);  
	//gl_FragColor = vec4(x[0], x[1], x[2], 0.13); //y);

}

//stride as argument
//coelescing