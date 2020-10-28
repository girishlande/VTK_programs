#ifndef TOPOOBSERVER_H
#define TOPOOBSERVER_H

#include <vector>

class avTopoViewerEx;

class avTopoObserver {
 public:
  static avTopoObserver& getInstance() {
    static avTopoObserver instance;  // Guaranteed to be destroyed.
                                     // Instantiated on first use.
    return instance;
  }

  void RegisterTopo(avTopoViewerEx* topo);
  void UnRegisterTopo(avTopoViewerEx* topo);
  void PositionChanging(avTopoViewerEx* topo, int xMov, int yMov);
  void PositionUpdated(avTopoViewerEx* topo,int xMov, int yMov);
  void SetEnabled(bool flag);
  bool Enabled();

  avTopoObserver(avTopoObserver const&) = delete;
  void operator=(avTopoObserver const&) = delete;

 private:
  avTopoObserver() {} // private constructor
  std::vector<avTopoViewerEx*> m_topos;
  bool m_enabled = true;
};

#endif