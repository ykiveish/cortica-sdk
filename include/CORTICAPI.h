#pragma once

#include <CORTICAPITypes.h>
#include <vector>
#pragma once

#include <stdint.h>
#include <cstddef>

#define CORTICAPI_EXPORT __attribute__((visibility ("default")))
#define CORTICAPI_LOCAL __attribute__((visibility("hidden")))

/**
* @brief API to Cortica tagging mechanism.
*
* This class define all supported methods in Cortica API.
*/
class CORTICAPI {
    public:
        /**
         * Load the user matching database into private CorticaDB.
         *
         * @return Result of operation.
         */
        virtual CORTICAPI_RESULT initializeDB () = 0;
        
        /**
         * Looking for matching tags in the database.
         * This method is blocking.
         *
         * @param * data pointer to RGB image.
         * @return Found tags list.
         */
        virtual std::vector<Tag> matchingSync (RGBImage * data) = 0;

        /**
         * Looking for matching tags in the database.
         * This method is not blocking but before calling this method
         * setMatchingCallback must be called.
         *
         * @param * data pointer to RGB image.
         * @return Result of operation
         */
        virtual CORTICAPI_RESULT matchingAsync (RGBImage * data) = 0;

        /**
         * Set callback on finishing the matching progress.
         *
         * @param onMatchingCallback callback to call when the matching occur.
         * @return Result of operation
         */
        virtual CORTICAPI_RESULT setMatchingCallback (onMatchingCallback) = 0;
        
    private:
        CorticaDB database; /**< Local TAGs database. */
        
    protected:
        CORTICAPI () {};
        CORTICAPI (const CORTICAPI & other) = delete;
        CORTICAPI (CORTICAPI && other) = delete;
        CORTICAPI & operator = (const CORTICAPI & other) = delete;
        CORTICAPI & operator = (CORTICAPI && other) = delete;
        virtual ~CORTICAPI () {};
};