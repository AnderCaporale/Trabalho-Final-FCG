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
    vec4 pontoL = vec4(camera_position[0], camera_position[1], camera_position[2], 1.0); //representa o ponto onde está localizada a fonte de luz
    vec4 direcao = normalize(vec4(flashligth_dir_x, flashligth_dir_y, flashligth_dir_z, 0.0)); //representa o vetor que indica o sentido da iluminação spotlight.
    float abertura = cos(M_PI/18); //10 graus

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    float cos_pos_light = cos(M_PI/segundosCicloDia*ligth_pos);
    float sin_pos_light = sin(M_PI/segundosCicloDia*ligth_pos);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //vec4 l = normalize(vec4(1.0,1.0,0.5,0.0)); //Luz fixa
    //vec4 l = normalize(camera_position - p); //Camera é a luz
    vec4 lFlash = normalize(pontoL - p);    //Luz Spotlight
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
        Kd = vec3(0.2, 0.2, 0.2);
        Ks = vec3(0.3, 0.3, 0.3);
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
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0, 1.0, 1.0); // PREENCHA AQUI o espectro da fonte de luz

    // Espectro da luz ambiente
    vec3 Ia_flash = vec3(1, 1, 1); // PREENCHA AQUI o espectro da luz ambiente
    vec3 Ia_time = vec3(0.01, 0.01, 0.01); // PREENCHA AQUI o espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term_flash = Kd*I*max(0, dot(n, lFlash)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_sun = Kd*I*max(0, dot(n, lTimeSun)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_moon = Kd*I*max(0, dot(n, lTimeMoon)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term_flash = Ka*Ia_flash; // PREENCHA AQUI o termo ambiente
    vec3 ambient_term_time = Ka*Ia_time; // PREENCHA AQUI o termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term_flash  = Ks*I*pow(max(0, dot(rFlash,v)), q); // PREENCHA AQUI o termo especular de Phong
    vec3 phong_specular_term_time_sun  = Ks*I*pow(max(0, dot(rTimeSun,v)), q); // PREENCHA AQUI o termo especular de Phong
    vec3 phong_specular_term_time_moon  = Ks*I*pow(max(0, dot(rTimeMoon,v)), q); // PREENCHA AQUI o termo especular de Phong

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

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.


    if (flashlight_on == 1){
        if(object_id == FLASHLIGHT){
            if (sin_pos_light > 0 ) {
                color.rgb = sin_pos_light*lambert_diffuse_term_time_sun + ambient_term_time + phong_specular_term_time_sun ;
            } else{
                color.rgb = 0.001*lambert_diffuse_term_time_moon + 0.5*ambient_term_time + phong_specular_term_time_moon;
            }

        } else {
            if(dot(normalize(p - pontoL), normalize(direcao)) >= abertura){
                color.rgb = lambert_diffuse_term_flash + ambient_term_flash + phong_specular_term_flash+
                            sin_pos_light*lambert_diffuse_term_time_sun + ambient_term_time + phong_specular_term_time_sun;
            } else{
                if (sin_pos_light > 0 ) {
                    color.rgb = sin_pos_light*lambert_diffuse_term_time_sun + ambient_term_time + phong_specular_term_time_sun ;
                } else{
                    color.rgb = 0.001*lambert_diffuse_term_time_moon + 0.5*ambient_term_time + phong_specular_term_time_moon;
                }
            }
        }
    } else {
        if (sin_pos_light > 0 ) {
            color.rgb = sin_pos_light*lambert_diffuse_term_time_sun + ambient_term_time + phong_specular_term_time_sun ;
        } else{
            color.rgb = 0.001*lambert_diffuse_term_time_moon + 0.1*ambient_term_time + phong_specular_term_time_moon;

        }
    }

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);

    if(object_id == SUN || object_id == MOON || object_id == PATH ){
        color.rgb = ambient_term_flash;
    }
}

