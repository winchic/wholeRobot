// Script that runs mining operation
// This file works with ProcessManager.h and NotDTClass.h and ProcessManager.cpp and NotDTClass.cpp
#include <gen2bot/NotDTClass.h>
#include <gen2bot/ProcessManager.h>
#include <iostream>
#include <chrono> 
#include <thread>
#include <mutex>

#include "ros/ros.h"
#include "std_msgs/String.h"

#include <iostream>

// Search up pointers in cpp, mutex, and threads in cpp before continuing

std::mutex data_mutex;

int data;

// For each function, we have to pass in the memory address of p_cmd so that we can see if it's value
// changes outside the function and thus, kill the function. We must also pass the NotDT object to run it's
// motor functions, and lastly, we must pass in nodehandle to have access to ros parameters
void goToDrive(int& p_cmd, NotDTClass notDT, ros::NodeHandle nh)
{
	int sentinel = p_cmd;
	ROS_INFO("goToDrive");
	// Probably don't need while loops, can't test if it's fine without it though
	while(p_cmd == sentinel)
	{
		// For each function from notDTClass, we must pass in p_cmd so that we can constantly
		// check if it changes inside the function and if so, kill the function mid-run
		notDT.driveMode(p_cmd, nh);
		// After function ends, put p_cmd in default value so process knows function has ended
		p_cmd = 0;
	}
}

void deposit(int& p_cmd, NotDTClass notDT, ros::NodeHandle nh)
{
	int sentinel = p_cmd;
	ROS_INFO("goToDeposit");
	while(p_cmd == sentinel)
	{
		notDT.deposit(p_cmd, nh);
		p_cmd = 0;
	}
}

void dig(int& p_cmd, NotDTClass notDT, ros::NodeHandle nh)
{
	int sentinel = p_cmd;
	ROS_INFO("goToDig");
	while(p_cmd == sentinel)
	{
		notDT.dig(p_cmd, nh);
		p_cmd = 0;
	}
}

void zero(int& p_cmd, NotDTClass notDT, ros::NodeHandle nh)
{
	int sentinel = p_cmd;
	ROS_INFO("zero");
	while(p_cmd == sentinel)
	{
		notDT.zero(p_cmd, nh);
		std::cout << "Exit notDT.zero with sentinel, notDTsentinel, z " << 
		" and p_cmd: " << sentinel << " " << notDT.sentinel << " " << p_cmd << std::endl;
		p_cmd = 0;
	}

}

void config(NotDTClass notDT, ros::NodeHandle nh)
{
	notDT.config(nh);
}

void updateProcess(const int& msg)
{
	std::lock_guard<std::mutex> lock(data_mutex);
	data = msg;
}

int getData()
{
	std::lock_guard<std::mutex> lock(data_mutex);
	return data;
}

int main(int argc, char** argv) 
{	

    ros::init(argc, argv, "notDTNode");

	ros::NodeHandle nh;

	NotDTClass notDT(nh);

	int p_cmd = 0;
	int* ptr = &p_cmd;

	// pass in ptr of p_cmd to always get it's updated position
	ProcessManager proMan(ptr);

  	ros::Rate loop_rate(6);

	// use the & to allow us to use this-> key word for pointers
	ros::Subscriber sub = nh.subscribe("robot_process", 0, &ProcessManager::callback, &proMan);
	
	ros::AsyncSpinner spinner(0);
	spinner.start();

	while (ros::ok())
	{
		ROS_INFO("From the main loop: %d", p_cmd);
		switch (p_cmd)
		{
		case 0:
			break;
		case 1:
			goToDrive(p_cmd, notDT, nh);
			p_cmd = 0;
			break;
		case 2:
			dig(p_cmd, notDT, nh);
			p_cmd = 0;
			break;
		case 3:
			deposit(p_cmd, notDT, nh);
			p_cmd = 0;
			break;
		case 4:
			zero(p_cmd, notDT, nh);
			p_cmd = 0;
			break;
		case 5:
			config(notDT, nh);
			break;
		default:
			break;
		}
		ros::spinOnce();
		loop_rate.sleep();

	}

	// shut down motors
	notDT.stop();

	// Tell us if the node properly closed and the motors have been shutdown 
	std::cout << "LinAct and BallS are now still: " << std::endl;

	std::cout << "Script has now ended: " << std::endl;
	return 0;
}

