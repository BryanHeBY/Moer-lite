#pragma once
#include "Acceleration.h"
class Octree : public Acceleration {
public:
  Octree() = default;
  void build() override;
  bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                    float *v) const override;

protected:
  static constexpr int ocLeafMaxSize = 64;
  struct OctreeNode {
    AABB boundingBox;
    std::unique_ptr<OctreeNode> subNodes[8];
    int primCount = -1;
    int primIdxBuffer[ocLeafMaxSize];
  };
  std::unique_ptr<OctreeNode> root;
  std::unique_ptr<OctreeNode>
  recursiveBuild(const AABB &aabb, const std::vector<int> &primIdxBuffer);
  bool recursiveRayIntersect(const std::unique_ptr<OctreeNode> &node, Ray &ray,
                             int *geomID, int *primID, float *u,
                             float *v) const;
};
