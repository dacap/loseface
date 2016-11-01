// Copyright (C) 2008-2010 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

// Based on code of Sebastián Long and Omar Müller

#ifndef LOSEFACE_IMAGE_FACELOC_H
#define LOSEFACE_IMAGE_FACELOC_H

#include <climits>
#include "imageproc.h"

namespace faceloc {

using namespace cimg_library;

// Define this to fix the errors that come from the original code
#define FIXED_IMPL 1

//////////////////////////////////////////////////////////////////////

/// Indicates information of position and translation of an
/// ellipse-template useful to check if it's the best ellipse that
/// fit a face region.
///
struct TranslationTemplate
{
  // translation
  int x, y;			// center of the template
  int x_step;			// step to move the center
  int y_step;
  // radius
  int w, h;			// radius of the template
  int w_step;			// step to move the radius
  int h_step;
  // rotation
  double degree;		// rotation of the template
  double degree_step;
  double degree_threshold;

  template<class T>
  double calculate_percentage(const CImg<T>& img, T label)
  {
    CImg<T> ellipse_mask;
    imageproc::create_ellipse_mask(ellipse_mask,
				   img.width, img.height,
				   this->x, this->y,
				   this->w, this->h, this->degree,
				   T(0), T(255));
    return imageproc::calculate_mask_percentage(img, ellipse_mask, label, T(0));
  }

};

//////////////////////////////////////////////////////////////////////

template<class T>
void locate_face_center(const CImg<T>& img,
			unsigned regions,
			T first_label,
			const TranslationTemplate& model_tt,
			TranslationTemplate& best_tt,
			double min_percentage,
			double max_percentage)
{
  double best_percetange = 0.0;
  best_tt = model_tt;

#ifndef FIXED_IMPL
  double old_degree_step = model_tt.degree_step;
#endif

  // for each labeled skin-region
  for (unsigned i=0; i<regions; ++i) {
    TranslationTemplate tt = model_tt;

#ifndef FIXED_IMPL
    tt.degree_step = old_degree_step;
#endif

    // we get the center of the region (this could be the center of a face)
    imageproc::calculate_region_center(img, first_label, tt.x, tt.y);

    // we calculate how many pixels of the skin-region (first_label)
    // are inside the template (tt)
    double percentage = tt.calculate_percentage(img, first_label);
    if (percentage > min_percentage) {
      // if we get the minimum of pixels inside the mask, then we can
      // improve the template (translating and rotating it to fit more
      // accuracy the face region)
      refine_translation_template(img, percentage, first_label, tt);
#ifndef FIXED_IMPL
      old_degree_step = tt.degree_step;
#endif
    }

    // did we get an improvement?
    if ((percentage > best_percetange) &&
	(percentage <= max_percentage)) {
      best_percetange = percentage;
      best_tt = tt;
    }

    ++first_label;
  }
}

//////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
void label_skin_regions(const CImg<T1>& src,
			CImg<T2>& dst,
			unsigned& regions,
			T1 threshold,
			T2 white,
			T2 first_label)
{
  imageproc::make_binary<T1, T2>(src, dst, threshold, T1(0), white);
  imageproc::labeling_image<T2>(dst, regions, first_label, white,
				imageproc::stack_floodfill8<T2>);
}

//////////////////////////////////////////////////////////////////////

/// Main routine to locate the face region in a photography.
///
template<class T>
void get_face(const CImg<T>& photo, CImg<T>& face,
	      int face_width,
	      int face_height)
{
  // first apply gamma correction
  double gamma = 1.2;
  double offset = 0.3;
  CImg<T> gamma_corrected_photo;
  imageproc::gamma_correction(photo, gamma_corrected_photo, gamma, offset);

  // then detect the skin from the photo
  CImg<T> skin_only;
  detect_skin_in_rgb_model(gamma_corrected_photo, skin_only);

  // labeling of regions with skin
  unsigned regions = 0;
  unsigned first_label(1);
  CImg<unsigned> labeled_skin_regions;
  label_skin_regions<T, unsigned>(skin_only, labeled_skin_regions, regions,
				  T(0), UINT_MAX, first_label);

  // locate an ellipsed-template in the photo where the face is
  TranslationTemplate tt, best_tt;
  tt.x = 0;
  tt.y = 0;
  tt.x_step = 2;
  tt.y_step = 2;
  tt.w = 27;
  tt.h = 35;
  tt.w_step = 2;
  tt.h_step = 3;
  tt.degree = 0.0;
  tt.degree_step = 1.0;
  tt.degree_threshold = 0.1;
  locate_face_center<unsigned>(labeled_skin_regions,
			       regions, first_label,
			       tt, best_tt, 20.0, 100.0);

  // cut the face from the photo
  int face_x, face_y;
#ifdef FIXED_IMPL
  crop_face_using_template(gamma_corrected_photo, face,
			   best_tt, 110, 140, face_x, face_y);
#else
  CImg<T> rotated_photo;
  crop_face_using_template(gamma_corrected_photo,
			   rotated_photo, face,
			   best_tt, 110, 140, face_x, face_y);
#endif

  // detect the position of both eyes
  imageproc::Point eye1, eye2;
  double pmax = detect_eyes(face, eye1, eye2);

  // segment possible eye regions
  double threshold = 0.5;
#ifdef FIXED_IMPL
  face = gamma_corrected_photo;
#else
  face = rotated_photo;
#endif
  crop_face_using_eyes(face, eye1, eye2,
		       (pmax > threshold), // rotate it?
		       face_x, face_y,
		       face_width, face_height);

  // rgb_to_gray(face, face);
}

//////////////////////////////////////////////////////////////////////

template<class T>
void detect_skin_in_rgb_model(const CImg<T>& src,
			      CImg<T>& dst,
			      T min_r = 80,
			      T min_g = 30,
			      T min_b = 15,
			      T rg_gap = 10)
{
  T r, g, b; // , max_channel, min_channel;

  dst = src;

  cimg_forXY(src,x,y) {
    r = src(x,y,0,0);
    g = src(x,y,0,1);
    b = src(x,y,0,2);

    // max_channel = std::max(std::max(r,g), std::max(g,b));
    // min_channel = std::min(std::min(r,g), std::min(g,b));

    if (!((r > min_r) &&
	  (g > min_g) &&
	  (b > min_b) &&
	  (std::abs(r - g) > rg_gap) &&
	  (r > g) &&
	  (r > b))) {
      dst(x,y,0,0) = 0;
      dst(x,y,0,1) = 0;
      dst(x,y,0,2) = 0;
    }
  }
}

//////////////////////////////////////////////////////////////////////

template<class T>
void refine_translation_template(const CImg<T>& img,
				 double& percentage,
				 T label,
				 TranslationTemplate& tt)
{
  // translation (refine center)
  refine_translation_template_center(img, percentage, label, tt, -tt.x_step, 0);
  refine_translation_template_center(img, percentage, label, tt, 0, -tt.y_step);
  refine_translation_template_center(img, percentage, label, tt, +tt.x_step, 0);
  refine_translation_template_center(img, percentage, label, tt, 0, +tt.y_step);

#ifdef FIXED_IMPL
  // do nothing
#else
  // scale (refine radius)
  {
    TranslationTemplate tt2 = tt;
    refine_translation_template_radius(img, percentage, label, tt2); // tt2 was ignored
  }
  // rotation (refine degree)
  refine_translation_template_degree(img, percentage, label, tt);
#endif
}

template<class T>
void refine_translation_template_center(const CImg<T>& img,
					double& percentage,
					T label,
					TranslationTemplate& tt,
					int delta_x, int delta_y)
{
  // is the template's center (tt.x, tt.y) outside the image's region?
  if ((tt.x < tt.w) || (tt.x >= img.width - tt.w) ||
      (tt.y < tt.h) || (tt.y >= img.height - tt.h))
    return;

  // copy the template and translate it with the specified delta
  TranslationTemplate refined_tt(tt);
  refined_tt.x += delta_x;
  refined_tt.y += delta_y;

  // we calculate the new percentage of skin-pixels inside the refined template
  double refined_percentage = refined_tt.calculate_percentage(img, label);
  if (refined_percentage > percentage) {
    percentage = refined_percentage;
    tt.x = refined_tt.x;
    tt.y = refined_tt.y;

    // try to refine more in the four directions
    refine_translation_template_center(img, percentage, label, tt, -tt.x_step, 0);
    refine_translation_template_center(img, percentage, label, tt, 0, -tt.y_step);
    refine_translation_template_center(img, percentage, label, tt, +tt.x_step, 0);
    refine_translation_template_center(img, percentage, label, tt, 0, +tt.y_step);
  }
}

template<class T>
void refine_translation_template_radius(const CImg<T>& img,
					double& percentage,
					T label,
					TranslationTemplate& tt)
{
  // copy the template
  TranslationTemplate refined_tt(tt);

  while (true) {
    // we amplify the template
    refined_tt.w += refined_tt.w_step;
    refined_tt.h += refined_tt.h_step;

    // we calculate the new percentage of skin-pixels inside the refined template
    double refined_percentage = refined_tt.calculate_percentage(img, label);
    if (refined_percentage > percentage) {
      percentage = refined_percentage;
      tt.w = refined_tt.w;
      tt.h = refined_tt.h;
    }
    else
      break;
  }
}

template<class T>
void refine_translation_template_degree(const CImg<T>& img,
					double& percentage,
					T label,
					TranslationTemplate& tt)
{
  TranslationTemplate refined_tt(tt);

  refined_tt.degree = tt.degree - tt.degree_step;
  double left_percentage = refined_tt.calculate_percentage(img, label);

  refined_tt.degree = tt.degree + tt.degree_step;
  double right_percentage = refined_tt.calculate_percentage(img, label);

  // std::cout << "left_percentage = " << left_percentage << "\n";
  // std::cout << "right_percentage = " << right_percentage << "\n";

  int dir = 0;
  if ((left_percentage > percentage+tt.degree_threshold) &&
      (left_percentage > right_percentage)) {
    dir = -1;
  }
  else if ((right_percentage > percentage+tt.degree_threshold) &&
	   (right_percentage > left_percentage)) {
    dir = 1;
  }
  else
    return;

  refined_tt.degree = tt.degree;
  while (true) {
#ifdef FIXED_IMPL
    refined_tt.degree += dir * refined_tt.degree_step;
    refined_tt.degree_step /= 2.0;
#else
    refined_tt.degree += dir * tt.degree_step;
    tt.degree_step /= 2.0;
#endif

    // we calculate the new percentage of skin-pixels inside the refined template
    double refined_percentage = refined_tt.calculate_percentage(img, label);
    if (refined_percentage > percentage) {
      percentage = refined_percentage;
      tt.degree = refined_tt.degree;
    }
    else
      break;
  }
}

//////////////////////////////////////////////////////////////////////

template<class T>
void crop_face_using_template(const CImg<T>& photo,
#ifndef FIXED_IMPL
			      CImg<T>& rotated_photo,
#endif
			      CImg<T>& face,
			      const TranslationTemplate& tt,
			      int width, int height,
			      int& x0, int& y0)
{
  int x1, y1;

  x0 = tt.x - (width/2);
  if (x0 < 0)
    x0 = 0;

  x1 = x0 + width;
  if (x1 > photo.width) {
    x1 = photo.width;
    x0 = x1 - width;
  }

  y0 = tt.y - (height/2);
  if (y0 < 0)
    y0 = 0;

  y1 = y0 + height;
  if (y1 > photo.height) {
    y1 = photo.height;
    y0 = y1 - height;
  }

#ifdef FIXED_IMPL
  face = photo.get_crop(x0, y0, x1-1, y1-1);
#else
  rotated_photo = photo;
  rotated_photo.rotate((float)-tt.degree, (float)tt.x, (float)tt.y);
  face = rotated_photo.get_crop(x0, y0, x1-1, y1-1);
#endif
}

//////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
void segment_eye_regions(const CImg<T1>& src, CImg<T2>& dst,
			 T2 white)
{
  CImg<double> Y = (CImg<double>)src.get_RGBtoYCbCr().get_channel(0);
  CImg<double> YD = Y.get_dilate(1);
  CImg<double> YE = Y.get_erode(1);

  CImg<double> map = (1.0 - YD.div(YE + 1.0));
  CImg<double> stats = map.get_stats();

  map.threshold(stats[2] + 0.8*(std::pow(stats[3], 0.5)));
  map.erode(1);
  map.dilate(1);

  dst = (CImg<T2>)map;
  dst *= UINT_MAX;
}

/// Determines which of all centers are eyes.
///
/// @param img
///   Must be a black-image with a pixel in each region's center
///   labeled with its corresponding color.
/// @param centers
///   A list of points that represent the center of each region.
/// @param first_label
///   Color for the first label (regions must be
///   labeled in sequence).
///
template<class T>
double locate_best_eyes(const CImg<T> img,
			const std::vector<imageproc::Point>& centers,
			T first_label,
			imageproc::Point& eye1,
			imageproc::Point& eye2)
{
  CImg<T> mask = img;
  const T color[3] = {1,1,1};

  double pmax = 0.0;

  // for each region's center
  for (std::vector<imageproc::Point>::const_iterator
	 it=centers.begin(); it!=centers.end(); ++it) {
    const imageproc::Point& center(*it);

    // check if the eye (center) is in a reasonable area of the image:
    //
    // x=0 ---> 7 <------- EYE AREA --------> 0.65*width        width
    //   |XXXXXX|                            |XXXXXXXXXXXXXXXXXXXX|
    //
    if (center.x > 7 && center.x <= (0.65*mask.width)) {
      // we draw a white rectangle where the right-eye should be
      mask.fill(0);
      mask.draw_rectangle(center.x + 25, center.y - 10,
			  center.x + 65, center.y + 10, color);

      // multiply with the "img" to filter-out all pixels outside the rectangle
      mask.mul(img);

      double p = 0;
      cimg_forXY(mask,x,y) {
	// there is a pixel of another region (no first_label) here?
        if (mask(x,y) != 0 && mask(x,y) != first_label) {
	  int other_region = mask(x,y)-1;
	  const imageproc::Point& center2(centers[other_region]);

          double dx = std::abs(center.x - center2.x);
          double dy = std::abs(center.y - center2.y);
          double row = std::abs(center.y);
          double pdx = 0.35*(1.0-(std::abs(35.0-dx)/35.0));
          double pdy = 0.2*(1.0-(dy/8.0));
          double prow = 0.35*(1.0-(std::abs((double)45.0-row)/45.0));
          double pcol = 0.1*(1.0-(std::abs((double)(mask.width/2) - (center.x+center2.x/2)))/mask.width);
          double p = pdx+pdy+prow+pcol;

          if (p > pmax) {
            pmax = p;
	    eye1 = center;
	    eye2 = center2;
          }
        }
      }
    }

    ++first_label;
  }

  return pmax;
}

template<class T>
double detect_eyes(const CImg<T>& img,
		   imageproc::Point& eye1,
		   imageproc::Point& eye2)
{
  // segment possible eye regions
  CImg<unsigned> eyes_regions;
  segment_eye_regions(img, eyes_regions, UINT_MAX);

  // labeling each region
  unsigned regions = 0;
  unsigned first_label(1);
  imageproc::labeling_image<unsigned>(eyes_regions,
				      regions, first_label, UINT_MAX,
				      imageproc::stack_floodfill8<unsigned>);

  // clean eye regions that are too small or big to be an eye
  imageproc::discard_regions<unsigned>(eyes_regions, regions,
				       5, 500, first_label, T(0));

  // binary the image
  eyes_regions.threshold(1);
  eyes_regions *= UINT_MAX;

  // relabeling
  imageproc::labeling_image<unsigned>(eyes_regions,
				      regions, first_label, UINT_MAX,
				      imageproc::stack_floodfill8<unsigned>);

  // calculate where is each region's center
  std::vector<imageproc::Point> centers;
  imageproc::calculate_regions_center(eyes_regions, regions, first_label, centers);
  imageproc::draw_points<unsigned>(eyes_regions, centers, 0, first_label);

  // locate the region where the eyes are in
  return locate_best_eyes<unsigned>(eyes_regions, centers, first_label, eye1, eye2);
}

template<class T>
void crop_face_using_eyes(CImg<T>& img,
			  const imageproc::Point& eye1,
			  const imageproc::Point& eye2,
			  bool rotate,
			  int face_x, int face_y,
			  int face_w, int face_h)
{
  if (rotate) {
#ifdef FIXED_IMPL
    int dx = eye2.x - eye1.x;
    int dy = eye2.y - eye1.y;
    double dist = std::sqrt((double)dx*dx + (double)dy*dy);
    double angle = 180.0 * std::atan2((double)-dy, (double)dx) / M_PI;
#else
    int dx = eye2.x - eye1.x;
    int dy = eye2.y - eye1.y;
    double dist = std::abs(dx);
    double angle = 180.0 * std::atan2((double)-dy, (double)dx) / M_PI;
#endif
    int x = face_x + eye1.x;
    int y = face_y + eye1.y;

    img.rotate(angle, x, y);
    img.crop(x - 0.75*dist, y - dist,
	     x + 1.75*dist, y + 1.95*dist);
  }
  else {
    img.crop(face_x, face_y,
	     face_x + 110 - 1,
	     face_y + 140 - 1);
  }
  img = img.resize(face_w, face_h);
}

} // namespace faceloc

#endif // LOSEFACE_IMAGE_FACELOC_H
