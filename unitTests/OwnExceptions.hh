#pragma once

#include <stdexcept>
#ifndef THROW_invalid_argument
#define THROW_invalid_argument(X) throw std::invalid_argument(X)
#endif
#ifndef THROW_out_of_range
#define THROW_out_of_range(X) throw std::out_of_range(X)
#endif
