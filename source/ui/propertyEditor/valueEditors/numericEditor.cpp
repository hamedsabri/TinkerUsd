#include "numericEditor.h"

namespace TINKERUSD_NS
{

// Explicit instantiations
template class NumericSliderGroupWidget<int32_t>;
template class NumericSliderGroupWidget<uint32_t>;
template class NumericSliderGroupWidget<float>;
template class NumericSliderGroupWidget<double>;
template class NumericEditor<int32_t>;
template class NumericEditor<uint32_t>;
template class NumericEditor<float>;
template class NumericEditor<double>;

} // namespace TINKERUSD_NS