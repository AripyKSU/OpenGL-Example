#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional> // 어떤 값의 유무를 포인터(동적 할당) 없이 처리 가능 - 메모리 관리에 용이
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

std::optional<std::string> LoadTextFile(const std::string& filename);

#endif // __COMMON_H__