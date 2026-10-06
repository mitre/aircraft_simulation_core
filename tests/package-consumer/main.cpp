#include <aaesim/build_info.h>
#include <public/CoreUtils.h>
#include <public/VerticalPathUtils.h>

#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

int main() {
   using mitre::oss::simcore::CoreUtils;
   if (std::string(AAESIM_VERSION_STR) != EXPECTED_VERSION || !std::filesystem::exists(".")) {
      return 1;
   }
   const std::vector<double> x{0.0, 1.0};
   const std::vector<Units::KnotsSpeed> speeds{Units::KnotsSpeed(100), Units::KnotsSpeed(200)};
   const auto speed = CoreUtils::LinearlyInterpolateByDistance(1, Units::MetersLength(0.5), x, speeds);
   return std::abs(speed.value() - 150.0) < 1e-10 && CoreUtils::LinearlyInterpolate(1, 0.5, x, {0.0, 1.0}) == 0.5 ? 0 : 1;
}
