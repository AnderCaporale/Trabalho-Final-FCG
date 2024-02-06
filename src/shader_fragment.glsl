#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;
in vec4 position_model;
in vec4 cor_v;

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
#define CUBEXY_FIM  13
#define CUBEYZ_FIM  14

uniform int object_id;

uniform int flashlight_on;
uniform float segundosCicloDia;
uniform float light_pos;

uniform float flashlight_pos_x;
uniform float flashlight_pos_y;
uniform float flashlight_pos_z;

uniform float flashlight_dir_x;
uniform float flashlight_dir_y;
uniform float flashlight_dir_z;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D wallTexture;
uniform sampler2D grassTexture;
uniform sampler2D moonTexture;
uniform sampler2D sunTexture;
uniform sampler2D sunCloudsTexture;
uniform sampler2D goldTexture;
uniform sampler2D swordTexture;
uniform sampler2D gun1Texture;
uniform sampler2D gun2Texture;
uniform sampler2D mapTexture;
uniform sampler2D skyDayTexture;
uniform sampler2D skyNightTexture;
uniform sampler2D brickTexture;
uniform sampler2D minotaurBodyTexture;
uniform sampler2D swordTextureS;

uniform int interpolation;


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
    vec4 direcao = normalize(vec4(flashlight_dir_x, flashlight_dir_y, flashlight_dir_z, 0.0)); //representa o vetor que indica o sentido da iluminação spotlight.
    float abertura = cos(M_PI/12); //12 graus

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    float cos_pos_light = cos(M_PI/segundosCicloDia*light_pos);
    float sin_pos_light = sin(M_PI/segundosCicloDia*light_pos);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));    //Luz fixa
    //vec4 l = normalize(camera_position - p);      //Camera é a luz
    vec4 lFlash = normalize(pontoL - p)/(max(pow(length(pontoL-p), 2),1));      //Luz Spotlight
    vec4 lTimeSun = normalize(vec4(-cos_pos_light, sin_pos_light, 0.0, 0.0));   //Luz sol
    vec4 lTimeMoon = normalize(vec4(cos_pos_light, -sin_pos_light, 0.0,0.0));   //Luz lua

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    //vec4 rFlash = -lFlash + 2*n*dot(n, lFlash); //Vetor de reflexão especular ideal da Lanterna
    //vec4 rTimeSun = -lTimeSun + 2*n*dot(n, lTimeSun); //Vetor de reflexão especular ideal do Sol
    //vec4 rTimeMoon = -lTimeMoon + 2*n*dot(n, lTimeMoon); //Vetor de reflexão especular ideal da Lua

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong
    float U = 0.0;
    float V = 0.0;
    vec3 debugColor;

    if ( object_id == SPHERE )
    {
        // Propriedades espectrais da esfera
        Kd = vec3(0.8, 0.4, 0.08);
        Ks = vec3(0.0, 0.0, 0.0);
        Ka = Kd/2;
        q = 1.0;
    }
    else if ( object_id == BUNNY){
        
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.z - minz)/(maxz - minz);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(goldTexture, vec2(U,V)).rgb;

        //Kd = texture(wallTexture, vec2(U,V)).rgb;
        Ks = Kd/2;
        Ka = Kd/2;
        q = 128.0;

    }
    else if ( object_id == PLANE )
    {
        U = (position_world.x);
        V = (position_world.z);

        Kd = texture(grassTexture, vec2(U,V)).rgb;
        Ks = vec3(0.0, 0.0, 0.0);
        Ka = Kd/5;
        q = 256.0;

    } else if ( object_id == PATH ){
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        Kd = vec3(1.0, 0.0, 0.0);
        Ks = vec3(1.0, 0.0, 0.0);
        Ka = vec3(1.0, 0.0, 0.0);
        q = 1.0;

    } else if ( object_id == COW){

        Kd = 5*texture(minotaurBodyTexture, vec2(texcoords.x,texcoords.y)).rgb;
        Ks = Kd/2;
        Ka = Kd/2;
        q = 2.0;

    } else if ( object_id == FLASHLIGHT ){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(gun1Texture, vec2(U,V)).rgb + texture(gun2Texture, vec2(U,V)).rgb;
        Ks = Kd/2;
        Ka = Kd/2;
        q = 256.0;

    } else if ( object_id == GUN ){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(swordTexture, vec2(texcoords.x,texcoords.y)).rgb;
        Ks = texture(swordTextureS, vec2(texcoords.x,texcoords.y)).rgb;
        Ka = Kd/2;
        q = 256.0;
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
    else if ( object_id == MOON)
    {
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
    else if (object_id == CUBEXY || object_id == CUBEXY_FIM){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = object_id == CUBEXY ? texture(wallTexture, vec2(U,V)).rgb : texture(brickTexture, vec2(U,V)).rgb;

        //Kd = texture(wallTexture, vec2(U,V)).rgb;
        Ks = Kd/2;
        Ka = Kd/2;
        q = 256.0;
    }
    else if(object_id == CUBEYZ || object_id == CUBEYZ_FIM)
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.z - minz)/(maxz - minz);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = object_id == CUBEYZ ? texture(wallTexture, vec2(U,V)).rgb : texture(brickTexture, vec2(U,V)).rgb;

        //Kd = texture(wallTexture, vec2(U,V)).rgb;
        Ks = Kd/2;
        Ka = Kd/2;
        q = 256.0;
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
        Ks = Kd/2;
        Ka = Kd/2;
        q = 256.0;
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
    vec3 I_moon = vec3(0.4, 0.4, 0.7);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.7, 0.7, 0.7)*max(0.1, sin_pos_light);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term_flash = Kd*I_flash*max(0, dot(n, lFlash)); //Termo difuso de Lambert da Lanterna
    vec3 lambert_diffuse_term_time_sun = Kd*I_sun*max(0, dot(n, lTimeSun)); //Termo difuso de Lambert do Sol
    vec3 lambert_diffuse_term_time_moon = Kd*I_moon*max(0, dot(n, lTimeMoon)); //Termo difuso de Lambert da Lua

    // Termo ambiente
    vec3 ambient_term = Ka*Ia;

    //half-vector: meio do caminho entre v e l - Blinn-Phong
    vec4 hFlash = normalize(v + lFlash);
    vec4 hTimeSun = normalize(v + lTimeSun);
    vec4 hTimeMoon = normalize(v + lTimeMoon);

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 blinn_phong_specular_term_flash  = Ks * I_flash * pow(max(0, dot(n, hFlash)), q); //Termo especular de Phong da Lanterna
    vec3 blinn_phong_specular_term_time_sun  = Ks * I_sun * pow(max(0, dot(n, hTimeSun)), q); //Termo especular de Phong do Sol
    vec3 blinn_phong_specular_term_time_moon  = Ks * I_moon * pow(max(0, dot(n, hTimeMoon)), q); //Termo especular de Phong da Lua

    color.a = 1;

    vec4 color_flash = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 color_time_sun = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 color_time_moon = vec4(0.0, 0.0, 0.0, 1.0);


    if (flashlight_on == 1 && object_id != FLASHLIGHT){
        float cos_angle = dot(normalize(p - pontoL), normalize(direcao));
        if(cos_angle >= abertura){
            color_flash.rgb = (lambert_diffuse_term_flash + blinn_phong_specular_term_flash);
        }
    }

    if(sin_pos_light > 0.0){
        color_time_sun.rgb = sin_pos_light*lambert_diffuse_term_time_sun + blinn_phong_specular_term_time_sun;
    }
    if(sin_pos_light < 0.0){
        color_time_moon.rgb = -sin_pos_light*lambert_diffuse_term_time_moon + blinn_phong_specular_term_time_moon;
    }

    color.rgb = color_flash.rgb + color_time_sun.rgb + color_time_moon.rgb + ambient_term;

    if(object_id == BUNNY) // Coelho é difuso sem lantera e especular com lanterna
        color.rgb = 2*color_flash.rgb + sin_pos_light*lambert_diffuse_term_time_sun + 0.05*-sin_pos_light*lambert_diffuse_term_time_moon + ambient_term;
    else
        color.rgb = 5*color_flash.rgb + color_time_sun.rgb + 0.05*color_time_moon.rgb + ambient_term;


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

    if(interpolation == 1){
        color = cor_v;
    }

    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}
