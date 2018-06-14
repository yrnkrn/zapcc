template <typename T> struct DuplicateInheritedTemplateBase {};
template <typename T>
struct DuplicateInheritedTemplateDerived : DuplicateInheritedTemplateBase<T> {};
