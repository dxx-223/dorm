#ifndef SPTR
#define SPTR

#include <memory>

template<class CLASS>
using SPC = std::shared_ptr<const CLASS>;

#endif
