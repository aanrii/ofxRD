#version 150
#define SQRT_INV 0.7071 // 1 / sqrt(2)
#define LAPLACIAN_8_P_WEIGHT 6.8284 // (1 / sqrt(2) + 1) * 4

in vec4 colorVarying;
in vec4 position;
in vec3 normal;

uniform float timeValue;

uniform vec3 maxParam;
uniform vec3 minParam;

// replace to options

uniform vec2 cellClusterPos;
uniform int cellClusterRadius;
uniform vec3 cellClusterParam;


uniform sampler2DRect cellMap;


in vec2 vTexCoord;

out vec4 vFragColor;

int cyclic(int value, int max_val){
    return (value >= max_val) ? value % max_val: (value >= 0) ? value: value + max_val;
}

ivec2 getNeighborPoint(ivec2 p, int n){
    if (n < 0 || n == 4 || n > 8) {
        return ivec2(-1, -1);
    } else {
        return ivec2(cyclic(p.x - int(floor(n / 3)) + 1, textureSize(cellMap).x),
                     cyclic(p.y - (n % 3) + 1, textureSize(cellMap).y));
    }
}

vec3 getLaplacian4(ivec2 p){
    vec3 pValue = texture(cellMap, p).xyz;
    mat4x3 neighborValues;
    neighborValues[0] = texture(cellMap, getNeighborPoint(p, 1)).xyz;
    neighborValues[1] = texture(cellMap, getNeighborPoint(p, 3)).xyz;
    neighborValues[2] = texture(cellMap, getNeighborPoint(p, 5)).xyz;
    neighborValues[3] = texture(cellMap, getNeighborPoint(p, 7)).xyz;
    return neighborValues * vec4(1.0) - pValue * 4.0;
}

vec3 getLaplacian8(ivec2 p){
    vec3 pValue = texture(cellMap, p).xyz;
    mat4x3 neighborValues;
    neighborValues[0] = texture(cellMap, getNeighborPoint(p, 1)).xyz;
    neighborValues[1] = texture(cellMap, getNeighborPoint(p, 3)).xyz;
    neighborValues[2] = texture(cellMap, getNeighborPoint(p, 5)).xyz;
    neighborValues[3] = texture(cellMap, getNeighborPoint(p, 7)).xyz;
    
    mat4x3 diagonalNeighborValues;
    diagonalNeighborValues[0] = texture(cellMap, getNeighborPoint(p, 0)).xyz;
    diagonalNeighborValues[1] = texture(cellMap, getNeighborPoint(p, 2)).xyz;
    diagonalNeighborValues[2] = texture(cellMap, getNeighborPoint(p, 6)).xyz;
    diagonalNeighborValues[3] = texture(cellMap, getNeighborPoint(p, 8)).xyz;
    
    return neighborValues * vec4(1.0) + neighborValues * vec4(SQRT_INV) - pValue * LAPLACIAN_8_P_WEIGHT;
}


void main() {
    if (cellClusterRadius > 0 && distance(vTexCoord, cellClusterPos) <= cellClusterRadius){
        vFragColor = vec4(cellClusterParam, 1.0f);
    } else {
        vec4 param = texture(cellMap, vTexCoord);
        float x = param.x;
        float y = param.y;
        float z = param.z;
        
        vec3 laplacian = getLaplacian8(ivec2(vTexCoord));
        
        vec3 newValue = clamp(vec3(
                             // replace to formulas
                                ), minParam, maxParam);
        vFragColor = vec4(newValue, 1.0f);
    }
}
