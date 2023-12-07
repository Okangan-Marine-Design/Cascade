#ifndef SYSTEM_PLUGIN_MOVEPLUGIN_HH_
#define SYSTEM_PLUGIN_MOVEPLUGIN_HH_

#include <gz/sim/System.hh>
#include <gz/msgs.hh>

namespace move_Plugin
{
    class movePlugin:
    public gz::sim::System,
    public gz::sim::ISystemPreUpdate,
    public gz::sim::ISystemPostUpdate
  {
        public: void PreUpdate(const gz::sim::UpdateInfo &_info,
                gz::sim::EntityComponentManager &_ecm) override;

        public: void PostUpdate(const gz::sim::UpdateInfo &_info,
                const gz::sim::EntityComponentManager &_ecm) override;

  };
}
#endif
