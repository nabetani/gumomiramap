#include <opencv2/opencv.hpp>
#include <vector>

struct gm_t {
  int w;
  double z;
};

std::vector<float> gumomira_value(gm_t const &gm) {
  std::vector<float> m(gm.w * gm.w);
  for (std::size_t i = 0; i < m.size(); ++i) {
    m[i] = std::sin(i * i * 0.01);
  }
  return m;
}

cv::Mat gumomira_image(gm_t const &gm) {
  cv::Mat image = cv::Mat::zeros(gm.w, gm.w, CV_8UC1);
  std::vector<float> m = gumomira_value(gm);
  auto [ilo, ihi] = std::minmax_element(cbegin(m), cend(m));
  auto lo = *ilo;
  auto hi = *ihi;
  auto map = [lo, hi](float v) -> std::uint8_t {
    return static_cast<std::uint8_t>((v - lo) * (255 / (hi - lo)));
  };
  for (size_t y = 0; y < gm.w; ++y) {
    for (size_t x = 0; x < gm.w; ++x) {
      image.at<std::uint8_t>(x, y) = map(m[y * gm.w + x]);
    }
  }

  return image;
}

int main() {
  cv::Mat image = gumomira_image({.w = 500, .z = 0.1});
  cv::imwrite("output.png", image);
  return 0;
}