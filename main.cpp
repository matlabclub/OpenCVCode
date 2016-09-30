/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program is an attempt to refactor "sortobstacles.m"(and related functions) from MATLAB to GPU accelerated C++ code //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <highgui.h> //when debugging GPU code I like to cover my bases
#include <gpu/gpu.hpp> //necessary for CUDA
#include <gpu/gpumat.hpp> //necessary for CUDA
#include <opencv_modules.hpp> //I was trying to find "nonfree"

#include <iostream> //I don't think this is necessary
#include <highgui/highgui.hpp> //boilerplate
#include <opencv.hpp> //boilerplate

using namespace std; //boilerplate
using namespace cv; //four less keys to press

Mat motionSeg(const Mat&, vector<Mat>&, int);  //declare the function
gpu::GpuMat motionSegCUDA(const cv::gpu::GpuMat&, vector<gpu::GpuMat>&, int); //declare the function
gpu::GpuMat BuoyFilter1(const cv::gpu::GpuMat&);

int main (int argc, char* argv[])
{
    vector<Mat> globalVec;  //declare a global vector to function as temporal storage container
    vector<cv::gpu::GpuMat> globalVecCUDA;  //declare a global vector to function as temporal storage container
    VideoCapture cap("/home/user/Downloads/2015-11-20_132417044.mp4");//lionfish2.mp4"); //open the default camera
    //VideoCapture cap(0); //open the default camera
    if(!cap.isOpened())  //check if we succeeded
        return -1; //boilerplate

    namedWindow("edges",1); //boilerplate
    for(;;)
    {
        Mat frame, frameout;  //declare placeholders

        cap >> frame; // get a new frame from camera
//begin differences
        gpu::GpuMat dst, src, dst2, src2;// GpuMat CUDA Mat equivilents
//start gpuprocessing
        src.upload(frame); //convert frame from Mat to GpuMat "src"
//start gpu processing

        dst = BuoyFilter1(src);
//gpu processing
        //dst = motionSegCUDA(src, globalVecCUDA, 2); //demo usage of "motionSeg()"
//gpu processing


//end gpu processing
        dst.download(frameout); //convert GpuMat dst to Mat "frameout"
//end gpu processing

//end differences
        imshow("edges",frameout); //boilerplate
        if(waitKey(30) >= 0) break; //boilerplate
    }
    return 0; //boilerplate
}
Mat motionSeg(const Mat& inMat, vector<Mat>& outMatvec, int framecount)
    {
    Mat outMat; //declare storage for return purpose
    if(outMatvec.size() < framecount) //check to make sure the necessarry # of frames are contained in the vector
    {
    outMatvec.push_back(inMat); //push the input mat in to the vector
    outMat = outMatvec.back(); //copy the contents of inMat in to the return "outMat"
    return outMat; //return the original inMat
    }
else{
    Mat tmp1, tmp2; //place holders
    tmp1 = outMatvec.back(); //put the oldest frame in tmp1
    outMatvec.pop_back(); //remove the oldest frame from the vector
    outMatvec.push_back(inMat); //put the newest frame("inMat") in to the vector
    tmp2 = outMatvec.back(); //put the second oldest frame in tmp2 TODO: Should this be .front() to make framecount meaningfull?
    absdiff(tmp1, tmp2, outMat); //compare the absolute per pixel difference between tmp1 and tmp2 and put the output in "outMat"
    return outMat; //return the absdiff() output "outMat"
    }
}
gpu::GpuMat motionSegCUDA(const cv::gpu::GpuMat& inGpuMat, vector<gpu::GpuMat>& outGpuMatvec, int framecount)
    {
    gpu::GpuMat outGpuMat; //declare storage for return purpose
    if(outGpuMatvec.size() < framecount) //check to make sure the necessarry # of frames are contained in the vector
    {
    outGpuMatvec.push_back(inGpuMat); //push the input GpuMat in to the vector
    outGpuMat = outGpuMatvec.back(); //copy the contents of inGpuMat in to the return "outGpuMat"
    return outGpuMat; //return the original "inGpuMat"
    }
else{
    gpu::GpuMat tmp1, tmp2; //place holders
    tmp1 = outGpuMatvec.back(); //put the oldest frame in tmp1
    outGpuMatvec.pop_back(); //remove the oldest frame from the vector
    outGpuMatvec.push_back(inGpuMat); //put the newest frame("inMat") in to the vector
    tmp2 = outGpuMatvec.back(); //put the second oldest frame in tmp2 TODO: Should this be .front() to make framecount meaningfull?
    gpu::absdiff(tmp1, tmp2, outGpuMat); //compare the absolute per pixel difference between tmp1 and tmp2 and put the output in "outGpuMat"
    return outGpuMat; //return the absdiff() output "outGpuMat"
    }
}
//TODO: make a variation for inputting Mat then converting them to GpuMat with a return type of GpuMat
//TODO: make a variation for inputting Mat then converting them to GpuMat with a return type of Mat
//TODO: make a variation for inputting GpuMat with a return type of Mat
//TODO: convert all of this in to a Class and then overload the inputs so type is detected automatically

gpu::GpuMat BuoyFilter1(const cv::gpu::GpuMat& src)
{
//    assert(src.type() == CV_8UC3);
    gpu::GpuMat channels[3];
    gpu::GpuMat channels2[3];
    gpu::split(src,channels);
    gpu::GpuMat BuoyMatB1, BuoyMatB2, BuoyMatG1, BuoyMatG2, BuoyMatR1, BuoyMatR2, outMat;

    //gpu::threshold(channels[0], BuoyMatB1, 118.0, 138.0, 0);
    gpu::threshold(channels[0], channels2[0], 118.0, 138.0, 0);
    //gpu::threshold(channels[0], BuoyMatB2, 118.0, 1);

    gpu::threshold(channels[1], channels2[1], 118.0, 138.0, 0);
    //gpu::threshold(channels[1], BuoyMatG1, 118.0, 138.0, 0);
    //gpu::threshold(channels[1], BuoyMatG2, 118.0, 1);

    gpu::threshold(channels[2], channels2[2], 118.0, 138.0, 0);
//    gpu::threshold(channels[2], BuoyMatR1, 118.0, 138.0, 0);
    //gpu::threshold(channels[2], BuoyMatR2, 118.0, 1);

    //gpu::bitwise_or(BuoyMatB1, BuoyMatB2, channels2[0]);
    //gpu::bitwise_or(BuoyMatG1, BuoyMatG2, channels2[1]);
    //gpu::bitwise_or(BuoyMatR1, BuoyMatR2, channels2[2]);

    gpu::merge(channels2, 3, outMat);

    return outMat;
}
