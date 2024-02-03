#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;
in vec4 position_model;

in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Identificador que define qual objeto está sendo desenhado no momento
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
#define SKY         12

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

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D wallTexture;
uniform sampler2D grassTexture;
uniform sampler2D moonTexture;
uniform sampler2D sunTexture;
uniform sampler2D sunCloudsTexture;
uniform sampler2D goldTexture;
uniform sampler2D flashlightTexture;
uniform sampler2D gun1Texture;
uniform sampler2D gun2Texture;
uniform sampler2D mapTexture;
uniform sampler2D skyDayTexture;
uniform sampler2D skyNightTexture;


// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
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
    float abertura = cos(M_PI/12); //12 graus

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    float cos_pos_light = cos(M_PI/segundosCicloDia*ligth_pos);
    float sin_pos_light = sin(M_PI/segundosCicloDia*ligth_pos);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //vec4 l = normalize(vec4(1.0,1.0,0.5,0.0)); //Luz fixa
    //vec4 l = normalize(camera_position - p); //Camera é a luz
    vec4 lFlash = normalize(pontoL - p)/(max(pow(length(pontoL-p), 2),1));    //Luz Spotlight
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
    float U = 0.0;
    float V = 0.0;
    vec3 debugColor;

    if ( object_id == SPHERE ){
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = vec3(0.8, 0.4, 0.08);
        Ks = vec3(0.0, 0.0, 0.0);
        Ka = Kd/2;
        q = 1.0;
    }
    else if ( object_id == BUNNY){
        // PREENCHA AQUI
        // Propriedades espectrais do coelho
        Kd = vec3(0.08, 0.4, 0.8);
        Ks = vec3(0.8 , 0.8, 0.8);
        Ka = Kd/2;
        q = 32.0;
    }
    else if ( object_id == PLANE ){
        U = (position_world.x);
        V = (position_world.z);

        Kd = texture(grassTexture, vec2(U,V)).rgb;
        Ks = vec3(0.0, 0.0, 0.0);
        Ka = Kd/5;
        q = 20.0;
    } else if ( object_id == PATH ){
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(1.0, 0.0, 0.0);
        Ks = vec3(1.0, 0.0, 0.0);
        Ka = vec3(1.0, 0.0, 0.0);
        q = 1.0;
    } else if ( object_id == COW){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = 5*texture(goldTexture, vec2(U,V)).rgb;
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;
    }
    else if ( object_id == FLASHLIGHT ){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(gun1Texture, vec2(U,V)).rgb + texture(gun2Texture, vec2(U,V)).rgb;
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;

    } else if ( object_id == GUN ){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(flashlightTexture, vec2(U,V)).rgb;
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;
    } else if ( object_id == SUN){
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        float theta = atan(position_model.x, position_model.z);
        float phi = asin(position_model.y);

        U = (theta+M_PI)/(2*M_PI);
        V = (phi+M_PI_2)/M_PI;

        Kd = vec3(0.0, 0.0, 0.0);
        Ka = (texture(sunTexture, vec2(U,V)).rgb + texture(sunCloudsTexture, vec2(U,V)).rgb*0.5)*max(0.01,sin_pos_light)*1.5;
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;

    }
    else if ( object_id == MOON){
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        float theta = atan(position_model.x, position_model.z);
        float phi = asin(position_model.y);

        U = (theta+M_PI)/(2*M_PI);
        V = (phi+M_PI_2)/M_PI;
        Kd = vec3(0.0, 0.0, 0.0);
        Ka = texture(moonTexture, vec2(U,V)).rgb * max(0.01, -sin_pos_light)*3;
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if (object_id == CUBEXY){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(wallTexture, vec2(U,V)).rgb;
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;
    }
    else if(object_id == CUBEYZ){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.z - minz)/(maxz - minz);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(wallTexture, vec2(U,V)).rgb;
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;
    } else if(object_id == MAP){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.z - minz)/(maxz - minz);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = 2*texture(mapTexture, vec2(U,V)).rgb * max(0.001, sin_pos_light);
        Ks = Kd;
        Ka = Kd/2;
        q = 20.0;
    }else if(object_id == SKY){

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        float theta = atan(position_model.x, position_model.z);
        float phi   = asin(position_model.y);

        U = (theta + M_PI)   / (2*M_PI);
        V = (phi   + M_PI_2) / M_PI;

        Kd = vec3(0.0, 0.0, 0.0);
                //Textura do Dia                      Só aparece de dia e bem pouco à noite
        Ka =  texture(skyDayTexture, vec2(U,V)).rgb * max(0.002, sin_pos_light)*2
            + texture(skyNightTexture, vec2(U,V)).rgb * max(0.000, -sin_pos_light) * (1-abs(cos_pos_light))*0.5 ;
                //Textura da Noite (estrelas)              Só aparece de noite         Aparece gradualmente
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;

    }else { // Objeto desconhecido = preto
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I_flash = vec3(1.0, 1.0, 1.0);
    vec3 I_sun = vec3(1.0, 0.95, 0.8);
    vec3 I_moon = vec3(0.2, 0.2, 0.5);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.5, 0.5, 0.5)*max(0.1, sin_pos_light);
    // Espectro da luz ambiente
    vec3 Ia_flash = vec3(1, 1, 1); // PREENCHA AQUI o espectro da luz ambiente
    vec3 Ia_time = vec3(0.01, 0.01, 0.01); // PREENCHA AQUI o espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term_flash = Kd*I_flash*max(0, dot(n, lFlash)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_sun = Kd*I_sun*max(0, dot(n, lTimeSun)); // PREENCHA AQUI o termo difuso de Lambert
    vec3 lambert_diffuse_term_time_moon = Kd*I_moon*max(0, dot(n, lTimeMoon)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term = Ka*Ia;
    // Termo ambiente
    vec3 ambient_term_flash = Ka*Ia_flash; // PREENCHA AQUI o termo ambiente
    vec3 ambient_term_time = Ka*Ia_time; // PREENCHA AQUI o termo ambiente

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
        color_time_moon.rgb = -sin_pos_light*lambert_diffuse_term_time_moon + phong_specular_term_time_moon;
    }

    if(object_id == BUNNY)
        color.rgb = 2*color_flash.rgb + color_time_sun.rgb + 0.05*color_time_moon.rgb + ambient_term_time;
    else
        color.rgb = 5*color_flash.rgb + color_time_sun.rgb + 0.05*color_time_moon.rgb + ambient_term_time;

    if(object_id == MOON || object_id == PATH || object_id == MAP || object_id == SKY){
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
