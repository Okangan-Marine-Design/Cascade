#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
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
    void publishJSONObject(const std::string& prefix, const json& jsonObject) {
        for (auto& element : jsonObject.items()) {
            std::string key = prefix.empty() ? element.key() : prefix + "." + element.key();

            if (element.value().is_object()) {
                // Recursively handle nested objects
                publishJSONObject(key, element.value());
            } else if (element.value().is_array()) {
                // Handle arrays
                publishJSONArray(key, element.value());
            } else {
                // Publish primitive types
                publishPrimitive(key, element.value());
            }
        }
    }

    void publishJSONArray(const std::string& prefix, const json& jsonArray) {
        int index = 0;
        for (auto& element : jsonArray) {
            std::string key = prefix + "[" + std::to_string(index++) + "]";

            if (element.is_object()) {
                // Recursively handle nested objects in array
                publishJSONObject(key, element);
            } else if (element.is_array()) {
                // Handle nested arrays
                publishJSONArray(key, element);
            } else {
                // Publish primitive types
                publishPrimitive(key, element);
            }
        }
    }

    void publishPrimitive(const std::string& topic, const json& value) {
        // Publish primitive types
        if (value.is_number_integer()) {
            auto publisher = create_publisher<std_msgs::msg::Int32>(topic, 10);
            auto message = std_msgs::msg::Int32();
            message.data = value;
            publisher->publish(message);
        } else if (value.is_number_float()) {
            auto publisher = create_publisher<std_msgs::msg::Float64>(topic, 10);
            auto message = std_msgs::msg::Float64();
            message.data = value;
            publisher->publish(message);
        } else if (value.is_string()) {
            auto publisher = create_publisher<std_msgs::msg::String>(topic, 10);
            auto message = std_msgs::msg::String();
            message.data = value;
            publisher->publish(message);
        }
        // Add more cases for other primitive types as needed
    }
    void json_callback(std_msgs::msg::String::SharedPtr msg) const
    {
        //parse json data
        //should auto loop through all elemements of the json data and publish to a topic automatically, ie create a topic based on the name of the sensor, so would not need to be pre defined
        json jsonObject = json::parse(jsonString);
        publishJSONObject("", jsonObject);
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
