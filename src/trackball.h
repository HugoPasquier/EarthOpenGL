
#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <Eigen/Core>

class Camera;

class Trackball
{
  public:

    Trackball() : mpCamera(0) {}
    
    void start() { mLastPointOk = false; }

    void setCamera(Camera* pCam) { mpCamera = pCam; }

    void track(const Eigen::Vector2i& newPoint2D);

  protected:

    bool mapToSphere( const Eigen::Vector2i& p2, Eigen::Vector3f& v3);

    Camera* mpCamera;
    Eigen::Vector3f mLastPoint3D;
    bool mLastPointOk;

};

#endif // TRACKBALL_H
