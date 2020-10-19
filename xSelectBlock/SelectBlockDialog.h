#pragma once
#include <Windows.h>
#include <stdint.h>

void ShowSelectBlockDialog(HINSTANCE instance, uint64_t start, uint64_t end);
void HideSelectBlockDialog();