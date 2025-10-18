#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/LinearMath/Matrix3x3.h"

class ModelBasedController : public rclcpp::Node
{
public:
    ModelBasedController()
    : Node("trajectory_follower")
    {
        // sub
        desired_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "wcr/desired_trajectory", 10,
            std::bind(&ModelBasedController::desired_callback, this, std::placeholders::_1));

        odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "wcr/odom", 10,
            std::bind(&ModelBasedController::odom_callback, this, std::placeholders::_1));

        // pub
        driving_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/driving_velocity_controller/commands", 10);

        steering_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/steering_position_controller/commands", 10);

        RCLCPP_INFO(this->get_logger(), "ModelBasedController node started");
    }

private:
    void desired_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        double roll_d, pitch_d, yaw_d;
        quaternionToEuler(msg->pose.pose.orientation, roll_d, pitch_d, yaw_d);
        double x_d = msg->pose.pose.position.x;
        double y_d = msg->pose.pose.position.y;
        double theta_d = yaw_d;

        double v_x_d = msg->twist.twist.linear.x;
        double v_y_d = msg->twist.twist.linear.y;
        double omega_d = msg->twist.twist.angular.z;

        
    }

    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        double roll, pitch, yaw;
        quaternionToEuler(msg->pose.pose.orientation, roll, pitch, yaw);
        x_ = msg->pose.pose.position.x;
        y_ = msg->pose.pose.position.y;
        theta_ = yaw;

        v_x_ = msg->twist.twist.linear.x;
        v_y_ = msg->twist.twist.linear.y;
        omega_ = msg->twist.twist.angular.z;
    }

    void quaternionToEuler(const geometry_msgs::msg::Quaternion &q, double &roll, double &pitch, double &yaw)
    {
    tf2::Quaternion tf_q(q.x, q.y, q.z, q.w);
    tf2::Matrix3x3(tf_q).getRPY(roll, pitch, yaw);
    }

    // sub
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr desired_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;

    // pub
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr driving_pub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr steering_pub_;

    // state variables
    double x_, y_, theta_;
    double v_x_, v_y_, omega_;
    
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ModelBasedController>());
    rclcpp::shutdown();
    return 0;
}
