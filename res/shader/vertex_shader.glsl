attribute vec4 aPosition;
attribute vec2 aTextureCoord;
uniform mat4 uMatrix;
varying vec2 vTextureCoord;

void main(){
    gl_Position=aPosition;
    vTextureCoord=aTextureCoord;
}