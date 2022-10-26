
#include "trackball.h"
#include "camera.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Eigen;

void Trackball::track(const Vector2i& point2D)
{
  if (mpCamera==0)
    return;
  Vector3f newPoint3D;
  bool newPointOk = mapToSphere(point2D, newPoint3D);

  if (mLastPointOk && newPointOk)
  {
    Vector3f axis = mLastPoint3D.cross(newPoint3D);
    float cos_angle = mLastPoint3D.dot(newPoint3D);
    if ( std::abs(cos_angle) < 1.0 && axis.norm()>0)
    {
      float angle = 2. * std::acos(cos_angle);
      mpCamera->rotateAroundTarget(angle, axis.normalized());
    }
  }

  mLastPoint3D = newPoint3D;
  mLastPointOk = newPointOk;
}

bool Trackball::mapToSphere(const Vector2i& p2, Vector3f& v3)
{
  if ((p2.x() >= 0) && (p2.x() <= int(mpCamera->vpWidth())) &&
      (p2.y() >= 0) && (p2.y() <= int(mpCamera->vpHeight())) )
  {
    double x  = (double)(p2.x() - 0.5*mpCamera->vpWidth())  / (double)mpCamera->vpWidth();
    double y  = (double)(0.5*mpCamera->vpHeight() - p2.y()) / (double)mpCamera->vpHeight();
    double sinx         = std::sin(M_PI * x * 0.5);
    double siny         = std::sin(M_PI * y * 0.5);
    double sinx2siny2   = sinx * sinx + siny * siny;

    v3 << sinx,
          siny,
          (sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0);

    return true;
  }
  else
    return false;
}
