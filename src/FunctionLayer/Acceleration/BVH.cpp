#include "BVH.h"
// struct  BVH::BVHNode{
//     //* todo BVH节点结构设计
// };

float surfaceArea(const AABB &aabb) {
  return 2.f * ((aabb.pMax[0] - aabb.pMin[0]) * (aabb.pMax[1] - aabb.pMin[1]) +
                (aabb.pMax[0] - aabb.pMin[0]) * (aabb.pMax[2] - aabb.pMin[2]) +
                (aabb.pMax[1] - aabb.pMin[1]) * (aabb.pMax[2] - aabb.pMin[2]));
}

std::unique_ptr<BVH::BVHNode>
BVH::recursiveBuild(const AABB &aabb, int l, int r) {
  if (l > r) return nullptr;

  // 如果几何体数量小于叶节点阈值，创建叶节点
  if (r - l < bvhLeafMaxSize) {
    auto node = std::make_unique<BVHNode>();
    node->boundingBox = aabb;
    node->lIdx = l;
    node->rIdx = r;
    node->lNode = nullptr;
    node->rNode = nullptr;
    return node;
  }

  // 初始化 SAH 分割参数
  int bestAxis = -1;
  int bestSplit = -1;
  float bestCost = std::numeric_limits<float>::infinity();

  // 父节点的表面积
  float parentArea = surfaceArea(aabb);

  // 遍历每个轴（X、Y、Z）
  for (int axis = 0; axis < 3; axis++) {
    // 按当前轴对几何体排序
    std::sort(
      orderedIdxBuffer.begin() + l, orderedIdxBuffer.begin() + r + 1,
      [&](int a, int b) {
        return shapes[a]->getAABB().Center()[axis] <
                shapes[b]->getAABB().Center()[axis];
      });

    // 预计算左右子节点的 AABB
    AABB leftBox, rightBox;
    std::vector<AABB> rightBoxes(r - l + 1);
    for (int i = r; i >= l; i--) {
      rightBox.Expand(shapes[orderedIdxBuffer[i]]->getAABB());
      rightBoxes[i - l] = rightBox;
    }

    // 遍历所有可能的分割位置
    for (int i = l; i < r; i++) {
      leftBox.Expand(shapes[orderedIdxBuffer[i]]->getAABB());

      // 左右子节点的表面积
      float leftArea = surfaceArea(leftBox);
      float rightArea = surfaceArea(rightBoxes[i - l + 1]);

      // 左右子节点的几何体数量
      int leftCount = i - l + 1;
      int rightCount = r - i;

      // 计算 SAH 成本
      float cost = 1.0f + (leftArea / parentArea) * leftCount +
                   (rightArea / parentArea) * rightCount;

      // 更新最优分割
      if (cost < bestCost) {
        bestAxis = axis;
        bestSplit = i;
        bestCost = cost;
      }
    }
  }

  // 如果未找到有效分割，创建叶节点
  if (bestAxis == -1) {
    auto node = std::make_unique<BVHNode>();
    node->boundingBox = aabb;
    node->lIdx = l;
    node->rIdx = r;
    node->lNode = nullptr;
    node->rNode = nullptr;
    return node;
  }

  // 按最佳轴对几何体排序
  std::sort(
    orderedIdxBuffer.begin() + l, orderedIdxBuffer.begin() + r + 1,
    [&](int a, int b) {
      return shapes[a]->getAABB().Center()[bestAxis] <
              shapes[b]->getAABB().Center()[bestAxis];
    });

  auto node = std::make_unique<BVHNode>();
  node->boundingBox = aabb;
  AABB lBox, rBox;
  // 递归构建左右子节点
  for (int i = l; i <= bestSplit; i++) {
    lBox.Expand(shapes[orderedIdxBuffer[i]]->getAABB());
  }
  for (int i = bestSplit + 1; i <= r; i++) {
    rBox.Expand(shapes[orderedIdxBuffer[i]]->getAABB());
  }
  node->lNode = recursiveBuild(lBox, l, bestSplit);
  node->rNode = recursiveBuild(rBox, bestSplit + 1, r);

  return node;
}

void BVH::build() {
  AABB sceneBox;
  for (const auto & shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();
    sceneBox.Expand(shape->getAABB());
  }
  //* todo 完成BVH构建
  //* 构建八叉树
  for (int i = 0; i < shapes.size(); i++) {
    orderedIdxBuffer.push_back(i);
  }
  boundingBox = sceneBox;
  root = recursiveBuild(sceneBox, 0, shapes.size() - 1);
}

bool BVH::recursiveRayIntersect(const std::unique_ptr<BVH::BVHNode> &node, Ray &ray,
  int *geomID, int *primID, float *u,
  float *v) const {
  if(node == nullptr)
    return false;

  if(!node->boundingBox.RayIntersect(ray, nullptr, nullptr))
    return false;

  if(node->lNode == nullptr && node->rNode == nullptr) {
    bool intersect = false;
    for(int i = node->lIdx; i <= node->rIdx; i++)
      if(shapes[orderedIdxBuffer[i]]->rayIntersectShape(ray, primID, u, v)) {
        intersect = true;
        *geomID = orderedIdxBuffer[i];
      }
    return intersect;
  }

  bool intersect = false;
  intersect |= recursiveRayIntersect(node->lNode, ray, geomID, primID, u, v);
  intersect |= recursiveRayIntersect(node->rNode, ray, geomID, primID, u, v);
  return intersect;
}

bool BVH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const {
  //* todo 完成BVH求交
  
  return recursiveRayIntersect(root, ray, geomID, primID, u, v);
}


