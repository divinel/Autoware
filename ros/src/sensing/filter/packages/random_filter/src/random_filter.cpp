/*
 *  Copyright (c) 2015, Nagoya University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>

#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

ros::Publisher filtered_points_pub;

static int sample_num = 1000;

static void scan_callback(const sensor_msgs::PointCloud2::ConstPtr& input)
{
    pcl::PointXYZ sampled_p;
    pcl::PointCloud<pcl::PointXYZ> scan;

    pcl::fromROSMsg(*input, scan);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr scan_ptr(new pcl::PointCloud<pcl::PointXYZ>(scan));
    pcl::PointCloud<pcl::PointXYZ>::Ptr filtered_scan_ptr(new pcl::PointCloud<pcl::PointXYZ>());
	filtered_scan_ptr->header = scan.header;

	int points_num = scan.size();
	int step = points_num / sample_num;

	for(int i = 0; i < points_num; i++){
		if(i % step == 0){
			filtered_scan_ptr->points.push_back(scan.at(i));
		}
	}

	sensor_msgs::PointCloud2 filtered_msg;
	pcl::toROSMsg(*filtered_scan_ptr, filtered_msg);
	filtered_points_pub.publish(filtered_msg);
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "random_filter");

	ros::NodeHandle nh;
	ros::NodeHandle private_nh("~");

    // Publishers
    filtered_points_pub = nh.advertise<sensor_msgs::PointCloud2>("/filtered_points", 10);

	// Subscribers
	ros::Subscriber scan_sub = nh.subscribe("points_raw", 10, scan_callback);

	ros::spin();

	return 0;
}
