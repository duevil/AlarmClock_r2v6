//
// Created by Malte on 16.09.2023.
//

#ifndef MD_PAROLA_MATRIX_32X8_H
#define MD_PAROLA_MATRIX_32X8_H

#if __cplusplus >= 201103L
#include <Arduino.h>
#include <vector>
#include <functional>
#include <string>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "text_utils.h"
#include "matrix_font.h"
#include "Matrix32x8.hpp"
using md_parola_matrix32x8::Matrix32x8;
#else
#error "This library requires C++11 or higher"
#endif

#endif //MD_PAROLA_MATRIX_32X8_H
