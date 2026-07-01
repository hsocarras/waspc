
#include "objects/global.h"

/**
 * @brief Default constructor for WpGlobalInstance objects
 * 
 * @param self Pointer to the WpGlobalInstance object to initialize
 */
void WpGlobalInstanceInit(WpGlobalInstance *self){

    self->wp_type = WP_OBJECT_GLOBAL_INSTANCE;

    self->imported = 0;
    self->address = NULL;

    self->mut =  0;
    self->type = WAS_EX_VAL_TYPE_NULL;
    self->val = (StackValue){WAS_EX_VAL_TYPE_NULL, 0};

    self->next = NULL;
}

/**
 * @brief Get the value of a global instance
 * 
 * If the global is imported, returns the value from the global instance pointed by address.
 * Otherwise, returns its own value.
 * 
 * @param self Pointer to the WpGlobalInstance object
 * @return StackValue The value of the global
 */
StackValue WpGlobalInstanceGetValue(WpGlobalInstance *self) {
    if (self == NULL) {
        return (StackValue){WAS_EX_VAL_TYPE_NULL, 0};
    }

    // If this global is imported, return the value from the address pointer
    if (self->imported) {
        if (self->address != NULL) {
            return self->address->val;
        }
        // Fallback if address is not set (shouldn't happen in valid code)
        return (StackValue){WAS_EX_VAL_TYPE_NULL, 0};
    }

    // Otherwise return the own value
    return self->val;
}

/**
 * @brief Set the value of a global instance
 * 
 * Checks if the global is mutable before setting the value.
 * If the global is imported, sets the value at the global instance pointed by address.
 * Otherwise, sets its own value.
 * 
 * @param self Pointer to the WpGlobalInstance object
 * @param value The new value to set
 * @return int 1 if the value was set successfully, 0 if the global is immutable
 */
int WpGlobalInstanceSetValue(WpGlobalInstance *self, StackValue value) {

    if (self == NULL) {
        return 0;
    }
    
    // If this global is imported, set the value at the address pointer
    if (self->imported) {
        
        if (self->address != NULL) {
            // Check if the global is mutable
            if (!self->address->mut) {
                return 0;  // Cannot write to immutable global
            }
            self->address->val = value;
            return 1;
        }
        // Fallback if address is not set
        return 0;
    }

    // Check if the global is mutable
    if (!self->mut) {
        return 0;  // Cannot write to immutable global
    }

    // Otherwise set the own value
    self->val = value;
    return 1;
}

