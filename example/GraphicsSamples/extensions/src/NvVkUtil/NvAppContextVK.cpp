//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppContextVK.cpp
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
#include "NvVkUtil/NvPlatformVK.h"
#include "NvVkUtil/NvGPUTimerVK.h"
#include "NvAppContextVK.h"
#include "NV/NvLogs.h"
#include <NvAssert.h>

#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <sstream>
#include <iomanip>

using std::vector;
using std::string;
using std::set;

std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance)
{
    VkResult result;
    std::vector<VkPhysicalDevice> devices;
    
       
    uint32_t gpuCount = 0;

    result = vkEnumeratePhysicalDevices(instance, &gpuCount, 0);
    CHECK_VK_RESULT();

    devices.resize(gpuCount);

    result = vkEnumeratePhysicalDevices(instance, &gpuCount, devices.data());
    CHECK_VK_RESULT();

    return devices;
}

std::vector<VkLayerProperties> GetInstanceLayerProperties(
	PFN_vkEnumerateInstanceLayerProperties _vkEnumerateInstanceLayerProperties)
{
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	uint32_t count = 0;
	result = _vkEnumerateInstanceLayerProperties(&count, NULL);
	CHECK_VK_RESULT();

	std::vector<VkLayerProperties> layers(count);

	result = _vkEnumerateInstanceLayerProperties(&count, layers.data());
	CHECK_VK_RESULT();
	return layers;
}


std::vector<VkExtensionProperties> GetInstanceExtensionProperties(
	PFN_vkEnumerateInstanceExtensionProperties _vkEnumerateInstanceExtensionProperties)
{
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	uint32_t count = 0;
	result = _vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
	CHECK_VK_RESULT();

	std::vector<VkExtensionProperties> extensions(count);

	result = _vkEnumerateInstanceExtensionProperties(NULL, &count, extensions.data());
	CHECK_VK_RESULT();
	return extensions;
}

std::vector<VkExtensionProperties> GetPhysicalDeviceExtensionProperties(VkPhysicalDevice _physicalDevice)
{
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	uint32_t count = 0;
	result = vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &count, NULL);
	CHECK_VK_RESULT();

	std::vector<VkExtensionProperties> extensions(count);

	result = vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &count, extensions.data());
	CHECK_VK_RESULT();
	return extensions;
}

std::vector<VkLayerProperties> GetPhysicalDeviceLayerProperties(VkPhysicalDevice _physicalDevice)
{
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	uint32_t count = 0;
	result = vkEnumerateDeviceLayerProperties(_physicalDevice, &count, NULL);
	CHECK_VK_RESULT();

	std::vector<VkLayerProperties> layers(count);

	result = vkEnumerateDeviceLayerProperties(_physicalDevice, &count, layers.data());
	CHECK_VK_RESULT();
	return layers;
}


std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(VkPhysicalDevice _physicalDevice)
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
        uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, NULL);

    queueFamilies.resize(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());
    return queueFamilies;
}

uint32_t FindQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& queuefamilies, VkQueueFlags  flags)
{
    for (uint32_t familyIndex = 0; familyIndex < queuefamilies.size(); ++familyIndex)
    {
        if (queuefamilies[familyIndex].queueFlags & (flags))
            return familyIndex;
    }
    return ~0u;
}

void logLayersAndExtensions(const char* type, const vector<VkExtensionProperties> &extentionsProperties, const vector<string> &extensionsToEnable, const vector<VkLayerProperties> &layerProperties, const vector<string> &layersToEnable, const char* layerEnvVar)
{
    LOGI("supported %s extensions:", type);
    for (size_t i = 0; i < extentionsProperties.size(); ++i)
    {
        LOGI("\t[%u] '%s' specVersion '%u'", i, extentionsProperties[i].extensionName, extentionsProperties[i].specVersion);
    }

    LOGI("enabled %s extensions:", type);
    for (size_t i = 0; i < extensionsToEnable.size(); ++i)
    {
        LOGI("\t[%u] '%s'", i, extensionsToEnable[i].c_str());
    }

    LOGI("supported %s layers:", type);
    for (size_t i = 0; i < layerProperties.size(); ++i)
    {
        LOGI("\t[%u] '%s' specVersion '%u' '%s' implementationVersion '%u'", i, layerProperties[i].layerName, layerProperties[i].specVersion, layerProperties[i].description, layerProperties[i].implementationVersion);
    }
    
    LOGI("enabled %s layers:", type);
    for (size_t i = 0; i < layersToEnable.size(); ++i)
    {
        LOGI("\t[%u] '%s'", i, layersToEnable[i].c_str());
    }

    char* layers = getenv(layerEnvVar);
    LOGI("implicit %s layers: '%s' = '%s'", type, layerEnvVar, layers ? layers : "not set");

}

std::vector<std::string> requestedLayers(const std::vector<std::string>& additionalLayers, bool useValidation, bool useApiDump)
{
    vector<string> layers (additionalLayers);
    if (useApiDump)
    {
        layers.push_back("VK_LAYER_LUNARG_api_dump");
    }

    if (useValidation)
    {
        // the standard meta layer documented here looks like this
        //https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/tree/master/layers#standard-validation
        
#if !SOURCE_SHADERS
        layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

        layers.push_back("VK_LAYER_GOOGLE_threading");
		
		layers.push_back("VK_LAYER_LUNARG_parameter_validation");
		layers.push_back("VK_LAYER_LUNARG_object_tracker");
		layers.push_back("VK_LAYER_LUNARG_image");

        layers.push_back("VK_LAYER_LUNARG_image");
        layers.push_back("VK_LAYER_LUNARG_core_validation");    // errors out in swapchain resize code, enable after this is fixed

        layers.push_back("VK_LAYER_LUNARG_swapchain");

#if !SOURCE_SHADERS
        layers.push_back("VK_LAYER_LUNARG_core_validation"); // should be anbled once we use SPIR-V
#endif
        layers.push_back("VK_LAYER_GOOGLE_unique_objects");
    }
    return layers;
}


std::vector<std::string> requestedExtensions(const std::vector<std::string>& additionalExtensions, bool useDebugReport)
{
	vector<string> extensions(additionalExtensions);
	
	extensions.push_back("VK_KHR_surface");
	extensions.push_back("VK_KHR_android_surface");
	extensions.push_back("VK_KHR_win32_surface");
	extensions.push_back("VK_KHR_swapchain");
	extensions.push_back("VK_KHR_xcb_surface");
	extensions.push_back("VK_KHR_xlib_surface");

#if defined(VK_EXT_debug_report)
    if (useDebugReport)
    {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

#if defined (VK_EXT_debug_marker)
	// always enable this since it's only supported if a tool exposes it
	extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
#endif

	return extensions;
}

std::vector<std::string> filterLayers(const std::vector<VkLayerProperties>& supportedLayers, const std::vector<std::string>& layersToEnable)
{
	vector<string> supportedLayerNames;
    vector<string> unsupportedLayerNames;

	for (size_t i = 0; i < layersToEnable.size(); ++i)
	{
		for (size_t j = 0; j < supportedLayers.size(); ++j)
		{
			if (layersToEnable[i] == supportedLayers[j].layerName)
			{
				supportedLayerNames.push_back(layersToEnable[i]);
				break;
			}
		}
        
	}
	return supportedLayerNames;
}

std::vector<std::string> filterExtensions(const std::vector<VkExtensionProperties>& supportedExtensions, const std::vector<std::string>& extensionsToEnable)
{
	vector<string> supportedExtensionNames;

	for (size_t i = 0; i < extensionsToEnable.size(); ++i)
	{
		for (size_t j = 0; j < supportedExtensions.size(); ++j)
		{
			if (extensionsToEnable[i] == supportedExtensions[j].extensionName)
			{
				supportedExtensionNames.push_back(extensionsToEnable[i]);
				break;
			}
		}
	}
	return supportedExtensionNames;
}


#if VK_EXT_debug_report

// those could be correlated with the per layer codes.
// e.g. https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/blob/sdk-1.0.3/layers/draw_state.h#L63
std::string messageCodeString(int32_t messageCode, const char* layerPrefix)
{
	std::stringstream str;
	str << messageCode;
	return str.str();
}

// this typically the source __LINE__  in the validation layer which triggered this text
// e.g. https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/blob/sdk-1.0.3/layers/draw_state.cpp#L5247
std::string locationString(size_t location, const char* layerPrefix)
{
	return "";
	std::stringstream str;
	str << location;
	return str.str();
}

const char* objectTypeString(VkDebugReportObjectTypeEXT objectType)
{
	
	switch (objectType)
	{
	case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:				return "unknown";
	case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:				return "instance";
	case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:		return "physical_device";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:				return "device";
	case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:					return "queue";
	case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:				return "semaphore";
	case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:		return "command_buffer";
	case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:					return "fence";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:			return "device_memory";
	case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:				return "buffer";
	case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:					return "image";
	case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:					return "event";
	case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:			return "query_pool";
	case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:			return "buffer_view";
	case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:			return "image_view";
	case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:			return "shader_module";
	case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:		return "pipeline_cache";
	case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:		return "pipeline_layout";
	case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:			return "render_pass";
	case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:				return "pipeline";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT: return "descriptor_set_layout";
	case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:				return "sampler";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:		return "descriptor_pool";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:		return "descriptor_set";
	case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:			return "framebuffer";
	case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:			return "command_pool";
	case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:			return "surface";
	case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:			return "swapchain";
	case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:			return "debug_report";
	default: return "extend objectTypeString";
	}
	
}

VkBool32 VKAPI_CALL vulkanDebugReportCallback(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage,
    void*                                 pUserData
)
{
	const bool showExtendedData = false;
	const bool showObjectData = true;
	
	std::stringstream prettyMessage;

	bool isUnknown = false;
	bool isError = false;
    bool isWarning = false;
	bool isPerf = false;
	bool isInfo = false;
	bool isDebug = false;

	prettyMessage << "[";
	{
		prettyMessage << std::setw(6);
		if (flags == 0)
		{
			prettyMessage << "UNKNOWN";
			isUnknown = true;
		}
		else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			prettyMessage << "INFO";
			isInfo = true;
		}
		else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			prettyMessage << "WARN";
			isWarning = true;
		}
		else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			prettyMessage << "PERF";
			isPerf = true;
		}
		else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			prettyMessage << "ERROR";
			isError = true;
		}
		else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			prettyMessage << "DEBUG";
			isDebug = true;
		}
	
		prettyMessage << "@" << pLayerPrefix << "]";
	}

	if (showExtendedData)
	{
		prettyMessage << "<" << messageCodeString(messageCode, pLayerPrefix) << ">";
	}
	if (showObjectData)
	{
		prettyMessage << std::setw(15 + 1);
		prettyMessage << objectTypeString(objectType) << "=" << (void*)object  ;
	}

	if (showExtendedData)
	{
		prettyMessage << "<" << locationString(location, pLayerPrefix) << ">";
	}
	
	{
		prettyMessage << ": " << pMessage;
	}

	// separate to be able to set a break point per category after the message has been pushed to debug out
	if (isError)
	{
		LOGE(prettyMessage.str().c_str());
		return false;
	}
    else if (isWarning)
	{
		LOGI(prettyMessage.str().c_str());
        return false;
	}
	else if (isPerf)
	{
		LOGE(prettyMessage.str().c_str());
		return false;
	}
	else if (isDebug) 
	{
		LOGI(prettyMessage.str().c_str());
		return false;
	}
	else if (isInfo) // separate to be able to set a break point
	{
		LOGI(prettyMessage.str().c_str());
		return false;
	}
	else if (isUnknown)
    {
		LOGI(prettyMessage.str().c_str());
        return false;
    }
	else
	{
		LOGI(prettyMessage.str().c_str());
		return false;
	}


    
}
#endif

bool NvAppContextVK::initializeInstance(const std::string& appTitle) {
	VkResult result;

	PFN_vkEnumerateInstanceExtensionProperties _vkEnumerateInstanceExtensionProperties = NULL;
	PFN_vkEnumerateInstanceLayerProperties _vkEnumerateInstanceLayerProperties = NULL;
	PFN_vkCreateInstance _vkCreateInstance = NULL;

    _vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    _vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    _vkCreateInstance = vkCreateInstance;

	VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	applicationInfo.pApplicationName = appTitle.c_str();
	applicationInfo.applicationVersion = 1;
	applicationInfo.pEngineName = "GameWorks SDK Sample";
	applicationInfo.engineVersion = 1;
#ifdef VK_API_VERSION
	applicationInfo.apiVersion = VK_API_VERSION;
#else
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
#endif
    
	// instance layers and extensions
	mInstanceLayerProperties = GetInstanceLayerProperties(_vkEnumerateInstanceLayerProperties);
	mInstanceExtensionsProperties = GetInstanceExtensionProperties(_vkEnumerateInstanceExtensionProperties);

	for (size_t i = 0; i < mInstanceExtensionsProperties.size(); ++i) {
		mCombinedExtensionNames.insert(mInstanceExtensionsProperties[i].extensionName);
	}
	for (size_t i = 0; i < mInstanceLayerProperties.size(); ++i) {
		mCombinedLayerNames.insert(mInstanceLayerProperties[i].layerName);
	}

	const vector<string> instanceLayersToEnable = filterLayers(mInstanceLayerProperties, requestedLayers(configuration().layersToEnable, useValidation(), useApiDump()));

	vector<const char*> instanceLayerNames;
	for (size_t i = 0; i < instanceLayersToEnable.size(); ++i)
		instanceLayerNames.push_back(instanceLayersToEnable[i].c_str());

	const vector<string> instanceExtensionsToEnable = filterExtensions(mInstanceExtensionsProperties, requestedExtensions(configuration().extensionsToEnable, useValidation() || useLoaderDebug()));

	vector<const char*> instanceExtensionNames;
	for (size_t i = 0; i < instanceExtensionsToEnable.size(); ++i)
		instanceExtensionNames.push_back(instanceExtensionsToEnable[i].c_str());


	VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceCreateInfo.pApplicationInfo = &applicationInfo;

	instanceCreateInfo.enabledExtensionCount = instanceExtensionNames.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

	instanceCreateInfo.enabledLayerCount = instanceLayerNames.size();
	instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames.data();


	logLayersAndExtensions("instance", mInstanceExtensionsProperties, instanceExtensionsToEnable, mInstanceLayerProperties, instanceLayersToEnable, "VK_INSTANCE_LAYERS");
	result = _vkCreateInstance(&instanceCreateInfo, NULL, &_instance);
	if (result != VK_SUCCESS)
		return false;

#if VK_EXT_debug_report

	if (std::find(instanceExtensionsToEnable.begin(), instanceExtensionsToEnable.end(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) != instanceExtensionsToEnable.end())
	{
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pNext = NULL;
		dbgCreateInfo.pfnCallback = vulkanDebugReportCallback;
		dbgCreateInfo.pUserData = NULL;
		dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;// | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		ext_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
		ext_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
		if (!ext_vkCreateDebugReportCallbackEXT) {
			LOGE("GetProcAddr: Unable to find vkDbgCreateMsgCallback\n");
		}
		if (!ext_vkDestroyDebugReportCallbackEXT) {
			LOGE("GetProcAddr: Unable to find vkDbgDestroyMsgCallback\n");
		}
		result = ext_vkCreateDebugReportCallbackEXT(
			_instance,
			&dbgCreateInfo,
			NULL,
			&msg_callback
			);
		switch (result)
		{
		case VK_SUCCESS:
			LOGI("dbgCreateMsgCallback: registered\n");
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			LOGE("dbgCreateMsgCallback: out of host memory\n");
			break;
		default:
			LOGE("dbgCreateMsgCallback: unknown failure\n");
			break;
		}
	}
#endif

	return true;
}

bool NvAppContextVK::initializeDevice()
{
	VkResult result;

	uint32_t gpuCount = 1;

    std::vector<VkPhysicalDevice> physicalDevices = GetPhysicalDevices(_instance);

    if (physicalDevices.empty())
        return false;


    _physicalDevice = VK_NULL_HANDLE;
    for (size_t i = 0; i < physicalDevices.size(); ++i)
    {
        const std::vector<VkQueueFamilyProperties> queuefamilies = GetQueueFamilyProperties(physicalDevices[i]);
        const uint32_t universalQueueIndex = FindQueueFamilyIndex(queuefamilies, VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT);
        if (universalQueueIndex < queuefamilies.size())
        {
            _physicalDevice = physicalDevices[i];
            _queueFamilyIndex = universalQueueIndex;
            _queueIndex = 0;
            break;
        }
    }

    if (!_physicalDevice)
        return false;


	VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueCreateInfo.queueFamilyIndex = _queueFamilyIndex;
	queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    
	// physical device layers and extensions
	mPhysicalDeviceLayerProperties = GetPhysicalDeviceLayerProperties(_physicalDevice);
	mPhysicalDeviceExtensionsProperties = GetPhysicalDeviceExtensionProperties(_physicalDevice);

	for (size_t i = 0; i < mPhysicalDeviceExtensionsProperties.size(); ++i) {
		mCombinedExtensionNames.insert(mPhysicalDeviceExtensionsProperties[i].extensionName);
	}
	for (size_t i = 0; i < mPhysicalDeviceLayerProperties.size(); ++i) {
		mCombinedLayerNames.insert(mPhysicalDeviceLayerProperties[i].layerName);
	}

	const vector<string> physicalDeviceLayersToEnable = filterLayers(mPhysicalDeviceLayerProperties, requestedLayers(configuration().layersToEnable, useValidation(), useApiDump()));

	vector<const char*> physicalDeviceLayerNames;
	for (size_t i = 0; i < physicalDeviceLayersToEnable.size(); ++i)
		physicalDeviceLayerNames.push_back(physicalDeviceLayersToEnable[i].c_str());

	const vector<string> physicalDeviceExtensionsToEnable = filterExtensions(mPhysicalDeviceExtensionsProperties, requestedExtensions(configuration().extensionsToEnable, useValidation() || useLoaderDebug()));

	vector<const char*> physicalDeviceExtensionNames;
	for (size_t i = 0; i < physicalDeviceExtensionsToEnable.size(); ++i)
		physicalDeviceExtensionNames.push_back(physicalDeviceExtensionsToEnable[i].c_str());

	deviceCreateInfo.enabledExtensionCount = physicalDeviceExtensionNames.size();
	deviceCreateInfo.ppEnabledExtensionNames = physicalDeviceExtensionNames.data();

	deviceCreateInfo.enabledLayerCount = physicalDeviceLayerNames.size();
    deviceCreateInfo.ppEnabledLayerNames = physicalDeviceLayerNames.data();

    // device features filtering: turn off features that are enabled by the configuration callback but not supported by the device
    {
		_physicalDeviceFeaturesEnabled = configuration().featuresToEnable;

        vkGetPhysicalDeviceFeatures(_physicalDevice, &_physicalDeviceFeatures);

		const VkBool32* firstSupportedFeature = &_physicalDeviceFeatures.robustBufferAccess;
		const VkBool32* lastSupportedFeature = &_physicalDeviceFeatures.inheritedQueries;
		const VkBool32* supportedFeature = firstSupportedFeature;

		VkBool32* firstEnabledFeature = &_physicalDeviceFeaturesEnabled.robustBufferAccess;
		VkBool32* lastEnabledFeature = &_physicalDeviceFeaturesEnabled.inheritedQueries;
		VkBool32* enabledFeature = firstEnabledFeature;

		do
		{
			if ( (*enabledFeature  == VK_TRUE) && (*supportedFeature == VK_FALSE) )
			{
				*enabledFeature = VK_FALSE;
				LOGI("Disabling vkPhysicalDeviceFeatures::feature #%u since it was enabled by the configurationCallback, but is not supported by the device.", enabledFeature - firstEnabledFeature);
			}

			++enabledFeature;
			++supportedFeature;
		} while (enabledFeature != lastEnabledFeature);

        deviceCreateInfo.pEnabledFeatures = &_physicalDeviceFeaturesEnabled;
    }

    logLayersAndExtensions("device", mPhysicalDeviceExtensionsProperties, physicalDeviceExtensionsToEnable, mPhysicalDeviceLayerProperties, physicalDeviceLayersToEnable, "VK_DEVICE_LAYERS");

	result = vkCreateDevice(_physicalDevice, &deviceCreateInfo, NULL, &_device);
	if (result != VK_SUCCESS)
		return false;

    vkGetDeviceQueue(_device, _queueFamilyIndex, _queueIndex, &_queue);

	vkGetPhysicalDeviceProperties(_physicalDevice, &_physicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_physicalDeviceMemoryProperties);

    _physicalDeviceLimits = _physicalDeviceProperties.limits;

    {
        LOGI("Vulkan GPU '%s': 'apiversion '%u' driver version '%u'\n", _physicalDeviceProperties.deviceName, _physicalDeviceProperties.apiVersion, _physicalDeviceProperties.driverVersion);
    }

#if defined (VK_EXT_debug_marker)

	mSupportsDebugMarkers = isExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	if (mSupportsDebugMarkers)
	{
		ext_vkDebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT)vkGetDeviceProcAddr(_device, "vkDebugMarkerSetObjectTagEXT");
		ext_vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(_device, "vkDebugMarkerSetObjectNameEXT");

		ext_vkCmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(_device, "vkCmdDebugMarkerBeginEXT");
		ext_vkCmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(_device, "vkCmdDebugMarkerEndEXT");
		ext_vkCmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(_device, "vkCmdDebugMarkerInsertEXT");
	}

#endif

	// temp cmd pool
	{
		VkCommandPoolCreateInfo cmdPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolInfo.queueFamilyIndex = _queueFamilyIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		result = vkCreateCommandPool(_device, &cmdPoolInfo, NULL, &mTempCmdPool);
		if (result != VK_SUCCESS)
			return false;
	}

	mStaging.init(this);

	// fences
	for (int i = 0; i < MAX_BUFFERED_FRAMES; i++) {
		mFrames[i].mFence = VK_NULL_HANDLE;

		mFrames[i].mCmdBuffers = createCmdBuffer(mTempCmdPool, true);
		mFrames[i].mOpen = false;
	}

	mCurrFrameIndex = 0;

	m_frameTimer = new NvGPUTimerVK;
	m_frameTimer->init(*this);

	return true;
}
