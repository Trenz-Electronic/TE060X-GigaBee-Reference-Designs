#include "gfwdevicecommunicator.h"

#include <QTime>
#include <iostream>

#include "my_types.h"

gfwDeviceCommunicator::gfwDeviceCommunicator(QObject *parent) :
    QObject(parent)
{
    m_socket = NULL;
    m_currentPacket = 0;
    m_allPackets = 0;
    m_port = 1000 + (((unsigned int)qrand()) % 10000);

    m_destinationPort = 5000;
    m_singlePayloadSize = 1024;

    m_transferPercent = 0;

}


// destructor
gfwDeviceCommunicator::~gfwDeviceCommunicator()
{


    delete m_socket;
}


int gfwDeviceCommunicator::setup(const unsigned char *mac, quint32 ip, unsigned char *flash_data, int flash_data_length)
{
    m_currentPacket = 0;
    m_allPackets = 0;
    // copy info

    memcpy(m_deviceMAC, mac, 6);
    m_deviceIP = ip;

    m_flashData = flash_data;
    m_flashDataLength = flash_data_length;


    // calculate needed number of packets.

    m_allPackets = m_flashDataLength / m_singlePayloadSize;
    if ((m_flashDataLength % m_singlePayloadSize) != 0) m_allPackets++;

    std::cout << "flash data length: " << m_flashDataLength
              << " single payload " << m_singlePayloadSize
              << " Number of packets needed " << m_allPackets
                 << std::endl;





    return 0;
}

int gfwDeviceCommunicator::waitForWriteAck(int timeout_msec)
{

    // wait until we receive an ACK from the device or until timeout_msec

    char *recvbuf;
    QTime time;

    time.start();
    while (time.elapsed() < timeout_msec)
    {
        if (m_socket->pendingDatagramSize() != -1) {
            int datagram_size = m_socket->pendingDatagramSize();
            //std::cout << "Wait for ACK, datagram received received, size " << datagram_size << std::endl;

            recvbuf = new char[datagram_size];
            m_socket->readDatagram(recvbuf,datagram_size);

            if (datagram_size >=  sizeof(gfw_datagram_t)) {
                gfw_datagram_t *gfww = (gfw_datagram_t *)recvbuf;

                if (gfww->op == GFW_OP_FLASH_WRITE_ACK) {
                    //std::cout << "transfer ACK received" << std::endl;
                        return 1;
                }


            }

            delete recvbuf;
        }

    }



    return 0;
}

int gfwDeviceCommunicator::waitForTransferAck(int packet_id, int timeout_msec)
{

    // wait until we receive an ACK from the device or until timeout_msec

    char *recvbuf;
    QTime time;

    time.start();
    while (time.elapsed() < timeout_msec)
    {
        if (m_socket->pendingDatagramSize() != -1) {
            int datagram_size = m_socket->pendingDatagramSize();
            //std::cout << "Wait for ACK, datagram received received, size " << datagram_size << std::endl;

            recvbuf = new char[datagram_size];
            m_socket->readDatagram(recvbuf,datagram_size);

            if (datagram_size >=  sizeof(gfw_datagram_t)) {
                gfw_datagram_t *gfww = (gfw_datagram_t *)recvbuf;

                if (gfww->op == GFW_OP_FLASH_DATA_TRANSFER_ACK) {
                    //std::cout << "transfer ACK received" << std::endl;

                    if (gfww->id == packet_id) {
                       // std::cout << "transfer ACK received for id " << gfww->id << std::endl;
                        return 1;
                    }

                }


            }

            delete recvbuf;
        }

    }



    return 0;
}

int gfwDeviceCommunicator::startSimulatedTransmission()
{
    // increase the counter
    for (int i = 0; i < 101; i++)
    {
        m_transferPercent = i;

        emit signalTransmissionProgress(     i     );

        //std::cout << "i = " << i << std::endl;
        for (int j = 0; j < 10000000; j++)
        {

        }

    }

    return 0;
}


// methods
int gfwDeviceCommunicator::startTransmission()
{

    // open socket
    m_socket = new QUdpSocket();
    m_socket->bind();


    QHostAddress addr(BYTE_SWAP4(m_deviceIP));

    //std::cout << "StartTransmission... for ip " << addr.toString().toStdString() << std::endl;


    //m_socket->connectToHost(addr, m_destinationPort);

    const unsigned char *currdata;
    int datalen;

    datalen = m_singlePayloadSize + sizeof(gfw_datagram_t);

    char *txbuf = new char[datalen];

    // prepare basic datagram
    gfw_datagram_t *gfww = (gfw_datagram_t *)txbuf;
    gfww->magic1 = 'G';
    gfww->magic2 = 'F';
    gfww->magic3 = 'W';
    gfww->op = GFW_OP_FLASH_DATA_TRANSFER_CMD;
    gfww->data_length = m_singlePayloadSize;
    gfww->id = m_currentPacket;
    gfww->param1 = m_allPackets;
    gfww->number_of_packets = m_allPackets;


    for (m_currentPacket = 0; m_currentPacket < m_allPackets; m_currentPacket++) {

        gfww->id = m_currentPacket;
        gfww->address_offset = m_currentPacket * m_singlePayloadSize;
        currdata = m_flashData + m_currentPacket * m_singlePayloadSize;


        int bytes_to_send = m_flashDataLength - m_currentPacket * m_singlePayloadSize;
        if (bytes_to_send > m_singlePayloadSize) bytes_to_send = m_singlePayloadSize; // at most m_singlePayloadSize bytes
        gfww->data_length = bytes_to_send;
        datalen = bytes_to_send + sizeof(gfw_datagram_t);
        memcpy(gfww->data, currdata, bytes_to_send);


        int retries = 100;
        while (retries--) {
            int bytes_sent = -1;
            while (bytes_sent == -1) {
                bytes_sent = m_socket->writeDatagram(txbuf, datalen, addr, m_destinationPort);
                //std::cout << "writeDatagram returns " << bytes_sent << std::endl;
            }


            // wait for ack

            if (waitForTransferAck(m_currentPacket, 50) == 1) break;

        }




        int percent =  (int) ((float)(m_currentPacket+1) / (float)m_allPackets  * 100.0f);

        m_transferPercent = percent; // set the variable

       // std::cout << "Send packet nr. " << (m_currentPacket+1) << " out of " << m_allPackets
       //           << " [" << percent << "%]" << std::endl;
        emit signalTransmissionProgress(     percent     );

    }




    // send start write
    //std::cout << "Sending write flash command.. " << std::endl;
    gfww->op = GFW_OP_FLASH_WRITE_CMD;
    gfww->data_length = 0;
    gfww->id = 0;
    gfww->param1 = 0;
    gfww->number_of_packets = 0;

    // retry untill we get WRITE_ACK

    int retries = 1;
    while (retries--) {
        int bytes_sent = -1;
        while (bytes_sent == -1) {
            bytes_sent = m_socket->writeDatagram(txbuf, sizeof(gfw_datagram_t), addr, m_destinationPort);
           // std::cout << "writeDatagram returns " << bytes_sent << std::endl;
        }


        // wait for ack

        if (  waitForWriteAck(100) == 1) {
            m_transferPercent = 101; // to signal writing
            break;
        }

    }

    if (m_transferPercent != 101)
        m_transferPercent = 255; // err code

    m_transferPercent = 101;
    emit signalTransmissionProgress(m_transferPercent);


    delete txbuf;
    delete m_socket; m_socket = NULL;
    return 0;
}
