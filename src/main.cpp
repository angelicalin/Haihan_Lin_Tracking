#include "kinect2_grabber.h"
#include <pcl/visualization/pcl_visualizer.h>
#include<iostream>
#include <pcl/io/ply_io.h>
using namespace pcl;

typedef pcl::PointXYZRGBA PointType;



int main(int argc, char* argv[])
{
	// PCL Visualizer
	PointCloud<PointXYZ>::Ptr cloudFixed(new PointCloud<PointXYZ>);
	if (io::loadPLYFile("H:/UsingPCL/src/ironman 50k faces.ply", *cloudFixed) == -1) {
		PCL_ERROR("Couldn't read file test_pcd.pcd \n");
		return (-1);
	}
	
	
	

	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(
		new pcl::visualization::PCLVisualizer("Point Cloud Viewer"));
	viewer->setCameraPosition(0.0, 0.0, -2.5, 0.0, 0.0, 0.0);

	// Point Cloud
	pcl::PointCloud<PointType>::ConstPtr cloud;

	// Retrieved Point Cloud Callback Function
	boost::mutex mutex;
	boost::function<void(const pcl::PointCloud<PointType>::ConstPtr&)> function =
		[&cloud, &mutex](const pcl::PointCloud<PointType>::ConstPtr& ptr) {
		boost::mutex::scoped_lock lock(mutex);

		/* Point Cloud Processing */

		cloud = ptr->makeShared();
	};

	// Kinect2Grabber
	boost::shared_ptr<pcl::Grabber> grabber = boost::make_shared<pcl::Kinect2Grabber>();

	// Register Callback Function
	boost::signals2::connection connection = grabber->registerCallback(function);

	// Start Grabber
	grabber->start();

	visualization::PCLVisualizer fixedviwer("ironman");
	fixedviwer.addPointCloud(cloudFixed);

	while (!viewer->wasStopped()) {
		// Update Viewer
		viewer->spinOnce();

		boost::mutex::scoped_try_lock lock(mutex);
		if (lock.owns_lock() && cloud) {
			// Update Point Cloud
			if (!viewer->updatePointCloud(cloud, "cloud")) {
				viewer->addPointCloud(cloud, "cloud");
			}
		}
	}

	// Stop Grabber
	grabber->stop();

	// Disconnect Callback Function
	if (connection.connected()) {
		connection.disconnect();
	}

	return 0;
}