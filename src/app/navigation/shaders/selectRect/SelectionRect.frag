uniform vec4 u_innerRect;

bool isInnerRect(vec2 point) {
    return  point.x > u_innerRect[0] && 
            point.x < u_innerRect[2] && 
            point.y > u_innerRect[1] && 
            point.y < u_innerRect[3];
} 

void main() {
    vec3 base_color = vec3(1.0, 0.7, 0.3);
    float opacity = 1.0;

    if(isInnerRect(gl_FragCoord.xy)) {
       opacity = 0.5;
    }

	gl_FragColor = vec4(base_color, opacity);
}