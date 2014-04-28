/* MarketData_publisher.cxx

 A publication of data of type MarketData

 This file is derived from code automatically generated by the rtiddsgen
 command:

 rtiddsgen -language C++ -example <arch> MarketData.idl

 Example publication of type MarketData automatically generated by
 'rtiddsgen'. To test them follow these steps:

 (1) Compile this file and the example subscription.

 (2) Start the subscription with the command
 objs/<arch>/MarketData_subscriber <domain_id> <sample_count>

 (3) Start the publication with the command
 objs/<arch>/MarketData_publisher <domain_id> <sample_count>

 (4) [Optional] Specify the list of discovery initial peers and
 multicast receive addresses via an environment variable or a file
 (in the current working directory) called NDDS_DISCOVERY_PEERS.

 You can run any number of publishers and subscribers programs, and can
 add and remove them dynamically from the domain.


 Example:

 To run the example application on domain <domain_id>:

 On Unix:

 objs/<arch>/MarketData_publisher <domain_id> o
 objs/<arch>/MarketData_subscriber <domain_id>

 On Windows:

 objs\<arch>\MarketData_publisher <domain_id>
 objs\<arch>\MarketData_subscriber <domain_id>


 modification history
 ------------ -------
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef RTI_VX653
#include <vThreadsData.h>
#endif
#include "MarketData.h"
#include "MarketDataSupport.h"
#include "ndds/ndds_cpp.h"

/* Delete all entities */
static int publisher_shutdown(DDSDomainParticipant *participant) {
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides finalize_instance() method on
     domain participant factory for people who want to release memory used
     by the participant factory. Uncomment the following block of code for
     clean destruction of the singleton. */
    /*
     retcode = DDSDomainParticipantFactory::finalize_instance();
     if (retcode != DDS_RETCODE_OK) {
     printf("finalize_instance error %d\n", retcode);
     status = -1;
     }
     */

    return status;
}

extern "C" int publisher_main(int domainId, int sample_count) {
    DDSDomainParticipant *participant = NULL;
    DDSPublisher *publisher = NULL;
    DDSTopic *topic = NULL;
    DDS_DataWriterQos writer_qos;
    DDSDataWriter *writer = NULL;
    MarketDataDataWriter * MarketData_writer = NULL;
    MarketData *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t send_period = { 0, 100000000 };

    /* To customize participant QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    participant = DDSTheParticipantFactory->create_participant(domainId,
            DDS_PARTICIPANT_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize publisher QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    publisher = participant->create_publisher(DDS_PUBLISHER_QOS_DEFAULT,
            NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = MarketDataTypeSupport::get_type_name();
    retcode = MarketDataTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize topic QoS, use 
     the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic("Example MarketData", type_name,
            DDS_TOPIC_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and modify the datawriter creation fuction using writer_qos.
     *
     * In this case, we set the publish as multichannel using the differents
     * channel to send differents symbol. Every channel have a IP to send the
     * data.
     */

    /* Start changes for MultiChannel */
    /*
    retcode = publisher->get_default_datawriter_qos(writer_qos);
    if (retcode != DDS_RETCODE_OK) {
        printf("get_default_datawriter_qos error\n");
        publisher_shutdown(participant);
        return -1;
    }
    */
    /* Create 8 channels based on Symbol */
    /*
    writer_qos.multi_channel.channels.ensure_length(8, 8);
    writer_qos.multi_channel.channels[0].filter_expression = DDS_String_dup(
            "Symbol MATCH '[A-C]*'");
    writer_qos.multi_channel.channels[0].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[0].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.2");
    writer_qos.multi_channel.channels[1].filter_expression = DDS_String_dup(
            "Symbol MATCH '[D-F]*'");
    writer_qos.multi_channel.channels[1].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[1].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.3");
    writer_qos.multi_channel.channels[2].filter_expression = DDS_String_dup(
            "Symbol MATCH '[G-I]*'");
    writer_qos.multi_channel.channels[2].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[2].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.4");
    writer_qos.multi_channel.channels[3].filter_expression = DDS_String_dup(
            "Symbol MATCH '[J-L]*'");
    writer_qos.multi_channel.channels[3].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[3].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.5");
    writer_qos.multi_channel.channels[4].filter_expression = DDS_String_dup(
            "Symbol MATCH '[M-O]*'");
    writer_qos.multi_channel.channels[4].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[4].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.6");
    writer_qos.multi_channel.channels[5].filter_expression = DDS_String_dup(
            "Symbol MATCH '[P-S]*'");
    writer_qos.multi_channel.channels[5].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[5].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.7");
    writer_qos.multi_channel.channels[6].filter_expression = DDS_String_dup(
            "Symbol MATCH '[T-V]*'");
    writer_qos.multi_channel.channels[6].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[6].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.8");
    writer_qos.multi_channel.channels[7].filter_expression = DDS_String_dup(
            "Symbol MATCH '[W-Z]*'");
    writer_qos.multi_channel.channels[7].multicast_settings.ensure_length(1, 1);
    writer_qos.multi_channel.channels[7].multicast_settings[0].receive_address =
            DDS_String_dup("239.255.0.9");
     */
    /* To customize data writer QoS, use
     the configuration file USER_QOS_PROFILES.xml */
    /* toggle between writer_qos and DDS_DATAWRITER_QOS_DEFAULT to alternate
     * between using code and using XML to specify the Qos */
    writer = publisher->create_datawriter(topic,
    /*writer_qos*/DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        printf("create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* End changes for MultiChannel */

    MarketData_writer = MarketDataDataWriter::narrow(writer);
    if (MarketData_writer == NULL) {
        printf("DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */

    instance = MarketDataTypeSupport::create_data();

    if (instance == NULL) {
        printf("MarketDataTypeSupport::create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
     written multiple times, initialize the key here
     and register the keyed instance prior to writing */
    /*
     instance_handle = MarketData_writer->register_instance(*instance);
     */

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {

        /* Changes for MultiChannel */
        /* Modify the data to be sent here */
        sprintf(instance->Symbol, "%c", 'A' + (count % 26));
        instance->Price = count;

        retcode = MarketData_writer->write(*instance, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            printf("write error %d\n", retcode);
        }

        NDDSUtility::sleep(send_period);
    }

    /*
     retcode = MarketData_writer->unregister_instance(
     *instance, instance_handle);
     if (retcode != DDS_RETCODE_OK) {
     printf("unregister instance error %d\n", retcode);
     }
     */

    /* Delete data sample */
    retcode = MarketDataTypeSupport::delete_data(instance);
    if (retcode != DDS_RETCODE_OK) {
        printf("MarketDataTypeSupport::delete_data error %d\n", retcode);
    }

    /* Delete all entities */
    return publisher_shutdown(participant);
}

#if defined(RTI_WINCE)
int wmain(int argc, wchar_t** argv)
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = _wtoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = _wtoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
     NDDSConfigLogger::get_instance()->
     set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return publisher_main(domainId, sample_count);
}

#elif !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
int main(int argc, char *argv[]) {
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
     NDDSConfigLogger::get_instance()->
     set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return publisher_main(domainId, sample_count);
}
#endif

#ifdef RTI_VX653
const unsigned char* __ctype = *(__ctypePtrGet());

extern "C" void usrAppInit ()
{
#ifdef  USER_APPL_INIT
    USER_APPL_INIT; /* for backwards compatibility */
#endif

    /* add application specific code here */
    taskSpawn("pub", RTI_OSAPI_THREAD_PRIORITY_NORMAL, 0x8, 0x150000,
            (FUNCPTR)publisher_main, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

}
#endif

