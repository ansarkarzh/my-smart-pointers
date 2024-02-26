#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

template <typename T>
struct ControlBlockOwning;

template <typename T>
struct ControlBlockPointer;

struct ControlBlockBase;

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
