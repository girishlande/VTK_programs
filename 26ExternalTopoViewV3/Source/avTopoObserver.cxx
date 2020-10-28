#include "avTopoObserver.h"

#include <algorithm>

#include "avTopoViewerEx.h"

void avTopoObserver::RegisterTopo(avTopoViewerEx* topo) {
  std::cout << "\n Register Topo called!";
  assert(topo);
  if (topo) m_topos.push_back(topo);
}

void avTopoObserver::UnRegisterTopo(avTopoViewerEx* topo) {
  assert(topo);
  std::cout << "\n UnRegister Topo called!";
  if (topo) {
    m_topos.erase(std::remove(m_topos.begin(), m_topos.end(), topo),
                  m_topos.end());
  }
}

void avTopoObserver::PositionChanging(avTopoViewerEx* topo, int moveX,
                                      int moveY) {
  if (!m_enabled) return;

  int inputGroupId = topo->GroupId();
  for (auto t : m_topos) {
    if (t != topo && t->GroupId() == inputGroupId) {
      t->PositionChanging(moveX, moveY);
    }
  }
}

void avTopoObserver::PositionUpdated(avTopoViewerEx* topo, int moveX,
                                     int moveY) {
  if (!m_enabled) return;

  int inputGroupId = topo->GroupId();
  for (auto t : m_topos) {
    if (t != topo && t->GroupId() == inputGroupId) {
      t->PositionUpdated(moveX, moveY);
    }
  }
}

void avTopoObserver::SetEnabled(bool flag) { m_enabled = flag; }
bool avTopoObserver::Enabled() { return m_enabled; }