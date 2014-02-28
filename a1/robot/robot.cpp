/***********************************************************
             CSC418/2504, Fall 2009
  
                 robot.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;

const float POS_MIN = -20.0f;
const float POS_MAX = 20.0f;

const float LEN_MIN = 20.0f;
const float LEN_MAX = 80.0f;

float joint_rot_neck = 0.0f;

float joint_rot_rarm =  0.0f;
float joint_rot_larm =  0.0f;

float joint_rot_rhand =  0.0f;
float joint_rot_lhand =  0.0f;


float joint_rot_rleg = 0.0f;
float joint_rot_lleg = 0.0f;

float rhand_len = 0.0f;
float lhand_len = 0.0f;

float robot_pos = 0.0f;
//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////



// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawCircle(float radius);


// Return the current system clock (in seconds)
double getTime();


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 300x200 window by default...\n");
        Win[0] = 300;
        Win[1] = 200;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();


    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the joint_rot_neck
    GLUI_Spinner *joint_spinner0
        = glui->add_spinner("Neck Joint", GLUI_SPINNER_FLOAT, &joint_rot_neck);
    joint_spinner0->set_speed(0.1);
    joint_spinner0->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    // Create a control to specify the rotation of the joint_rot_larm
    GLUI_Spinner *joint_spinner1
        = glui->add_spinner("Left Arm Joint", GLUI_SPINNER_FLOAT, &joint_rot_larm);
    joint_spinner1->set_speed(0.1);
    joint_spinner1->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    // Create a control to specify the rotation of the joint_rot_rarm
    GLUI_Spinner *joint_spinner2
        = glui->add_spinner("Right Arm Joint", GLUI_SPINNER_FLOAT, &joint_rot_rarm);
    joint_spinner2->set_speed(0.1);
    joint_spinner2->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    // Create a control to specify the rotation of the joint_rot_lhand  
    GLUI_Spinner *joint_spinner3
        = glui->add_spinner("Left Hand Joint", GLUI_SPINNER_FLOAT, &joint_rot_lhand);
    joint_spinner3->set_speed(0.1);
    joint_spinner3->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    // Create a control to specify the rotation of the joint_rot_rhand
    GLUI_Spinner *joint_spinner4
        = glui->add_spinner("Right Hand Joint", GLUI_SPINNER_FLOAT, &joint_rot_rhand);
    joint_spinner4->set_speed(0.1);
    joint_spinner4->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    // Create a control to specify the rotation of the joint_rot_lleg
    GLUI_Spinner *joint_spinner5
        = glui->add_spinner("Left Leg Joint", GLUI_SPINNER_FLOAT, &joint_rot_lleg);
    joint_spinner5->set_speed(0.1);
    joint_spinner5->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    // Create a control to specify the rotation of the joint_rot_rleg
    GLUI_Spinner *joint_spinner6
        = glui->add_spinner("Right Leg Joint", GLUI_SPINNER_FLOAT, &joint_rot_rleg);
    joint_spinner6->set_speed(0.1);
    joint_spinner6->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    // Create a control to specify the rotation of the hand_len
    GLUI_Spinner *joint_spinner7
        = glui->add_spinner("Length of Right Hand", GLUI_SPINNER_FLOAT, &rhand_len);
    joint_spinner7->set_speed(0.1);
    joint_spinner7->set_float_limits(LEN_MIN, LEN_MAX, GLUI_LIMIT_CLAMP);

    GLUI_Spinner *joint_spinner8
        = glui->add_spinner("Length of Left Hand", GLUI_SPINNER_FLOAT, &lhand_len);
    joint_spinner8->set_speed(0.1);
    joint_spinner8->set_float_limits(LEN_MIN, LEN_MAX, GLUI_LIMIT_CLAMP);
    // Create a control to specify the rotation of the robot_pos
    GLUI_Spinner *joint_spinner9
        = glui->add_spinner("Position of Robot", GLUI_SPINNER_FLOAT, &robot_pos);
    joint_spinner9->set_speed(0.1);
    joint_spinner9->set_float_limits(POS_MIN, POS_MAX, GLUI_LIMIT_CLAMP);

 

    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
    const double joint_rot_speed = 0.1;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;

    //animation control function for arms
    joint_rot_rarm =  joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    joint_rot_larm =  joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    //animation control function for hands
    joint_rot_rhand =  joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    joint_rot_lhand =  joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    //animation control function for neck
    joint_rot_neck = 0.3*joint_rot_t * JOINT_MIN + 0.3*(1 - joint_rot_t) * JOINT_MAX;
    //animation control function for legs
    joint_rot_lleg = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    joint_rot_rleg = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    
    //animation control function for rhand_len
    rhand_len = joint_rot_t * LEN_MIN + (1 - joint_rot_t) * LEN_MAX;
    //animation control function for rhand_len
    lhand_len = joint_rot_t * LEN_MIN + (1 - joint_rot_t) * LEN_MAX;
    //animation control function for robot_pos
    robot_pos = joint_rot_t * POS_MIN + (1 - joint_rot_t) * POS_MAX;
    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}


// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    const float EYE_RADIUS = 15.0f;
    const float BODY_WIDTH = 60.0f;
    const float BODY_LENGTH = 90.0f;
    const float HAND_WIDTH = 20.0f;
    const float ARM_LENGTH = 60.0f;
    const float ARM_WIDTH = 30.0f;
    const float HEAD_WIDTH = 40.0f;
    const float HEAD_LENGTH = 100.0f;
    const float NECK_WIDTH = 30.0f;
    const float NECK_LENGTH = 50.0f;
    const float WAIST_WIDTH = 30.0f;
    const float WAIST_LENGTH = 140.0f;
    const float JOINT_OFFSET = 15.0f;
    const float THIGH_WIDTH = 30.0f;
    const float THIGH_LENGTH = 80.0f;

    // Push the current transformation matrix on the stack
    glPushMatrix();
        glTranslatef(0.0, robot_pos, 0.0);
        // Draw the 'body'
        glPushMatrix();
            // Scale square to size of body
            glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);
            // Set the colour to white
            glColor3f(1.0, 1.0, 1.0);
            // Draw the square for the body
            drawSquare(1.0);
            // Scale square to size of body
            glScalef(1/BODY_WIDTH, 1/BODY_LENGTH, 1.0);
            // Draw right arms
            glPushMatrix();
                // Move the to the joint hinge
                glTranslatef(BODY_WIDTH/2 - JOINT_OFFSET ,0.0, 0.0);
                // Rotate along the hinge
                glRotatef(joint_rot_rarm, 0.0, 0.0, 1.0);

                glPushMatrix();
                    //Translate to the center of the arm
                    glTranslatef(ARM_LENGTH/2 - JOINT_OFFSET/2, 0.0, 0.0);
                    glScalef(ARM_LENGTH, ARM_WIDTH, 1.0);
                    // Set the colour to white
                    glColor3f(0.5, 0.5, 0.5);
                    // Draw the square for the arm
                    drawSquare(1.0);
                glPopMatrix();
                //Draw the right hand
                glPushMatrix();
                    // Move the to the joint hinge
                    glTranslatef(ARM_LENGTH - JOINT_OFFSET, 0.0, 0.0);
                    // Rotate along the hinge
                    glRotatef(joint_rot_rhand, 0.0, 0.0, 1.0);

                    glPushMatrix();
                        //Translate to the center of the hand
                        glTranslatef(rhand_len/2- JOINT_OFFSET/2, 0.0, 0.0);
                        glScalef(rhand_len, HAND_WIDTH, 1.0);
                        // Set the colour to white
                        glColor3f(0.3, 0.3, 0.3);
                        // Draw the square for the hand
                        drawSquare(1.0);
                    glPopMatrix();
                    // Draw joint
                    glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                    glColor3f(1.0, 0.0, 0.0);
                    drawCircle(1.0);
                glPopMatrix();
                // Draw joint
                glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                glColor3f(1.0, 0.0, 0.0);
                drawCircle(1.0);
            glPopMatrix();

           // Draw left arms
            glPushMatrix();
                // Move the to the joint hinge
                glTranslatef(-(BODY_WIDTH/2 - JOINT_OFFSET) ,0.0, 0.0);
                // Rotate along the hinge
                glRotatef(joint_rot_larm, 0.0, 0.0, 1.0);
                glPushMatrix();
                    //Translate to the center of the arm
                    glTranslatef(-(ARM_LENGTH/2 - JOINT_OFFSET/2), 0.0, 0.0);
                    glScalef(ARM_LENGTH, ARM_WIDTH, 1.0);
                    // Set the colour to white
                    glColor3f(0.5, 0.5, 0.5);
                    // Draw the square for the arm
                    drawSquare(1.0);
                glPopMatrix();
                //Draw the left hand
                glPushMatrix();
                    // Move the to the joint hinge
                    glTranslatef(-(ARM_LENGTH - JOINT_OFFSET), 0.0, 0.0);
                    // Rotate along the hinge
                    glRotatef(joint_rot_lhand, 0.0, 0.0, 1.0);
                    glPushMatrix();
                        //Translate to the center of the hand
                        glTranslatef(-(lhand_len/2- JOINT_OFFSET/2), 0.0, 0.0);
                        glScalef(lhand_len, HAND_WIDTH, 1.0);
                        // Set the colour to grey
                        glColor3f(0.3, 0.3, 0.3);
                        // Draw the square for the hand
                        drawSquare(1.0);
                    glPopMatrix();
                    // Draw joint
                    glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                    glColor3f(1.0, 0.0, 0.0);
                    drawCircle(1.0);
                glPopMatrix();
                // Draw joint
                glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                glColor3f(1.0, 0.0, 0.0);
                drawCircle(1.0);
            glPopMatrix();
            //Draw the waist
            glPushMatrix();
                glTranslatef(0.0, -0.5*(BODY_LENGTH + WAIST_WIDTH), 0.0);
                glScalef(WAIST_LENGTH, WAIST_WIDTH, 1.0);
                // Set the colour to white
                glColor3f(1.0, 1.0, 1.0);
                // Draw the square for the waist
                drawSquare(1.0);
                // Scale back to normal scale
                glScalef(1/WAIST_LENGTH, 1/WAIST_WIDTH, 1.0);
                // Draw the right thigh
                glPushMatrix();
                    // Move the to the joint hinge
                    glTranslatef(WAIST_LENGTH/2 - JOINT_OFFSET, 0.0, 0.0);
                    // Rotate along the hinge
                    glRotatef(joint_rot_rleg, 0.0, 0.0, 1.0);
                    glPushMatrix();
                        glTranslatef(0.0, - THIGH_LENGTH/2 + JOINT_OFFSET, 0.0);
                        glScalef(THIGH_WIDTH, THIGH_LENGTH, 1.0);
                        // Set the colour to grey
                        glColor3f(0.3, 0.3, 0.3);
                        // Draw the square for the right thigh
                        drawSquare(1.0);
                    glPopMatrix();
                    // Draw joint
                    glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                    glColor3f(1.0, 0.0, 0.0);
                    drawCircle(1.0);
                glPopMatrix();
                // Draw the left thigh
                glPushMatrix();
                    // Move the to the joint hinge
                    glTranslatef(-(WAIST_LENGTH/2 - JOINT_OFFSET), 0.0, 0.0);
                    // Rotate along the hinge
                    glRotatef(joint_rot_lleg, 0.0, 0.0, 1.0);
                    glPushMatrix();
                        glTranslatef(0.0, - THIGH_LENGTH/2 + JOINT_OFFSET, 0.0);
                        glScalef(THIGH_WIDTH, THIGH_LENGTH, 1.0);
                        // Set the colour to white
                        glColor3f(0.3, 0.3, 0.3);
                        // Draw the square for the left thigh
                        drawSquare(1.0);
                    glPopMatrix();
                    // Draw joint
                    glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
                    glColor3f(1.0, 0.0, 0.0);
                    drawCircle(1.0);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        // Draw the 'head' and 'neck' and 'eye'
        glPushMatrix();
            // Move the to the joint hinge
            glTranslatef(0.0, BODY_LENGTH/2 - JOINT_OFFSET, 0.0);
            // Rotate along the hinge
            glRotatef(joint_rot_neck, 0.0, 0.0, 1.0);
            // Draw Head
            glPushMatrix();
                //tranlsate to the center of the head
                glTranslatef(0.0, NECK_LENGTH + HEAD_WIDTH/2 -JOINT_OFFSET/2, 0.0);
                // Scale square to size of head
                glScalef(HEAD_LENGTH, HEAD_WIDTH, 1.0);
                // Set the colour to white
                glColor3f(1.0, 1.0, 1.0);
                // Draw the square for the head
                drawSquare(1.0);
                glScalef(1/HEAD_LENGTH, 1/HEAD_WIDTH, 1.0);
                // Draw left eye
                glPushMatrix();
                    //Translate to the center of the head
                    glTranslatef(0.25*HEAD_LENGTH, 0.0, 0.0);
                    glScalef(EYE_RADIUS, EYE_RADIUS, 1.0);
                    // Set the colour to black
                    glColor3f(0.0, 0.0, 0.0);
                    drawCircle(1.0);
                    glScalef(0.7, 0.7, 1.0);
                    // Set the colour to black
                    glColor3f(0.5, 0.5, 0.5);
                    drawCircle(1.0);
                glPopMatrix();
                // Draw right eye
                glPushMatrix();
                    //Translate to the center of the head
                    glTranslatef(-0.25*HEAD_LENGTH, 0.0, 0.0);
                    glScalef(EYE_RADIUS, EYE_RADIUS, 1.0);
                    // Set the colour to black
                    glColor3f(0.0, 0.0, 0.0);
                    drawCircle(1.0);
                    glScalef(0.7, 0.7, 1.0);
                    // Set the colour to black
                    glColor3f(0.5, 0.5, 0.5);
                    drawCircle(1.0);
                glPopMatrix(); 
            glPopMatrix();
            // Draw neck
            glPushMatrix();
                //Translate to the center of the neck
                glTranslatef(0.0, NECK_LENGTH/2 - JOINT_OFFSET/2, 0.0);
                glScalef(NECK_WIDTH, NECK_LENGTH, 1.0);
                // Set the colour to grey
                glColor3f(0.5, 0.5, 0.5);
                // Draw the square for the neck
                drawSquare(1.0);
            glPopMatrix();
            // Draw joint
            glScalef(JOINT_OFFSET/4, JOINT_OFFSET/4, 1.0);
            glColor3f(1.0, 0.0, 0.0);
            drawCircle(1.0);
        glPopMatrix(); 
    // Retrieve the previous state of the transformation stack
    glPopMatrix();
    // Execute any GL functions that are in the queue just to be safe
    glFlush();
    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}

// Draw a circle of the specified size, centered at the current location
void drawCircle(float radius)
{
   glBegin(GL_POLYGON);
 //plot points on the circle
   for (int i=0; i < 720; i++)
   {
      float degInRad = i*PI/360;
      glVertex3f(cos(degInRad)*radius,sin(degInRad)*radius,0.0f);
   }
 
   glEnd();
}