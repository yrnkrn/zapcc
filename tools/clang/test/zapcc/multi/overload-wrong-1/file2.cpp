const char *YesNo() { return "No"; }
namespace YesNoNameSpace {
const char *getYesNo() { return YesNo(); }
}
using namespace YesNoNameSpace;
extern "C" int puts(const char *);
int main() { puts(getYesNo()); }
