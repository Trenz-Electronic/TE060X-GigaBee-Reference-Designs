#ifndef GFWDEVICECOMMUNICATOR_H
#define GFWDEVICECOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>

//#include "gfwupgrader.h"

class gfwDeviceCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit gfwDeviceCommunicator(QObject *parent = 0);
    ~gfwDeviceCommunicator();

    // methods
    int setup(const unsigned char *mac, quint32 ip, unsigned char *flash_data, int flash_data_length);
    int startSimulatedTransmission();
    int startTransmission();



    // variables
    unsigned char m_deviceMAC[6];
    quint32 m_deviceIP;
    const unsigned char *m_flashData;
    int m_flashDataLength;
    int m_singlePayloadSize;
    quint16 m_port;
    quint16 m_destinationPort;

    int m_transferPercent;


private:
    int m_currentPacket;
    int m_allPackets;
    QUdpSocket *m_socket;

    int waitForWriteAck(int timeout_msec);
    int waitForTransferAck(int packet_id, int timeout_msec);


    
signals:
    void signalTransmissionProgress(int percent);

    
public slots:
    
};

#endif // GFWDEVICECOMMUNICATOR_H
