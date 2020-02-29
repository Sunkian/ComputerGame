// Simplified Renderer application for GP course
// Features:
// Reduced OpenGL version from 4.5 to 3.3 to allow it to render in older laptops.
// Added Shapes library for rendering cubes, spheres and vectors.
// Added examples of matrix multiplication on Update.
// Added resize screen and keyboard callbacks.
// Added FPS camera functionality
// Update 2019/01 updated libraries and created project for VS2017 including directory dependant links to libraries.
// Update 2020/01 updated libraries for x64 and for VS2020, also adding MAC compiled Libraries.

// Suggestions or extra help please do email me at S.Padilla@hw.ac.uk

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

// Helper graphic libraries
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <GLUT/GLUT.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "Graphics.hpp"
#include "Shapes.hpp"

// MAIN FUNCTIONS
void startup();
void updateCamera();
void updateSceneElements();
void renderScene();

//MAIN FUNCTIONS FOR THE PARTICLES
void Particle();
void updateParticle1();
void updateParticle2();
void updateParticle3();
float modifyAcceleration(float acc);


//MAIN FUNCTIONS FOR THE FLOCK
void Flock();

// CALLBACK FUNCTIONS
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);

// VARIABLES
bool        quit = false;
float       deltaTime = 0.0f;    // Keep track of time per frame.
float       lastTime = 0.0f;    // variable to keep overall time.
bool        keyStatus[1024];    // Hold key status.
bool        mouseEnabled = true; // keep track of mouse toggle.

// MAIN GRAPHICS OBJECT
Graphics    myGraphics;        // Runing all the graphics in this object

// DEMO OBJECTS
Cube        myCube;
Sphere      mySphere;
Arrow       arrowX;
Arrow       arrowY;
Arrow       arrowZ;
Cube        myFloor;
Line        myLine;
Cylinder    myCylinder;
// PARTICLE
Cube2       myCube2;
Cube2       myParticle1;
Cube2       myParticle2;
Cube2       myParticle3;
// FLOCK
Sphere      myFlock1;
Sphere      myFlock2;
Sphere      myFlock3;
Sphere      myFlock4;
Sphere      myFlock5;

// GLOBAL VARIABLES FOR ANINAMTION
// CUBE EXPLODING
float position = 0.001f;
float velocity = 0.35f;
const GLfloat gravity = -0.005f;

// PARTICLE 1
float positionParticleX1 = 0.5f;
float positionParticleY1 = 2.5f;
float positionParticleZ1 = 0.5f;
float velocityParticle1 = 0.0f;
float reboundParticle1 = 0.8f;

// PARTICLE 2
float positionParticleX2 = 0.5f;
float positionParticleY2 = 2.5f;
float positionParticleZ2 = 0.5f;
float velocityParticle2 = 0.0f;
float reboundParticle2 = 0.8f;

// PARTICLE 3
float positionParticleX3 = 0.5f;
float positionParticleY3 = 2.5f;
float positionParticleZ3 = 0.5f;
float velocityParticle3 = 0.0f;
float reboundParticle3 = 0.8f;

float coeffPosition = 0.2; //friction
bool particlesEnabled = true;

// FLOOR
float floorPostitionX, floorPostitionY, floorPostitionZ = 0.0f;

// FLOCKING QUESTION
// QUEEN
//velocities, the queen and the flock are moving on the 3 axes
float velocityQueenX = 0.0f;
float velocityQueenY = 0.0f;
float velocityQueenZ = 0.0f;
float queenGravityCoeff = 0.005f;
//accelerations
float accelerationQueenX = 0.000f;
float accelerationQueenY = 0.0f;
float accelerationQueenZ = 0.0f;
//positions
float positionQueenX = 0.0f;
float positionQueenY = 2.0f;
float positionQueenZ = 2.0f;

// FLOCK
//velocities
float velocityFlockX = 0.0f;
float velocityFlockY = 0.0f;
float velocityFlockZ = 0.0f;
//positions
float positionFlockX = 2.0f;
float positionFlockY = 2.6f;
float positionFlockZ = 2.0f;





int main()
{
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    
    
    int errorGraphics = myGraphics.Init();            // Launch window and graphics context
    if (errorGraphics) return 0;                    // Close if something went wrong...
    

    startup();                                        // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

    // MAIN LOOP run until the window is closed
    while (!quit) {

        // Update the camera transform based on interactive inputs or by following a predifined path.
        updateCamera();

        // Update position, orientations and any other relevant visual state of any dynamic elements in the scene.
        updateSceneElements();
        Particle();
        Flock();

        // Render a still frame into an off-screen frame buffer known as the backbuffer.
        renderScene();

        // Swap the back buffer with the front buffer, making the most recently rendered image visible on-screen.
        glfwSwapBuffers(myGraphics.window);        // swap buffers (avoid flickering and tearing)

    }

    myGraphics.endProgram();            // Close and clean everything up...

   // cout << "\nPress any key to continue...\n";
   // cin.ignore(); cin.get(); // delay closing console to read debugging errors.

    return 0;
}

void startup() {
    // Keep track of the running time
    GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
    deltaTime = currentTime;                        // start delta time
    lastTime = currentTime;                        // Save for next frame calculations.

    // Callback graphics and key update functions - declared in main to avoid scoping complexity.
    // More information here : https://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowSizeCallback(myGraphics.window, onResizeCallback);            // Set callback for resize
    glfwSetKeyCallback(myGraphics.window, onKeyCallback);                    // Set Callback for keys
    glfwSetMouseButtonCallback(myGraphics.window, onMouseButtonCallback);    // Set callback for mouse click
    glfwSetCursorPosCallback(myGraphics.window, onMouseMoveCallback);        // Set callback for mouse move
    glfwSetScrollCallback(myGraphics.window, onMouseWheelCallback);            // Set callback for mouse wheel.

    // Calculate proj_matrix for the first time.
    myGraphics.aspect = (float)myGraphics.windowWidth / (float)myGraphics.windowHeight;
    myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);

    // Load Geometry examples
    myCube.Load();
    myCube.fillColor = glm::vec4(0.8f, 0.4f, 1.0f, 1.0f); 
    myCube2.Load();
    myCube2.fillColor = glm::vec4(1.0f, 0.6f, 1.0f, 1.0f);
    
    // PARTICLES
    myParticle1.Load();
    myParticle1.fillColor = glm::vec4(1.0f, 0.8f, 0.4f, 1.0f);
    myParticle2.Load();
    myParticle2.fillColor = glm::vec4(1.0f, 0.5f, 0.3f, 1.0f);
    myParticle3.Load();
    myParticle3.fillColor = glm::vec4(0.2f, 0.2f, 0.4f, 1.0f);
    
    // FLOCK
    myFlock1.Load();
    myFlock1.fillColor = glm::vec4(0.149f, 0.141f, 0.912f, 1.0f);
    myFlock2.Load();
    myFlock2.fillColor = glm::vec4(0.330f, 0.568f, 0.912f, 1.0f);
    myFlock3.Load();
    myFlock3.fillColor = glm::vec4(0.330f, 0.568f, 0.912f, 1.0f);
    myFlock4.Load();
    myFlock4.fillColor = glm::vec4(0.330f, 0.568f, 0.912f, 1.0f);
    myFlock5.Load();
    myFlock5.fillColor = glm::vec4(0.330f, 0.568f, 0.912f, 1.0f);
    
    // OTHER GEOMETRIES
    mySphere.Load();
    mySphere.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);    // You can change the shape fill colour, line colour or linewidth

    arrowX.Load(); arrowY.Load(); arrowZ.Load();
    arrowX.fillColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); arrowX.lineColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    arrowY.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); arrowY.lineColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    arrowZ.fillColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); arrowZ.lineColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    myFloor.Load();
    myFloor.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
    myFloor.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

    myCylinder.Load();
    myCylinder.fillColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    myCylinder.lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    myLine.Load();
    myLine.fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    myLine.lineColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    myLine.lineWidth = 5.0f;

    // Optimised Graphics
    myGraphics.SetOptimisations();        // Cull and depth testing

}

void updateCamera() {

    // calculate movement for FPS camera
    GLfloat xoffset = myGraphics.mouseX - myGraphics.cameraLastX;
    GLfloat yoffset = myGraphics.cameraLastY - myGraphics.mouseY;    // Reversed mouse movement
    myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX;
    myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY;

    GLfloat sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    myGraphics.cameraYaw += xoffset;
    myGraphics.cameraPitch += yoffset;

    // check for pitch out of bounds otherwise screen gets flipped
    if (myGraphics.cameraPitch > 89.0f) myGraphics.cameraPitch = 89.0f;
    if (myGraphics.cameraPitch < -89.0f) myGraphics.cameraPitch = -89.0f;

    // Calculating FPS camera movement (See 'Additional Reading: Yaw and Pitch to Vector Calculations' in VISION)
    glm::vec3 front;
    front.x = cos(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));
    front.y = sin(glm::radians(myGraphics.cameraPitch));
    front.z = sin(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));

    myGraphics.cameraFront = glm::normalize(front);

    // Update movement using the keys
    GLfloat cameraSpeed = 1.0f * deltaTime;
    if (keyStatus[GLFW_KEY_W]) myGraphics.cameraPosition += cameraSpeed * myGraphics.cameraFront;
    if (keyStatus[GLFW_KEY_S]) myGraphics.cameraPosition -= cameraSpeed * myGraphics.cameraFront;
    if (keyStatus[GLFW_KEY_A]) myGraphics.cameraPosition -= glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;
    if (keyStatus[GLFW_KEY_D]) myGraphics.cameraPosition += glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;

    // IMPORTANT PART
    // Calculate my view matrix using the lookAt helper function
    if (mouseEnabled) {
        myGraphics.viewMatrix = glm::lookAt(myGraphics.cameraPosition,             // eye
            myGraphics.cameraPosition + myGraphics.cameraFront,                    // centre
            myGraphics.cameraUp);                                                  // up
    }
}

void updateSceneElements() {

    glfwPollEvents();                                // poll callbacks

    // Calculate frame time/period -- used for all (physics, animation, logic, etc).
    GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
    deltaTime = currentTime - lastTime;                // Calculate delta time
    lastTime = currentTime;                            // Save for next frame calculations.

    // Do not forget your ( T * R * S ) http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/

    // Calculate Cube position
    glm::mat4 mv_matrix_cube =
        glm::translate(glm::vec3(2.0f, 0.5f, 0.0f)) *
        glm::mat4(1.0f);
    myCube.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
    myCube.proj_matrix = myGraphics.proj_matrix;

    // calculate Sphere movement
    glm::mat4 mv_matrix_sphere =
        glm::translate(glm::vec3(-2.0f, 0.5f, 0.0f)) *
        glm::rotate(-position, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(-position, glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::mat4(1.0f);
    mySphere.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
    mySphere.proj_matrix = myGraphics.proj_matrix;

    //Calculate Arrows translations (note: arrow model points up)
    glm::mat4 mv_matrix_x =
        glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::vec3(0.2f, 0.5f, 0.2f)) *
        glm::mat4(1.0f);
    arrowX.mv_matrix = myGraphics.viewMatrix * mv_matrix_x;
    arrowX.proj_matrix = myGraphics.proj_matrix;

    glm::mat4 mv_matrix_y =
        glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
        //glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *    // already model pointing up
        glm::scale(glm::vec3(0.2f, 0.5f, 0.2f)) *
        glm::mat4(1.0f);
    arrowY.mv_matrix = myGraphics.viewMatrix * mv_matrix_y;
    arrowY.proj_matrix = myGraphics.proj_matrix;

    glm::mat4 mv_matrix_z =
        glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::scale(glm::vec3(0.2f, 0.5f, 0.2f)) *
        glm::mat4(1.0f);
    arrowZ.mv_matrix = myGraphics.viewMatrix * mv_matrix_z;
    arrowZ.proj_matrix = myGraphics.proj_matrix;

    // Calculate floor position and resize
    myFloor.mv_matrix = myGraphics.viewMatrix *
        glm::translate(glm::vec3(floorPostitionX, floorPostitionY, floorPostitionZ)) *
        glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
        glm::mat4(1.0f);
    myFloor.proj_matrix = myGraphics.proj_matrix;

    // Calculate cylinder
    myCylinder.mv_matrix = myGraphics.viewMatrix *
        glm::translate(glm::vec3(-1.0f, 0.5f, 2.0f)) *
        glm::mat4(1.0f);
    myCylinder.proj_matrix = myGraphics.proj_matrix;

    // Calculate Line
    myLine.mv_matrix = myGraphics.viewMatrix *
        glm::translate(glm::vec3(1.0f, 0.5f, 2.0f)) *
        glm::mat4(1.0f);
    myLine.proj_matrix = myGraphics.proj_matrix;
    

    if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.

}

void Particle(){

    /*
    glm::vec3 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;*/
    
    //PARTICLE1
    glm::mat4 mv_matrix_particle1 =
        glm::translate(glm::vec3(positionParticleX1, positionParticleY1, positionParticleZ1)) *
    glm::rotate(-position, glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(-position, glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)) *
        glm::mat4(1.0f);
    myParticle1.mv_matrix = myGraphics.viewMatrix * mv_matrix_particle1;
    myParticle1.proj_matrix = myGraphics.proj_matrix;
    
    //PARTICLE2
    glm::mat4 mv_matrix_particle2 =
        glm::translate(glm::vec3(positionParticleX2, positionParticleY2, positionParticleZ2)) *
    glm::rotate(-position, glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(-position, glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)) *
        glm::mat4(1.0f);
    myParticle2.mv_matrix = myGraphics.viewMatrix * mv_matrix_particle2;
    myParticle2.proj_matrix = myGraphics.proj_matrix;
    
    //PARTICLE3
    glm::mat4 mv_matrix_particle3 =
        glm::translate(glm::vec3(positionParticleX3, positionParticleY3, positionParticleZ3)) *
    glm::rotate(-position, glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(-position, glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)) *
        glm::mat4(1.0f);
    myParticle3.mv_matrix = myGraphics.viewMatrix * mv_matrix_particle3;
    myParticle3.proj_matrix = myGraphics.proj_matrix;
    
    
    // Calculate Cube2 position
    glm::mat4 mv_matrix_cube2 =
        glm::translate(glm::vec3(0.5f, position, 0.5f)) *
    glm::rotate(-position, glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(-position, glm::vec3(1.0f, 0.0f, 0.0f)) *
    glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)) *
        glm::mat4(1.0f);
    myCube2.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube2;
    myCube2.proj_matrix = myGraphics.proj_matrix;

  
    if (!particlesEnabled)
    {
        updateParticle1();
        updateParticle2();
        updateParticle3();
        
    }
    else // Before enabling the particles, we move the cube2 up
    {
        velocity += gravity;
        position += velocity * coeffPosition;
        velocityParticle1 = velocity; // velocity of particle is the same as the cube so when the cube disappears and the particle explodes, the movement is smooth
        positionParticleY1 = position;

    }
    
    
    //PARTICLE EXPLOSION other method without bouncing on the ground
    /*
     if (!particlesEnabled)
    {
        
    positionParticleX1 -= (pow(positionParticleX1, 0.6f)) * 0.3f * 0.15f;
    positionParticleY1 -= 1.0f * 0.7f * 0.1f;
    
    positionParticleX2 -= (pow(positionParticleX1, 0.4f)) * -0.4f * 0.15f;
    positionParticleY2 -= 1.4f * 0.7f * 0.1f;
    
    positionParticleX3 -= (pow(positionParticleX1, 0.2f)) * -0.4f * 0.15f;
    positionParticleY3 -= 1.2f * 0.7f * 0.1f;
        if (positionParticleY1 <= 0.5f && positionParticleY2 <= 0.5f && positionParticleY3 <= 0.5f)
        {
            myParticle1.fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            myParticle2.fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            myParticle3.fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        }
    }*/
    
    //if ((position >= 5.0f) || (position <= 0.001f))
    //{
    //    velocity = velocity * -1;
    //}
}

void Flock()
{
    glm::mat4 mv_matrix_flock1 =
        glm::translate(glm::vec3(positionQueenX, positionQueenY, positionQueenZ)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
        glm::mat4(1.0f);
    myFlock1.mv_matrix = myGraphics.viewMatrix * mv_matrix_flock1;
    myFlock1.proj_matrix = myGraphics.proj_matrix;
    
    glm::mat4 mv_matrix_flock2 =
        glm::translate(glm::vec3(positionFlockX, positionFlockY, positionFlockZ)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
        glm::rotate(glm::radians(-90.0f), glm::vec3(positionQueenX, positionQueenY, positionQueenZ)) *
        glm::mat4(1.0f);
    myFlock2.mv_matrix = myGraphics.viewMatrix * mv_matrix_flock2;
    myFlock2.proj_matrix = myGraphics.proj_matrix;
    
    glm::mat4 mv_matrix_flock3 =
        glm::translate(glm::vec3(positionFlockX, positionFlockY + 2.0f, positionFlockZ)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
        glm::mat4(1.0f);
    myFlock3.mv_matrix = myGraphics.viewMatrix * mv_matrix_flock3;
    myFlock3.proj_matrix = myGraphics.proj_matrix;
    
    glm::mat4 mv_matrix_flock4 =
        glm::translate(glm::vec3(positionFlockX, positionFlockY, positionFlockZ + 2.0f)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
    
        glm::mat4(1.0f);
    myFlock4.mv_matrix = myGraphics.viewMatrix * mv_matrix_flock4;
    myFlock4.proj_matrix = myGraphics.proj_matrix;
    
    glm::mat4 mv_matrix_flock5 =
        glm::translate(glm::vec3(positionFlockX + 2.0f, positionFlockY, positionFlockZ)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
        glm::mat4(1.0f);
    myFlock5.mv_matrix = myGraphics.viewMatrix * mv_matrix_flock5;
    myFlock5.proj_matrix = myGraphics.proj_matrix;
    
    float gravityAQX, gravityAQY, gravityAQZ; // Gravity made by the Queen, the flock is attracted by her
    gravityAQX = -(positionQueenX - positionFlockX) * queenGravityCoeff;
    gravityAQY = -(positionQueenY - positionFlockY) * queenGravityCoeff;
    gravityAQZ = -(positionQueenZ - positionFlockZ) * queenGravityCoeff;
    
    velocityFlockX = velocityFlockX * 0.5f + gravityAQX; //on reduce la vitesse plus on se rapproche
    positionFlockX -= velocityFlockX;
    // + (((double) rand() / (RAND_MAX)) - 0.5) * 0.1
    
    velocityFlockY = velocityFlockY * 0.5f + gravityAQY;
    positionFlockY -= velocityFlockY;
    
    velocityFlockZ = velocityFlockZ * 0.5f + gravityAQZ;
    positionFlockZ -= velocityFlockZ;
    
    

    //accelerationQueenX = modifyAcceleration(accelerationQueenX); //function with a parameter
    velocityQueenX = max(min(velocityQueenX + accelerationQueenX, 0.1f), -0.1f); //we limit the velocity
    positionQueenX += velocityQueenX;

    accelerationQueenY = modifyAcceleration(accelerationQueenY); //function with a parameter
    velocityQueenY = max(min(velocityQueenY + accelerationQueenY, 0.1f), -0.1f);
    positionQueenY += velocityQueenY;

    accelerationQueenZ = modifyAcceleration(accelerationQueenZ); //function with a parameter
    velocityQueenZ = max(min(velocityQueenZ + accelerationQueenZ, 0.1f), -0.1f);
    positionQueenZ += velocityQueenZ;
    
    if (positionQueenY <= 0.5f)
    {
        velocityQueenY = -velocityQueenY;
    }
    
    
    if (positionQueenX - positionFlockX <= 0.5f )
    {
        velocityFlockX = velocityFlockX * 1.0f - gravityAQX;
    
    }
    else if (positionQueenY - positionFlockY <= 0.5f)
    {
        velocityFlockY = velocityFlockY * 1.0f - gravityAQY;
    }
    else if (positionQueenZ - positionFlockZ <= 0.5f)
    {
        velocityFlockZ = velocityFlockZ * 1.0f - gravityAQZ;
    }
}

float modifyAcceleration(float acc)
{
    if ((((double) rand() / (RAND_MAX))) > 0.50) //1% of chances to change the acceleration
    {
        return ((((double) rand() / (RAND_MAX))) - 0.5f) * 0.001f;
    }
    return acc;
}


void updateParticle1()
{
    velocityParticle1 += gravity;
    positionParticleY1 += velocityParticle1 * coeffPosition;
    positionParticleX1 += 0.01f;
    
    if(positionParticleY1 <= 0.5f && velocityParticle1 <= 0.0f)
    {
        velocityParticle1 = -velocityParticle1 * reboundParticle1;
    }
    //Si la velocité est trop basse ET qu'on est trop près du sol
}

void updateParticle2()
{
    velocityParticle2 += gravity;
    positionParticleY2 += velocityParticle2 * coeffPosition;
    positionParticleX2 -= 0.01f;
    
    if(positionParticleY2 <= 0.5f && velocityParticle2 <= 0.0f)
    {
        velocityParticle2 = -velocityParticle2 * reboundParticle2;
    }
    //Si la velocité est trop basse ET qu'on est trop près du sol
}

void updateParticle3()
{
    velocityParticle3 += gravity;
    positionParticleY3 += velocityParticle2 * coeffPosition;
    positionParticleZ3 += 0.01f;
    
    if(positionParticleY3 <= 0.5f && velocityParticle3 <= 0.0f)
    {
        velocityParticle3 = -velocityParticle3 * reboundParticle3;
    }
    //Si la velocité est trop basse ET qu'on est trop près du sol
    
}

void renderScene() {
    // Clear viewport - start a new frame.
    myGraphics.ClearViewport();

    // Draw objects in screen
    myFloor.Draw();
    myCube.Draw();
    
    double now = glfwGetTime();
        
    if (velocity >= 0.05f) //While the cube rises up, it will slow down at some point, draw it. But it is slowed down enough, don't show it anymore
    {
        myCube2.Draw();
    }
     
    else
    {
        particlesEnabled = false;
    }
    
    if (!particlesEnabled)
    {
        if ((now < 10.0f))
        {
            myParticle1.Draw();
            myParticle2.Draw();
            myParticle3.Draw();
        }
    }
   
    myFlock1.Draw();
    myFlock2.Draw();
    //myFlock3.Draw();
    //myFlock4.Draw();
    //myFlock5.Draw();
    
    mySphere.Draw();

    arrowX.Draw();
    arrowY.Draw();
    arrowZ.Draw();

    myLine.Draw();
    myCylinder.Draw();    
    
}


// CallBack functions low level functionality.
void onResizeCallback(GLFWwindow* window, int w, int h) {    // call everytime the window is resized
    //myGraphics.windowWidth = w;
    //myGraphics.windowHeight = h;

    glfwGetFramebufferSize(window, &myGraphics.windowWidth, &myGraphics.windowHeight);

    myGraphics.aspect = (float)w / (float)h;
    myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { // called everytime a key is pressed
    if (action == GLFW_PRESS) keyStatus[key] = true;
    else if (action == GLFW_RELEASE) keyStatus[key] = false;

    // toggle showing mouse.
    if (keyStatus[GLFW_KEY_M]) {
        mouseEnabled = !mouseEnabled;
        myGraphics.ToggleMouse();
    }
    // If exit key pressed.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
    int mouseX = static_cast<int>(x);
    int mouseY = static_cast<int>(y);

    myGraphics.mouseX = mouseX;
    myGraphics.mouseY = mouseY;

    // helper variables for FPS camera
    if (myGraphics.cameraFirstMouse) {
        myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX; myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY; myGraphics.cameraFirstMouse = false;
    }
}

void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
    int yoffsetInt = static_cast<int>(yoffset);
}

