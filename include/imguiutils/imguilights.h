#ifndef IMGUILIGHTS_H
#define IMGUILIGHTS_H

#include <imgui.h>
#include <vector>
#include "glutils/gllight.h"

void guiMultipleDirectionalLights(std::vector<DirectionalLight> &lights);
void guiMultiplePointLights(std::vector<PointLight> &lights);
void guiMultipleSpotLights(std::vector<SpotLight> &lights);

#endif //IMGUILIGHTS_H
