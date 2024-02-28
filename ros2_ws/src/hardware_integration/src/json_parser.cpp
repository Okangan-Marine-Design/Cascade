#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/int32.hpp"
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
        auto publisher = this->create_publisher<std_msgs::msg::String>(topic, 10);
        auto message = std_msgs::msg::String();
        message.data = value.dump();
        publisher->publish(message);
    }   

    void json_callback(std_msgs::msg::String::SharedPtr msg)
    {
        //parse json data
        //should auto loop through all elemements of the json data and publish to a topic automatically, ie create a topic based on the name of the sensor, so would not need to be pre defined
        json jsonObject = json::parse(msg->data);
        Json_Parser::publishSensorReadings(jsonObject);
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Json_Parser>());
    rclcpp::shutdown();
    return 0;
}
