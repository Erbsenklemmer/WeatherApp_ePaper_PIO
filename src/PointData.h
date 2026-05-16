#pragma once

class PointData
{
public:
  PointData(int16_t px, int16_t py) : x(px), y(py) {}

  int16_t x;
  int16_t y;
};