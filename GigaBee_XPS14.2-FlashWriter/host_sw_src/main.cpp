#include <QtCore/QCoreApplication>
#include <QTimer>
#include <QtConcurrentRun>
#include <iostream>
#include <cstdio>
#include "gfwupgrader.h"

using namespace std;



void print_ip(quint32 ip)
{
    printf("%3d.%3d.%3d.%3d",
           (ip & 0x000000FF) >> 0,
           (ip & 0x0000FF00) >> 8,
           (ip & 0x00FF0000) >> 16,
           (ip & 0xFF000000) >> 24

           );
}

void print_devices(const QList<gigabee_device_t> *dev_list)
{
    printf(" id | MAC address:      | IP address:     | Self-test result:\n");
    printf("----+-------------------+-----------------+---------------------\n");

    for (int i = 0; i < dev_list->size(); i++) {
        const gigabee_device_t &dev = dev_list->at(i);

        printf(" %2d | ", i);
        printf("%02X:%02X:%02X:%02X:%02X:%02X",
               dev.mac[0],
               dev.mac[1],
               dev.mac[2],
               dev.mac[3],
               dev.mac[4],
               dev.mac[5]
               );

        printf(" | ");
        print_ip(dev.ip);
        printf(" | ");

        if (dev.test_result == SELF_TEST_ALL_OK)
            printf("All OK.");
        else {
            printf("FAILED: ");
            if (dev.test_result & SELF_TEST_RAM) printf("RAM ");
            if (dev.test_result & SELF_TEST_FLASH) printf("flash ");
            if (dev.test_result & SELF_TEST_EEPROM) printf("eeprom ");
            if (dev.test_result & SELF_TEST_ETHERNET) printf("ethernet ");
        }

        printf("\n");

    }

    printf("----+-------------------+-----------------+---------------------\n");


}

void print_header()
{
    cout << endl;
    cout << "GigaBee XC6SLX Series Module Flash Writer by Optomotive" << endl;
    cout << "Copyright (c) 2012 Optomotive" << endl;
    cout << "www.optomotive.com" << endl;
    cout << endl;
}

void print_usage()
{
    cout << endl;
    cout << "Usage: " << endl;
    cout << "gigabeeflash [options] file" << endl;
    cout << "[options]: options" << endl;
    cout << "file: file containing binary image to be written to flash." << endl;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    print_header();


    // load flash file...

    // last argument is the flash file
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char *file_name = argv[argc-1];
    cout << "Loading flash image from file: \"" << file_name << "\"" << endl;

    gfwUpgrader gfwupgrader;

    gfwupgrader.loadFlashImage(QString(file_name));

    //cout << "Flash file ok ? " << gfwupgrader.m_isFlashFileOK << endl;

    if (gfwupgrader.m_isFlashFileOK == false) {
        cout << "Flash file is not OK, quitting." << endl;
        return 1;
    }

    cout << "File OK." << endl << endl;

    // discover devices

    int discover_repeats = 5;
    while (discover_repeats--) {
        cout << "Discovering connected GigaBee modules (" << discover_repeats+1 << " tries remaining):";
        gfwupgrader.doDiscover();
        cout << endl ;


        if ( gfwupgrader.m_deviceList->size() < 1) {
            cout << "No devices found" << endl << endl;
        } else break;

    }

     if ( gfwupgrader.m_deviceList->size() < 1) {
         cout << "No GigaBee module responded. Exiting..." << endl;
         return 0;
     }

    cout << endl
         << "Discovered devices (" << gfwupgrader.m_deviceList->size()
         << "):"
         << endl;

    print_devices(gfwupgrader.m_deviceList);
    cout << endl;


    //return 0; // debug

    // select devices

    QList<gigabee_device_t> dev_list; dev_list.clear();

    for (int i = 0; i < gfwupgrader.m_deviceList->size(); i++) {

        if (gfwupgrader.m_deviceList->value(i).test_result == SELF_TEST_ALL_OK) {

            cout << "Upgrading dev " << i << endl;
            dev_list.append(gfwupgrader.m_deviceList->value(i));

        } else {
            cout << "Device " << i << " failed self-test. Try to upgrade it anyway? [Y/n]";
            char a = getchar();      // get input

            if ((a == 'y') || (a == 'Y') || (a == EOF)) { // y, Y are valid
                cout << endl;
                cout << "Upgrading dev " << i << endl;
                dev_list.append(gfwupgrader.m_deviceList->value(i));
            }

        }
    }

    QThreadPool::globalInstance()->setMaxThreadCount(10);


    // upgrade
    cout << "Starting upgrade..." << endl;

    DummyMainClass dummy;
    dummy.m_gfwupgrader = &gfwupgrader;

    dummy.connect(&gfwupgrader, SIGNAL(updateProgressSignal()),
                  &dummy, SLOT(writeProgress()),
                  Qt::QueuedConnection);


    QFuture<void> future = QtConcurrent::run(&gfwupgrader,
                                             &gfwUpgrader::startUpgrade,
                                             &dev_list);

    while( future.isFinished() == false) {
        QCoreApplication::processEvents();


    }


    cout << endl << "Finished uploading binary flash image to GigaBee module." << endl;
    cout << "Wait approx. 35 seconds for the writing to flash to be completed." << endl;
    cout << "The module will automatically reconfigure (reset) after the flash is written with the new bitstream." << endl;

    return 0;
    return a.exec();
}
