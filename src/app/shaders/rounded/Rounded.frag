uniform vec4 u_color;
uniform vec2 u_rectBottomLeftPx;
uniform vec2 u_rectSizePx;
uniform vec2 u_resolutionPx;

// from http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float udRoundBox(vec2 p, vec2 b, float r){
    return length(max(abs(p) - b + r, 0.0)) - r;
}

void main() {
    // setup radius
    float iRadius = min(u_rectSizePx.x, u_rectSizePx.y) / 2.0;
    
    // prepare
    vec2 start = vec2(
        (u_resolutionPx.x - u_rectSizePx.x) * 0.5,
        (u_resolutionPx.y - u_rectSizePx.y) * 0.5
    );
    vec2 halfRes = 0.5 * u_resolutionPx.xy - start;

    // compute box
    float b = udRoundBox(
        gl_FragCoord.xy - halfRes - u_rectBottomLeftPx,
        halfRes,
        iRadius
    );
    
    // colorize
	gl_FragColor = mix(
		u_color,
		vec4(0.0),
		smoothstep(0.0, 1.0, b)
	);
}