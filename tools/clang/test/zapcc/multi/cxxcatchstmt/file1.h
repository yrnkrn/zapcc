#pragma once
template <class> struct Bug380shared_ptr { template <int> friend struct Bug380A; };
