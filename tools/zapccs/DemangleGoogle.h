// Demangle "mangled".  On success, return true and write the
// demangled symbol name to "out".  Otherwise, return false.
// "out" is modified even if demangling is unsuccessful.
bool Demangle(const char *mangled, char *out, int out_size);
