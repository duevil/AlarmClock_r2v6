//
// Created by Malte on 16.09.2023.
//

#ifndef MD_PAROLA_MATRIX_64X16_H
#define MD_PAROLA_MATRIX_64X16_H

#if __cplusplus >= 201103L
#include <Arduino.h>
#include <vector>
#include <functional>
#include <string>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "text_utils.h"
#include "matrix_font.h"
#include "Matrix64x16.hpp"
using md_parola_matrix64x16::Matrix64x16;
#else
#error "This library requires C++11 or higher"
#endif

#endif //MD_PAROLA_MATRIX_64X16_H
