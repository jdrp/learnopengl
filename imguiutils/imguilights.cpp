#include "imguiutils/imguilights.h"

#include <glutils/glcamera.h>

void guiLightColorSettings(Light &light, int i, std::string type) {
    ImGui::ColorEdit3(("Ambient##" + type + std::to_string(i)).c_str(), glm::value_ptr(light.ambient), ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    ImGui::ColorEdit3(("Diffuse##" + type + std::to_string(i)).c_str(), glm::value_ptr(light.diffuse), ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    ImGui::ColorEdit3(("Specular##" + type + std::to_string(i)).c_str(), glm::value_ptr(light.specular), ImGuiColorEditFlags_NoInputs);
}

void guiDirectionalLightSettings(DirectionalLight &light, int i) {
    ImGui::DragFloat3(("Direction##d" + std::to_string(i)).c_str(), glm::value_ptr(light.direction), 0.001f, 0, 0);
    guiLightColorSettings(light, i, "d");
}

void guiPointLightSettings(PointLight &light, int i) {
    ImGui::Checkbox(("Attach to Camera##s" + std::to_string(i)).c_str(), &light.attachedToCamera);
    if (!light.attachedToCamera) {
    ImGui::DragFloat3(("Position##p" + std::to_string(i)).c_str(), glm::value_ptr(light.position), 0.001f, 0, 0);
    }

    guiLightColorSettings(light, i, "p");
    ImGui::DragFloat(("Constant##p" + std::to_string(i)).c_str(), &light.constant, 0.001f, 0.0f, FLT_MAX);
    ImGui::DragFloat(("Linear##p" + std::to_string(i)).c_str(), &light.linear, 0.001f, 0.0f, FLT_MAX);
    ImGui::DragFloat(("Quadratic##p" + std::to_string(i)).c_str(), &light.quadratic, 0.001f, 0.0f, FLT_MAX);
}

void guiSpotLightSettings(SpotLight &light, int i) {
    ImGui::Checkbox(("Attach to Camera##s" + std::to_string(i)).c_str(), &light.attachedToCamera);
    if (!light.attachedToCamera) {
        ImGui::DragFloat3(("Position##s" + std::to_string(i)).c_str(), glm::value_ptr(light.position), 0.001f, 0, 0);
        ImGui::DragFloat3(("Direction##s" + std::to_string(i)).c_str(), glm::value_ptr(light.direction), 0.001f, 0, 0);
    }

    guiLightColorSettings(light, i, "s");
    ImGui::DragFloat(("Constant##s" + std::to_string(i)).c_str(), &light.constant, 0.001f, 0.0f, FLT_MAX);
    ImGui::DragFloat(("Linear##s" + std::to_string(i)).c_str(), &light.linear, 0.001f, 0.0f, FLT_MAX);
    ImGui::DragFloat(("Quadratic##s" + std::to_string(i)).c_str(), &light.quadratic, 0.001f, 0.0f, FLT_MAX);
    ImGui::DragFloat(("Inner##s" + std::to_string(i)).c_str(), &light.innerCutoff, 0.1f, 0.0f, FLT_MAX, "%.1f");
    ImGui::DragFloat(("Outer##s" + std::to_string(i)).c_str(), &light.outerCutoff, 0.1f, 0.0f, FLT_MAX, "%.1f");

}

template<typename LightType>
void guiMultipleLights(std::vector<LightType> &lights, void (*guiLightSettings)(LightType &, int), const std::string &lightTypeName) {
    for (int i = 0; i < lights.size(); i++) {
        if (ImGui::Button(("-##" + lightTypeName + std::to_string(i)).c_str())) {
            lights.erase(lights.begin() + i--);
        }
        ImGui::SameLine();
        ImGui::SeparatorText((lightTypeName + " light " + std::to_string(i)).c_str());
        guiLightSettings(lights[i], i);
    }
    ImGui::Separator();
    if (ImGui::Button(("+##" + lightTypeName).c_str())) {
        lights.push_back({});
    }
}

void guiMultipleDirectionalLights(std::vector<DirectionalLight> &lights) {
    guiMultipleLights(lights, guiDirectionalLightSettings, "Directional");
}

void guiMultiplePointLights(std::vector<PointLight> &lights) {
    guiMultipleLights(lights, guiPointLightSettings, "Point");
}

void guiMultipleSpotLights(std::vector<SpotLight> &lights) {
    guiMultipleLights(lights, guiSpotLightSettings, "Spot");
}
