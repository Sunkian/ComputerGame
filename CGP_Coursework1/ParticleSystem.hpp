//
//  ParticleSystem.hpp
//  CGP_Coursework1
//
//  Created by Alice Pagnoux on 11/02/2020.
//  Copyright © 2020 Alice Pagnoux. All rights reserved.
//

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp
#include <vector>

#include <stdio.h>
#include <glm/glm.hpp>

struct Particle
{
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
    
    Particle() : Position(0.0f), Velocity(0.0f), VelocityVariation(0.0f), ColorBegin(1.0f), ColorEnd(0.0f), LifeTime(0.0f) {}
};

class ParticleSystem
{
public:
    //Constructor
    
};




#endif /* ParticleSystem_hpp */
