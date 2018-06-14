#pragma once
namespace Bug702dealii {
template <typename> class Table {};
template <typename> class Accessor { template <typename> friend class Table; };
}
