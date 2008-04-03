#include"YadeCamera.hpp"

float YadeCamera::zNear() const
{
  float z = distanceToSceneCenter() - zClippingCoefficient()*sceneRadius()*(1.f-2*cuttingDistance);

  // Prevents negative or null zNear values.
  const float zMin = zNearCoefficient() * zClippingCoefficient() * sceneRadius();
  if (z < zMin)
/*    switch (type())
      {
      case Camera::PERSPECTIVE  :*/ z = zMin; /*break;
      case Camera::ORTHOGRAPHIC : z = 0.0;  break;
      }*/
  return z;
}

