#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include "glutils/glutils.h"
#include "glutils/glshader.h"
#include "glutils/gllight.h"
#include "glutils/glmodel.h"
#include "glutils/glmesh.h"
#include "glutils/glkernel.h"
#include "glutils/gltexture.h"

#include "imgui.h"
#include "imgui_theme.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imguiutils/imguilights.h"


const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

const float FPS_CAP = 60.0f;
const float FRAME_TIME = 1.0f / FPS_CAP;

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool showSettings = false;
bool showStats = false;

Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// continuous
void processInput(GLFWwindow *window) {
    camera.keyCallback(window, deltaTime);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        showSettings = true;
        camera.releaseCursor(window);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        showStats = !showStats;
    }

    //if (key == GLFW_MOUSE)
}

void mouseCallback(GLFWwindow *window, double xPos, double yPos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
    if (ImGui::GetIO().WantCaptureMouse) return;
    camera.mouseCallback(xPos, yPos);
}

void mouseClickCallback(GLFWwindow *window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        camera.captureCursor(window);
        showSettings = false;
    }
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
    if (ImGui::GetIO().WantCaptureMouse) return;
    camera.scrollCallback(yOffset);
}

int main() {
    // WINDOW
    GLFWwindow *window = setupGlfwOpenGl(SCR_WIDTH, SCR_HEIGHT, "Lighting", &camera);
    glfwSwapInterval(1); // v-sync

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &imGuiIO = ImGui::GetIO();
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // imGuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    imGuiDarkMode();

    // SHADER
    const Shader objectShader("../shaders/objectShader.vsh", "../shaders/objectShader.fsh", {{"MAX_SPOT_LIGHTS", "5"}});
    const Shader outlineShader("../shaders/objectShader.vsh", "../shaders/singleColorShader.fsh");
    const Shader flatShader("../shaders/objectShader.vsh", "../shaders/diffuseShader.fsh");
    const Shader screenShader("../shaders/screenShader.vsh", "../shaders/diffuseShader.fsh");

    // CAMERA
    camera.captureCursor(window);
    camera.velocity = 5.0f;
    camera.moveKeys = Camera::GLCAMERA_WASDARROWS;
    camera.relativeMovement = false;
    camera.allowVerticalMovement = true;
    //camera.fov = 120.0f;

    // INPUT
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // WIREFRAME
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    /*
    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);

    for (GLint i = 0; i < nExtensions; ++i) {
        const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
        std::cout << "Extension " << i << ": " << extension << std::endl;
    }
    */

    // OBJECTS
    Model *backpack = new Model("../models/backpack/backpack.obj");
    Model *backpack2 = new Model(*backpack);
    Model *windowModel = getPlaneModel("../models/window/window.png", 2.0f, 2.0f);

    Texture grass;
    grass.id = load2DTexture("../models/grass/grass.png");
    grass.type = "texture_diffuse";

    DirectionalLight directionalLight = {};
    directionalLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLight.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    directionalLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight.specular = glm::vec3(0.7f, 0.7f, 0.7f);

    std::vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    windows.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    windows.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    windows.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    windows.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));

    SpotLight spotLight = {};
    spotLight.attachedToCamera = true;

    std::vector directionalLights = {directionalLight};
    std::vector<PointLight> pointLights = {};
    std::vector<SpotLight> spotLights = {spotLight};

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // FRAMEBUFFER
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // renderbuffer for depth/stencil (not sampling)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Model *defaultPlane = getPlaneModel();

    /******* RENDER LOOP *******/
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        // imgui frame
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // ui
        if (showSettings) {
            ImGui::Begin("Settings", &showSettings);
            ImGui::Text("Lights");
            ImGui::BeginTabBar("Lighting_settings");
            if (ImGui::BeginTabItem("Directional")) {
                guiMultipleDirectionalLights(directionalLights);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Point")) {
                guiMultiplePointLights(pointLights);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Spot")) {
                guiMultipleSpotLights(spotLights);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
            ImGui::End();
        }

        // timing
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;

        if (deltaTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_TIME - deltaTime));
            currentTime = static_cast<float>(glfwGetTime());
            deltaTime = currentTime - lastFrame;
        }

        lastFrame = currentTime;

        if (showStats) {
            float fps = 1.0f / deltaTime;
            ImGui::Begin("Performance", &showStats);
            ImGui::Text((std::to_string(static_cast<int>(fps)) + " FPS").data());
            ImGui::End();
        }

        processInput(window);

        // SHADER
        objectShader.use();
        objectShader.setVec3("viewPos", camera.position);

        // LIGHTS
        // directional lights
        for (int i = 0; i < directionalLights.size(); i++)
            directionalLights[i].setLightInShader(objectShader, i);
        objectShader.setInt("numDirectionalLights", directionalLights.size());
        // point lights
        for (int i = 0; i < pointLights.size(); i++) {
            if (pointLights[i].attachedToCamera) {
                pointLights[i].attachTo(camera);
            }
            pointLights[i].setLightInShader(objectShader, i);
        }
        objectShader.setInt("numPointLights", pointLights.size());
        // spotlights
        for (int i = 0; i < spotLights.size(); i++) {
            if (spotLights[i].attachedToCamera) {
                spotLights[i].attachTo(camera);
            }
            spotLights[i].setLightInShader(objectShader, i);
        }
        objectShader.setInt("numSpotLights", spotLights.size());

        // CAMERA
        const glm::mat4 view = camera.getViewMatrix();
        const glm::mat4 projection = camera.getProjectionMatrix();
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);
        outlineShader.use();
        outlineShader.setMat4("view", view);
        outlineShader.setMat4("projection", projection);
        flatShader.use();
        flatShader.setMat4("view", view);
        flatShader.setMat4("projection", projection);


        // framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // texture for the color (for sampling)
        Texture textureColorbuffer = generateEmptyTexture(camera.displayWidth, camera.displayHeight);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer.id, 0);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, camera.displayWidth, camera.displayHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // CLEAR
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // DRAW TO FRAMEBUFFER TEXTURE
        // without outline
        backpack->setPosition(glm::vec3(-3.0f, 0.0f, -2.0f));
        drawModels({backpack}, objectShader);
        // with outline
        backpack->setPosition(glm::vec3(0.0f));
        backpack2->setPosition(glm::vec3(3.0f));
        outlineShader.use();
        outlineShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
        std::vector<Model*> outlinedModels = {backpack, backpack2};
        drawOutlinedModels(outlinedModels, objectShader, outlineShader, 20.0f, OUTLINE_COMPLETE_OPAQUE);
        // transparent objects
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_CULL_FACE);
        glm::mat4 model;
        std::multimap<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.position - windows[i]);
            sorted.insert(std::make_pair(distance, windows[i]));
        }
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            windowModel->setPosition(it->second);
            windowModel->draw(objectShader);
        }
        glEnable(GL_CULL_FACE);

        // RENDER TO SCREEN QUAD
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        defaultPlane->getMesh(0)->setTexture(0, textureColorbuffer);
        // effects
        screenShader.use();
        screenShader.setInt("screenWidth", camera.displayWidth);
        screenShader.setInt("screenHeight", camera.displayHeight);
        screenShader.setFloat("increaseKernel", 1.0f);
        auto a = blurKernel;
        auto b = sharpenKernel;
        for (int i = 0; i < a.size(); i++) {
            a[i] += b[i];
            a[i] /= 2.0f;
        }
        screenShader.setFloatVector("kernel", sharpenKernel);
        //screenShader.setBool("applyKernel", true);
        //screenShader.setBool("invertColor", true);
        //screenShader.setBool("blackAndWhite", true);
        defaultPlane->draw(screenShader);


        // REFRESH
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (imGuiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // DEALLOCATION
    backpack->deallocate();
    backpack2->deallocate();
    windowModel->deallocate();
    delete backpack;
    delete backpack2;
    delete windowModel;
    objectShader.deleteShader();

    glfwTerminate();
    return 0;
}