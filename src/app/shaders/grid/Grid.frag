uniform vec3 color = vec3(0.0, 0.0, 0.0);
uniform sampler2D textureData;
uniform float u_scale;

in vec2 interpolatedTextureCoordinates;

void main(){
    vec4 t = texture(textureData, interpolatedTextureCoordinates);
    
    if (u_scale < 0.5) {
        t.a = t.a * u_scale;
    }

    gl_FragColor.rgba = t;
}