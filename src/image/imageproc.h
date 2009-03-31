// Lose Face - An open source face recognition project
// Copyright (C) 2008-2009 David Capello
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the authors nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

// Based on code of Sebastián Long and Omar Müller

#ifndef LOSEFACE_IMAGE_IMAGEPROC_H
#define LOSEFACE_IMAGE_IMAGEPROC_H

#include "CImg.h"
#include <cmath>
#include <vector>
#include <stack>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

namespace imageproc {

struct Point
{
  int x, y;
  Point() { x=y=0; }
  Point(int x, int y) : x(x), y(y) { }
  Point(const Point& pt) { x=pt.x; y=pt.y; }
};

using namespace cimg_library;

//////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
void rgb_to_gray(const CImg<T1>& src, CImg<T2>& dst)
{
  dst.assign(src.width,src.height,1,1,0);
  cimg_forXY(dst,x,y) {
    dst(x,y) = ((src(x,y,0,0) +
		 src(x,y,0,1) +
		 src(x,y,0,2)) / T2(3));
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
void make_binary(const CImg<T1>& src, CImg<T2>& dst,
		 T1 threshold = T1(0),
		 T2 black = T2(0),
		 T2 white = T2(255))
{
  dst.assign(src.width,src.height,1,1,black);
  cimg_forXY(dst,x,y) {
    if ((src(x,y,0,0) > threshold) &&
	(src(x,y,0,1) > threshold) &&
	(src(x,y,0,2) > threshold))
      dst(x,y) = white;
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void recursive_floodfill8(CImg<T>& img,
			  int x, int y,
			  T replace_this,
			  T with_this)
{
  if ((replace_this == with_this) ||
      (x < 0) || (x >= img.width) ||
      (y < 0) || (y >= img.height))
    return;

  if (img(x,y) == replace_this) {
    img(x,y) = with_this;

    recursive_floodfill8(img, x + 1, y,     replace_this, with_this);
    recursive_floodfill8(img, x + 1, y + 1, replace_this, with_this);
    recursive_floodfill8(img, x,     y + 1, replace_this, with_this);
    recursive_floodfill8(img, x - 1, y + 1, replace_this, with_this);
    recursive_floodfill8(img, x - 1, y,     replace_this, with_this);
    recursive_floodfill8(img, x - 1, y - 1, replace_this, with_this);
    recursive_floodfill8(img, x,     y - 1, replace_this, with_this);
    recursive_floodfill8(img, x + 1, y - 1, replace_this, with_this);
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void stack_floodfill8(CImg<T>& img,
		      int x, int y,
		      T replace_this,
		      T with_this)
{
  if ((replace_this == with_this) ||
      (x < 0) || (x >= img.width) ||
      (y < 0) || (y >= img.height))
    return;

  std::stack<Point> points;
  points.push(Point(x, y));

  while (!points.empty()) {
    Point pt = points.top();
    points.pop();

    if ((pt.x >= 0) && (pt.x < img.width) &&
	(pt.y >= 0) && (pt.y < img.height) &&
	(img(pt.x,pt.y) == replace_this)) {
      img(pt.x,pt.y) = with_this;

      points.push(Point(pt.x + 1, pt.y));
      points.push(Point(pt.x + 1, pt.y + 1));
      points.push(Point(pt.x,     pt.y + 1));
      points.push(Point(pt.x - 1, pt.y + 1));
      points.push(Point(pt.x - 1, pt.y));
      points.push(Point(pt.x - 1, pt.y - 1));
      points.push(Point(pt.x,     pt.y - 1));
      points.push(Point(pt.x + 1, pt.y - 1));
    }
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T, typename Floodfill>
void labeling_image(CImg<T>& img,
		    unsigned& regions,  // number of regions found
		    T first_label,      // value for the first label to use
		    T unlabeled,	// value for unlabeled pixels
		    Floodfill floodfill)
{
  regions = 0;			// number of resulting regions

  cimg_forXY(img,x,y) {
    // is this pixel unlabeled yet?
    if (img(x,y) == unlabeled) {
      // fill this region from (x,y) pixel
      floodfill(img, x, y, unlabeled, first_label);
      ++first_label;
      ++regions;
    }
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void calculate_region_center(const CImg<T>& img,
			     T label,
			     int& center_x,
			     int& center_y)
{
  int total_x = 0;
  int total_y = 0;
  int area = 0;

  cimg_forXY(img,x,y) {
    if (img(x,y) == label) {
      total_x += x;
      total_y += y;
      ++area;
    }
  }

  if (((double)(total_x%area) / (double)area) >= 0.5)
    center_x = (int)(total_x / area) + 1;
  else
    center_x = (int)(total_x / area);

  if (((double)(total_y%area) / (double)area) >= 0.5)
    center_y = (int)(total_y / area) + 1;
  else
    center_y = (int)(total_y / area);
}

//////////////////////////////////////////////////////////////////////

/// @return Percentage of masked pixels (in @a mask image) touching
///         labeled pixels (in @a img) with the specified @a label.
/// 
template<typename T1, typename T2>
double calculate_mask_percentage(const CImg<T1>& img,
				 const CImg<T2>& mask,
				 T1 label,
				 T2 black = T2(0))
{
  int mask_area = 0;
  int mask_labeled = 0;

  cimg_forXY(mask,x,y) {
    if (mask(x,y) > black) {
      ++mask_area;
      if (img(x,y) == label)
        ++mask_labeled;
    }
  }

  return 100.0 * (double)mask_labeled / (double)mask_area;
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void create_ellipse_mask(CImg<T>& img,
			 int w, int h,
			 int center_x, int center_y,
			 double radius_x, double radius_y,
			 double degree, T black, T white)
{
  double rv = std::sin(M_PI * degree / 180.0);
  double ru = std::sqrt(1.0 - std::pow(rv, 2.0));

  img.assign(w,h,1,1,black);
  img.draw_ellipse(center_x, center_y,
		   (float)radius_x, (float)radius_y,
		   (float)ru, (float)rv, &white);
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void discard_regions(CImg<T>& img,
		     unsigned regions,
		     unsigned min_threshold, // minimum number of pixels accepted in a region
		     unsigned max_threshold, // maximum number of pixels accepted in a region
		     T first_label,
		     T black = T(0))
{
  for (unsigned i=0; i<regions; ++i) {
    unsigned labeled_pixels = 0;

    // count the number of labeled pixels
    cimg_forXY(img,x,y) {
      if (img(x,y) == first_label)
        ++labeled_pixels;
    }

    // are labeled pixels out of range?
    if (labeled_pixels > max_threshold ||
	labeled_pixels <= min_threshold) {
      // clear the region
      cimg_forXY(img,x,y) {
        if (img(x,y) == first_label)
          img(x,y) = black;
      }
    }

    // go to the next region
    ++first_label;
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T>
void calculate_regions_center(const CImg<T>& img,
			      unsigned regions,
			      T first_label,
			      std::vector<Point>& centers)
{
  centers.resize(regions);
  for (unsigned k=0; k<regions; ++k) {
    calculate_region_center(img, first_label,
			    centers[k].x,
			    centers[k].y);
    ++first_label;
  }
}

template<typename T>
void draw_points(CImg<T>& img,
		 const std::vector<Point>& centers,
		 T black,
		 T first_label)
{
  // clear all
  img.assign(img.width,img.height,1,1,black);

  for (std::vector<Point>::const_iterator
	 it=centers.begin(); it!=centers.end(); ++it) {
    const Point& pt(*it);
    img(pt.x, pt.y) = first_label;
    ++first_label;
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T1, typename T2, typename T>
void gamma_correction(const CImg<T1>& src,
		      CImg<T2>& dst,
		      T gamma,
		      T offset)
{
  CImg<T> normalized = src;
  normalized.normalize(T(0), T(1));

  {
    CImg<T> gray;
    rgb_to_gray(normalized, gray);

    CImg<T> stats = gray.get_stats();
    T mean = stats[2];

    gamma = ((gamma * mean) + offset);
  }

  dst.assign(src.dimx(),src.dimy(),src.dimz(),src.dimv());
  cimg_forXYZV(dst,x,y,z,v) {
    dst(x,y,z,v) = T2(255) * std::pow(normalized(x,y,z,v), gamma);
  }
}

//////////////////////////////////////////////////////////////////////

} // namespace imageproc

#endif // LOSEFACE_IMAGE_IMAGEPROC_H
