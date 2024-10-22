// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// stb_image 用于加载图像
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

// ImGui 库
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

// 定义屏幕宽度和高度
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 摄像机参数
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// 时间控制
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f;

// 鼠标控制
float yaw   = -90.0f;	
float pitch =  0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 物体类定义
struct Object {
    std::string type; // "cube" 或 "sphere"
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 size;
    glm::vec3 velocity; // 动态物体的速度
    bool isDynamic;
    glm::vec3 minAABB; // AABB 最小点
    glm::vec3 maxAABB; // AABB 最大点
};

// 物体列表
std::vector<Object> objects;

// 碰撞检测标志
bool collisionDetected = false;

// 函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(std::vector<std::string> faces);
void updateAABB(Object& obj);
bool checkCollision(const Object& obj1, const Object& obj2);
void renderScene(Shader& shader);
void renderSkybox(Shader& skyboxShader, unsigned int skyboxVAO, unsigned int cubemapTexture);
void loadScene(const std::string& filename);

// 渲染用的 VAO、VBO
unsigned int cubeVAO = 0, cubeVBO = 0;
unsigned int sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
unsigned int indexCount;
unsigned int quadVAO, quadVBO;
unsigned int skyboxVAO, skyboxVBO;

// 帧缓冲对象
unsigned int framebuffer, textureColorbuffer, rbo;

// Skybox 纹理
unsigned int cubemapTexture;

// 初始化立方体
void initCube() {
    if (cubeVAO == 0) {
        float vertices[] = {
            // positions          
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,  
             0.5f,  0.5f, -0.5f,  
             0.5f,  0.5f, -0.5f,  
            -0.5f,  0.5f, -0.5f,  
            -0.5f, -0.5f, -0.5f,  
    
            -0.5f, -0.5f,  0.5f,  
             0.5f, -0.5f,  0.5f,  
             0.5f,  0.5f,  0.5f,  
             0.5f,  0.5f,  0.5f,  
            -0.5f,  0.5f,  0.5f,  
            -0.5f, -0.5f,  0.5f,  
    
            -0.5f,  0.5f,  0.5f,  
            -0.5f,  0.5f, -0.5f,  
            -0.5f, -0.5f, -0.5f,  
            -0.5f, -0.5f, -0.5f,  
            -0.5f, -0.5f,  0.5f,  
            -0.5f,  0.5f,  0.5f,  
    
             0.5f,  0.5f,  0.5f,  
             0.5f,  0.5f, -0.5f,  
             0.5f, -0.5f, -0.5f,  
             0.5f, -0.5f, -0.5f,  
             0.5f, -0.5f,  0.5f,  
             0.5f,  0.5f,  0.5f,  
    
            -0.5f, -0.5f, -0.5f,  
             0.5f, -0.5f, -0.5f,  
             0.5f, -0.5f,  0.5f,  
             0.5f, -0.5f,  0.5f,  
            -0.5f, -0.5f,  0.5f,  
            -0.5f, -0.5f, -0.5f,  
    
            -0.5f,  0.5f, -0.5f,  
             0.5f,  0.5f, -0.5f,  
             0.5f,  0.5f,  0.5f,  
             0.5f,  0.5f,  0.5f,  
            -0.5f,  0.5f,  0.5f,  
            -0.5f,  0.5f, -0.5f,  
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

// 初始化球体
void initSphere() {
    if (sphereVAO == 0) {
        std::vector<float> positions;
        std::vector<unsigned int> indices;
        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
                float yPos = cos(ySegment * M_PI);
                float zPos = sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);

                positions.push_back(xPos);
                positions.push_back(yPos);
                positions.push_back(zPos);
            }
        }
        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y       * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        indexCount = (int)indices.size();

        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);

        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

// 初始化天空盒
void initSkybox(unsigned int& skyboxVAO, unsigned int& skyboxVBO) {
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// 加载场景
void loadScene(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        Object obj;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue; // 跳过空行
            std::istringstream iss(line);
            iss >> obj.type
                >> obj.position.x >> obj.position.y >> obj.position.z
                >> obj.color.r >> obj.color.g >> obj.color.b
                >> obj.size.x >> obj.size.y >> obj.size.z
                >> obj.isDynamic
                >> obj.velocity.x >> obj.velocity.y >> obj.velocity.z;
            updateAABB(obj);
            objects.push_back(obj);
        }
        file.close();
    } else {
        std::cerr << "无法打开场景文件：" << filename << std::endl;
    }
}

// 更新 AABB
void updateAABB(Object& obj) {
    obj.minAABB = obj.position - obj.size * 0.5f;
    obj.maxAABB = obj.position + obj.size * 0.5f;
}

// 碰撞检测
bool checkCollision(const Object& obj1, const Object& obj2) {
    return (obj1.maxAABB.x >= obj2.minAABB.x && obj1.minAABB.x <= obj2.maxAABB.x) &&
           (obj1.maxAABB.y >= obj2.minAABB.y && obj1.minAABB.y <= obj2.maxAABB.y) &&
           (obj1.maxAABB.z >= obj2.minAABB.z && obj1.minAABB.z <= obj2.maxAABB.z);
}

// 渲染场景中的物体
void renderScene(Shader& shader) {
    for (const auto& obj : objects) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj.position);
        model = glm::scale(model, obj.size);
        shader.setMat4("model", model);
        shader.setVec3("objectColor", obj.color);

        if (obj.type == "cube") {
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        } else if (obj.type == "sphere") {
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}

// 渲染天空盒
void renderSkybox(Shader& skyboxShader, unsigned int skyboxVAO, unsigned int cubemapTexture) {
    glDepthFunc(GL_LEQUAL);  // 更改深度函数
    skyboxShader.use();
    glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp))); // 移除位移分量
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);

    // 绘制天空盒
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // 还原深度函数
}

int main() {
    // 初始化 GLFW 和 GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Scene - OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // 捕捉鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 编译着色器
    Shader shader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
    Shader screenShader("shaders/screen_vertex.glsl", "shaders/screen_fragment.glsl");
    Shader skyboxShader("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");

    // 初始化物体
    initCube();
    initSphere();

    // 加载场景
    loadScene("scene.txt");

    // 初始化帧缓冲
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // 创建纹理附件
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 附加纹理到帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // 创建深度和模板缓冲
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // 附加渲染缓冲对象到帧缓冲
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // 检查帧缓冲完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 初始化屏幕四边形
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f, // 左上
        -1.0f, -1.0f,  0.0f, 0.0f, // 左下
         1.0f, -1.0f,  1.0f, 0.0f, // 右下

        -1.0f,  1.0f,  0.0f, 1.0f, // 左上
         1.0f, -1.0f,  1.0f, 0.0f, // 右下
         1.0f,  1.0f,  1.0f, 1.0f  // 右上
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 加载天空盒纹理
    std::vector<std::string> faces{
        "skybox/right.png",
        "skybox/left.png",
        "skybox/top.png",
        "skybox/bottom.png",
        "skybox/front.png",
        "skybox/back.png"
    };
    cubemapTexture = loadCubemap(faces);
    initSkybox(skyboxVAO, skyboxVBO);

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // 设置 ImGui 样式
    ImGui::StyleColorsDark();
    // 初始化 ImGui GLFW 和 OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 计算帧时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 设置 deltaTime 上限
        if (deltaTime > 0.1f) // 最大为 0.1 秒
            deltaTime = 0.1f;

        // 处理输入
        processInput(window);

        // 更新动态物体位置
        collisionDetected = false;
        for (auto& obj : objects) {
            if (obj.isDynamic) {
                glm::vec3 oldPosition = obj.position;
                obj.position += obj.velocity * deltaTime;
                updateAABB(obj);

                // 碰撞检测
                for (const auto& other : objects) {
                    if (&obj != &other && checkCollision(obj, other)) {
                        // 输出碰撞信息
                        std::cout << "Collision detected between dynamic object at (" 
                                << obj.position.x << ", " << obj.position.y << ", " << obj.position.z 
                                << ") and object at (" 
                                << other.position.x << ", " << other.position.y << ", " << other.position.z 
                                << ")" << std::endl;
                        // 恢复到之前的位置，阻止穿透
                        obj.position = oldPosition;
                        updateAABB(obj);
                        collisionDetected = true;
                        break;
                    }
                }
            }
        }

        // 第一遍渲染到帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // 开启深度测试
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 清除颜色和深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置视图和投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 渲染物体
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        renderScene(shader);

        // 绘制天空盒
        renderSkybox(skyboxShader, skyboxVAO, cubemapTexture);

        // 解绑帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 清除屏幕
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST); // 禁用深度测试

        // 应用反相滤镜
        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 启用深度测试以便 ImGui 正常显示
        glEnable(GL_DEPTH_TEST);

        // 开始新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 如果发生碰撞，显示提示
        if (collisionDetected) {
            ImGui::Begin("提示");
            ImGui::Text("发生碰撞！");
            ImGui::End();
        }

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区和轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);

    // 清理 ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 终止 GLFW
    glfwTerminate();
    return 0;
}

// 窗口大小调整回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 鼠标移动回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是 lastY 减去 ypos
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // 限制俯仰角度
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // 更新摄像机前向量
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 处理输入
void processInput(GLFWwindow *window) {
    float cameraSpeed = 2.5f * deltaTime; // 调整速度
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// 加载立方体贴图
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int w, h, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &w, &h, &nrChannels, 0);
        if (data) {
            // 注意不同的图像格式，可能需要调整 GL_RGB
            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    return textureID;
}
