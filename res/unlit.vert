attribute vec3 vertexPosition;
attribute vec3 vertexColor;

uniform mat4 MVP;

varying vec3 fragColor;

void main(){
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = MVP * v;
    fragColor = vertexColor;
}
