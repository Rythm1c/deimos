#include "material.h"
#include "shader.h"

void Material::configShader(Shader &shader) {
  shader.updateFloat("ao", this->ao);
  shader.updateFloat("roughness", this->roughness);
  shader.updateFloat("metallicFactor", this->metallicness);
  shader.updateVec3("baseColor", this->baseCol);
  shader.updateInt("hasBaseTexture", (this->baseTex != -1));
  shader.updateInt("hasMetallicMap", (this->metallicMap != -1));
}
