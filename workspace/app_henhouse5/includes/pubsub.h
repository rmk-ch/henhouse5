#pragma once
#include <functional>
#include <ecpp/static_vector.hpp>
#include "errorcode.h"

template<int MAX_N_SUBSCRIBERS, class ARG_TYPE> class Publisher {
    /**
     * Publisher class. 
     * Use lambdas to register callbacks, according to https://blog.mbedded.ninja/programming/languages/c-plus-plus/callbacks/#stdfunction-with-lambdas
     */
    public:
        Publisher(ErrorCode::Instance instance) : m_instance(instance), m_callbacks() {};
        virtual ~Publisher() {};

        const ErrorCode registerCallback(std::function<void(ARG_TYPE)> callback) {
            if(m_callbacks.size() >= m_callbacks.capacity()) {
                // LOG_ERR("Callback container is full!");
                return ErrorCode(m_instance, ErrorCode::container_full, 1);
            }
            m_callbacks.push_back(callback);
            return ErrorCode(m_instance, ErrorCode::success);
        }

        /**
         * Notify can be called class-internally or in an ISR to notify all subscribers.
         */
        const ErrorCode _notify(ARG_TYPE message) {
            for (auto callback : m_callbacks) {
                callback(message);
            }
            return ErrorCode(m_instance, ErrorCode::success);
        }
                
    protected:
        ErrorCode::Instance m_instance;
        ecpp::static_vector<std::function<void(ARG_TYPE)>, MAX_N_SUBSCRIBERS> m_callbacks;
        

};