#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define PATH   3
#define COW    4
#define FLASHLIGHT    5
#define SUN    6
#define MOON    7
#define CUBE    8

uniform int object_id;

uniform int flashlight_on;
uniform int segundosCicloDia;
uniform float ligth_pos;

uniform float flashligth_pos_x;
uniform float flashligth_pos_y;
uniform float flashligth_pos_z;

uniform float flashligth_dir_x;
uniform float flashligth_dir_y;
uniform float flashligth_dir_z;


// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

# define M_PI          3.141592653589793238462643383279502884
void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    //Spotlight
    vec4 pontoL = vec4(camera_position.x, camera_position.y, camera_position.z, 1.0); //representa o ponto onde está localizada a fonte de luz
    vec4 direcao = normalize(vec4(flashligth_dir_x, flashligth_dir_y, flashligth_dir_z, 0.0)); //representa o vetor que indica o sentido da iluminação spotlight.
    float abertura = cos(M_PI/15); //12 graus

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    float cos_pos_light = cos(M_PI/segundosCicloDia*ligth_pos);
    float sin_pos_light = sin(M_PI/segundosCicloDia*ligth_pos);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //vec4 l = normalize(vec4(1.0,1.0,0.5,0.0)); //Luz fixa
    //vec4 l = normalize(camera_position - p); //Camera é a luz
    vec4 lFlash = normalize(pontoL - p)/(max(length(pontoL-p),1));    //Luz Spotlight
    vec4 lTimeSun = normalize(vec4(-cos_pos_light, sin_pos_light, 0.0, 0.0));   //Luz sol
    vec4 lTimeMoon = normalize(vec4(cos_pos_light, -sin_pos_light, 0.0,0.0));  //Luz lua

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);


    // Vetor que define o sentido da reflexão especular ideal.
    vec4 rFlash = -lFlash + 2*n*dot(n, lFlash); // PREENCHA AQUI o vetor de reflexão especular ideal
    vec4 rTimeSun = -lTimeSun + 2*n*dot(n, lTimeSun); // PREENCHA AQUI o vetor de reflexão especular ideal
    vec4 rTimeMoon = -lTimeMoon + 2*n*dot(n, lTimeMoon); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong


    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = vec3(0.8, 0.4, 0.08);
        Ks = vec3(0.0, 0.0, 0.0);
        Ka = Kd/2;
        q = 1.0;
    }
    else if ( object_id == BUNNY)
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.08, 0.4, 0.8);
        Ks = vec3(0.8 , 0.8, 0.8);
        Ka = Kd/2;
        q = 32.0;
    }
    else if ( object_id == PLANE )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(0.5, 0.5, 0.5);
        Ks = vec3(0.6, 0.6, 0.6);
        Ka = Kd/2;
        q = 20.0;
    } else if ( object_id == PATH )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(1.0, 0.0, 0.0);
        Ks = vec3(1.0, 0.0, 0.0);
        Ka = vec3(1.0, 0.0, 0.0);
        q = 1.0;
    } else if ( object_id == COW)
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.7, 0.7, 0.0);
        Ks = vec3(1.0 , 1.0, 0.0);
        Ka = Kd/2;
        q = 32.0;
    }
    else if ( object_id == FLASHLIGHT )
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.2, 0.2, 0.2);
        Ks = vec3(0.3, 0.3, 0.3);
        Ka = Kd/2;
        q = 20.0;
    } else if ( object_id == SUN)
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(1.0, 1.0, 0.0);
        Ks = vec3(1.0 , 1.0, 0.0);
        if (cos_pos_light > -0.80710678118)
            Ka = Kd;
        else
            Ka = Kd/1.5;
        q = 32.0;
    }
    else if ( object_id == MOON)
    {
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.0 , 0.0, 0.0);
        Ks = vec3(0.0 , 0.0, 0.0);
        Ka = vec3(0.9, 0.9, 1.0);
        q = 1.0;
    }
    else if (object_id == CUBE){
        Kd = vec3(0.6, 0.6, 0.2);
        Ks = vec3(0.6, 0.6, 0.2);
        Ka = Kd/2;
        q = 20.0;
    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I_flash = vec3(1.0, 1.0, 1.0);
    vec3 I_sun = vec3(1.0, 0.95, 0.8); 
    vec3 I_moon = vec3(0.5, 0.5, 0.55);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2, 0.2, 0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term_flash = Kd*I_flash*max(0, dot(n, lFlash)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_sun = Kd*I_sun*max(0, dot(n, lTimeSun)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_moon = Kd*I_moon*max(0, dot(n, lTimeMoon)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term = Ka*Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term_flash  = Ks*I_flash*pow(max(0, dot(rFlash,v)), q); // PREENCHA AQUI o termo especular de Phong
    vec3 phong_specular_term_time_sun  = Ks*I_sun*pow(max(0, dot(rTimeSun,v)), q); // PREENCHA AQUI o termo especular de Phong
    vec3 phong_specular_term_time_moon  = Ks*I_moon*pow(max(0, dot(rTimeMoon,v)), q); // PREENCHA AQUI o termo especular de Phong

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    vec4 color_flash = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 color_time_sun = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 color_time_moon = vec4(0.0, 0.0, 0.0, 1.0);

    if (flashlight_on == 1 && object_id != FLASHLIGHT){
        float cos_angle = dot(normalize(p - pontoL), normalize(direcao));
        if(cos_angle >= abertura){
            color_flash.rgb = (lambert_diffuse_term_flash + phong_specular_term_flash)*(pow(cos_angle, 5));
         }
    }

    if(sin_pos_light > 0.0){
        color_time_sun.rgb = sin_pos_light*lambert_diffuse_term_time_sun + phong_specular_term_time_sun;
    }
    if(sin_pos_light < 0.0){
        color_time_moon.rgb = sin_pos_light*lambert_diffuse_term_time_moon + phong_specular_term_time_moon;
    }

    color.rgb = color_flash.rgb + color_time_sun.rgb + color_time_moon.rgb + ambient_term;

    if(object_id == MOON || object_id == PATH ){
        color.rgb = Ka*vec3(1.0, 1.0, 1.0);
    }
    if(object_id == SUN){
        if(cos_pos_light < 0){
            color.rgb = Ka*vec3(1.0, max(sin_pos_light, 0.3), max(sin_pos_light,0.3));
        }
        else{
            color.rgb = Ka*vec3(1.0, max(sin_pos_light, 0.7), max(sin_pos_light,0.7));
        }

    }

    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);

}

