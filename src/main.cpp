#include <opencv2/opencv.hpp>
#include <vector>

using point_t = cv::Point2d;
using num_t = double;

struct gm_t {
  int w;
  point_t p0 = {5, 0};
  int pre = 100;
  int rep = 100000000;

  num_t a = 0.008;
  num_t s = 0.05;
  num_t mu = -0.496;

  point_t progress(point_t const &p) const {
    auto x = p.x;
    auto y = p.y;
    auto xx = y + a * y * (1 - s * y * y) + mu * x +
              2 * (1 - mu) * x * x / (1 + x * x);
    return {xx, -x + mu * xx + 2 * (1 - mu) * xx * xx / (1 + xx * xx)};
  }
};

std::vector<point_t> gumomira_value(gm_t const &gm) {
  std::vector<point_t> m;
  m.reserve(gm.rep);
  auto p = gm.p0;
  for (int i = 0; i < gm.pre; ++i) {
    p = gm.progress(p);
  }
  for (int i = 0; i < gm.rep; ++i) {
    p = gm.progress(p);
    m.push_back(p);
  }
  return m;
}

cv::Mat gumomira_image(gm_t const &gm) {
  cv::Mat image = cv::Mat::zeros(gm.w, gm.w, CV_8UC1);
  auto pts = gumomira_value(gm);
  auto [i_xlo, i_xhi] = std::minmax_element(
      cbegin(pts), cend(pts),
      [](point_t const &a, point_t const &b) -> bool { return a.x < b.x; });
  auto [i_ylo, i_yhi] = std::minmax_element(
      cbegin(pts), cend(pts),
      [](point_t const &a, point_t const &b) -> bool { return a.y < b.y; });
  auto xlo = i_xlo->x;
  auto xhi = i_xhi->x;
  auto ylo = i_ylo->y;
  auto yhi = i_yhi->y;
  auto dx = (xhi - xlo) / 10;
  auto dy = (yhi - ylo) / 10;
  xlo -= dy;
  xhi += dx;
  ylo -= dy;
  yhi += dy;
  std::vector<int> im(gm.w * gm.w);

  auto z = gm.w / std::max(xhi - xlo, yhi - ylo);
  std::cout << "z=" << z << std::endl;

  for (auto v : pts) {
    auto x = std::lrint((v.x - xlo) * z);
    auto y = std::lrint((v.y - ylo) * z);
    ++im[y * gm.w + x];
  }
  auto imcopy = im;
  std::sort(begin(imcopy), end(imcopy));
  double max = imcopy[imcopy.size() - 1];
  std::cout << "max=" << max << std::endl;

  for (size_t y = 0; y < gm.w; ++y) {
    for (size_t x = 0; x < gm.w; ++x) {
      auto v0 = im[y * gm.w + x] / max;
      auto v = std::pow(v0, 0.2);
      image.at<std::uint8_t>(x, y) = cv::saturate_cast<std::uint8_t>(v * 255);
    }
  }

  return image;
}

int main() {
  cv::Mat image = gumomira_image({.w = 2000});
  cv::imwrite("output.png", image);
  return 0;
}