#ifndef GLKERNEL_H
#define GLKERNEL_H

#include<vector>

inline std::vector<float> sharpenKernel =
    {
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
    };

inline std::vector<float> blurKernel =
    {
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
    };

inline std::vector<float> edgeKernel =
    {
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
    };

#endif //GLKERNEL_H
