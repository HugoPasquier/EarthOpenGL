
#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Geometry>

class Camera
{
  public:
    Camera();
    virtual ~Camera();

    /** Setup the camera position and orientation based on its \a position, \a a target point, \a the up vector */
    void lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up);

    /** Setup the perspective projection matrix */
    void setPerspective(float fovY, float near, float far);

    void zoom(float x);
    
    void rotateAroundTarget(float angle, Eigen::Vector3f axis);
    
    /** Returns the affine transformation matrix from the global space to the camera space */
    const Eigen::Matrix4f& viewMatrix() const;
    /** Returns the perspective projection matrix */
    Eigen::Matrix4f projectionMatrix() const;
    
    void setViewport(int width, int height);
    
    int vpWidth() const { return mVpWidth; }
    int vpHeight() const { return mVpHeight; }
    
  protected:

    Eigen::Matrix4f mViewMatrix;
    Eigen::Vector3f mTarget;
    float m_fovY, m_near, m_far;
    int mVpWidth, mVpHeight;
};

#endif // EIGEN_CAMERA_H
