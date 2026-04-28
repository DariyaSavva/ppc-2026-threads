
#include <cstdint>
#include <random>
#include <vector>
#include <cstddef>

#include "savva_d_monte_carlo/common/include/common.hpp"
#include "savva_d_monte_carlo/stl/include/ops_stl.hpp"

namespace savva_d_monte_carlo {

SavvaDMonteCarloSTL::SavvaDMonteCarloSTL(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0;
}

bool SavvaDMonteCarloSTL::ValidationImpl() {
  const auto &input = GetInput();

  // Проверка количества точек
  if (input.count_points == 0) {
    return false;
  }

  // Проверка наличия функции
  if (!input.f) {
    return false;
  }

  // Проверка размерности
  if (input.Dimension() == 0) {
    return false;
  }

  // Проверка корректности границ
  for (size_t i = 0; i < input.Dimension(); ++i) {
    if (input.lower_bounds[i] > input.upper_bounds[i]) {
      return false;
    }
  }

  return true;
}

bool SavvaDMonteCarloSTL::PreProcessingImpl() {
  return true;
}

bool SavvaDMonteCarloSTL::RunImpl() {
  const auto &input = GetInput();
  auto &result = GetOutput();

  const size_t dim = input.Dimension();
  const double vol = input.Volume();
  const auto n = static_cast<int64_t>(input.count_points);
  const auto &func = input.f;

  if (!func) {
    return false;
  }

  const auto &lb = input.lower_bounds;
  const auto &ub = input.upper_bounds;

  // распределения создаём один раз
  std::vector<std::uniform_real_distribution<double>> dists;
  dists.reserve(dim);
  for (size_t i = 0; i < dim; ++i) {
    dists.emplace_back(lb[i], ub[i]);
  }

  // генератор
  std::minstd_rand gen(std::random_device{}());

  // буфер точки — один на весь цикл
  std::vector<double> point(dim);

  double sum = 0.0;

  // 🔥 основной цикл (максимально "чистый")
  for (int64_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < dim; ++j) {
      point[j] = dists[j](gen);
    }
    sum += func(point);
  }

  result = vol * sum / static_cast<double>(n);
  return true;
}

bool SavvaDMonteCarloSTL::PostProcessingImpl() {
  return true;
}

}  // namespace savva_d_monte_carlo
