#pragma once

#include <stdint.h>
#include <string>

/**
 * Cortica TAG item, each matched object return tag.
 */
struct Tag {
    uint32_t tagId; /**< Tag id for enumeration */
    std::string Name;
};

/**
 * Cortica raw image item.
 */
struct RGBImage {
    std::string ImageURL;
};

/**
 * Callback for asynchronous method to find matched tags 
 * from RGB raw data.
 */
typedef void (* onMatchingCallback) (
    const Tag * tag /**< The found tag info */
);

/**
 * Local database for Cortica tags.
 */
struct CorticaDB {
    bool isLoaded; /**< Set to TRUE when the DB is loaded from the files */
};

/**
 * CORTICAPI provider types
 */
typedef enum {
    CORTICAPI_PROVIDER_LOCAL = 0,       /**< The engine is local */
    CORTICAPI_PROVIDER_CLOUD = 1        /**< The engine is hosted */
} CORTICAPI_PROVIDER;

/**
 * CORTICAPI return codes.
 */
typedef enum {
    CORTICAPI_SUCCESS = 0,                             /**< Expected response */
    CORTICAPI_ERROR_FEATURE_NOT_IMPLEMENTED = 1,       /**< Feature TODO */
    CORTICAPI_ERROR_FEATURE_NOT_SUPPORTED = 2,         /**< Feature not supported by HW */
    CORTICAPI_ERROR_INVALID_PARAMETER = 3,             /**< Parameter invalid */
    CORTICAPI_ERROR_INVALID_HANDLE = 4,                /**< Handle invalid */
    CORTICAPI_ERROR_NO_RESOURCES = 5,                  /**< No resource of that type avail */
    CORTICAPI_ERROR_INVALID_RESOURCE = 6,              /**< Resource invalid */
    CORTICAPI_ERROR_NO_DATA_AVAILABLE = 7,             /**< No data available */
    CORTICAPI_ERROR_UNSPECIFIED = 99                   /**< Unknown Error */
} CORTICAPI_RESULT;