namespace Bug906std2 {
template <typename> class unique_ptr { template <int> friend class a; };
}
