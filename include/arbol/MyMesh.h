#include <MyEntity.h>

class MyMesh: public MyEntity
{
public:
    // CONSTRUCTOR
    MyMesh() {}

    // DESTRUCTOR
    ~MyMesh() {}

protected:
    int* mesh;

public:
    // CUSTOM FUNCTIONS
    // void loadMesh(/*FILE*/);

    void draw(glm::mat4 matrix) override;
};