#include "mbed.h"
#include "XBeeLib.h"
#if defined(ENABLE_LOGGING)
#include "DigiLoggerMbedSerial.h"
using namespace DigiLog;
#endif

using namespace XBeeLib;

Serial *log_serial;

/** Callback function, invoked at packet reception */
static void receive_cb(const RemoteXBeeZB &remote, bool broadcast, const uint8_t *const data, uint16_t len)
{
    const uint64_t remote_addr64 = remote.get_addr64();

    log_serial->printf("\r\nGot a %s RX packet [%08x:%08x|%04x], len %d\r\nData: ", broadcast ? "BROADCAST" : "UNICAST", UINT64_HI32(remote_addr64), UINT64_LO32(remote_addr64), remote.get_addr16(), len);

    for (int i = 0; i < len; i++)
        log_serial->printf("%02x ", data[i]);

    log_serial->printf("\r\n");
}

int main()
{
    log_serial = new Serial(DEBUG_TX, DEBUG_RX);
    log_serial->baud(115200);
    log_serial->printf("Sample application to demo how to receive unicast and broadcast data with the XBeeZB\r\n\r\n");
    log_serial->printf(XB_LIB_BANNER);

#if defined(ENABLE_LOGGING)
    new DigiLoggerMbedSerial(log_serial, LogLevelInfo);
#endif

    XBeeZB xbee = XBeeZB(RADIO_TX, RADIO_RX, RADIO_RESET, NC, NC, 115200);

    /* Register callbacks */
    xbee.register_receive_cb(&receive_cb);

    RadioStatus const radioStatus = xbee.init();
    MBED_ASSERT(radioStatus == Success);

    /* Wait until the device has joined the network */
    log_serial->printf("Waiting for device to join the network: ");

    while (!xbee.is_joined())
    {
        wait_ms(1000);
        log_serial->printf(".");
    }
    
    log_serial->printf("OK\r\n");

    while (true)
    {
        xbee.process_rx_frames();
        wait_ms(5);
        //log_serial->printf(".");
    }

    delete (log_serial);
}
