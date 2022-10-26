// Copyright (C) 2008-2011 Gael Guennebaud <gael.guennebaud@inria.fr>

#ifndef _Mesh_h_
#define _Mesh_h_

#include <vector>
#include <string>
#include <Eigen/Core>

class Shader;

/** \class Mesh
  * A class to represent a 3D triangular mesh
  */
class Mesh
{
    typedef Eigen::Matrix<float,2,1,Eigen::DontAlign> Vector2f;
    typedef Eigen::Vector3f Vector3f;
    typedef Eigen::Matrix<float,4,1,Eigen::DontAlign> Vector4f;
    typedef Eigen::Vector3i Vector3i;
public:
  
    Mesh() {}

    /** Destructor */
    virtual ~Mesh();

    /** load a triangular mesh from the file \a filename (.off or .obj) */
    bool load(const std::string& filename);

    void computeNormals();

    /** initialize OpenGL's Vertex Buffer Array (must be called once before calling draw()) */
    void initVBA();

    /** Send the mesh to OpenGL for drawing using shader \a shd */
    virtual void draw(const Shader& shd);

protected:

    /** Loads a triangular mesh in the OFF format */
    bool loadOFF(const std::string& filename);
    bool loadOBJ(const std::string& filename);
  
    /** Represents a vertex of the mesh */
    struct Vertex
    {
      Vertex(const Vector3f& pos = Vector3f::Zero(), const Vector3f& n = Vector3f::Zero())
        : position(pos), normal(n), color(Vector4f(0.6,0.6,0.6,1.0)), texcoord(Vector2f::Zero())
      {}
      
      Vector3f position;
      Vector3f normal;
      Vector4f color;
      Vector2f texcoord;
    };

    /** The list of vertices */
    std::vector<Vertex> mVertices;
    
    /** The list of face indices */
    std::vector<Vector3i> mFaces;
    
    unsigned int mVertexArrayId;
    unsigned int mVertexBufferId; ///< the id of the BufferObject storing the vertex attributes
    unsigned int mIndexBufferId;  ///< the id of the BufferObject storing the faces indices
    bool mIsInitialized;
};

#endif
