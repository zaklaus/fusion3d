/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
 * Copyright (C) 2014 Benny Bobaganoosh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __3DENGINE_H_INCLUDED
#define __3DENGINE_H_INCLUDED

#if 0
#pragma warning( disable : 4305)
#pragma warning( disable : 4996)
#pragma warning( disable : 4006)
#pragma warning( disable : 4258)
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#elif __cplusplus >= 201103L
#define OS_OTHER_CPP11
#else
#define OS_OTHER
#endif

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

#define RUNTIME_TESTS
#define ENGINE_VERSION 1

#define FOREVER for (;;)
#define FOR(p, q, z) for (size_t z = p; z < q; ++z)
#define DFOR(p, q, z) for (size_t z = q; z > p; --z)
#define PRINT(p) std::cout << p << std::endl
#define STRING(s) #s
#define SCALL(x, c) if (x != nullptr) c

#include "btBulletDynamicsCommon.h"
#include "core/terrain.h"
#include "rendering/bitmap.h"
#include "core/level.h"
#include "core/util.h"
#include "rendering/mesh.h"
#include "rendering/shader.h"
#include "core/transform.h"
#include "rendering/camera.h"
#include "rendering/lighting.h"
#include "core/entity.h"
#include "components/meshRenderer.h"
#include "components/programComponent.h"
#include "components/devMode.h"
#include "components/freeLook.h"
#include "components/freeMove.h"
#include "components/rigidBody.h"
#include "rendering/window.h"
#include "core/mapLoader.h"
#include "core/coreEngine.h"
#include "core/game.h"

#include "staticLibs\imgui.h"
#include "staticLibs\imgui_impl_sdl_gl3.h"

//SDL2 defines a main macro, which can prevent certain compilers from finding the main function.
#undef main

#endif // 3DENGINE_H_INCLUDED
