#ifndef TOPOOBSERVER_H
#define TOPOOBSERVER_H

#include <vector>
#include <map>
#include <qstring.h>

class avTopoViewerEx;

class avTopoObserver {
 public:
  static avTopoObserver& getInstance() {
    static avTopoObserver instance;  // Guaranteed to be destroyed.
                                     // Instantiated on first use.
    return instance;
  }
  
  void SetVisibility(bool flag);

  void RegisterTopo(avTopoViewerEx* topo);
  void UnRegisterTopo(avTopoViewerEx* topo);

  // Synchronous update in positioning of all topos
  void BeginPositioning(avTopoViewerEx* topo);
  void PositionChanging(avTopoViewerEx* topo, int xMov, int yMov);
  void PositionUpdated(avTopoViewerEx* topo,int xMov, int yMov);
  void ResetPosition();
  
  // Enable / Disable synchronization between all topos with same group id
  void SetEnabled(bool flag);
  bool Enabled();

  avTopoObserver(avTopoObserver const&) = delete;
  void operator=(avTopoObserver const&) = delete;

 private:

  avTopoObserver() {} // private constructor

  std::vector<avTopoViewerEx*> m_topos;
  std::map<QString, int> m_groupids;
  bool m_enabled = true;
  
};

#endif