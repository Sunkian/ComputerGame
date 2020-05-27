//
//  Particles.hpp
//  CGP_Coursework1
//
//  Created by Alice Pagnoux on 27/02/2020.
//  Copyright © 2020 Alice Pagnoux. All rights reserved.
//
#pragma once
#ifndef Particles_hpp
#define Particles_hpp

#include <stdio.h>

#include <iostream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Particles {

public:
    Particles();
    ~Particles();

    //void Load();
    //void Draw();


    GLuint          program;
    GLuint          vao;
    GLuint          buffer;
    GLint           mv_location;
    GLint           proj_location;
    GLint           color_location;
    glm::mat4        proj_matrix = glm::mat4(1.0f);
    glm::mat4        mv_matrix = glm::mat4(1.0f);

    glm::vec4        fillColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    glm::vec4        lineColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
    float            lineWidth = 2.0f;

protected:
    string rawData;            // Import obj file from Blender (note: no textures or UVs).
    void LoadObj();
};



#endif /* Particles_hpp */
