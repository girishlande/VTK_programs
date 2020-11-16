#include "avTopoObserver.h"
#include "avTopoViewerEx.h"

#include <algorithm>

std::mutex m_topoObserverMutex;
int g_observer_group_id = 1;

void avTopoObserver::RegisterTopo(avTopoViewerEx* topo) {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    assert(topo);
    if (topo) {
        m_topos.push_back(topo);
        QString seriesId = topo->SeriesId();
        auto it = m_groupids.find(seriesId);
        int groupId = g_observer_group_id;
        if (it != m_groupids.end()) {
            groupId = it->second;
        }
        else {
            m_groupids[seriesId] = g_observer_group_id;
            g_observer_group_id++;
        }
        topo->SetGroupId(groupId);
    }
}

void avTopoObserver::UnRegisterTopo(avTopoViewerEx* topo) {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    assert(topo);
    if (topo) {
        m_topos.erase(std::remove(m_topos.begin(), m_topos.end(), topo),
            m_topos.end());
    }
}

void avTopoObserver::BeginPositioning(avTopoViewerEx* topo) {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    if (!m_enabled) return;
    assert(topo);

    int relativeX = 0;
    int relativeY = 0;
    topo->GetPositionRelativeToViewport(relativeX, relativeY);
    int inputGroupId = topo->GroupId();
    for (auto t : m_topos) {
        if (t != topo && t->GroupId() == inputGroupId) {
            t->SetPositionRelativeToViewport(relativeX, relativeY);
        }
    }
}


void avTopoObserver::PositionChanging(avTopoViewerEx* topo, int moveX,
    int moveY) {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
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
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    if (!m_enabled) return;

    int inputGroupId = topo->GroupId();
    for (auto t : m_topos) {
        if (t != topo && t->GroupId() == inputGroupId) {
            t->PositionUpdated(moveX, moveY);
        }
    }
}

void avTopoObserver::ResetPosition() {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    for (auto t : m_topos) {
        t->ResetPosition();
    }
}

void avTopoObserver::SetVisibility(bool flag) {
    std::lock_guard<std::mutex> guard(m_topoObserverMutex);
    for (auto t : m_topos) {
        t->SetVisibility(flag);
    }
}

void avTopoObserver::SetEnabled(bool flag) { m_enabled = flag; }
bool avTopoObserver::Enabled() { return m_enabled; }