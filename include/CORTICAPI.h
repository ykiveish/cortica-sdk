/*
* Author: Kiveisha Yevgeniy
* Copyright (c) 2015 Intel Corporation.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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