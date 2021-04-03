//
// Created by DefTruth on 2021/3/28.
//

#ifndef LITEHUB_ORT_CORE_ORT_TYPES_H
#define LITEHUB_ORT_CORE_ORT_TYPES_H

#include <limits>
#include <cmath>
#include <vector>
#include <type_traits>
#include "opencv2/opencv.hpp"

// global namespace
namespace {
  template<typename _T1 = float, typename _T2 = float>
  static inline void __assert_type() {
    static_assert(std::is_pod<_T1>::value && std::is_pod<_T2>::value
                  && std::is_floating_point<_T2>::value
                  && (std::is_integral<_T1>::value || std::is_floating_point<_T1>::value),
                  "not support type.");
  } // only support for some specific types. check at complie-time.
}

namespace ortcv {
  namespace types {
    // bounding box.
    template<typename T1 = float, typename T2 = float>
    struct BoundingBoxType {
      typedef T1 value_type;
      typedef T2 score_type;
      value_type x1;
      value_type y1;
      value_type x2;
      value_type y2;
      score_type score;
      char *label_text;
      unsigned int label; // for general object detection.
      bool flag; // future use.
      // convert type.
      template<typename O1, typename O2 = score_type>
      BoundingBoxType<O1, O2> convert_type() const;
      template<typename O1, typename O2 = score_type>
      value_type iou_of(const BoundingBoxType<O1, O2> &other) const;
      value_type width() const;
      value_type height() const;
      value_type area() const;
      cv::Rect rect() const;
      cv::Point2i tl() const;
      cv::Point2i rb() const;
      BoundingBoxType():
      x1(static_cast<value_type>(0)), y1(static_cast<value_type>(0)),
      x2(static_cast<value_type>(0)), y2(static_cast<value_type>(0)),
      score(static_cast<score_type>(0)), label_text(nullptr),label(0),
      flag(false) {::__assert_type<value_type, score_type>();}
    }; // End BoundingBox.
    // specific alias.
    typedef BoundingBoxType<int, float> Boxi;
    typedef BoundingBoxType<float, float> Boxf;
    typedef BoundingBoxType<double, double> Boxd;
    typedef struct LandmarksType {std::vector<cv::Point2f> points; bool flag; LandmarksType():flag(false) {};} Landmarks;
    typedef struct EulerAnglesType { float yaw; float pitch; float roll; bool flag; EulerAnglesType():flag(false){};} EulerAngles;
    typedef struct EmotionsType{ float score; unsigned int label; const char *text; bool flag; EmotionsType():flag(false){};} Emotions;
    typedef struct AgeType { float age; unsigned int age_interval[2]; float interval_prob; bool flag; AgeType():flag(false){};} Age;
    typedef struct GenderType { float score; unsigned int label; const char *text; bool flag; GenderType():flag(false){};} Gender;
    typedef struct OCRContentType { const char *text; unsigned int *codecs; bool flag; OCRContentType():flag(false){};} OCRContent;
  } // NAMESPACE TYPES
} // NAMESPACE ORTCV

namespace ortnlp {
  namespace types {} // NAMESPACE TYPES
} // NAMESPACE ORTNLP

namespace ortasr {
  namespace types {} // NAMESPACE TYPES
} // NAMESPACE ORTASR

/**
 * Implemeantation details for all custom types using on liteort.
 */

/* implementation for 'BoundingBox'. */
template<typename T1, typename T2>
template<typename O1, typename O2>
inline ortcv::types::BoundingBoxType<O1, O2>
ortcv::types::BoundingBoxType<T1, T2>::convert_type() const {
  typedef O1 other_value_type; typedef O2 other_score_type;
  ::__assert_type<other_value_type, other_score_type>();
  ::__assert_type<value_type, score_type>();
  BoundingBoxType<other_value_type, other_score_type> other;
  other.x1 = static_cast<other_value_type>(x1);
  other.y1 = static_cast<other_value_type>(y1);
  other.x2 = static_cast<other_value_type>(x2);
  other.y2 = static_cast<other_value_type>(y2);
  other.score = static_cast<other_score_type>(score);
  return other;
}

template<typename T1, typename T2>
template<typename O1, typename O2>
inline typename ortcv::types::BoundingBoxType<T1, T2>::value_type
ortcv::types::BoundingBoxType<T1, T2>::iou_of(const BoundingBoxType<O1, O2> &other) const {
  BoundingBoxType<value_type, score_type> tbox = \
    other.template convert_type<value_type, score_type>();
  value_type inner_x1 = x1 > tbox.x1 ? x1: tbox.x1;
  value_type inner_y1 = y1 > tbox.y1 ? y1: tbox.y1;
  value_type inner_x2 = x2 < tbox.x2 ? x2: tbox.x2;
  value_type inner_y2 = y2 < tbox.y2 ? y2: tbox.y2;
  value_type inner_h = inner_y2 - inner_y1 + static_cast<value_type>(1.0f);
  value_type inner_w = inner_x2 - inner_x1 + static_cast<value_type>(1.0f);
  if (inner_h <= static_cast<value_type>(0.f) || inner_w <= static_cast<value_type>(0.f))
    return std::numeric_limits<value_type>::min();
  value_type inner_area = inner_h * inner_w;
  value_type w1 = tbox.x2 - x1 + static_cast<value_type>(1.0f);
  value_type h1 = tbox.y2 - y1 + static_cast<value_type>(1.0f);
  value_type area1 = h1 * w1;
  return static_cast<value_type>(inner_area / (area() + area1 - inner_area));
}

template<typename T1, typename T2>
inline cv::Rect ortcv::types::BoundingBoxType<T1, T2>::rect() const {
  ::__assert_type<value_type, score_type>();
  BoundingBoxType<int> boxi = this->template convert_type<int>();
  return cv::Rect(boxi.x1, boxi.y1, boxi.width(), boxi.height());
}

template<typename T1, typename T2>
inline cv::Point2i ortcv::types::BoundingBoxType<T1, T2>::tl() const {
  ::__assert_type<value_type, score_type>();
  BoundingBoxType<int> boxi = this->template convert_type<int>();
  return cv::Point2i(boxi.x1, boxi.y1);
}

template<typename T1, typename T2>
inline cv::Point2i ortcv::types::BoundingBoxType<T1, T2>::rb() const {
  ::__assert_type<value_type, score_type>();
  BoundingBoxType<int> boxi = this->template convert_type<int>();
  return cv::Point2i(boxi.x2, boxi.y2);
}

template<typename T1, typename T2>
inline typename ortcv::types::BoundingBoxType<T1, T2>::value_type
ortcv::types::BoundingBoxType<T1, T2>::width() const
{::__assert_type<value_type, score_type>(); return (x2 - x1 + static_cast<value_type>(1)); }

template<typename T1, typename T2>
inline typename ortcv::types::BoundingBoxType<T1, T2>::value_type
ortcv::types::BoundingBoxType<T1, T2>::height() const
{::__assert_type<value_type, score_type>(); return (y2 - y1 + static_cast<value_type>(1)); }

template<typename T1, typename T2>
inline typename ortcv::types::BoundingBoxType<T1, T2>::value_type
ortcv::types::BoundingBoxType<T1, T2>::area() const
{::__assert_type<value_type, score_type>(); return std::abs<value_type>(width() * height()); }

#endif //LITEHUB_ORT_CORE_ORT_TYPES_H
