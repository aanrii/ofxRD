#version 150

in vec4 colorVarying;
out vec4 fragColor;

void main(){
    fragColor = colorVarying;
}
