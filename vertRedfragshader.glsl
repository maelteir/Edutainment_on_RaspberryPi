varying vec2 tcoord;
uniform sampler2D tex;
uniform float texWidth;
uniform float texHeight;

void main(void) 
{

//     vec4 temp = texture2D(tex, tcoord);
  //   gl_FragColor = temp; //vec4( temp.g,temp.b, tcoord.x*256.0, tcoord.y*256.0);

	highp float col = gl_FragCoord.x; 
	highp float row = gl_FragCoord.y; 
	highp float y;//[4];

	int stride = 4;

	//highp float y0 = ( (row * float(stride)/4.0)/ 1080.0 ) * 7.5;
	//if (y0 >= 1.0)	return;
	
	vec4 sum = vec4(0,0,0,0);
	int c=0;

	highp float x;

	for (int i=0; i< 4; i++)
	{
            x =  0.0; 
            y =  (row *float(stride)+ float(i))/512.0 + 0.25;
	   
	     vec4 temp = texture2D(tex, vec2( x,y) );

             if (temp.r !=  0.0){
		 c++;
		 if (c > 1){
		   //calculate weighted average
		   float c2 = sum.r + temp.r;
		   sum = vec4 ( c2, ((sum.r*sum.g)+(temp.r*temp.g))/c2, ((sum.r*sum.b)+(temp.r*temp.b))/c2, 0);
		 }
		 else
		   sum+= temp; 
	     }
	}

	//if (sum.r == 0.0)
	// 	gl_FragColor = vec4(sum.r,sum.g,sum.b, 1.0); //sum.b 
	//else
	gl_FragColor = vec4(sum.r,sum.g, sum.b, 0.5); //sum.b 
	//gl_FragColor = vec4(y[0], y[1], y[2], x); //0.25,0.25,0.25,0.25);



}
//stride as argument
//coelescing