template <int N> struct DuplicateStaticMemberTemplate { static const int value = N; };
template <int N> int const DuplicateStaticMemberTemplate<N>::value;
