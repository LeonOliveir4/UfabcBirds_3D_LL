#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;
in vec2 fragTexCoord;
in vec3 fragPObj;
in vec3 fragNObj;



// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;

// Diffuse texture sampler
uniform sampler2D diffuseTex;

//Diffuse false
uniform vec4 color;

// Mapping mode
// 0: mix color; 1: texture 2: material definition
uniform int materialMode;

out vec4 outColor;

// Blinn-Phong reflection model
vec4 BlinnPhong(vec3 N, vec3 L, vec3 V, vec2 texCoord) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 map_Kd = texture(diffuseTex, texCoord);
  vec4 map_Ka = map_Kd;

  vec4 diffuseColor = map_Kd * Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = map_Ka * Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

vec4 BlinnPhongPureMaterial(vec3 N, vec3 L, vec3 V) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}


void main() {
  vec4 colorFinal;
  vec2 texCoord;
  if(materialMode == 0){
    colorFinal = BlinnPhongPureMaterial(fragN, fragL, fragV);  
    colorFinal = mix(colorFinal, color, 0.5);
  } else {
    if(materialMode == 1){
        texCoord = fragTexCoord;
        colorFinal = BlinnPhong(fragN, fragL, fragV, texCoord);
      }else {
        if(materialMode == 2){
          colorFinal = BlinnPhongPureMaterial(fragN, fragL, fragV);  
        }else{
          colorFinal = color;
        }
    }
  }
  outColor = colorFinal;

}