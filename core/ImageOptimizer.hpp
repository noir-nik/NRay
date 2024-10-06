#pragma once
namespace ImageOptimizer {

void CreateShaders();
void CreateImages(uint32_t width, uint32_t height);
void Destroy();

void forwardPass();

}