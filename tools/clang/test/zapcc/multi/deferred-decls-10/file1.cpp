#include "ExternalSemaSource.h"
struct Bug1641LateParsedTemplate {
  Bug1641SmallVector Toks;
};
Bug1641LateParsedTemplate LateParsedTemplateMap;
void foo1() { Bug1641DeleteContainerSeconds<Bug1641LateParsedTemplate *>(); }
