#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/sensor_reading.hpp"
#include "cascade_msgs/msg/json_stamped.hpp"
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
            this->create_subscription<cascade_msgs::msg::JsonStamped>("/hardware/rawJson", 10, std::bind(&Json_Parser::json_callback, this, _1));
        //must bind callback for some reason?
        RCLCPP_INFO(this->get_logger(), "created json parser node");
    }
private:
     void publishSensorReadings(const json& sensorReadings, cascade_msgs::msg::JsonStamped raw) {
        for (auto& element : sensorReadings.items()) {
            std::string sensorType = element.key();
            auto value = element.value();

            if (value.is_object()) {
                // If value is an object, publish nested values
                publishNestedValues(sensorType, value, raw);
            } else {
                // Publish primitive types
                publishPrimitive(sensorType, value, raw);
            }
        }
    }

    void publishNestedValues(const std::string& prefix, const json& nestedValues, cascade_msgs::msg::JsonStamped raw) {
        for (auto& nestedElement : nestedValues.items()) {
            std::string topic =  prefix.empty() ? "/"+prefix + "/" + nestedElement.key() : prefix + "/" + nestedElement.key();
            auto value = nestedElement.value();

            if (value.is_object()) {
                // Recursively publish nested values
                publishNestedValues(topic, value, raw);
            } else {
                // Publish primitive types
                publishPrimitive(topic, value, raw);
            }
        }
    }

    void publishPrimitive(const std::string& topic, const json& value, cascade_msgs::msg::JsonStamped raw) {
        // Publish primitive types

        if(sensorPublisherMap.count(topic)==0){
            auto publisher = this->create_publisher<cascade_msgs::msg::SensorReading>("/raw_sensors/"+topic, 10);
            sensorPublisherMap.insert(std::pair{topic,publisher});
        }
        auto message = cascade_msgs::msg::SensorReading();
        message.data = std::stof(value.dump());
        message.header = raw.header;
        sensorPublisherMap[topic]->publish(message);
    }   

    void json_callback(cascade_msgs::msg::JsonStamped raw)
    {
        //parse json data
        //should auto loop through all elemements of the json data and publish to a topic automatically, ie create a topic based on the name of the sensor, so would not need to be pre defined
        json jsonObject = json::parse(raw.data);
        Json_Parser::publishSensorReadings(jsonObject, raw);
    }
    std::map<std::string, rclcpp::Publisher<cascade_msgs::msg::SensorReading>::SharedPtr> sensorPublisherMap;
    rclcpp::Subscription<cascade_msgs::msg::JsonStamped>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Json_Parser>());
    rclcpp::shutdown();
    return 0;
}
