//
// Created by root on 12/11/20.
//

#ifndef ORB_SLAM2_ORB_PCH
#define ORB_SLAM2_ORB_PCH


#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <chrono>
#include <limits>
#include <memory>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include<opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <Eigen/Dense>

#include "Thirdparty/g2o/g2o/types/types_six_dof_expmap.h"
#include "Thirdparty/g2o/g2o/types/types_seven_dof_expmap.h"
#include "Thirdparty/DBoW2/DUtils/Random.h"
#include "Thirdparty/g2o/g2o/core/block_solver.h"
#include "Thirdparty/g2o/g2o/core/optimization_algorithm_levenberg.h"
#include "Thirdparty/g2o/g2o/solvers/linear_solver_eigen.h"
#include "Thirdparty/g2o/g2o/core/robust_kernel_impl.h"
#include "Thirdparty/g2o/g2o/solvers/linear_solver_dense.h"

#include "Thirdparty/DBoW2/DBoW2/FORB.h"
#include "Thirdparty/DBoW2/DBoW2/TemplatedVocabulary.h"
#include "Thirdparty/DBoW2/DBoW2/BowVector.h"
#include "Thirdparty/DBoW2/DBoW2/FeatureVector.h"


#include <pangolin/pangolin.h>
#include <unistd.h>


#endif //ORB_SLAM2_ORB_PCH
