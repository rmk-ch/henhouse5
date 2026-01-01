#pragma once
#include <ecpp/static_vector.hpp>
#include "errorcode.h"




template<class ARG_TYPE> class Publisher {
    public:
        typedef void (static_fcn)(void*, ARG_TYPE);
        struct callback_entry {
            static_fcn* func_ptr;
            void * instance;
        };


        Publisher(ErrorCode::Instance instance) : m_instance(instance), m_callbacks() {};
        virtual ~Publisher() {};

        const ErrorCode registerCallback(static_fcn* function, void * instance) {
            callback_entry entry;
            entry.func_ptr = function;
            entry.instance = instance;
            if(m_callbacks.size() >= m_callbacks.capacity()) {
                // LOG_ERR("Callback container is full!");
                return ErrorCode(m_instance, ErrorCode::container_full, 1);
            }
            m_callbacks.push_back(entry);
            return ErrorCode(m_instance, ErrorCode::success);
        }

        /**
         * Notify can be called class-internally or in an ISR to notify all subscribers.
         */
        const ErrorCode _notify(ARG_TYPE message) {
            for (auto cb : m_callbacks) {
                // std::invoke(cb.func_ptr, cb.instance, message);
                (*cb.func_ptr)(cb.instance, message);
            }
            return ErrorCode(m_instance, ErrorCode::success);
        }
                
    protected:
        ErrorCode::Instance m_instance;
        ecpp::static_vector<callback_entry, 4> m_callbacks;
        

};