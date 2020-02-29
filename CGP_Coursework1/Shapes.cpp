#include "Shapes.hpp"
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Shapes::Shapes() {

};

Shapes::~Shapes() {

}

void Shapes::LoadObj() {

    std::vector< glm::vec3 > obj_vertices;
    std::vector< unsigned int > vertexIndices;
    istringstream rawDataStream(rawData);
    string dataLine;  int linesDone = 0;

    while (std::getline(rawDataStream, dataLine)) {
        if (dataLine.find("v ") != string::npos) {    // does this line have a vector?
            glm::vec3 vertex;

            int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
            vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

            foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
            vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

            foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
            vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

            obj_vertices.push_back(vertex);
        }
        else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
            string parts[3];

            int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
            parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

            foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
            parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

            foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
            parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

            for (int i = 0; i < 3; i++) {        // for each part

                vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

                int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

                if (firstSlash != (secondSlash + 1)) {    // there is texture coordinates.
                                                        // add code for my texture coordintes here.
                }
            }
        }

        linesDone++;
    }

    for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 0] - 1].x);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 0] - 1].y);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 0] - 1].z);

        vertexPositions.push_back(obj_vertices[vertexIndices[i + 1] - 1].x);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 1] - 1].y);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 1] - 1].z);

        vertexPositions.push_back(obj_vertices[vertexIndices[i + 2] - 1].x);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 2] - 1].y);
        vertexPositions.push_back(obj_vertices[vertexIndices[i + 2] - 1].z);
    }
}


void Shapes::Load() {
    //Vertex Shader
    const char * vs_source[] = { R"(
#version 330 core

in vec4 position;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void){
    gl_Position = proj_matrix * mv_matrix * position;
}
)" };

    //Fragment Shader
    const char * fs_source[] = { R"(
#version 330 core

uniform vec4 inColor;
out vec4 color;

void main(void){
    color = inColor;
}
)" };

    program = glCreateProgram();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, fs_source, NULL);
    glCompileShader(fs);
    checkErrorShader(fs);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, vs_source, NULL);
    glCompileShader(vs);
    checkErrorShader(vs);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    mv_location = glGetUniformLocation(program, "mv_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");
    color_location = glGetUniformLocation(program, "inColor");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
        vertexPositions.size() * sizeof(GLfloat),
        &vertexPositions[0],
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glLinkProgram(0);    // unlink
    glDisableVertexAttribArray(0); // Disable
    glBindVertexArray(0);    // Unbind
}

void Shapes::Draw() {
    glUseProgram(program);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);

    glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
    glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mv_matrix[0][0]);

    glUniform4f(color_location, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size() / 3);

    glUniform4f(color_location, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  glLineWidth(lineWidth);
    glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size() / 3);
}


void Shapes::checkErrorShader(GLuint shader) {
    // Get log length
    GLint maxLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // Init a string for it
    std::vector<GLchar> errorLog(maxLength);

    if (maxLength > 1) {
        // Get the log file
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        cout << "--------------Shader compilation error-------------\n";
        cout << errorLog.data();
    }
}

Cube::Cube() {
    // Exported from Blender a cube by default (OBJ File)
    rawData = R"(
v 0.500000 -0.500000 -0.500000
v 0.500000 -0.500000 0.500000
v -0.500000 -0.500000 0.500000
v -0.500000 -0.500000 -0.500000
v 0.500000 0.500000 -0.499999
v 0.499999 0.500000 0.500000
v -0.500000 0.500000 0.500000
v -0.500000 0.500000 -0.500000
f 1 3 4
f 8 6 5
f 5 2 1
f 6 3 2
f 7 4 3
f 1 8 5
f 1 2 3
f 8 7 6
f 5 6 2
f 6 7 3
f 7 8 4
f 1 4 8)";

    LoadObj();
}

Cube::~Cube() {

}


Cube2::Cube2() {
    // Exported from Blender a cube by default (OBJ File)
    rawData = R"(
v 0.500000 -0.500000 -0.500000
v 0.500000 -0.500000 0.500000
v -0.500000 -0.500000 0.500000
v -0.500000 -0.500000 -0.500000
v 0.500000 0.500000 -0.499999
v 0.499999 0.500000 0.500000
v -0.500000 0.500000 0.500000
v -0.500000 0.500000 -0.500000
f 1 3 4
f 8 6 5
f 5 2 1
f 6 3 2
f 7 4 3
f 1 8 5
f 1 2 3
f 8 7 6
f 5 6 2
f 6 7 3
f 7 8 4
f 1 4 8)";

    LoadObj();
}

Cube2::~Cube2() {

}

Sphere::Sphere() {

    rawData = R"(
o Sphere
v 0.000000 -0.154509 0.475528
v 0.000000 -0.404509 0.293893
v -0.090818 0.475528 0.125000
v -0.172746 0.404509 0.237764
v -0.237764 0.293893 0.327254
v -0.279509 0.154508 0.384710
v -0.293893 -0.000000 0.404508
v -0.279509 -0.154509 0.384710
v -0.237764 -0.293893 0.327254
v -0.172746 -0.404509 0.237764
v -0.090818 -0.475528 0.125000
v -0.146946 0.475528 0.047746
v -0.279509 0.404509 0.090818
v -0.384711 0.293893 0.125000
v -0.452254 0.154508 0.146946
v -0.475528 -0.000000 0.154508
v -0.452254 -0.154509 0.146946
v -0.384711 -0.293893 0.125000
v -0.279509 -0.404509 0.090818
v -0.146946 -0.475528 0.047746
v -0.146946 0.475528 -0.047746
v -0.279509 0.404509 -0.090818
v -0.384711 0.293893 -0.125000
v -0.452254 0.154508 -0.146946
v -0.475528 -0.000000 -0.154509
v -0.452254 -0.154509 -0.146947
v -0.384711 -0.293893 -0.125000
v -0.279509 -0.404509 -0.090818
v -0.146946 -0.475528 -0.047746
v -0.090818 0.475528 -0.125000
v -0.172746 0.404509 -0.237764
v -0.237764 0.293893 -0.327255
v -0.279509 0.154508 -0.384711
v -0.293893 -0.000000 -0.404509
v -0.279509 -0.154509 -0.384711
v -0.237764 -0.293893 -0.327255
v -0.172746 -0.404509 -0.237764
v -0.090818 -0.475528 -0.125000
v -0.000000 0.475528 -0.154509
v -0.000000 0.404509 -0.293893
v 0.000000 0.293893 -0.404509
v -0.000000 0.154508 -0.475529
v -0.000000 -0.000000 -0.500000
v 0.000000 -0.154509 -0.475529
v 0.000000 -0.293893 -0.404509
v -0.000000 -0.404509 -0.293893
v -0.000000 -0.475528 -0.154509
v 0.000000 0.500000 -0.000000
v 0.090818 0.475528 -0.125000
v 0.172746 0.404509 -0.237764
v 0.237764 0.293893 -0.327255
v 0.279509 0.154508 -0.384711
v 0.293893 -0.000000 -0.404509
v 0.279509 -0.154509 -0.384711
v 0.237764 -0.293893 -0.327255
v 0.172746 -0.404509 -0.237764
v 0.090818 -0.475528 -0.125000
v 0.146946 0.475528 -0.047746
v 0.279509 0.404509 -0.090818
v 0.384711 0.293893 -0.125000
v 0.452254 0.154508 -0.146947
v 0.475528 -0.000000 -0.154509
v 0.452254 -0.154509 -0.146947
v 0.384711 -0.293893 -0.125000
v 0.279509 -0.404509 -0.090818
v 0.146946 -0.475528 -0.047746
v 0.146946 0.475528 0.047746
v 0.279509 0.404509 0.090818
v 0.384711 0.293893 0.125000
v 0.452254 0.154508 0.146946
v 0.475528 -0.000000 0.154508
v 0.452254 -0.154509 0.146946
v 0.384711 -0.293893 0.125000
v 0.279509 -0.404509 0.090818
v 0.146946 -0.475528 0.047746
v 0.090818 0.475528 0.125000
v 0.172746 0.404509 0.237764
v 0.237764 0.293893 0.327254
v 0.279509 0.154508 0.384710
v 0.293893 -0.000000 0.404508
v 0.279509 -0.154509 0.384710
v 0.237764 -0.293893 0.327254
v 0.172746 -0.404509 0.237764
v 0.090818 -0.475528 0.125000
v 0.000000 0.475528 0.154509
v 0.000000 0.404509 0.293893
v 0.000000 0.293893 0.404509
v 0.000000 0.154508 0.475528
v 0.000000 -0.000000 0.500000
v 0.000000 -0.293893 0.404509
v 0.000000 -0.475528 0.154508
v 0.000000 -0.500000 -0.000000
vt 0.700000 0.400000
vt 0.600000 0.500000
vt 0.600000 0.400000
vt 0.700000 0.800000
vt 0.600000 0.700000
vt 0.700000 0.700000
vt 0.650000 0.000000
vt 0.700000 0.100000
vt 0.600000 0.100000
vt 0.600000 0.300000
vt 0.700000 0.300000
vt 0.600000 0.600000
vt 0.700000 0.600000
vt 0.700000 0.900000
vt 0.650000 1.000000
vt 0.600000 0.900000
vt 0.700000 0.200000
vt 0.600000 0.200000
vt 0.700000 0.500000
vt 0.600000 0.800000
vt 0.500000 0.700000
vt 0.550000 0.000000
vt 0.500000 0.100000
vt 0.500000 0.300000
vt 0.500000 0.600000
vt 0.550000 1.000000
vt 0.500000 0.900000
vt 0.500000 0.200000
vt 0.500000 0.500000
vt 0.500000 0.800000
vt 0.500000 0.400000
vt 0.400000 0.200000
vt 0.400000 0.100000
vt 0.400000 0.500000
vt 0.400000 0.400000
vt 0.400000 0.700000
vt 0.450000 0.000000
vt 0.400000 0.300000
vt 0.400000 0.600000
vt 0.450000 1.000000
vt 0.400000 0.900000
vt 0.400000 0.800000
vt 0.300000 0.600000
vt 0.300000 0.500000
vt 0.300000 0.800000
vt 0.300000 0.100000
vt 0.300000 0.400000
vt 0.300000 0.700000
vt 0.350000 0.000000
vt 0.300000 0.300000
vt 0.350000 1.000000
vt 0.300000 0.900000
vt 0.300000 0.200000
vt 0.250000 1.000000
vt 0.200000 0.900000
vt 0.200000 0.200000
vt 0.200000 0.500000
vt 0.200000 0.800000
vt 0.200000 0.100000
vt 0.200000 0.400000
vt 0.200000 0.700000
vt 0.250000 0.000000
vt 0.200000 0.300000
vt 0.200000 0.600000
vt 0.100000 0.600000
vt 0.150000 1.000000
vt 0.100000 0.900000
vt 0.100000 0.200000
vt 0.100000 0.500000
vt 0.100000 0.800000
vt 0.100000 0.100000
vt 0.100000 0.400000
vt 0.100000 0.700000
vt 0.150000 0.000000
vt 0.100000 0.300000
vt 0.050000 0.000000
vt 0.000000 0.100000
vt 0.000000 0.300000
vt 0.000000 0.600000
vt 0.050000 1.000000
vt 0.000000 0.900000
vt 0.000000 0.200000
vt 0.000000 0.500000
vt 0.000000 0.800000
vt 0.000000 0.400000
vt 0.000000 0.700000
vt 1.000000 0.400000
vt 0.900000 0.500000
vt 0.900000 0.400000
vt 1.000000 0.700000
vt 0.900000 0.800000
vt 0.900000 0.700000
vt 0.950000 0.000000
vt 1.000000 0.100000
vt 0.900000 0.100000
vt 0.900000 0.300000
vt 1.000000 0.300000
vt 0.900000 0.600000
vt 1.000000 0.600000
vt 1.000000 0.900000
vt 0.950000 1.000000
vt 0.900000 0.900000
vt 0.900000 0.200000
vt 1.000000 0.200000
vt 1.000000 0.500000
vt 1.000000 0.800000
vt 0.800000 0.800000
vt 0.800000 0.200000
vt 0.800000 0.100000
vt 0.800000 0.500000
vt 0.800000 0.400000
vt 0.800000 0.700000
vt 0.850000 0.000000
vt 0.800000 0.300000
vt 0.800000 0.600000
vt 0.850000 1.000000
vt 0.800000 0.900000
vt 0.750000 0.000000
vt 0.750000 1.000000
vn -0.3056 -0.1490 0.9404
vn -0.2239 0.6892 0.6892
vn -0.0508 -0.9864 0.1562
vn -0.2781 -0.4361 0.8559
vn -0.2781 0.4361 0.8559
vn -0.0508 0.9864 0.1562
vn -0.2239 -0.6892 0.6892
vn -0.3056 0.1490 0.9404
vn -0.1459 0.8815 0.4491
vn -0.1459 -0.8815 0.4491
vn -0.5862 0.6892 0.4259
vn -0.1329 -0.9864 0.0966
vn -0.7280 -0.4361 0.5290
vn -0.7280 0.4361 0.5290
vn -0.1329 0.9864 0.0966
vn -0.5862 -0.6892 0.4259
vn -0.8000 0.1490 0.5812
vn -0.3821 0.8815 0.2776
vn -0.3821 -0.8815 0.2776
vn -0.8000 -0.1490 0.5812
vn -0.4722 -0.8815 0.0000
vn -0.9888 -0.1490 0.0000
vn -0.7246 0.6892 0.0000
vn -0.1643 -0.9864 0.0000
vn -0.8999 -0.4361 0.0000
vn -0.8999 0.4361 0.0000
vn -0.1643 0.9864 0.0000
vn -0.7246 -0.6892 0.0000
vn -0.9888 0.1490 0.0000
vn -0.4722 0.8815 0.0000
vn -0.8000 0.1490 -0.5812
vn -0.3821 0.8815 -0.2776
vn -0.3821 -0.8815 -0.2776
vn -0.8000 -0.1490 -0.5812
vn -0.5862 0.6892 -0.4259
vn -0.1329 -0.9864 -0.0966
vn -0.7280 -0.4361 -0.5290
vn -0.7280 0.4361 -0.5290
vn -0.1329 0.9864 -0.0966
vn -0.5862 -0.6892 -0.4259
vn -0.0508 0.9864 -0.1562
vn -0.2239 -0.6892 -0.6892
vn -0.3056 0.1490 -0.9404
vn -0.1459 0.8815 -0.4491
vn -0.1459 -0.8815 -0.4491
vn -0.3056 -0.1490 -0.9404
vn -0.2239 0.6892 -0.6892
vn -0.0508 -0.9864 -0.1562
vn -0.2781 -0.4361 -0.8559
vn -0.2781 0.4361 -0.8559
vn 0.2781 0.4361 -0.8559
vn 0.0508 0.9864 -0.1562
vn 0.2239 -0.6892 -0.6892
vn 0.3056 0.1490 -0.9404
vn 0.1459 0.8815 -0.4491
vn 0.1459 -0.8815 -0.4491
vn 0.3056 -0.1490 -0.9404
vn 0.2239 0.6892 -0.6892
vn 0.0508 -0.9864 -0.1562
vn 0.2781 -0.4361 -0.8559
vn 0.1329 -0.9864 -0.0966
vn 0.7280 -0.4361 -0.5290
vn 0.7280 0.4361 -0.5290
vn 0.1329 0.9864 -0.0966
vn 0.5862 -0.6892 -0.4259
vn 0.8000 0.1490 -0.5812
vn 0.3821 0.8815 -0.2776
vn 0.3821 -0.8815 -0.2776
vn 0.8000 -0.1490 -0.5812
vn 0.5862 0.6892 -0.4259
vn 0.9888 -0.1490 0.0000
vn 0.7246 0.6892 0.0000
vn 0.1643 -0.9864 -0.0000
vn 0.8999 -0.4361 0.0000
vn 0.8999 0.4361 -0.0000
vn 0.1643 0.9864 0.0000
vn 0.7246 -0.6892 -0.0000
vn 0.9888 0.1490 -0.0000
vn 0.4722 0.8815 0.0000
vn 0.4722 -0.8815 0.0000
vn 0.3821 0.8815 0.2776
vn 0.3821 -0.8815 0.2776
vn 0.8000 -0.1490 0.5812
vn 0.5862 0.6892 0.4259
vn 0.1329 -0.9864 0.0966
vn 0.7280 -0.4361 0.5290
vn 0.7280 0.4361 0.5290
vn 0.1329 0.9864 0.0966
vn 0.5862 -0.6892 0.4259
vn 0.8000 0.1490 0.5812
vn 0.3056 0.1490 0.9404
vn 0.1459 0.8815 0.4491
vn 0.1459 -0.8815 0.4491
vn 0.3056 -0.1490 0.9404
vn 0.2239 0.6892 0.6892
vn 0.0508 -0.9864 0.1562
vn 0.2781 -0.4361 0.8559
vn 0.2781 0.4361 0.8559
vn 0.0508 0.9864 0.1562
vn 0.2239 -0.6892 0.6892
s off
f 1/1/1 7/2/1 8/3/1
f 86/4/2 5/5/2 87/6/2
f 92/7/3 91/8/3 11/9/3
f 1/1/4 9/10/4 90/11/4
f 87/6/5 6/12/5 88/13/5
f 85/14/6 48/15/6 3/16/6
f 2/17/7 9/10/7 10/18/7
f 88/13/8 7/2/8 89/19/8
f 85/14/9 4/20/9 86/4/9
f 91/8/10 10/18/10 11/9/10
f 4/20/11 14/21/11 5/5/11
f 92/22/12 11/9/12 20/23/12
f 8/3/13 18/24/13 9/10/13
f 6/12/14 14/21/14 15/25/14
f 3/16/15 48/26/15 12/27/15
f 9/10/16 19/28/16 10/18/16
f 6/12/17 16/29/17 7/2/17
f 3/16/18 13/30/18 4/20/18
f 10/18/19 20/23/19 11/9/19
f 8/3/20 16/29/20 17/31/20
f 20/23/21 28/32/21 29/33/21
f 17/31/22 25/34/22 26/35/22
f 13/30/23 23/36/23 14/21/23
f 92/37/24 20/23/24 29/33/24
f 17/31/25 27/38/25 18/24/25
f 14/21/26 24/39/26 15/25/26
f 12/27/27 48/40/27 21/41/27
f 18/24/28 28/32/28 19/28/28
f 15/25/29 25/34/29 16/29/29
f 12/27/30 22/42/30 13/30/30
f 25/34/31 33/43/31 34/44/31
f 21/41/32 31/45/32 22/42/32
f 28/32/33 38/46/33 29/33/33
f 26/35/34 34/44/34 35/47/34
f 23/36/35 31/45/35 32/48/35
f 92/49/36 29/33/36 38/46/36
f 26/35/37 36/50/37 27/38/37
f 23/36/38 33/43/38 24/39/38
f 21/41/39 48/51/39 30/52/39
f 27/38/40 37/53/40 28/32/40
f 30/52/41 48/54/41 39/55/41
f 36/50/42 46/56/42 37/53/42
f 33/43/43 43/57/43 34/44/43
f 30/52/44 40/58/44 31/45/44
f 37/53/45 47/59/45 38/46/45
f 35/47/46 43/57/46 44/60/46
f 32/48/47 40/58/47 41/61/47
f 92/62/48 38/46/48 47/59/48
f 35/47/49 45/63/49 36/50/49
f 32/48/50 42/64/50 33/43/50
f 41/61/51 52/65/51 42/64/51
f 39/55/52 48/66/52 49/67/52
f 45/63/53 56/68/53 46/56/53
f 42/64/54 53/69/54 43/57/54
f 39/55/55 50/70/55 40/58/55
f 46/56/56 57/71/56 47/59/56
f 44/60/57 53/69/57 54/72/57
f 41/61/58 50/70/58 51/73/58
f 92/74/59 47/59/59 57/71/59
f 44/60/60 55/75/60 45/63/60
f 92/76/61 57/71/61 66/77/61
f 54/72/62 64/78/62 55/75/62
f 51/73/63 61/79/63 52/65/63
f 49/67/64 48/80/64 58/81/64
f 55/75/65 65/82/65 56/68/65
f 52/65/66 62/83/66 53/69/66
f 49/67/67 59/84/67 50/70/67
f 56/68/68 66/77/68 57/71/68
f 54/72/69 62/83/69 63/85/69
f 51/73/70 59/84/70 60/86/70
f 63/87/71 71/88/71 72/89/71
f 60/90/72 68/91/72 69/92/72
f 92/93/73 66/94/73 75/95/73
f 63/87/74 73/96/74 64/97/74
f 60/90/75 70/98/75 61/99/75
f 58/100/76 48/101/76 67/102/76
f 64/97/77 74/103/77 65/104/77
f 61/99/78 71/88/78 62/105/78
f 58/100/79 68/91/79 59/106/79
f 65/104/80 75/95/80 66/94/80
f 67/102/81 77/107/81 68/91/81
f 75/95/82 83/108/82 84/109/82
f 72/89/83 80/110/83 81/111/83
f 69/92/84 77/107/84 78/112/84
f 92/113/85 75/95/85 84/109/85
f 72/89/86 82/114/86 73/96/86
f 69/92/87 79/115/87 70/98/87
f 67/102/88 48/116/88 76/117/88
f 73/96/89 83/108/89 74/103/89
f 71/88/90 79/115/90 80/110/90
f 79/115/91 89/19/91 80/110/91
f 76/117/92 86/4/92 77/107/92
f 83/108/93 91/8/93 84/109/93
f 81/111/94 89/19/94 1/1/94
f 78/112/95 86/4/95 87/6/95
f 92/118/96 84/109/96 91/8/96
f 81/111/97 90/11/97 82/114/97
f 78/112/98 88/13/98 79/115/98
f 76/117/99 48/119/99 85/14/99
f 83/108/100 90/11/100 2/17/100
f 1/1/1 89/19/1 7/2/1
f 86/4/2 4/20/2 5/5/2
f 1/1/4 8/3/4 9/10/4
f 87/6/5 5/5/5 6/12/5
f 2/17/7 90/11/7 9/10/7
f 88/13/8 6/12/8 7/2/8
f 85/14/9 3/16/9 4/20/9
f 91/8/10 2/17/10 10/18/10
f 4/20/11 13/30/11 14/21/11
f 8/3/13 17/31/13 18/24/13
f 6/12/14 5/5/14 14/21/14
f 9/10/16 18/24/16 19/28/16
f 6/12/17 15/25/17 16/29/17
f 3/16/18 12/27/18 13/30/18
f 10/18/19 19/28/19 20/23/19
f 8/3/20 7/2/20 16/29/20
f 20/23/21 19/28/21 28/32/21
f 17/31/22 16/29/22 25/34/22
f 13/30/23 22/42/23 23/36/23
f 17/31/25 26/35/25 27/38/25
f 14/21/26 23/36/26 24/39/26
f 18/24/28 27/38/28 28/32/28
f 15/25/29 24/39/29 25/34/29
f 12/27/30 21/41/30 22/42/30
f 25/34/31 24/39/31 33/43/31
f 21/41/32 30/52/32 31/45/32
f 28/32/33 37/53/33 38/46/33
f 26/35/34 25/34/34 34/44/34
f 23/36/35 22/42/35 31/45/35
f 26/35/37 35/47/37 36/50/37
f 23/36/38 32/48/38 33/43/38
f 27/38/40 36/50/40 37/53/40
f 36/50/42 45/63/42 46/56/42
f 33/43/43 42/64/43 43/57/43
f 30/52/44 39/55/44 40/58/44
f 37/53/45 46/56/45 47/59/45
f 35/47/46 34/44/46 43/57/46
f 32/48/47 31/45/47 40/58/47
f 35/47/49 44/60/49 45/63/49
f 32/48/50 41/61/50 42/64/50
f 41/61/51 51/73/51 52/65/51
f 45/63/53 55/75/53 56/68/53
f 42/64/54 52/65/54 53/69/54
f 39/55/55 49/67/55 50/70/55
f 46/56/56 56/68/56 57/71/56
f 44/60/57 43/57/57 53/69/57
f 41/61/58 40/58/58 50/70/58
f 44/60/60 54/72/60 55/75/60
f 54/72/62 63/85/62 64/78/62
f 51/73/63 60/86/63 61/79/63
f 55/75/65 64/78/65 65/82/65
f 52/65/66 61/79/66 62/83/66
f 49/67/67 58/81/67 59/84/67
f 56/68/68 65/82/68 66/77/68
f 54/72/69 53/69/69 62/83/69
f 51/73/70 50/70/70 59/84/70
f 63/87/71 62/105/71 71/88/71
f 60/90/72 59/106/72 68/91/72
f 63/87/74 72/89/74 73/96/74
f 60/90/75 69/92/75 70/98/75
f 64/97/77 73/96/77 74/103/77
f 61/99/78 70/98/78 71/88/78
f 58/100/79 67/102/79 68/91/79
f 65/104/80 74/103/80 75/95/80
f 67/102/81 76/117/81 77/107/81
f 75/95/82 74/103/82 83/108/82
f 72/89/83 71/88/83 80/110/83
f 69/92/84 68/91/84 77/107/84
f 72/89/86 81/111/86 82/114/86
f 69/92/87 78/112/87 79/115/87
f 73/96/89 82/114/89 83/108/89
f 71/88/90 70/98/90 79/115/90
f 79/115/91 88/13/91 89/19/91
f 76/117/92 85/14/92 86/4/92
f 83/108/93 2/17/93 91/8/93
f 81/111/94 80/110/94 89/19/94
f 78/112/95 77/107/95 86/4/95
f 81/111/97 1/1/97 90/11/97
f 78/112/98 87/6/98 88/13/98
f 83/108/100 82/114/100 90/11/100
)";

    LoadObj();
}

Sphere::~Sphere() {

}

Arrow::Arrow() {

    rawData = R"(
o Cone
v 0.000000 0.800000 -0.100000
v 0.070711 0.800000 -0.070711
v 0.100000 0.800000 -0.000000
v 0.000000 1.000000 0.000000
v 0.070711 0.800000 0.070711
v -0.000000 0.800000 0.100000
v -0.070711 0.800000 0.070711
v -0.100000 0.800000 -0.000000
v -0.070711 0.800000 -0.070711
s off
f 4 7 6
f 5 7 2
f 4 8 7
f 3 4 5
f 5 4 6
f 4 9 8
f 4 1 9
f 2 1 4
f 2 4 3
f 9 1 2
f 2 3 5
f 5 6 7
f 7 8 9
f 9 2 7
o Cylinder
v 0.000000 0.000000 -0.050000
v 0.009755 0.900000 -0.049039
v 0.019134 0.000000 -0.046194
v 0.027779 0.900000 -0.041573
v 0.035355 0.000000 -0.035355
v 0.041573 0.900000 -0.027779
v 0.046194 0.000000 -0.019134
v 0.049039 0.900000 -0.009755
v 0.050000 0.000000 -0.000000
v 0.049039 0.900000 0.009755
v 0.046194 0.000000 0.019134
v 0.041573 0.900000 0.027779
v 0.035355 0.000000 0.035355
v 0.027779 0.900000 0.041573
v 0.019134 0.000000 0.046194
v 0.009755 0.900000 0.049039
v -0.000000 0.000000 0.050000
v -0.009755 0.900000 0.049039
v -0.019134 0.000000 0.046194
v -0.027779 0.900000 0.041573
v -0.035355 0.000000 0.035355
v -0.041574 0.900000 0.027778
v -0.046194 0.000000 0.019134
v -0.049039 0.900000 0.009754
v -0.050000 0.000000 -0.000000
v -0.049039 0.900000 -0.009755
v -0.046194 0.000000 -0.019134
v -0.041573 0.900000 -0.027779
v -0.035355 0.000000 -0.035355
v -0.027778 0.900000 -0.041574
v -0.019134 0.000000 -0.046194
v -0.009754 0.900000 -0.049039
s off
f 13 15 14
f 16 14 15
f 17 19 18
f 18 16 17
f 19 21 20
f 20 18 19
f 21 23 22
f 22 20 21
f 23 25 24
f 24 22 23
f 25 27 26
f 26 24 25
f 27 29 28
f 28 26 27
f 29 31 30
f 30 28 29
f 31 33 32
f 32 30 31
f 33 35 34
f 34 32 33
f 35 37 36
f 36 34 35
f 37 39 38
f 38 36 37
f 41 40 39
f 40 38 39
f 41 10 40
f 29 21 37
f 11 12 10
f 24 32 16
f 15 17 16
f 11 13 12
f 14 12 13
f 10 41 11
f 13 11 41
f 41 39 37
f 37 35 33
f 33 31 29
f 29 27 25
f 25 23 29
f 21 19 17
f 17 15 13
f 13 41 37
f 37 33 29
f 29 23 21
f 21 17 13
f 13 37 21
f 40 10 12
f 12 14 16
f 16 18 20
f 20 22 24
f 24 26 28
f 28 30 32
f 32 34 36
f 36 38 40
f 40 12 16
f 16 20 24
f 24 28 32
f 32 36 40
f 40 16 32
)";

    LoadObj();
}

Arrow::~Arrow() {

}

Cylinder::Cylinder() {

    rawData = R"(
o Cylinder
v 0.000000 -0.500000 -0.500000
v 0.000000 0.500000 -0.500000
v 0.293893 -0.500000 -0.404509
v 0.293893 0.500000 -0.404509
v 0.475528 -0.500000 -0.154508
v 0.475528 0.500000 -0.154508
v 0.475528 -0.500000 0.154509
v 0.475528 0.500000 0.154509
v 0.293893 -0.500000 0.404509
v 0.293893 0.500000 0.404509
v -0.000000 -0.500000 0.500000
v -0.000000 0.500000 0.500000
v -0.293893 -0.500000 0.404508
v -0.293893 0.500000 0.404508
v -0.475528 -0.500000 0.154509
v -0.475528 0.500000 0.154509
v -0.475528 -0.500000 -0.154509
v -0.475528 0.500000 -0.154509
v -0.293892 -0.500000 -0.404509
v -0.293892 0.500000 -0.404509
vt 1.000000 1.000000
vt 0.900000 0.500000
vt 1.000000 0.500000
vt 0.900000 1.000000
vt 0.800000 0.500000
vt 0.800000 1.000000
vt 0.700000 0.500000
vt 0.700000 1.000000
vt 0.600000 0.500000
vt 0.600000 1.000000
vt 0.500000 0.500000
vt 0.500000 1.000000
vt 0.400000 0.500000
vt 0.400000 1.000000
vt 0.300000 0.500000
vt 0.300000 1.000000
vt 0.200000 0.500000
vt 0.250000 0.490000
vt 0.021746 0.324164
vt 0.391068 0.055836
vt 0.200000 1.000000
vt 0.100000 0.500000
vt 0.100000 1.000000
vt -0.000000 0.500000
vt 0.978254 0.175836
vt 0.750000 0.010000
vt 0.521746 0.175836
vt 0.478254 0.324164
vt 0.391068 0.444164
vt 0.108932 0.444164
vt 0.021746 0.175836
vt 0.108932 0.055836
vt 0.250000 0.010000
vt 0.478254 0.175836
vt -0.000000 1.000000
vt 0.608932 0.444164
vt 0.750000 0.490000
vt 0.891068 0.444164
vt 0.978254 0.324164
vt 0.891068 0.055836
vt 0.608932 0.055836
vt 0.521746 0.324164
vn 0.3090 0.0000 -0.9511
vn 0.8090 0.0000 -0.5878
vn 1.0000 0.0000 0.0000
vn 0.8090 0.0000 0.5878
vn 0.3090 0.0000 0.9511
vn -0.3090 0.0000 0.9511
vn -0.8090 0.0000 0.5878
vn -1.0000 0.0000 0.0000
vn 0.0000 1.0000 0.0000
vn -0.8090 0.0000 -0.5878
vn -0.3090 0.0000 -0.9511
vn 0.0000 -1.0000 0.0000
usemtl None
s off
f 2/1/1 3/2/1 1/3/1
f 4/4/2 5/5/2 3/2/2
f 6/6/3 7/7/3 5/5/3
f 8/8/4 9/9/4 7/7/4
f 10/10/5 11/11/5 9/9/5
f 12/12/6 13/13/6 11/11/6
f 14/14/7 15/15/7 13/13/7
f 16/16/8 17/17/8 15/15/8
f 2/18/9 18/19/9 10/20/9
f 18/21/10 19/22/10 17/17/10
f 20/23/11 1/24/11 19/22/11
f 7/25/12 11/26/12 15/27/12
f 2/1/1 4/4/1 3/2/1
f 4/4/2 6/6/2 5/5/2
f 6/6/3 8/8/3 7/7/3
f 8/8/4 10/10/4 9/9/4
f 10/10/5 12/12/5 11/11/5
f 12/12/6 14/14/6 13/13/6
f 14/14/7 16/16/7 15/15/7
f 16/16/8 18/21/8 17/17/8
f 6/28/9 4/29/9 2/18/9
f 2/18/9 20/30/9 18/19/9
f 18/19/9 16/31/9 14/32/9
f 14/32/9 12/33/9 18/19/9
f 12/33/9 10/20/9 18/19/9
f 10/20/9 8/34/9 6/28/9
f 6/28/9 2/18/9 10/20/9
f 18/21/10 20/23/10 19/22/10
f 20/23/11 2/35/11 1/24/11
f 19/36/12 1/37/12 3/38/12
f 3/38/12 5/39/12 7/25/12
f 7/25/12 9/40/12 11/26/12
f 11/26/12 13/41/12 15/27/12
f 15/27/12 17/42/12 19/36/12
f 19/36/12 3/38/12 15/27/12
f 3/38/12 7/25/12 15/27/12
)";

    LoadObj();
}

Cylinder::~Cylinder() {

}


Line::Line() {

    rawData = R"(
o line
v 0.000000 0.000100 -0.500000
v 0.000000 0.000100 0.500000
v 0.000087 -0.000050 -0.500000
v 0.000087 -0.000050 0.500000
v -0.000087 -0.000050 -0.500000
v -0.000087 -0.000050 0.500000
vt 1.000000 1.000000
vt 0.666667 0.500000
vt 1.000000 0.500000
vt 0.666667 1.000000
vt 0.333333 0.500000
vt 0.457846 0.130000
vt 0.250000 0.490000
vt 0.042154 0.130000
vt 0.333333 1.000000
vt -0.000000 0.500000
vt 0.750000 0.490000
vt 0.957846 0.130000
vt 0.542154 0.130000
vt -0.000000 1.000000
vn 0.8660 0.5000 0.0000
vn -0.0000 -1.0000 -0.0000
vn 0.0000 0.0000 1.0000
vn -0.8660 0.5000 0.0000
vn 0.0000 0.0000 -1.0000
usemtl None
s off
f 2/1/1 3/2/1 1/3/1
f 4/4/2 5/5/2 3/2/2
f 4/6/3 2/7/3 6/8/3
f 6/9/4 1/10/4 5/5/4
f 1/11/5 3/12/5 5/13/5
f 2/1/1 4/4/1 3/2/1
f 4/4/2 6/9/2 5/5/2
f 6/9/4 2/14/4 1/10/4
)";

    LoadObj();
}

Line::~Line() {

}
