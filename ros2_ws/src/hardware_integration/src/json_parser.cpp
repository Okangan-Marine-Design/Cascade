#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float32.hpp"
#include <nlohmann/json.hpp>

using std::placeholders::_1;
using json = nlohmann::json;

class Json_Parser : public rclcpp::Node
{
public:
    Json_Parser()
    : Node("json_parser")
    {
        subscription_ =
            this->create_subscription<std_msgs::msg::String>("/hardware/rawJson", 10, std::bind(&Json_Parser::json_callback, this, _1));
        //must bind callback for some reason?
        RCLCPP_INFO(this->get_logger(), "created json parser node");
    }
private:
     void publishSensorReadings(const json& sensorReadings) {
        for (auto& element : sensorReadings.items()) {
            std::string sensorType = element.key();
            auto value = element.value()["value"];

            if (value.is_object()) {
                // If value is an object, publish nested values
                publishNestedValues(sensorType, value);
            } else {
                // Publish primitive types
                publishPrimitive(sensorType, value);
            }
        }
    }

    void publishNestedValues(const std::string& prefix, const json& nestedValues) {
        for (auto& nestedElement : nestedValues.items()) {
            std::string topic =  prefix.empty() ? "/"+prefix + "/" + nestedElement.key() : prefix + "/" + nestedElement.key();
            auto value = nestedElement.value();

            if (value.is_object()) {
                // Recursively publish nested values
                publishNestedValues(topic, value);
            } else {
                // Publish primitive types
                publishPrimitive(topic, value);
            }
        }
    }

    void publishPrimitive(const std::string& topic, const json& value) {
        // Publish primitive types

        if(sensorPublisherMap.count(topic)==0){
            auto publisher = this->create_publisher<std_msgs::msg::Float32>("/sensors/"+topic, 10);
            sensorPublisherMap.insert(std::pair{topic,publisher});
        }
        auto message = std_msgs::msg::Float32();
        message.data = std::stof(value.dump());
        sensorPublisherMap[topic]->publish(message);
    }   

    void json_callback(std_msgs::msg::String::SharedPtr msg)
    {
        //parse json data
        //should auto loop through all elemements of the json data and publish to a topic automatically, ie create a topic based on the name of the sensor, so would not need to be pre defined
        json jsonObject = json::parse(msg->data);
        Json_Parser::publishSensorReadings(jsonObject);
    }
    std::map<std::string, rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr> sensorPublisherMap;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Json_Parser>());
    rclcpp::shutdown();
    return 0;
}
