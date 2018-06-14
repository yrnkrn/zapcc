#include "l.h"
void GetStandardLayerName() {
  Bug2019basic_string m_Layer[1];
  for (int layer;; ++layer) {
    Bug2019ToLAYER_ID(layer);
    m_Layer[layer].foo(m_Layer[0]);
  }
}
