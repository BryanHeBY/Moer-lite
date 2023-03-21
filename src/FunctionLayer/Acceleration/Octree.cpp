#include "Octree.h"
#include <queue>

std::unique_ptr<Octree::OctreeNode>
Octree::recursiveBuild(const AABB &aabb,
                       const std::vector<int> &primIdxBuffer) {
  //* todo 完成递归构建八叉树
  //* 构建方法请看实验手册
  //* 要注意的一种特殊是当节点的某个子包围盒和当前节点所有物体都相交，我们就不用细分了，当前节点作为叶子节点即可。

  if (primIdxBuffer.size() == 0)
    return nullptr;

  auto node = std::make_unique<Octree::OctreeNode>();
  node->boundingBox = aabb;
  node->primCount = 0;

  if (primIdxBuffer.size() < ocLeafMaxSize) {
    node->primCount = primIdxBuffer.size();
    for (int i = 0; i < primIdxBuffer.size(); i++)
      node->primIdxBuffer[i] = primIdxBuffer[i];
    return node;
  }

  Point3f pBox[3] = {aabb.pMin, aabb.Center(), aabb.pMax};
  AABB subBoxes[8];
  std::vector<int> subBufferes[8];

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++)
        subBoxes[i * 4 + j * 2 + k] =
            AABB(Point3f{pBox[i][0], pBox[j][1], pBox[k][2]},
                 Point3f{pBox[i + 1][0], pBox[j + 1][1], pBox[k + 1][2]});

  for (int i = 0; i < 8; i++)
    for (int idx : primIdxBuffer)
      if (shapes[idx]->getAABB().Overlap(subBoxes[i]))
        subBufferes[i].push_back(idx);

  int overallSubBoxIdx = -1;

  for (int i = 0; i < 8; i++) {
    if (subBufferes[i].size() == primIdxBuffer.size())
      return recursiveBuild(subBoxes[i], subBufferes[i]);
    node->subNodes[i] = recursiveBuild(subBoxes[i], subBufferes[i]);
  }
  return node;
}
void Octree::build() {
  //* 首先计算整个场景的范围
  for (const auto &shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();

    boundingBox.Expand(shape->getAABB());
  }

  //* 构建八叉树
  std::vector<int> primIdxBuffer(shapes.size());
  std::iota(primIdxBuffer.begin(), primIdxBuffer.end(), 0);
  root = recursiveBuild(boundingBox, primIdxBuffer);
}

bool Octree::recursiveRayIntersect(const std::unique_ptr<OctreeNode> &node, Ray &ray,
                                   int *geomID, int *primID, float *u,
                                   float *v) const {
  if(node == nullptr)
    return false;

  if(!node->boundingBox.RayIntersect(ray, nullptr, nullptr))
    return false;
  
  if(node->primCount != 0) {
    bool intersect = false;
    for(int i = 0; i < node->primCount; i++)
      if(shapes[node->primIdxBuffer[i]]->rayIntersectShape(ray, primID, u, v)) {
        intersect = true;
        *geomID = node->primIdxBuffer[i];
      }
    return intersect;
  }

  bool intersect = false;
  for(int i = 0; i < 8; i++)
    intersect |= recursiveRayIntersect(node->subNodes[i], ray, geomID, primID, u, v);
  return intersect;
}

bool Octree::rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                          float *v) const {
  //*todo 完成八叉树求交

  return recursiveRayIntersect(root, ray, geomID, primID, u, v);
}