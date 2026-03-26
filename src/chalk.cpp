#include <polycpp/chalk/chalk.hpp>

namespace polycpp {
namespace chalk {

Chalk::Chalk() : level_(0) {}

Chalk::Chalk(const Options& options) : level_(options.level.value_or(0)) {}

int Chalk::level() const { return level_; }

void Chalk::setLevel(int level) { level_ = level; }

std::string Chalk::operator()(const std::string& text) const {
    return text;
}

} // namespace chalk
} // namespace polycpp
