#pragma once
#include "Acceleration.h"
class BVH : public Acceleration{
public:
  BVH() = default;
  void build() override;
  bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const override;
protected:
  static constexpr int bvhLeafMaxSize = 64;
  std::vector<int> orderedIdxBuffer;
  struct BVHNode {
    std::unique_ptr<BVHNode> lNode, rNode;
    int lIdx = -1, rIdx = -1;
    AABB boundingBox;
  };
  std::unique_ptr<BVHNode> root;
  std::unique_ptr<BVHNode>
  recursiveBuild(const AABB &aabb, int l, int r);
  bool recursiveRayIntersect(const std::unique_ptr<BVHNode> &node, Ray &ray,
                             int *geomID, int *primID, float *u,
                             float *v) const;
};