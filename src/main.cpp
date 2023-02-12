#include <opencv2/opencv.hpp>
#include <vector>

using point_t = cv::Point2d;
using num_t = double;

struct gm_t {
  int w = 2500;
  point_t p0 = {5, 0};
  int pre = 1000;
  int rep = 100000000;

  num_t a = 0.008;
  num_t s = 0.05;
  num_t mu = -0.496;
  double pow = 0.2;

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
  auto cx = (xhi + xlo) / 2;
  auto cy = (yhi + ylo) / 2;
  auto xyw = std::max(xhi - xlo, yhi - ylo) * 1.1 / 2;
  std::vector<uint64_t> im(gm.w * gm.w);
  auto z = gm.w / 2 / xyw;
  std::cout << "z=" << z << std::endl;

  for (auto v : pts) {
    auto x = (v.x - (cx - xyw)) * z;
    auto y = (v.y - (cy - xyw)) * z;
    auto ix = std::floor(x);
    auto iy = std::floor(y);
    auto dx = x - ix;
    auto dy = y - iy;
    const uint64_t pw = (1ull << 60) / gm.rep;
    im[iy * gm.w + ix] += pw * (1 - dx) * (1 - dy);
    im[iy * gm.w + (ix + 1)] += pw * dx * (1 - dy);
    im[(iy + 1) * gm.w + ix] += pw * (1 - dx) * dy;
    im[(iy + 1) * gm.w + (ix + 1)] += pw * (1 - dx) * (1 - dy);
  }
  double max = *std::max_element(cbegin(im), cend(im));
  std::cout << "max=" << max << std::endl;

  for (size_t y = 0; y < gm.w; ++y) {
    for (size_t x = 0; x < gm.w; ++x) {
      auto v0 = im[y * gm.w + x] / max;
      auto v = std::pow(v0, gm.pow);
      image.at<std::uint8_t>(x, y) = cv::saturate_cast<std::uint8_t>(v * 255);
    }
  }

  return image;
}

int main() {
  cv::Mat image = gumomira_image({});
  cv::imwrite("output.png", image);
  return 0;
}