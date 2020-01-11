/*****************************************************************************\
 * TP CPE, 3ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
\*****************************************************************************/



#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glut.h>

#include "glutils.hpp"

#include "mat4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "triangle_index.hpp"
#include "vertex_opengl.hpp"
#include "image.hpp"
#include "mesh.hpp"


/*****************************************************************************\
 * Variables globales
 *
 *
\*****************************************************************************/


//identifiant du shader
GLuint shader_program_id;

//identifiants pour object 1 (vaisseau)
GLuint vbo_object_1=0;
GLuint vboi_object_1=0;
GLuint texture_id_object_1=0;
int nbr_triangle_object_1;

//identifiants pour object 2 (sol)
GLuint vbo_object_2=0;
GLuint vboi_object_2=0;
GLuint texture_id_object_2=0;
int nbr_triangle_object_2;

//identifiants pour object 3 (jupiter)
GLuint vbo_object_3=0;
GLuint vboi_object_3=0;
GLuint texture_id_object_3=0;
int nbr_triangle_object_3;

//identifiants pour objet 4 (soleil)
GLuint vbo_object_4=0;
GLuint vboi_object_4=0;
GLuint texture_id_object_4=0;
int nbr_triangle_object_4;

//identifiants pour object 5 (Terre)
GLuint vbo_object_5=0;
GLuint vboi_object_5=0;
GLuint texture_id_object_5=0;
int nbr_triangle_object_5;

//identifiants pour object 6 (missile)
GLuint vbo_object_6=0;
GLuint vboi_object_6=0;
GLuint texture_id_object_6=0;
int nbr_triangle_object_6;

//Matrice de transformation
struct transformation
{
    mat4 rotation;
    vec3 rotation_center;
    vec3 translation;

    transformation():rotation(),rotation_center(),translation(){}
};

struct planete
{
  vec3 centre;
  float rayon;
};

//Création des planètes
planete jupiter;
planete terre;
planete soleil;

//Transformation des modeles
transformation transformation_model_1;
transformation transformation_model_2;
transformation transformation_model_3;
transformation transformation_model_6;

//Transformation de la vue (camera)
transformation transformation_view;

//Matrice de projection
mat4 projection;

//angle de deplacement
float angle_x_model_1 = 0.0f;
float angle_y_model_1 = 0.0f;
float angle_x_model_3 = 0.0f;
float angle_y_model_3 = 0.0f;
float angle_x_model_6 = 0.0f;
float angle_y_model_6 = 0.0f;
float angle_view = 0.0f;
float angle_tire = 0.0f;
int tir=0;

//angle de deplacement
// variable globales utilisé notamment dans la fonction keyboard qui permet d'effectuer les différents déplacements
float d_angle=0.01f;
float d_angle_vertical=0.005f;
float dL=0.08f;
float dz=0.1f;

//variables globales de déplacements utilisées dans les 3 fonctions qui gèrent les déplacements :
// keyboard_callback, keyboard_callback_2, keyboard
int forward;
int backward;
int up;
int down;
int rota_left;
int rota_right;
int rota_up;
int rota_down;
int leave;

void load_texture(const char* filename,GLuint *texture_id);

void init_model_1();
void init_model_2();
void init_model_3();
void init_model_4();
void init_model_5();
void init_model_6();

void draw_model_1();
void draw_model_2();
void draw_model_3();
void draw_model_4();
void draw_model_5();
void draw_model_6();

int collision(double x, double y, double z);
int collisionPlanete(double x, double y, double z);

static void init()
{

    // Chargement du shader
    shader_program_id = read_shader("shader.vert", "shader.frag");

    //matrice de projection
    projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
    glUniformMatrix4fv(get_uni_loc(shader_program_id,"projection"),1,false,pointeur(projection)); PRINT_OPENGL_ERROR();

    //centre de rotation de la 'camera' (les objets sont centres en z=-2)
    transformation_view.rotation_center = vec3(0.0f,0.0f,-2.0f);

    //activation de la gestion de la profondeur
    glEnable(GL_DEPTH_TEST); PRINT_OPENGL_ERROR();

    // Charge modele 1 sur la carte graphique
    init_model_1();
    // Charge modele 2 sur la carte graphique
    init_model_2();
    // Charge modele 3 sur la carte graphique
    init_model_3();
    // Charge modele 4 sur la carte graphique
    init_model_4();
    // Charge modele 5 sur la carte graphique
    init_model_5();
}


//Fonction d'affichage
static void display_callback()
{
    //effacement des couleurs du fond d'ecran
    glClearColor(0.5f, 0.6f, 0.9f, 1.0f);                 PRINT_OPENGL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   PRINT_OPENGL_ERROR();


  {
       //Camera troisieme personne :
      transformation_view.rotation_center = vec3();
      vec3 target = transformation_model_1.translation + vec3(0.,-0.5,-2.);
      vec3 pos_camera = target - transformation_model_1.rotation * vec3(0., -3., 5.)  ;
      transformation_view.rotation = matrice_lookat(pos_camera, target, vec3(0., 1., 0.));
      transformation_view.translation = extract_translation(transformation_view.rotation);
    }

    // Affecte les parametres uniformes de la vue (identique pour tous les modeles de la scene)
    {
        //envoie de la rotation
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_view"),1,false,pointeur(transformation_view.rotation)); PRINT_OPENGL_ERROR();

        //envoie du centre de rotation
        vec3 cv = transformation_view.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_view") , cv.x,cv.y,cv.z , 0.0f); PRINT_OPENGL_ERROR();

        //envoie de la translation
        vec3 tv = transformation_view.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_view") , tv.x,tv.y,tv.z , 0.0f); PRINT_OPENGL_ERROR();
    }



    // Affiche le modele numero 1 (vaisseau)
    draw_model_1();
    // Affiche le modele numero 2 (sol)
    draw_model_2();
    // Affiche le modele numero 3 (jupiter)
    draw_model_3();
    // Affiche le modele numero 4 (Soleil)
    draw_model_4();
    // Affiche le modele numero 5 (Terre)
    draw_model_5();

    //Changement de buffer d'affichage pour eviter un effet de scintillement
    glutSwapBuffers();
}

static void keyboard_callback(unsigned char key, int, int)
// fonction qui met la variable a 1 lorsque qu'on appuie sur la touche du clavier
{
    float d_angle=0.1f;
    float dz=0.1f;

    if (key == 27){  //passe la touche esc à 1, va permettre de quitter le jeu 
        leave = 1;
    }

    if (key == 'z'){
        forward = 1;
    }
    if (key == 's'){        //Dans le Lore de notre jeu, un vaisseau ne peut pas reculer on desactive donc cette fonction//
      backward = 1;
    }
    if (key == 'q'){
        rota_right = 1;
    }
    if (key == 'd'){
        rota_left = 1;
    }
    if (key == 'o'){
        up = 1;
    }
    if (key == 'l'){
        down = 1;
    }
    if (key == 'k'){
        rota_down = 1;
    }
    if (key == 'm'){
        rota_up = 1;
    }
}
static void keyboard_callback_2(unsigned char key, int, int)
// fonction qui met la remet la variable a 0 une fois que l'on a retiré le doigt de la touche, qu'elle est relevé)
{
    float d_angle=0.1f;
    float dz=0.1f;

    if (key == 27){
     leave = 0;
    }

    if (key == 'z'){
        forward = 0;
    }
    
    if (key == 's'){
      backward = 0;
    }
    
    if (key == 'q'){
        rota_right = 0;
    }
    if (key == 'd'){
        rota_left = 0;
    }
    if (key == 'o'){
        up = 0;
    }
    if (key == 'l'){
        down = 0;
    }
    if (key == 'k'){
        rota_down = 0;
    }
    if (key == 'm'){
        rota_up = 0;
    }
}

void keyboard() {

    if (leave == 1){            // quitte le jeu si on appuie sur esc
        exit(0);
    }
	
	//Si on rencontre une planète ou le bord de la map, on ne peut plus avancer, il faut reculer
    if (forward == 1 && collision(transformation_model_1.translation.x+dL*sin(angle_y_model_1),transformation_model_1.translation.y,transformation_model_1.translation.z)==0 && collisionPlanete(transformation_model_1.translation.x+dL*sin(angle_y_model_1),transformation_model_1.translation.y,transformation_model_1.translation.z)==0){
        transformation_model_1.translation += transformation_model_1.rotation*vec3(0,0,0.1);
    }
//   else {
//    angle_y_model_1=3.14f;
//  transformation_model_1.rotation = matrice_rotation(angle_y_model_1 , //0.0f,1.0f,0.0f) * matrice_rotation(angle_x_model_1 , 1.0f,0.0f,0.0f);
//    }

    if (backward == 1 /*&& collision(transformation_model_1.translation.x+dL*sin(angle_y_model_1),transformation_model_1.translation.y,transformation_model_1.translation.z)==0*/ ){
	transformation_model_1.translation -= transformation_model_1.rotation*vec3(0,0,0.1);
    }
    
    if (rota_left == 1){
        angle_y_model_1 -= d_angle;
    }
    if (rota_right == 1){
        angle_y_model_1 += d_angle;
    }
    if (up == 1){
        transformation_model_1.translation.y += dL;
    }
    if (down ==1){
        transformation_model_1.translation.y -= dL;
    }
    if (rota_down == 1){
        angle_x_model_1 += d_angle_vertical;
    } 
    if (rota_up == 1){
        angle_x_model_1 -= d_angle_vertical;
    } 

    transformation_model_1.rotation = matrice_rotation(angle_y_model_1 , 0.0f,1.0f,0.0f) * matrice_rotation(angle_x_model_1 , 1.0f,0.0f,0.0f);
    transformation_view.rotation = matrice_rotation(angle_view , 0.0f,1.0f,0.0f);
}


/*****************************************************************************\
 * timer_callback                                                            *
\*****************************************************************************/
static void timer_callback(int)
{
    float dL=0.1f;
  
    //demande de rappel de cette fonction dans 25ms
    glutTimerFunc(25, timer_callback, 0);

    keyboard();        //reactualisation de l'affichage

    //reactualisation de l'affichage
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    //**********************************************//
    //Lancement des fonctions principales de GLUT
    //**********************************************//

    //necesssaire sur certains pc 
    glewExperimental = GL_TRUE;
    
    //initialisation
    glutInit(&argc, argv);

    //Mode d'affichage (couleur, gestion de profondeur, ...)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    //Taille de la fenetre a l'ouverture
    glutInitWindowSize(900,900);

    //Titre de la fenetre
    glutCreateWindow("OpenGL");

    //Fonction de la boucle d'affichage
    glutDisplayFunc(display_callback);

    //Fonction de gestion du clavier
    glutKeyboardFunc(keyboard_callback);

    //Fonction clavier quand on relache la touche
    glutKeyboardUpFunc(keyboard_callback_2);

    //Fonction des touches speciales du clavier (fleches directionnelles)
    //glutSpecialFunc(special_callback);

    //Fonction d'appel d'affichage en chaine
    glutTimerFunc(25, timer_callback, 0);

    //Initialisation des fonctions OpenGL
    glewInit();

    //Notre fonction d'initialisation des donnees et chargement des shaders
    init();


    //Lancement de la boucle (infinie) d'affichage de la fenetre
    glutMainLoop();

    //Plus rien n'est execute apres cela

    return 0;
}

//Gestion des collisions

//Collision avec limites du jeu

int  collision(double x,double y,double z) 
{
    int KO=0;
    if(z >= 25.0f || z <= -22.0f || x >= 24.0f || x <= -24.0f || y >= 12.5f || y <= -11.0f)
    {
        KO = 1;
    }

    return KO;
}

//Collision avec les planetes

int collisionPlanete(double x, double y, double z)
{
    int KO=0;
    if(((x-jupiter.centre.x)*(x-jupiter.centre.x)+(y-jupiter.centre.y)*(y-jupiter.centre.y)+(z-jupiter.centre.z)*(z-jupiter.centre.z)) < (jupiter.rayon*jupiter.rayon +3.0f))
    //collision avec jupiter
    {
        KO=1;
    }
    else if(((x-soleil.centre.x)*(x-soleil.centre.x)+(y-soleil.centre.y)*(y-soleil.centre.y)+(z-soleil.centre.z)*(z-soleil.centre.z)) < (soleil.rayon*soleil.rayon +3.9f))
    //collision avec soleil
    {
        KO=1;
    }
    else if(((x-terre.centre.x)*(x-terre.centre.x)+(y-terre.centre.y)*(y-terre.centre.y)+(z-terre.centre.z)*(z-terre.centre.z)) < (terre.rayon*terre.rayon +1.5f) )
    //collision avec terre
    {
        KO=1;
    }

    return KO;
    
}


void draw_model_1()
{

    //envoie des parametres uniformes
    {
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_model"),1,false,pointeur(transformation_model_1.rotation));    PRINT_OPENGL_ERROR();

        vec3 c = transformation_model_1.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_model") , c.x,c.y,c.z , 0.0f);                                 PRINT_OPENGL_ERROR();

        vec3 t = transformation_model_1.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_model") , t.x,t.y,t.z , 0.0f);                                     PRINT_OPENGL_ERROR();
    }

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_1);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_1);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_1);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_1, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }

}

void draw_model_2()
{

    //envoie des parametres uniformes
    {
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_model"),1,false,pointeur(transformation_model_2.rotation));    PRINT_OPENGL_ERROR();

        vec3 c = transformation_model_2.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_model") , c.x,c.y,c.z , 0.0f);                                 PRINT_OPENGL_ERROR();

        vec3 t = transformation_model_2.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_model") , t.x,t.y,t.z , 0.0f);                                     PRINT_OPENGL_ERROR();
    }

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_2);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_2);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_2);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_2, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }
}

void draw_model_3()
{
    //envoie des parametres uniformes
    {
        glUniformMatrix4fv(get_uni_loc(shader_program_id,"rotation_model"),1,false,pointeur(transformation_model_3.rotation));    PRINT_OPENGL_ERROR();

        vec3 c = transformation_model_3.rotation_center;
        glUniform4f(get_uni_loc(shader_program_id,"rotation_center_model") , c.x,c.y,c.z , 0.0f);                                 PRINT_OPENGL_ERROR();

        vec3 t = transformation_model_3.translation;
        glUniform4f(get_uni_loc(shader_program_id,"translation_model") , t.x,t.y,t.z , 0.0f);                                     PRINT_OPENGL_ERROR();
    }

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_3);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_3);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_3);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_3, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }
}

void draw_model_4()
{


    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_4);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_4);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_4);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_4, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }

}

void draw_model_5()
{

    //placement des VBO
    {
        //selection du VBO courant
        glBindBuffer(GL_ARRAY_BUFFER,vbo_object_5);                                                    PRINT_OPENGL_ERROR();

        // mise en place des differents pointeurs
        glEnableClientState(GL_VERTEX_ARRAY);                                                          PRINT_OPENGL_ERROR();
        glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0);                                        PRINT_OPENGL_ERROR();

        glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();                                    PRINT_OPENGL_ERROR();
        glNormalPointer(GL_FLOAT, sizeof(vertex_opengl), buffer_offset(sizeof(vec3)));                 PRINT_OPENGL_ERROR();

        glEnableClientState(GL_COLOR_ARRAY); PRINT_OPENGL_ERROR();                                     PRINT_OPENGL_ERROR();
        glColorPointer(3,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(2*sizeof(vec3)));              PRINT_OPENGL_ERROR();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY); PRINT_OPENGL_ERROR();                             PRINT_OPENGL_ERROR();
        glTexCoordPointer(2,GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));           PRINT_OPENGL_ERROR();

    }

    //affichage
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_5);                           PRINT_OPENGL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture_id_object_5);                             PRINT_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3*nbr_triangle_object_5, GL_UNSIGNED_INT, 0);     PRINT_OPENGL_ERROR();
    }

}

void init_model_1()
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("../data/falcon2.obj");

    // Affecte une transformation sur les sommets du maillage
    float s = 0.2f;
    mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
                          0.0f,    s, 0.0f,-0.9f,
                          0.0f, 0.0f,   s ,-2.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m,transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
    transformation_model_1.rotation_center = vec3(0.0f,-0.5f,-2.0f);

    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_1); PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_1); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte     
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_1); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_1); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 1
    nbr_triangle_object_1 = m.connectivity.size();

    //angle de départ
    angle_y_model_1=3.14f;
    transformation_model_1.rotation = matrice_rotation(angle_y_model_1 , 0.0f,1.0f,0.0f) * matrice_rotation(angle_x_model_1 , 1.0f,0.0f,0.0f);
    
    // Chargement de la texture
    load_texture("../data/MF.tga",&texture_id_object_1);


}

void init_model_2()
{
    //Creation manuelle du cube de jeu

     //coordonnees geometriques des sommets
    vec3 p0=vec3(-25.0f,-12.5f,-25.0f);
    vec3 p10=vec3(-25.0f,-12.5f,-25.0f);
    vec3 p7=vec3(-25.0f,-12.5f,-25.0f);
    vec3 p1=vec3( 25.0f,-12.5f,-25.0f);
    vec3 p6=vec3( 25.0f,-12.5f,-25.0f);   
    vec3 p19=vec3( 25.0f,-12.5f,-25.0f);
    vec3 p2=vec3( 25.0f,-12.5f, 25.0f);
    vec3 p13=vec3( 25.0f,-12.5f, 25.0f);
    vec3 p18=vec3( 25.0f,-12.5f, 25.0f);
    vec3 p3=vec3(-25.0f,-12.5f, 25.0f);
    vec3 p11=vec3(-25.0f,-12.5f, 25.0f);
    vec3 p12=vec3(-25.0f,-12.5f, 25.0f);
    vec3 p4=vec3(-25.0f,12.5f,-25.0f);
    vec3 p9=vec3(-25.0f,12.5f,-25.0f);
    vec3 p20=vec3(-25.0f,12.5f,-25.0f);
    vec3 p5=vec3( 25.0f,12.5f,-25.0f);
    vec3 p21=vec3( 25.0f,12.5f,-25.0f);
    vec3 p16=vec3( 25.0f,12.5f,-25.0f);
    vec3 p14=vec3( 25.0f,12.5f, 25.0f);
    vec3 p22=vec3( 25.0f,12.5f, 25.0f);
    vec3 p17=vec3( 25.0f,12.5f, 25.0f);
    vec3 p8=vec3(-25.0f,12.5f, 25.0f);
    vec3 p15=vec3(-25.0f,12.5f, 25.0f);
    vec3 p23=vec3(-25.0f,12.5f, 25.0f);


    
    //normales pour chaque sommet
    vec3 n0=vec3(0.0f,1.0f,0.0f);    
    vec3 n1=n0;
    vec3 n2=n0;
    vec3 n3=n0;
    vec3 n4=vec3(0.0f,0.0f,1.0f);
    vec3 n5=n4;
    vec3 n6=n4;
    vec3 n7=n4;
    vec3 n8=vec3(1.0f,0.0f,0.0f);
    vec3 n9=n8;
    vec3 n10=n8;
    vec3 n11=n8;
    vec3 n12=vec3(0.0f,0.0f,-1.0f);
    vec3 n13=n12;
    vec3 n14=n12;
    vec3 n15=n12;
    vec3 n16=vec3(-1.0f,0.0f,0.0f);
    vec3 n17=n16;
    vec3 n18=n16;
    vec3 n19=n16;
    vec3 n20=vec3(0.0f,-1.0f,0.0f);
    vec3 n21=n20;
    vec3 n22=n20;
    vec3 n23=n20;

    //couleur pour chaque sommet
    vec3 c0=vec3(1.0f,1.0f,1.0f);
    vec3 c1=c0;
    vec3 c2=c0;
    vec3 c3=c0;
    vec3 c4=c0;
    vec3 c5=c0;
    vec3 c6=c0;
    vec3 c7=c0;
    vec3 c8=c0;
    vec3 c9=c0;
    vec3 c10=c0;
    vec3 c11=c0;
    vec3 c12=c0;
    vec3 c13=c0;
    vec3 c14=c0;
    vec3 c15=c0;
    vec3 c16=c0;
    vec3 c17=c0;
    vec3 c18=c0;
    vec3 c19=c0;
    vec3 c20=c0;
    vec3 c21=c0;
    vec3 c22=c0;
    vec3 c23=c0;
   

    //texture du sommet
    vec2 t0=vec2(0.0f,0.0f);
    vec2 t1=vec2(1.0f,0.0f);
    vec2 t2=vec2(1.0f,1.0f);
    vec2 t3=vec2(0.0f,1.0f);
    vec2 t4=vec2(0.0f,0.0f);
    vec2 t5=vec2(1.0f,0.0f);
    vec2 t6=vec2(1.0f,1.0f);
    vec2 t7=vec2(0.0f,1.0f);
    vec2 t8=vec2(0.0f,0.0f);
    vec2 t9=vec2(1.0f,0.0f);
    vec2 t10=vec2(1.0f,1.0f);
    vec2 t11=vec2(0.0f,1.0f);
    vec2 t12=vec2(0.0f,0.0f);
    vec2 t13=vec2(1.0f,0.0f);
    vec2 t14=vec2(1.0f,1.0f);
    vec2 t15=vec2(0.0f,1.0f);
    vec2 t16=vec2(0.0f,0.0f);
    vec2 t17=vec2(1.0f,0.0f);
    vec2 t18=vec2(1.0f,1.0f);
    vec2 t19=vec2(0.0f,1.0f);
    vec2 t20=vec2(0.0f,0.0f);
    vec2 t21=vec2(1.0f,0.0f);
    vec2 t22=vec2(1.0f,1.0f);
    vec2 t23=vec2(0.0f,1.0f);
    
    vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
    vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
    vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
    vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);
    vertex_opengl v4=vertex_opengl(p4,n4,c4,t4);
    vertex_opengl v5=vertex_opengl(p5,n5,c5,t5);
    vertex_opengl v6=vertex_opengl(p6,n6,c6,t6);
    vertex_opengl v7=vertex_opengl(p7,n7,c7,t7);
    vertex_opengl v8=vertex_opengl(p8,n8,c8,t8);
    vertex_opengl v9=vertex_opengl(p9,n9,c9,t9);
    vertex_opengl v10=vertex_opengl(p10,n10,c10,t10);
    vertex_opengl v11=vertex_opengl(p11,n11,c11,t11);
    vertex_opengl v12=vertex_opengl(p12,n12,c12,t12);
    vertex_opengl v13=vertex_opengl(p13,n13,c13,t13);
    vertex_opengl v14=vertex_opengl(p14,n14,c14,t14);
    vertex_opengl v15=vertex_opengl(p15,n15,c15,t15);
    vertex_opengl v16=vertex_opengl(p16,n16,c16,t16);
    vertex_opengl v17=vertex_opengl(p17,n17,c17,t17);
    vertex_opengl v18=vertex_opengl(p18,n18,c18,t18);
    vertex_opengl v19=vertex_opengl(p19,n19,c19,t19);
    vertex_opengl v20=vertex_opengl(p20,n20,c20,t20);
    vertex_opengl v21=vertex_opengl(p21,n21,c21,t21);
    vertex_opengl v22=vertex_opengl(p22,n22,c22,t22);
    vertex_opengl v23=vertex_opengl(p23,n23,c23,t23);
    
    //tableau entrelacant coordonnees-normales
    vertex_opengl geometrie[]={v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23};


    //indice des triangles
    triangle_index tri0=triangle_index(0,1,2);
    triangle_index tri1=triangle_index(0,2,3);
    triangle_index tri2=triangle_index(4,5,6);
    triangle_index tri3=triangle_index(4,6,7);
    triangle_index tri4=triangle_index(8,9,10);
    triangle_index tri5=triangle_index(8,10,11);
    triangle_index tri6=triangle_index(12,13,14);
    triangle_index tri7=triangle_index(12,14,15);
    triangle_index tri8=triangle_index(16,17,18);
    triangle_index tri9=triangle_index(16,18,19);
    triangle_index tri10=triangle_index(20,21,22);
    triangle_index tri11=triangle_index(20,22,23);
    triangle_index index[]={tri0,tri1,tri2,tri3,tri4,tri5,tri6,tri7,tri8,tri9,tri10,tri11};
    nbr_triangle_object_2 = 12;

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_2);                                             PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_2);                                PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);  PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_2);                                            PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_2);                       PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);  PRINT_OPENGL_ERROR();

    // Chargement de la texture
    load_texture("../data/space.tga",&texture_id_object_2);

}


void init_model_3()
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("../data/planete.obj");

    jupiter.centre.x=5.0f;
    jupiter.centre.y=5.0f;
    jupiter.centre.z=5.0f;
    // Affecte une transformation sur les sommets du maillage
    float s = 1.0f;
    jupiter.rayon=3.0f*s;
    mat4 transform = mat4(   s, 0.0f, 0.0f,jupiter.centre.x,
                          0.0f,    s, 0.0f,-0.9f+jupiter.centre.y,
                          0.0f, 0.0f,   s ,-2.0f+jupiter.centre.z,
                          0.0f, 0.0f, 0.0f, 1.0f);
    
    apply_deformation(&m,transform);

    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_3);                                 PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_3); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_3); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_3); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 3
    nbr_triangle_object_3 = m.connectivity.size();

    // Chargement de la texture
    load_texture("../data/jupiter.tga",&texture_id_object_3);

}

void init_model_4()
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("../data/planete.obj");

    soleil.centre.x=-10.0f;
    soleil.centre.y=-5.0f;
    soleil.centre.z=15.0f;
    // Affecte une transformation sur les sommets du maillage
    float s = 2.0f;
    soleil.rayon=3.0f*s;
    mat4 transform = mat4(   s, 0.0f, 0.0f,soleil.centre.x,
                          0.0f,    s, 0.0f,-0.9f+soleil.centre.y,
                          0.0f, 0.0f,   s ,-2.0f+soleil.centre.z,
                          0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m,transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
//     transformation_model_1.rotation_center = vec3(0.0f,-0.5f,-2.0f);

    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_4); PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_4); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_4); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_4); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 1
    nbr_triangle_object_4 = m.connectivity.size();

    // Chargement de la texture
    load_texture("../data/sunmap.tga",&texture_id_object_4);


}

void init_model_5()
{
    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("../data/planete.obj");
    terre.centre.x=-15.0f;
    terre.centre.y=-3.9f;
    terre.centre.z=-10.0f;
    // Affecte une transformation sur les sommets du maillage
    float s = 0.5f;
    terre.rayon=3.0f*s;
    mat4 transform = mat4(   s, 0.0f, 0.0f, terre.centre.x,
                          0.0f,    s, 0.0f,terre.centre.y-0.9,
                          0.0f, 0.0f,   s ,terre.centre.z -2.0,
                          0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m,transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
//     transformation_model_1.rotation_center = vec3(0.0f,-0.5f,-2.0f);

    // Calcul automatique des normales du maillage
    update_normals(&m);
    // Les sommets sont affectes a une couleur blanche
    fill_color(&m,vec3(1.0f,1.0f,1.0f));

    //attribution d'un buffer de donnees (1 indique la création d'un buffer)
    glGenBuffers(1,&vbo_object_5); PRINT_OPENGL_ERROR();
    //affectation du buffer courant
    glBindBuffer(GL_ARRAY_BUFFER,vbo_object_5); PRINT_OPENGL_ERROR();
    //copie des donnees des sommets sur la carte graphique
    glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();


    //attribution d'un autre buffer de donnees
    glGenBuffers(1,&vboi_object_5); PRINT_OPENGL_ERROR();
    //affectation du buffer courant (buffer d'indice)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_object_5); PRINT_OPENGL_ERROR();
    //copie des indices sur la carte graphique
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); PRINT_OPENGL_ERROR();

    // Nombre de triangles de l'objet 1
    nbr_triangle_object_5 = m.connectivity.size();

    // Chargement de la texture
   load_texture("../data/Earth.tga",&texture_id_object_5);

}

void load_texture(const char* filename,GLuint *texture_id)
{
    // Chargement d'une texture (seul les textures tga sont supportes)
    Image  *image = image_load_tga(filename);
    if (image) //verification que l'image est bien chargee
    {

        //Creation d'un identifiant pour la texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); PRINT_OPENGL_ERROR();
        glGenTextures(1, texture_id); PRINT_OPENGL_ERROR();

        //Selection de la texture courante a partir de son identifiant
        glBindTexture(GL_TEXTURE_2D, *texture_id); PRINT_OPENGL_ERROR();

        //Parametres de la texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();

        //Envoie de l'image en memoire video
        if(image->type==IMAGE_TYPE_RGB){ //image RGB
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data); PRINT_OPENGL_ERROR();}
        else if(image->type==IMAGE_TYPE_RGBA){ //image RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data); PRINT_OPENGL_ERROR();}
        else{
            std::cout<<"Image type not handled"<<std::endl;}

        delete image;
    }
    else
    {
        std::cerr<<"Erreur chargement de l'image, etes-vous dans le bon repertoire?"<<std::endl;
        abort();
    }

    glUniform1i (get_uni_loc(shader_program_id, "texture"), 0); PRINT_OPENGL_ERROR();
}
