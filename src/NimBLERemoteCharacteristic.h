/*
 * NimBLERemoteCharacteristic.h
 *
 *  Created: on Jan 27 2020
 *      Author H2zero
 *
 * Originally:
 *
 * BLERemoteCharacteristic.h
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_NIMBLEREMOTECHARACTERISTIC_H_
#define COMPONENTS_NIMBLEREMOTECHARACTERISTIC_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined( CONFIG_BT_NIMBLE_ROLE_CENTRAL)

#include "NimBLERemoteService.h"
#include "NimBLERemoteDescriptor.h"

#include <vector>

class NimBLERemoteService;
class NimBLERemoteDescriptor;


typedef void (*notify_callback)(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                                uint8_t* pData, size_t length, bool isNotify);

/**
 * @brief A model of a remote %BLE characteristic.
 */
class NimBLERemoteCharacteristic {
public:
    ~NimBLERemoteCharacteristic();

    // Public member functions
    bool                                           canBroadcast();
    bool                                           canIndicate();
    bool                                           canNotify();
    bool                                           canRead();
    bool                                           canWrite();
    bool                                           canWriteNoResponse();
    std::vector<NimBLERemoteDescriptor*>::iterator begin();
    std::vector<NimBLERemoteDescriptor*>::iterator end();
    NimBLERemoteDescriptor*                        getDescriptor(const NimBLEUUID &uuid);
    std::vector<NimBLERemoteDescriptor*>*          getDescriptors(bool refresh = false);
    void                                           deleteDescriptors();
    size_t                                         deleteDescriptor(const NimBLEUUID &uuid);
    uint16_t                                       getHandle();
    uint16_t                                       getDefHandle();
    NimBLEUUID                                     getUUID();
    std::string                                    readValue(time_t *timestamp = nullptr);

    template<typename T>
    T                                              readValue(time_t *timestamp = nullptr, bool skipSizeCheck = false) {
        std::string value = readValue(timestamp);
        if(!skipSizeCheck && value.size() < sizeof(T)) return T();
        const char *pData = value.data();
        return *((T *)pData);
    }

    uint8_t                                        readUInt8()  __attribute__ ((deprecated));
    uint16_t                                       readUInt16() __attribute__ ((deprecated));
    uint32_t                                       readUInt32() __attribute__ ((deprecated));
    std::string                                    getValue(time_t *timestamp = nullptr);

    template<typename T>
    T                                              getValue(time_t *timestamp = nullptr, bool skipSizeCheck = false) {
        std::string value = getValue(timestamp);
        if(!skipSizeCheck && value.size() < sizeof(T)) return T();
        const char *pData = value.data();
        return *((T *)pData);
    }

    bool                                           subscribe(bool notifications = true,
                                                             bool response = true,
                                                             notify_callback notifyCallback = nullptr);
    bool                                           unsubscribe(bool response = true);
    bool                                           registerForNotify(notify_callback notifyCallback,
                                                                     bool notifications = true,
                                                                     bool response = true) __attribute__ ((deprecated));
    bool                                           writeValue(const uint8_t* data,
                                                              size_t length,
                                                              bool response = false);
    bool                                           writeValue(const std::string &newValue,
                                                              bool response = false);
    bool                                           writeValue(uint8_t newValue,
                                                              bool response = false);
    std::string                                    toString();
    NimBLERemoteService*                           getRemoteService();

private:

    NimBLERemoteCharacteristic(NimBLERemoteService *pRemoteservice, const struct ble_gatt_chr *chr);

    friend class      NimBLEClient;
    friend class      NimBLERemoteService;
    friend class      NimBLERemoteDescriptor;

    // Private member functions
    bool              setNotify(uint16_t val, bool response = true, notify_callback notifyCallback = nullptr);
    bool              retrieveDescriptors(const NimBLEUUID *uuid_filter = nullptr);
    static int        onReadCB(uint16_t conn_handle, const struct ble_gatt_error *error,
                               struct ble_gatt_attr *attr, void *arg);
    static int        onWriteCB(uint16_t conn_handle, const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr, void *arg);
    void              releaseSemaphores();
    static int        descriptorDiscCB(uint16_t conn_handle, const struct ble_gatt_error *error,
                                       uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                                       void *arg);

    // Private properties
    NimBLEUUID              m_uuid;
    uint8_t                 m_charProp;
    uint16_t                m_handle;
    uint16_t                m_defHandle;
    NimBLERemoteService*    m_pRemoteService;
    FreeRTOS::Semaphore     m_semaphoreGetDescEvt       = FreeRTOS::Semaphore("GetDescEvt");
    FreeRTOS::Semaphore     m_semaphoreReadCharEvt      = FreeRTOS::Semaphore("ReadCharEvt");
    FreeRTOS::Semaphore     m_semaphoreWriteCharEvt     = FreeRTOS::Semaphore("WriteCharEvt");
    std::string             m_value;
    notify_callback         m_notifyCallback;
    time_t                  m_timestamp;

    // We maintain a vector of descriptors owned by this characteristic.
    std::vector<NimBLERemoteDescriptor*> m_descriptorVector;
}; // NimBLERemoteCharacteristic

#endif // #if defined( CONFIG_BT_NIMBLE_ROLE_CENTRAL)
#endif /* CONFIG_BT_ENABLED */
#endif /* COMPONENTS_NIMBLEREMOTECHARACTERISTIC_H_ */
