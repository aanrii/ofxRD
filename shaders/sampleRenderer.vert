#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform vec4 colors[6];
uniform float colors_pos[6];
uniform float resolution;
uniform float meshDepth;
uniform int interpolationMode;
uniform bool enableHeatMap;
uniform vec3 minParam;
uniform vec3 maxParam;

uniform int componentColor;
uniform int componentHeight;

uniform sampler2DRect cellMap;
uniform sampler2DRect bicubicWeights;

in vec4 position;

out vec4 colorVarying;

float mapValue(float value, float minVal, float maxVal){
    return (value - minVal) / abs(maxVal - minVal);
}

vec4 HSVToRGB(vec4 hsv){
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;
    
    float r = v;
    float g = v;
    float b = v;
    
    if (s > 0.0f) {
        h *= 6.0f;
        int i = int(h);
        float f = h - float(i);
        switch (i) {
            default:
            case 0:
                g *= 1.0f - s * (1.0f - f);
                b *= 1.0f - s;
                break;
            case 1:
                r *= 1.0f - s * f;
                b *= 1.0f - s;
                break;
            case 2:
                r *= 1.0f - s;
                b *= 1.0f - s * (1.0f - f);
                break;
            case 3:
                r *= 1.0f - s;
                g *= 1.0f - s * f;
                break;
            case 4:
                r *= 1.0f - s * (1.0f - f);
                g *= 1.0f - s;
                break;
            case 5:
                g *= 1.0f - s;
                b *= 1.0f - s * f;
                break;
        }
    }
    
    vec4 rgb = vec4(r, g, b, hsv.w);
    return rgb;
}

vec2 getTextureValue(vec2 p, int idx1, int idx2) {
    vec4 tex = texture(cellMap, ivec2(p));
    return vec2(tex[idx1], tex[idx2]);
}

vec2 bilinear(vec2 p, int idx1, int idx2) {
    vec2 diff_floor = p - floor(p);
    vec2 diff_ceil = ceil(p) - p;
    
    if (diff_floor == vec2(0.0)){
        return getTextureValue(p, idx1, idx2);
    } else if (diff_floor.x == 0.0f) {
        vec2 param_floor = getTextureValue(vec2(p.x, floor(p.y)), idx1, idx2);
        vec2 param_ceil = getTextureValue(vec2(p.x, ceil(p.y)), idx1, idx2);
        
        return mix(param_floor, param_ceil, diff_floor.y);
    } else if (diff_floor.y == 0.0f) {
        vec2 param_floor = getTextureValue(vec2(floor(p.x), p.y), idx1, idx2);
        vec2 param_ceil = getTextureValue(vec2(ceil(p.x), p.y), idx1, idx2);
        
        return mix(param_floor, param_ceil, diff_floor.x);
    } else {
        vec2 param_xfloor_yfloor = getTextureValue(vec2(floor(p.x), floor(p.y)), idx1, idx2);
        vec2 param_xceil_yfloor = getTextureValue(vec2(ceil(p.x), floor(p.y)), idx1, idx2);
        vec2 param_xfloor_yceil = getTextureValue(vec2(floor(p.x), ceil(p.y)), idx1, idx2);
        vec2 param_xceil_yceil = getTextureValue(vec2(ceil(p.x), ceil(p.y)), idx1, idx2);
        
        vec2 param_yfloor = mix(param_xfloor_yfloor, param_xceil_yfloor, diff_floor.x);
        vec2 param_yceil = mix(param_xfloor_yceil, param_xceil_yceil, diff_floor.x);
        
        return mix(param_yfloor, param_yceil, diff_floor.y);
    }
}

vec4 getBicubicWeight(vec4 value) {
    vec4 weight = vec4(0.0);
    
    for (int i = 0; i < 4; i++) {
        int d = abs(int(value[i] / resolution));
        weight[i] = texture(bicubicWeights, ivec2(d, 0)).x;
    }
    
    return weight;
}

vec2 bicubic(vec2 p, int idx1, int idx2) {
    vec2 relative_p = fract(p);
    
    if (relative_p == vec2(0.0)) {
        return getTextureValue(p, idx1, idx2);
    } else {
        vec4 dist_x, dist_y, dist_x_weight, dist_y_weight;
        
        if (relative_p.x == 0.0f) {
            dist_x = vec4(0.0f);
            dist_x_weight = vec4(0.25f);
        } else {
            dist_x = vec4(relative_p.x) + vec4(1.0f, 0.0f, -1.0f, -2.0f);
            dist_x_weight = getBicubicWeight(dist_x);
        }
        if (relative_p.y == 0.0f) {
            dist_y = vec4(0.0f);
            dist_y_weight = vec4(0.25f);
        } else {
            dist_y = vec4(relative_p.y) + vec4(1.0f, 0.0f, -1.0f, -2.0f);
            dist_y_weight = getBicubicWeight(dist_y);
        }
        
        mat4 sourceMat1, sourceMat2;
        for (int i = 0; i < 4; i ++){
            for (int j = 0; j < 4; j ++){
                vec2 tex = getTextureValue(vec2(p.x - dist_x[j], p.y - dist_y[i]), idx1, idx2);
                sourceMat1[j][i] = tex[0];
                sourceMat2[j][i] = tex[1];
            }
        }
        
        return vec2(dot(dist_y_weight * sourceMat1, dist_x_weight),
                    dot(dist_y_weight * sourceMat2, dist_x_weight));
    }
    
}

vec4 getCellColor(float value) {
    for (int i = 1; i < 6; i++){
        if (value < colors_pos[i]){
            vec4 color1 = colors[i - 1];
            vec4 color2 = colors[i];
            float cellColorPos = mapValue(value, colors_pos[i - 1], colors_pos[i]);
            
            return mix(color1, color2, cellColorPos);
        }
    }
}

void main()
{
    int meshWidth = int(float(textureSize(cellMap).x) / resolution);
    ivec2 meshIdx = ivec2(int(mod(gl_VertexID, meshWidth)), int(gl_VertexID / meshWidth));
    vec2 texIdx = vec2(meshIdx.x, meshIdx.y) * resolution;
    
    vec2 param;
    switch (interpolationMode) {
        case 2:
            param = bicubic(texIdx, componentColor, componentHeight);
            break;
        case 1:
            param = bilinear(texIdx, componentColor, componentHeight);
            break;
        default:
            param = getTextureValue(texIdx, componentColor, componentHeight);
            break;
    }
    
    vec4 vPos = position;
    vPos.z = param[1] * meshDepth;
    
    gl_Position = projectionMatrix * modelViewMatrix * vPos;
    colorVarying =
        (!enableHeatMap) ? getCellColor(mapValue(param[0], minParam[componentColor], maxParam[componentColor])) :
        HSVToRGB(vec4(mapValue(param[0], minParam[componentColor], maxParam[componentColor]), 1.0f, 1.0f, 1.0f));
}
