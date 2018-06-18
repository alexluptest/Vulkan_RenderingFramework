#include<iostream>

#include "VulkanEngine.h"
 
int main(int argc, char const *argv[])
{
    
    auto &vulkanInstance = VulkanEngine::getInstance();
    vulkanInstance.printVersion();

    return 0;
}