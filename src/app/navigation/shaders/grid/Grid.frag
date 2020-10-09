uniform vec3 color = vec3(0.0, 0.0, 0.0);
uniform sampler2D textureData;
uniform float u_scale;

in vec2 interpolatedTextureCoordinates;

void main(){
    vec4 t = texture(textureData, interpolatedTextureCoordinates);
    gl_FragColor.rgb = color;
    
    float alphaDrop = 0.0;
    if (u_scale < 1.0) {
        alphaDrop = 1.0 - u_scale;
    }
    
    gl_FragColor.a = t.g - alphaDrop;
}