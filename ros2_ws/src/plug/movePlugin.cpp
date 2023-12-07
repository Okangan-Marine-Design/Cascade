#include <string>
#include <gz/common/Console.hh>
#include <gz/sim/Link.hh>
#include <gz/msgs.hh>
#include <gz/transport.hh>
#include "gz/sim/Util.hh"
#include <gz/sim/Types.hh>
#include <gz/math/Vector3.hh>
#include <gz/plugin/Register.hh>
#include <chrono>

#include "movePlugin.hpp"

using namespace move_Plugin;
using namespace gz;
using namespace sim;
using namespace math;

gz::transport::Node node;
std::string topic = "/box/cmd_vel", modelName = "box";
gz::msgs::Twist msg;
Vector3 lin=Vector3(0.0,0.0,0.0),ang=Vector3(0.0,0.0,0.0);
std::chrono::time_point lastTime=std::chrono::steady_clock::now();
bool gotBox=false;
gz::sim::v7::Link box;
std::chrono::duration<double, std::milli> topicWaitTime (100);

void onTopicReceive(const gz::msgs::Twist &temp_msg){
    msg=gz::msgs::Twist(temp_msg);
    lin=Vector3(msg.linear().x(),msg.linear().y(),msg.linear().z());
    ang=Vector3(msg.angular().x(),msg.angular().y(),msg.angular().z());
    lastTime=std::chrono::steady_clock::now();
}

void movePlugin::PreUpdate(const gz::sim::UpdateInfo &_info,
    gz::sim::EntityComponentManager &_ecm){
    if(!gotBox){
        std::unordered_set<gz::sim::v7::Entity> set=gz::sim::entitiesFromScopedName(modelName,_ecm);	
        for (const auto& elem : set)
            if(_ecm.HasEntity(elem)){
                gzmsg << "Got Entity!"<<'\n';
                box=Link(elem);
                box.EnableVelocityChecks(_ecm);
                box.EnableAccelerationChecks(_ecm);
                gotBox=node.Subscribe(topic, onTopicReceive);
                if (!gotBox){
                    gzmsg << "Error subscribing to topic [" << topic << "]" << std::endl;
                }
                else {
                    gzmsg << "subscribing to topic [" << topic << "]" << std::endl;
                }
            }
    }
    else{
        //change to only apply velocity if constantly being published,
        //maybe record time of last subscription callback and reset velocity to 0 after 100ms or so
        //
        /*
        if(!box.WorldLinearVelocity(_ecm)->Equal(lin,0.05)
        || !box.WorldAngularVelocity(_ecm)->Equal(ang,0.05)){
            //box.AddWorldWrench(_ecm,lin,ang);
            //std::cout<<lin.Max()<<"\n";
        }
        */
        box.SetLinearVelocity(_ecm,lin);
        box.SetAngularVelocity(_ecm,ang);
    }
    if(std::chrono::steady_clock::now()-lastTime>topicWaitTime){
        lin.Set(0.0,0.0,0.0);
        ang.Set(0.0,0.0,0.0);
        lastTime=std::chrono::steady_clock::now();
        //resets lastTime to current time to not waste resources 
        //reseting lin and ang Vectors every iteration
    }
}

void movePlugin::PostUpdate(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm){
        //gzmsg << (int)_ecm.EntityCount() << std::endl;
}

GZ_ADD_PLUGIN(
    move_Plugin::movePlugin,
    gz::sim::System,
    move_Plugin::movePlugin::ISystemPreUpdate,
    move_Plugin::movePlugin::ISystemPostUpdate)

