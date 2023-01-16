#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaderinit.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float brick_vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f // top left 

};

unsigned int indices[] = {
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};

glm::vec3 brick_color[] = {
    glm::vec3(1.0f, 0.5f, 0.0f),
    glm::vec3(0.5f, 1.0f, 0.5f),
    glm::vec3(0.9f, 0.2f, 1.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 0.6f, 0.5f),
    glm::vec3(1.0f, 0.0f, 0.5f),
};


void processInput(GLFWwindow* window);
void init(void);
void render();
GLuint sphere();

void transformation(Shader& ourShader);

// variables
const unsigned int screen_width = 1024;
const unsigned int screen_height = 768;

bool launch = false;

//unsigned int VBO, VAO;
GLuint VBO;
GLuint VAO;
GLuint EBO;


int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window;
    window = glfwCreateWindow(screen_width, screen_height, "Arkanoid(protoype)", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load all OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // create an instance of the Shader class
    Shader ourShader("shader.vs", "shader.fs"); // you can name your shader files however you like

    init();
    transformation(ourShader);

    // render/game loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        render();
        ourShader.use();
        glm::mat4 view = glm::mat4(1.0f);
        ourShader.setMat4("view", view);
        glBindVertexArray(VAO);

        for (unsigned j = 0; j < 6; j++)
        {
            for (unsigned int i = 0; i < 9; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(((0.0 + i) / 4.6) + -0.87f, (-(0.0 + j) / 8.5) + 0.85f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2f, 0.1f, 0.2f));
                ourShader.setVec3("aColor", brick_color[j]);
                ourShader.setMat4("model", model);
                glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
            }
        }

        glm::mat4 player = glm::mat4(1.0f);
        player = glm::translate(player, glm::vec3(0.0f, -0.85f, 0.0f));
        player = glm::scale(player, glm::vec3(0.2f, 0.05f, 0.2f));
        ourShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", player);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glm::mat4 player_outline = glm::mat4(1.0f);
        player_outline = glm::translate(player_outline, glm::vec3(0.0f, -0.85f, 0.0f));
        player_outline = glm::scale(player_outline, glm::vec3(0.2f, 0.05f, 0.2f));
        ourShader.setVec3("aColor", glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", player_outline);
        glLineWidth(5);
        glDrawArrays(GL_LINES, 0, 8);

        glm::mat4 player_outline_y = glm::mat4(1.0f);
        player_outline_y = glm::translate(player_outline_y, glm::vec3(0.0f, -0.85f, 0.0f));
        player_outline_y = glm::rotate(player_outline_y, glm::radians(180.0f), glm::vec3(1.0f, 1.0f, 0.0f));
        player_outline_y = glm::scale(player_outline_y, glm::vec3(0.05f, 0.2f, 0.2f));
        ourShader.setVec3("aColor", glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", player_outline_y);
        glLineWidth(5);
        glDrawArrays(GL_LINES, 0, 8);

        GLuint ball = sphere();
        glBindVertexArray(ball);
        glEnable(GL_POINT_SMOOTH);
        glm::mat4 ball_model = glm::mat4(1.0f);
        if (launch == true)
        {
            ball_model = glm::translate(ball_model, glm::vec3(-1.0f, (glfwGetTime() * 0.3) + (-1.80f), 0.0f));
        }
        else
        {
            ball_model = glm::translate(ball_model, glm::vec3(-1.0f, (-1.80f), 0.0f));
        }
        ourShader.setVec3("aColor", glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", ball_model);
        glPointSize(15);
        glDrawArrays(GL_POINTS, 0, 1);

        // glfw: swap buffers
        glfwSwapBuffers(window);
        // glfw: poll events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void transformation(Shader& ourShader)
{  
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // render
    ourShader.use();
    // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    ourShader.setMat4("projection", projection);
}

void render()
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear(GL_COLOR_BUFFER_BIT);
}

void init(void)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(brick_vertices), brick_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute pointer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint sphere()
{
    float sphere_vert[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vert), sphere_vert, GL_STATIC_DRAW);

    // position attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute pointer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// user input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        glfwSetTime(0);
        launch = true;
        
    }
}