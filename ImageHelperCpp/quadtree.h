#pragma once

#include <vector>
#include <iostream>

struct Point
{
  float x, y;
  Point(float x = 0, float y = 0) :x(x), y(y){};
};

struct AABB
{
  Point centre;
  Point halfSize;

  AABB(Point centre = Point(), Point halfSize = Point()) : centre(centre), halfSize(halfSize){};

  bool contains(const Point& a) const
  {
    if (a.x < centre.x + halfSize.x && a.x > centre.x - halfSize.x)
    {
      if (a.y < centre.y + halfSize.y && a.y > centre.y - halfSize.y)
      {
        return true;
      }
    }
    return false;
  }

  bool intersects(const AABB& other) const
  {
    //this right > that left                                          this left <s that right
    if (centre.x + halfSize.x > other.centre.x - other.halfSize.x || centre.x - halfSize.x < other.centre.x + other.halfSize.x)
    {
      // This bottom > that top
      if (centre.y + halfSize.y > other.centre.y - other.halfSize.y || centre.y - halfSize.y < other.centre.y + other.halfSize.y)
      {
        return true;
      }
    }
    return false;
  }
};

template <typename T>
struct Data
{
  Point pos;
  T* load;

  Data(Point pos = Point(), T* data = nullptr) : pos(pos), load(data){};
};


template <class T>
class Quadtree
{
private:
  //4 children
  Quadtree* nw;
  Quadtree* ne;
  Quadtree* sw;
  Quadtree* se;

  AABB boundary;

  std::vector< Data<T> > objects;

  int CAPACITY;
public:
  Quadtree<T>();
  Quadtree<T>(const AABB& boundary);

  ~Quadtree();

  bool insert(const Data<T>& d);
  void subdivide();
  std::vector< Data<T> > queryRange(const AABB& range);
};

template <class T>
Quadtree<T>::Quadtree()
{
  CAPACITY = 4;
  nw = NULL;
  ne = NULL;
  sw = NULL;
  se = NULL;
  boundary = AABB();
  objects = std::vector< Data<T> >();
}

template <class T>
Quadtree<T>::Quadtree(const AABB& boundary)
  : nw(nullptr)
  , ne(nullptr)
  , sw(nullptr)
  , se(nullptr)
{
  objects = std::vector< Data<T> >();
  CAPACITY = 4;
  this->boundary = boundary;
}

template <class T>
Quadtree<T>::~Quadtree()
{
  delete nw;
  delete sw;
  delete ne;
  delete se;
}

template <class T>
void Quadtree<T>::subdivide()
{
  Point qSize = Point(boundary.halfSize.x, boundary.halfSize.y);
  Point qCentre = Point(boundary.centre.x - qSize.x, boundary.centre.y - qSize.y);
  nw = new Quadtree(AABB(qCentre, qSize));

  qCentre = Point(boundary.centre.x + qSize.x, boundary.centre.y - qSize.y);
  ne = new Quadtree(AABB(qCentre, qSize));

  qCentre = Point(boundary.centre.x - qSize.x, boundary.centre.y + qSize.y);
  sw = new Quadtree(AABB(qCentre, qSize));

  qCentre = Point(boundary.centre.x + qSize.x, boundary.centre.y + qSize.y);
  se = new Quadtree(AABB(qCentre, qSize));
}

template <class T>
bool Quadtree<T>::insert(const Data<T>& d)
{
  if (!boundary.contains(d.pos))
  {
    return false;
  }

  if (int(objects.size()) < CAPACITY)
  {
    objects.push_back(d);
    return true;
  }

  if (nw == nullptr)
  {
    subdivide();
  }

  if (nw->insert(d))
  {
    return true;
  }
  if (ne->insert(d))
  {
    return true;
  }
  if (sw->insert(d))
  {
    return true;
  }
  if (se->insert(d))
  {
    return true;
  }

  return false;
}

template <class T>
std::vector< Data<T> > Quadtree<T>::queryRange(const AABB& range)
{
  std::vector< Data<T> > pInRange = std::vector< Data<T> >();

  if (!boundary.intersects(range))
  {
    return pInRange;
  }

  for (int i = 0; i < int(objects.size()); i++)
  {
    if (range.contains(objects.at(i).pos))
    {
      pInRange.push_back(objects.at(i));
    }
  }

  if (nw == nullptr)
  {
    return pInRange;
  }

  std::vector< Data<T> > temp = nw->queryRange(range);
  pInRange.insert(pInRange.end(), temp.begin(), temp.end());

  temp = ne->queryRange(range);
  pInRange.insert(pInRange.end(), temp.begin(), temp.end());

  temp = sw->queryRange(range);
  pInRange.insert(pInRange.end(), temp.begin(), temp.end());

  temp = se->queryRange(range);
  pInRange.insert(pInRange.end(), temp.begin(), temp.end());

  return pInRange;
}
