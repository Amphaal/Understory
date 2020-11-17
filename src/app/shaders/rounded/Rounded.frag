uniform vec4 u_color;
uniform vec2 u_geomPixel_min;
uniform vec2 u_geomPixel_max;

// from http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float udRoundBox(vec2 p, vec2 b, float r){
    return length(max(abs(p) - b + r, 0.0)) - r;
}

void main(){
    // setup
    float iRadius = 50.0;
    
    // compute box
    float b = udRoundBox(gl_FragCoord.xy - u_geomPixel_max, u_geomPixel_max, iRadius);
    
    // colorize
	gl_FragColor = mix(
		u_color,
		vec4(0.0),
		smoothstep(0.0, 1.0, b)
	);
}