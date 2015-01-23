#ifndef CUBE_H
#define CUBE_H

#include <geometry.h>

class Cube : public Geometry
{
    Q_OBJECT

public:
    Cube(QObject* parent = nullptr) : Geometry(parent) {
        initProgram(this, parent);
    }

protected:
    std::vector<GLfloat>& getVertices() {
        return c_vertices;
    }
    std::vector<GLfloat>& getColors() {
        return c_colors;
    }
    std::vector<quint32>& getIndices() {
        return c_indices;
    }

    static std::vector<GLfloat> c_vertices;
    static std::vector<GLfloat> c_colors;
    static std::vector<quint32> c_indices;
};

#endif // CUBE_H
