//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   TRABALHO FINAL
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include "stb_image.h"

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "structs.hpp"
#include "collisions.hpp"

// Tamanho da tela
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   700

#define SPHERE      0
#define BUNNY       1
#define PLANE       2
#define PATH        3
#define COW         4
#define FLASHLIGHT  5
#define SUN         6
#define MOON        7
#define CUBEXY      8
#define CUBEYZ      9
#define GUN         10
#define MAP         11

#define XYWALL      1
#define YZWALL      2
#define XY_YZWALL   3

// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void DrawCube(GLint render_as_black_uniform); // Desenha um cubo
GLuint BuildTriangles(); // Constrói triângulos para renderização
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso não existam.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void LoadTextureImage(const char* filename);
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Função para debugging

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = -M_PI_2; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 4.5f; // Distância da câmera para a origem

float g_CameraTheta_backup_Free;
float g_CameraPhi_backup_Free;
float g_CameraTheta_backup_lookAt;
float g_CameraPhi_backup_lookAt;


// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_flashlight_on_uniform;

GLfloat g_ligth_pos_uniform;
GLint g_segundos_ciclo_dia;

GLfloat flashligth_pos_x;
GLfloat flashligth_pos_y;
GLfloat flashligth_pos_z;

GLfloat flashligth_dir_x;
GLfloat flashligth_dir_y;
GLfloat flashligth_dir_z;

GLuint g_NumLoadedTextures = 0;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

bool tecla_W_pressionada = false;
bool tecla_A_pressionada = false;
bool tecla_S_pressionada = false;
bool tecla_D_pressionada = false;
bool tecla_R_pressionada = false;
bool tecla_F_pressionada = false;
bool tecla_C_pressionada = false;
bool tecla_L_pressionada = false;
bool tecla_M_pressionada = false;
bool tecla_TAB_pressionada = false;

float speed = 3.0f; // Velocidade da câmera
glm::vec3 g_PlayerSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec4 player_position = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
glm::vec4 camera_position_c = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

std::vector<Wall> walls;

int paredes[22][26] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {2, 1, 1, 1, 3, 1, 3, 1, 1, 1, 3, 1, 1, 0, 1, 3, 1, 3, 1, 1, 1, 1, 3, 1, 1, 3},
    {2, 2, 1, 2, 0, 2, 0, 1, 1, 2, 1, 2, 1, 1, 3, 0, 3, 0, 0, 1, 3, 1, 0, 1, 0, 3},
    {2, 1, 2, 2, 2, 1, 2, 1, 3, 1, 2, 1, 0, 3, 0, 2, 0, 1, 1, 1, 2, 1, 3, 0, 3, 2},
    {2, 0, 3, 1, 2, 0, 1, 0, 1, 2, 2, 0, 3, 2, 1, 3, 2, 1, 3, 1, 3, 2, 2, 1, 2, 2},
    {2, 1, 3, 2, 1, 1, 3, 3, 0, 1, 1, 3, 2, 2, 2, 0, 1, 2, 2, 2, 2, 2, 2, 0, 1, 3},
    {2, 2, 0, 1, 0, 3, 1, 0, 1, 2, 1, 2, 2, 0, 1, 1, 1, 3, 2, 1, 0, 1, 2, 1, 1, 2},
    {2, 0, 3, 3, 1, 2, 0, 3, 1, 1, 2, 3, 1, 1, 3, 1, 3, 0, 2, 1, 1, 3, 1, 1, 1, 2},
    {2, 2, 0, 2, 2, 1, 3, 2, 0, 1, 1, 2, 2, 0, 1, 2, 2, 1, 1, 0, 0, 3, 2, 1, 1, 3},
    {2, 3, 2, 2, 0, 2, 2, 0, 3, 1, 2, 0, 2, 1, 1, 2, 2, 0, 3, 1, 3, 2, 0, 3, 1, 2},
    {2, 2, 2, 0, 1, 3, 0, 3, 0, 1, 1, 1, 2, 0, 1, 3, 0, 3, 0, 2, 0, 1, 1, 2, 2, 3},
    {2, 0, 0, 3, 2, 1, 3, 2, 1, 2, 2, 1, 1, 3, 1, 2, 3, 0, 1, 0, 3, 1, 3, 2, 2, 2},
    {2, 1, 0, 1, 2, 2, 2, 0, 1, 2, 0, 0, 3, 2, 0, 1, 2, 1, 3, 3, 2, 0, 1, 2, 2, 2},
    {2, 1, 1, 3, 1, 3, 0, 1, 1, 1, 1, 3, 0, 3, 0, 3, 1, 2, 0, 2, 2, 1, 1, 1, 2, 2},
    {2, 2, 1, 0, 2, 0, 1, 1, 2, 1, 2, 0, 1, 2, 2, 1, 2, 3, 0, 3, 0, 2, 2, 0, 1, 2},
    {2, 3, 0, 3, 1, 3, 1, 1, 3, 1, 3, 3, 1, 3, 1, 2, 1, 0, 1, 0, 3, 2, 0, 3, 1, 3},
    {2, 0, 2, 0, 3, 2, 2, 1, 0, 2, 0, 2, 2, 0, 2, 2, 0, 1, 1, 3, 0, 2, 1, 0, 0, 3},
    {2, 1, 1, 3, 0, 2, 2, 0, 3, 1, 3, 0, 2, 3, 1, 2, 1, 0, 1, 1, 1, 3, 2, 1, 2, 2},
    {2, 2, 2, 0, 2, 2, 0, 3, 2, 0, 1, 3, 0, 2, 2, 1, 0, 3, 1, 2, 1, 2, 0, 3, 1, 2},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

Coordenadas CaminhoCorreto[80] = {
    {0,-2}, {0,-1}, {0,0}, {-1, 0}, {-1,1}, {-1,2}, {-1,3}, {0,3}, {0,2}, {1,2}, {1,3}, {2,3}, {2,4}, {3,4}, {3,5},
    {4,5}, {4,6}, {5,6}, {5,5}, {5,4}, {6,4}, {6,3}, {7,3}, {7,2}, {8,2}, {8,3}, {8,4}, {7,4}, {7,5}, {7,6},
    {7,7}, {6,7}, {5,7}, {4,7}, {4,8}, {3,8}, {3,9}, {3,10}, {3,11}, {2,11}, {2,10}, {1,10}, {0,10}, {0,11},
    {-1,11}, {-1,10}, {-1,9},{-2,9}, {-2,10}, {-3,10}, {-4,10}, {-5,10}, {-5,11}, {-4,11}, {-3,11}, {-3,12},
    {-2,12},{-2,13}, {-3,13}, {-4,13}, {-5,13}, {-5,14}, {-4,14}, {-4,15}, {-3,15}, {-3,16}, {-2,16}, {-2,17},
    {-1,17}, {0,17}, {0,18}, {0,19}, {0,20}, {0,21}
};

float posXFree , posZFree, posXLookAt , posZLookAt ;
glm::vec4 camera_view_vector_backup;
glm::vec4 move_direction_backup;

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "INF01047 - Trabalho Final FCG", NULL, NULL);

    //Tela cheia
    //window = glfwCreateWindow(1366, 800, "INF01047 - Trabalho Final FCG", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << error << std::endl;
    }

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, SCREEN_WIDTH, SCREEN_HEIGHT); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    LoadTextureImage("../../data/textures/wallTexture.jpg");
    LoadTextureImage("../../data/textures/grassTexture.jpg");
    LoadTextureImage("../../data/textures/moonTexture.jpg");
    LoadTextureImage("../../data/textures/sunTexture.jpg");
    LoadTextureImage("../../data/textures/sunCloudsTexture.jpg");
    LoadTextureImage("../../data/textures/gold1Texture.jpg");
    LoadTextureImage("../../data/textures/flashlightTexture.jpg");
    LoadTextureImage("../../data/textures/gun1Texture.jpg");
    LoadTextureImage("../../data/textures/gun2Texture.jpg");
    LoadTextureImage("../../data/textures/mapTexture.png");


    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ObjModel spheremodel("../../data/objs/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel("../../data/objs/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);

    ObjModel planemodel("../../data/objs/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel cowModel("../../data/objs/cow.obj");
    ComputeNormals(&cowModel);
    BuildTrianglesAndAddToVirtualScene(&cowModel);

    ObjModel flashlightModel("../../data/objs/flashlight.obj");
    ComputeNormals(&flashlightModel);
    BuildTrianglesAndAddToVirtualScene(&flashlightModel);

    ObjModel gunModel("../../data/objs/gun.obj");
    ComputeNormals(&gunModel);
    BuildTrianglesAndAddToVirtualScene(&gunModel);

    ObjModel cubeModel("../../data/objs/cube.obj");
    ComputeNormals(&cubeModel);
    BuildTrianglesAndAddToVirtualScene(&cubeModel);

    // Construímos a representação de um triângulo
    GLuint vertex_array_object_id = BuildTriangles();

    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    GLint render_as_black_uniform = glGetUniformLocation(g_GpuProgramID, "render_as_black"); // Variável booleana em shader_vertex.glsl

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Computamos a posição da câmera utilizando coordenadas esféricas.  As
    // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
    // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
    // e ScrollCallback().
    float r = g_CameraDistance;
    float y = r*sin(g_CameraPhi);
    float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
    float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

    // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
    // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::vec4 player_position = glm::vec4(0, y-0.495, 0, 1.0f);
    camera_position_c = player_position;
    camera_position_c.y += 0.5f;
    glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    glm::vec4 camera_view_vector = normalize(glm::vec4(x, y, z, 0.0f));
    glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    float prev_time = (float)glfwGetTime();
    float delta_t;
    glm::mat4 model = Matrix_Identity();

    int posCoelhoX[10], posCoelhoZ[10];

    for(int i=0 ; i<10 ; i++){
        posCoelhoX[i] = rand()%24 + 1;
        posCoelhoZ[i] = rand()%17 + 2;
    }
    float cor_r=1,cor_g=1,cor_b=1;
    int tempo = 0;
    float segundosCicloDia = 30;

    Wall wall;
    for(int i=0; i<22; i++){
        for(int j=0; j<26; j++){
            if(paredes[i][j] == XYWALL){
                wall.position = glm::vec3(j-13.5, -0.5f, i - 19.5);
                wall.dir = CUBEXY;
                wall.model = Matrix_Translate(wall.position.x, wall.position.y, wall.position.z)*
                             Matrix_Scale(1.0f, 1.0f, 0.01f);
            }
            else if(paredes[i][j] == YZWALL){
                wall.position = glm::vec3(j-12.5, -0.5f, i - 19.5);
                wall.dir = CUBEYZ;
                wall.model = Matrix_Translate(wall.position.x, wall.position.y, wall.position.z)*
                             Matrix_Scale(0.01f, 1.0f, 1.0f);
            }
            else if(paredes[i][j] == XY_YZWALL){
                wall.position = glm::vec3(j-13.5, -0.5f, i - 19.5);
                wall.dir = CUBEXY;
                wall.model = Matrix_Translate(wall.position.x, wall.position.y, wall.position.z)*
                             Matrix_Scale(1.0f, 1.0f, 0.01f);
                walls.push_back(wall);

                wall.position = glm::vec3(j-12.5, -0.5f, i - 19.5);
                wall.dir = CUBEYZ;
                wall.model = Matrix_Translate(wall.position.x, wall.position.y, wall.position.z)*
                             Matrix_Scale(0.01f, 1.0f, 1.0f);
            }
            if (paredes[i][j] != 0)
                walls.push_back(wall);
        }
    }

    std::cout << walls.size() << std::endl;
    for(auto wall : walls){
        //std::cout << wall.position.x << " " << wall.position.y << " " << wall.position.z << std::endl;
    }

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        //Enviamos os segundos, que controlará a posição da luz
        float seconds = glfwGetTime();
        glUniform1f(g_ligth_pos_uniform, seconds);
        glUniform1i(g_segundos_ciclo_dia, segundosCicloDia);

        // Computação da posição da câmera
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float x = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float z = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Atualiza delta de tempo
        float current_time = (float)glfwGetTime();
        delta_t = current_time - prev_time;
        prev_time = current_time;

        if (tecla_L_pressionada){   //Camera Look-At
            camera_position_c  = glm::vec4(x, y, z-23, 1.0f); // Ponto "c", centro da câmera
            camera_lookat_l    = glm::vec4(0.0f,0.0f,-24.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
            camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada

        } else {    //Camera Normal
            // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
            camera_view_vector = glm::vec4(x, y, z, 0.0f); // Vetor "view", sentido para onde a câmera está virada

            glm::vec4 vetor_w = -camera_view_vector / norm(camera_view_vector);
            if (!tecla_C_pressionada){  //Camera Livre
                vetor_w.y = 0.0f;
                camera_position_c.y=0;
            }
            glm::vec4 vetor_u = crossproduct(camera_up_vector, vetor_w) / norm(crossproduct(camera_up_vector, vetor_w));

            if (tecla_W_pressionada){
                // Movimenta câmera para frente
                camera_position_c += -vetor_w * speed * delta_t;
            }

            if (tecla_A_pressionada){
                // Movimenta câmera para esquerda
                camera_position_c += -vetor_u * speed * delta_t;
            }

            if (tecla_S_pressionada){
                // Movimenta câmera para trás
                camera_position_c += vetor_w * speed * delta_t;
            }

            if (tecla_D_pressionada){
                // Movimenta câmera para direita
                camera_position_c += vetor_u * speed * delta_t;
            }
        }

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);


        glUniform1f(flashligth_pos_x, camera_position_c[0]);
        glUniform1f(flashligth_pos_y, camera_position_c[1]);
        glUniform1f(flashligth_pos_z, camera_position_c[2]);

        glUniform1f(flashligth_dir_x, camera_view_vector[0]);
        glUniform1f(flashligth_dir_y, camera_view_vector[1]);
        glUniform1f(flashligth_dir_z, camera_view_vector[2]);

        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A



        if (tempo - int(seconds) != 0){
            tempo++;
        }

        if ( tempo%(int(segundosCicloDia)*2) > segundosCicloDia*0.75 && tempo%(int(segundosCicloDia)*2) < int(segundosCicloDia)){
            cor_r = 1 - (1/segundosCicloDia * (tempo%(int(segundosCicloDia))));
            cor_g = 1 - (1/segundosCicloDia * (tempo%(int(segundosCicloDia))));
            cor_b = 1 - (1/segundosCicloDia * (tempo%(int(segundosCicloDia))));
        } else if ( tempo%(int(segundosCicloDia)*2) > segundosCicloDia*1.75){
            cor_r = 1/segundosCicloDia * (tempo%(int(segundosCicloDia)));
            cor_g = 1/segundosCicloDia * (tempo%(int(segundosCicloDia)));
            cor_b = 1/segundosCicloDia * (tempo%(int(segundosCicloDia)));
        }
        glClearColor(cor_r, cor_g, cor_b, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -35.0f; // Posição do "far plane"

        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        //Desenhamos o modelo do sol
        model = Matrix_Identity();
        model = Matrix_Translate(-cos(M_PI/segundosCicloDia*seconds)*19, sin(M_PI/segundosCicloDia*seconds)*19, -13.0f)
                * Matrix_Rotate_Y(g_AngleY/10)
                * Matrix_Rotate_Z(g_AngleY/5)
                * Matrix_Rotate_X(g_AngleY/10)
                * Matrix_Scale(3.0f, 3.0f, 3.0f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, SUN);
        DrawVirtualObject("the_sphere");

        //std::cout << sin(M_PI/15*seconds) << std::endl;

        //Desenhamos o modelo da lua
        model = Matrix_Identity();
        model = Matrix_Translate(cos(M_PI/segundosCicloDia*seconds)*19, -sin(M_PI/segundosCicloDia*seconds)*19, -13.0f)
                * Matrix_Rotate_Y(g_AngleY/10)
                * Matrix_Rotate_Z(g_AngleY/5)
                * Matrix_Rotate_X(g_AngleY/10)
                * Matrix_Scale(2.0f, 2.0f, 2.0f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, MOON);
        DrawVirtualObject("the_sphere");


         // Desenho do labirinto
        for(Wall wall : walls){
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(wall.model));
            glUniform1i(g_object_id_uniform, wall.dir);
            DrawVirtualObject("the_cube");
        }

        //Desenha o chão
        model = Matrix_Identity();
        model = Matrix_Translate(-13, -0.505f, -25.5 )*
                Matrix_Scale(26.0f, 0.01f, 27.0f);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_cube");

        glBindVertexArray(vertex_array_object_id);
        //Desenha o caminho correto
        if(tecla_R_pressionada){
            model = Matrix_Identity();
            for(int i=0; i< 80; i++){
                //Correção para não ficar exatamente sobreposto nas paredes: 0.005
                model = Matrix_Translate(CaminhoCorreto[i].x-0.005, -0.495f, -CaminhoCorreto[i].z-1+0.005) *
                        Matrix_Scale(1.0f-0.005, 0.01f, 1.0f-0.005);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, PATH);
                DrawCube(render_as_black_uniform);
            }
        }

         // Desenhamos o modelo dos coelhos
        for(int i=0; i< 10; ++i){
            model = Matrix_Identity();
            model = Matrix_Translate(posCoelhoX[i] - 13, -0.4f , -posCoelhoZ[i])
                    * Matrix_Scale(0.1f, 0.1f, 0.1f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, BUNNY);
            DrawVirtualObject("the_bunny");
        }


        // Desenhamos o modelo da vaca
        model = Matrix_Identity();
        model = Matrix_Translate(0.0f, 0.1f, -24.0f)
                * Matrix_Rotate_Y(g_AngleY);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, COW);
        DrawVirtualObject("the_cow");
        g_AngleY += 0.5 * delta_t;


        //Desenhamos o mapa do labirinto, grudada na tela
        if(tecla_M_pressionada){
            glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(Matrix_Identity()));
            model = Matrix_Identity();
            model = Matrix_Translate(-0.04, -0.055, -0.1)
                    * Matrix_Scale(0.05f, 0.05f, 0.001f)
                    * Matrix_Rotate_Y(M_PI/2);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, MAP);
            glClear(GL_DEPTH_BUFFER_BIT);
            DrawVirtualObject("the_cube");
        }


        glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(Matrix_Identity()));
        if(tecla_TAB_pressionada == 1){
            //Desenhamos o modelo da lanterna, grudada na tela
            model = Matrix_Translate(0.25, -0.35, -0.7)
                * Matrix_Scale(0.1f, 0.1f, 0.1f)
                * Matrix_Rotate_X(-M_PI);

            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, FLASHLIGHT);
            glClear(GL_DEPTH_BUFFER_BIT);
            DrawVirtualObject("the_flashlight");
        } else {
            tecla_F_pressionada = 0;
            //Desenhamos o modelo da arma, grudada na tela
            model = Matrix_Translate(0.25, -0.3, -0.8)
                    * Matrix_Scale(0.03f, 0.03f, 0.03f)
                    * Matrix_Rotate_Y(M_PI/2);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, GUN);
            glClear(GL_DEPTH_BUFFER_BIT);
            DrawVirtualObject("the_gun");
        }



        if (tecla_F_pressionada){
            glUniform1i(g_flashlight_on_uniform, 1);
        } else {
            glUniform1i(g_flashlight_on_uniform, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

void DrawCube(GLint render_as_black_uniform)
{
    // Informamos para a placa de vídeo (GPU) que a variável booleana
    // "render_as_black" deve ser colocada como "false". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, false);

    // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
    // VAO como triângulos, formando as faces do cubo. Esta
    // renderização irá executar o Vertex Shader definido no arquivo
    // "shader_vertex.glsl", e o mesmo irá utilizar as matrizes
    // "model", "view" e "projection" definidas acima e já enviadas
    // para a placa de vídeo (GPU).
    //
    // Veja a definição de g_VirtualScene["cube_faces"] dentro da
    // função BuildTriangles(), e veja a documentação da função
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_faces"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
        g_VirtualScene["cube_faces"].num_indices,    //
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_faces"].first_index
    );

    // Pedimos para OpenGL desenhar linhas com largura de 4 pixels.
    glLineWidth(4.0f);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene["axes"] dentro da função BuildTriangles(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    //
    // Importante: estes eixos serão desenhamos com a matriz "model"
    // definida acima, e portanto sofrerão as mesmas transformações
    // geométricas que o cubo. Isto é, estes eixos estarão
    // representando o sistema de coordenadas do modelo (e não o global)!
    /*glDrawElements(
        g_VirtualScene["axes"].rendering_mode,
        g_VirtualScene["axes"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["axes"].first_index
    );*/

    // Informamos para a placa de vídeo (GPU) que a variável booleana
    // "render_as_black" deve ser colocada como "true". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, true);

    // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
    // VAO como linhas, formando as arestas pretas do cubo. Veja a
    // definição de g_VirtualScene["cube_edges"] dentro da função
    // BuildTriangles(), e veja a documentação da função
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_edges"].rendering_mode,
        g_VirtualScene["cube_edges"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_edges"].first_index
    );
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl

    g_ligth_pos_uniform = glGetUniformLocation(g_GpuProgramID, "ligth_pos");
    g_segundos_ciclo_dia = glGetUniformLocation(g_GpuProgramID, "segundosCicloDia");
    g_flashlight_on_uniform = glGetUniformLocation(g_GpuProgramID, "flashlight_on");

    flashligth_pos_x = glGetUniformLocation(g_GpuProgramID, "flashligth_pos_x");
    flashligth_pos_y = glGetUniformLocation(g_GpuProgramID, "flashligth_pos_y");
    flashligth_pos_z = glGetUniformLocation(g_GpuProgramID, "flashligth_pos_z");

    flashligth_dir_x = glGetUniformLocation(g_GpuProgramID, "flashligth_dir_x");
    flashligth_dir_y = glGetUniformLocation(g_GpuProgramID, "flashligth_dir_y");
    flashligth_dir_z = glGetUniformLocation(g_GpuProgramID, "flashligth_dir_z");

    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");

    // Variáveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "wallTexture"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "grassTexture"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "moonTexture"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "sunTexture"), 3);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "sunCloudsTexture"), 4);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "goldTexture"), 5);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "flashlightTexture"), 6);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "gun1Texture"), 7);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "gun2Texture"), 8);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "mapTexture"), 9);


    glUseProgram(0);
}

// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            const glm::vec4  u = c-a;
            const glm::vec4  v = b-a;

            // PREENCHA AQUI o cálculo da normal de um triângulo cujos vértices
            // estão nos pontos "a", "b", e "c", definidos no sentido anti-horário.
            const glm::vec4  n = crossproduct(v,u);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

GLuint BuildTriangles()
{
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
    // homogêneas.  Veja as seguintes referências:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
    // Vértices de um cubo
    //    X      Y     Z     W
        -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 0
        -0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 1
         0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 2
         0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 3
        -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 4
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 5
         0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 6
         0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 7
    // Vértices para desenhar o eixo X
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 8
         1.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 9
    // Vértices para desenhar o eixo Y
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 10
         0.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 11
    // Vértices para desenhar o eixo Z
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 12
         0.0f,  0.0f,  1.0f, 1.0f, // posição do vértice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posição.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_model_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    GLfloat color_coefficients[] = {
    // Cores dos vértices do cubo
    //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 7
    // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 9
    // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 11
    // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir polígonos utilizando os vértices do array
    // model_coefficients.
    //
    // Para referência sobre os modos de renderização, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint indices[] = {
    // Definimos os índices dos vértices que definem as FACES de um cubo
    // através de 12 triângulos que serão desenhados com o modo de renderização
    // GL_TRIANGLES.
        0, 1, 2, // triângulo 1
        7, 6, 5, // triângulo 2
        3, 2, 6, // triângulo 3
        4, 0, 3, // triângulo 4
        4, 5, 1, // triângulo 5
        1, 5, 6, // triângulo 6
        0, 2, 3, // triângulo 7
        7, 5, 4, // triângulo 8
        3, 6, 7, // triângulo 9
        4, 3, 7, // triângulo 10
        4, 1, 0, // triângulo 11
        1, 6, 2, // triângulo 12
    // Definimos os índices dos vértices que definem as ARESTAS de um cubo
    // através de 12 linhas que serão desenhadas com o modo de renderização
    // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
    // Z, que serão desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = 0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 36;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere às arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (36*sizeof(GLuint)); // Primeiro índice está em indices[36]
    cube_edges.num_indices    = 24; // Último índice está em indices[59]; total de 24 índices.
    cube_edges.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (60*sizeof(GLuint)); // Primeiro índice está em indices[60]
    axes.num_indices    = 6; // Último índice está em indices[65]; total de 6 índices.
    axes.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.
    g_VirtualScene["axes"] = axes;

    // Criamos um buffer OpenGL para armazenar os índices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name           = model->shapes[shape].name;
        theobject.first_index    = first_index; // Primeiro índice
        theobject.num_indices    = last_index - first_index + 1; // Número de indices
        theobject.rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}


// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.


       if (xpos != SCREEN_WIDTH/2 || ypos != SCREEN_HEIGHT/2){
            float dx = xpos - SCREEN_WIDTH/2;
            float dy = SCREEN_HEIGHT/2 - ypos;

            // Atualizamos parâmetros da câmera com os deslocamentos
            g_CameraTheta += 0.005f*dx;
            g_CameraPhi      += 0.005f*dy;

            // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
            float phimax = M_PI_2;
            float phimin = -phimax;

            if (g_CameraPhi > phimax)
                g_CameraPhi = phimax;

            if (g_CameraPhi < phimin)
                g_CameraPhi = phimin;

            // Atualizamos as variáveis globais para armazenar a posição atual do
            // cursor como sendo a última posição conhecida do cursor.
            g_LastCursorPosX = xpos;
            g_LastCursorPosY = ypos;

            glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
        }

}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ===================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ===================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }

     if (key == GLFW_KEY_W ){ // Movimento para frente
        if (action == GLFW_PRESS) {
            tecla_W_pressionada = true;
        } if(action == GLFW_RELEASE) {
            tecla_W_pressionada = false;
        }
    }

    if (key == GLFW_KEY_A) { // Movimento para a esquerda
        if (action == GLFW_PRESS) {
            tecla_A_pressionada = true;
        } if(action == GLFW_RELEASE) {
            tecla_A_pressionada = false;
        }
    }

    if (key == GLFW_KEY_S) { // Movimento para tras
        if (action == GLFW_PRESS) {
            tecla_S_pressionada = true;
        } if(action == GLFW_RELEASE) {
            tecla_S_pressionada = false;
        }
    }

    if (key == GLFW_KEY_D) { // Movimento para a direita
        if (action == GLFW_PRESS) {
            tecla_D_pressionada = true;
        } if(action == GLFW_RELEASE) {
            tecla_D_pressionada = false;
        }
    }

    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        if (action == GLFW_PRESS)
            speed = 6.0f;
        else if (action == GLFW_RELEASE)
            speed = 3.0f;
    }


    if (key == GLFW_KEY_R && action == GLFW_PRESS && (mod & GLFW_MOD_SHIFT))
    {
        tecla_R_pressionada = !tecla_R_pressionada;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        tecla_F_pressionada = !tecla_F_pressionada;
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        tecla_M_pressionada = !tecla_M_pressionada;
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS && (mod & GLFW_MOD_SHIFT))
    {
        if(!tecla_C_pressionada){
            posXFree = camera_position_c.x;
            posZFree = camera_position_c.z;
            g_CameraPhi_backup_Free = g_CameraPhi;
            g_CameraTheta_backup_Free = g_CameraTheta;
            //move_direction_backup
        } else{
            camera_position_c.x = posXFree;
            camera_position_c.z = posZFree;
            g_CameraPhi = g_CameraPhi_backup_Free;
            g_CameraTheta = g_CameraTheta_backup_Free;
        }
        tecla_C_pressionada = !tecla_C_pressionada;
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS && (mod & GLFW_MOD_SHIFT))
    {
        if(!tecla_L_pressionada){
            posXLookAt = camera_position_c.x;
            posZLookAt = camera_position_c.z;
            g_CameraPhi_backup_lookAt = g_CameraPhi;
            g_CameraTheta_backup_lookAt = g_CameraTheta;
        } else{
            camera_position_c.x = posXLookAt;
            camera_position_c.z = posZLookAt;
            g_CameraPhi = g_CameraPhi_backup_lookAt;
            g_CameraTheta = g_CameraTheta_backup_lookAt;
        }

        tecla_L_pressionada = !tecla_L_pressionada;
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        tecla_TAB_pressionada = !tecla_TAB_pressionada;
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.
// void TextRendering_ShowModelViewProjection(
//     GLFWwindow* window,
//     glm::mat4 projection,
//     glm::mat4 view,
//     glm::mat4 model,
//     glm::vec4 p_model
// )
// {
//     if ( !g_ShowInfoText )
//         return;

//     glm::vec4 p_world = model*p_model;
//     glm::vec4 p_camera = view*p_world;
//     glm::vec4 p_clip = projection*p_camera;
//     glm::vec4 p_ndc = p_clip / p_clip.w;

//     float pad = TextRendering_LineHeight(window);

//     TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
//     TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

//     TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
//     TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
//     TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

//     TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
//     TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

//     TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
//     TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
//     TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

//     TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
//     TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

//     int width, height;
//     glfwGetFramebufferSize(window, &width, &height);

//     glm::vec2 a = glm::vec2(-1, -1);
//     glm::vec2 b = glm::vec2(+1, +1);
//     glm::vec2 p = glm::vec2( 0,  0);
//     glm::vec2 q = glm::vec2(width, height);

//     glm::mat4 viewport_mapping = Matrix(
//         (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
//         0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
//         0.0f , 0.0f , 1.0f , 0.0f ,
//         0.0f , 0.0f , 0.0f , 1.0f
//     );

//     TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
//     TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
//     TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

//     TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
//     TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
// }

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
// void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
// {
//     if ( !g_ShowInfoText )
//         return;

//     // Variáveis estáticas (static) mantém seus valores entre chamadas
//     // subsequentes da função!
//     static float old_seconds = (float)glfwGetTime();
//     static int   ellapsed_frames = 0;
//     static char  buffer[20] = "?? fps";
//     static int   numchars = 7;

//     ellapsed_frames += 1;

//     // Recuperamos o número de segundos que passou desde a execução do programa
//     float seconds = (float)glfwGetTime();

//     // Número de segundos desde o último cálculo do fps
//     float ellapsed_seconds = seconds - old_seconds;

//     if ( ellapsed_seconds > 1.0f )
//     {
//         numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

//         old_seconds = seconds;
//         ellapsed_frames = 0;
//     }

//     float lineheight = TextRendering_LineHeight(window);
//     float charwidth = TextRendering_CharWidth(window);

//     TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
// }

// Função para debugging: imprime no terminal todas informações de um modelo
// geométrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
// void PrintObjModelInfo(ObjModel* model)
// {
//   const tinyobj::attrib_t                & attrib    = model->attrib;
//   const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
//   const std::vector<tinyobj::material_t> & materials = model->materials;

//   printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
//   printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
//   printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
//   printf("# of shapes    : %d\n", (int)shapes.size());
//   printf("# of materials : %d\n", (int)materials.size());

//   for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
//     printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
//            static_cast<const double>(attrib.vertices[3 * v + 0]),
//            static_cast<const double>(attrib.vertices[3 * v + 1]),
//            static_cast<const double>(attrib.vertices[3 * v + 2]));
//   }

//   for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
//     printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
//            static_cast<const double>(attrib.normals[3 * v + 0]),
//            static_cast<const double>(attrib.normals[3 * v + 1]),
//            static_cast<const double>(attrib.normals[3 * v + 2]));
//   }

//   for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
//     printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
//            static_cast<const double>(attrib.texcoords[2 * v + 0]),
//            static_cast<const double>(attrib.texcoords[2 * v + 1]));
//   }

//   // For each shape
//   for (size_t i = 0; i < shapes.size(); i++) {
//     printf("shape[%ld].name = %s\n", static_cast<long>(i),
//            shapes[i].name.c_str());
//     printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
//            static_cast<unsigned long>(shapes[i].mesh.indices.size()));

//     size_t index_offset = 0;

//     assert(shapes[i].mesh.num_face_vertices.size() ==
//            shapes[i].mesh.material_ids.size());

//     printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
//            static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

//     // For each face
//     for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
//       size_t fnum = shapes[i].mesh.num_face_vertices[f];

//       printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
//              static_cast<unsigned long>(fnum));

//       // For each vertex in the face
//       for (size_t v = 0; v < fnum; v++) {
//         tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
//         printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
//                static_cast<long>(v), idx.vertex_index, idx.normal_index,
//                idx.texcoord_index);
//       }

//       printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
//              shapes[i].mesh.material_ids[f]);

//       index_offset += fnum;
//     }

//     printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
//            static_cast<unsigned long>(shapes[i].mesh.tags.size()));
//     for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
//       printf("  tag[%ld] = %s ", static_cast<long>(t),
//              shapes[i].mesh.tags[t].name.c_str());
//       printf(" ints: [");
//       for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
//         printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
//         if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
//           printf(", ");
//         }
//       }
//       printf("]");

//       printf(" floats: [");
//       for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
//         printf("%f", static_cast<const double>(
//                          shapes[i].mesh.tags[t].floatValues[j]));
//         if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
//           printf(", ");
//         }
//       }
//       printf("]");

//       printf(" strings: [");
//       for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
//         printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
//         if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
//           printf(", ");
//         }
//       }
//       printf("]");
//       printf("\n");
//     }
//   }

//   for (size_t i = 0; i < materials.size(); i++) {
//     printf("material[%ld].name = %s\n", static_cast<long>(i),
//            materials[i].name.c_str());
//     printf("  material.Ka = (%f, %f ,%f)\n",
//            static_cast<const double>(materials[i].ambient[0]),
//            static_cast<const double>(materials[i].ambient[1]),
//            static_cast<const double>(materials[i].ambient[2]));
//     printf("  material.Kd = (%f, %f ,%f)\n",
//            static_cast<const double>(materials[i].diffuse[0]),
//            static_cast<const double>(materials[i].diffuse[1]),
//            static_cast<const double>(materials[i].diffuse[2]));
//     printf("  material.Ks = (%f, %f ,%f)\n",
//            static_cast<const double>(materials[i].specular[0]),
//            static_cast<const double>(materials[i].specular[1]),
//            static_cast<const double>(materials[i].specular[2]));
//     printf("  material.Tr = (%f, %f ,%f)\n",
//            static_cast<const double>(materials[i].transmittance[0]),
//            static_cast<const double>(materials[i].transmittance[1]),
//            static_cast<const double>(materials[i].transmittance[2]));
//     printf("  material.Ke = (%f, %f ,%f)\n",
//            static_cast<const double>(materials[i].emission[0]),
//            static_cast<const double>(materials[i].emission[1]),
//            static_cast<const double>(materials[i].emission[2]));
//     printf("  material.Ns = %f\n",
//            static_cast<const double>(materials[i].shininess));
//     printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
//     printf("  material.dissolve = %f\n",
//            static_cast<const double>(materials[i].dissolve));
//     printf("  material.illum = %d\n", materials[i].illum);
//     printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
//     printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
//     printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
//     printf("  material.map_Ns = %s\n",
//            materials[i].specular_highlight_texname.c_str());
//     printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
//     printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
//     printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
//     printf("  <<PBR>>\n");
//     printf("  material.Pr     = %f\n", materials[i].roughness);
//     printf("  material.Pm     = %f\n", materials[i].metallic);
//     printf("  material.Ps     = %f\n", materials[i].sheen);
//     printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
//     printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
//     printf("  material.aniso  = %f\n", materials[i].anisotropy);
//     printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
//     printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
//     printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
//     printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
//     printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
//     printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
//     std::map<std::string, std::string>::const_iterator it(
//         materials[i].unknown_parameter.begin());
//     std::map<std::string, std::string>::const_iterator itEnd(
//         materials[i].unknown_parameter.end());

//     for (; it != itEnd; it++) {
//       printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
//     }
//     printf("\n");
//   }
// }

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

