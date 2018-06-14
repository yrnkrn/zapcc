#pragma once
extern "C" {
void bb();
void putchar() { bb(); }
}
