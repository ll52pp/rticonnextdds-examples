/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
/* batch_data_subscriber.cs

   A subscription example

   This file is derived from code automatically generated by the rtiddsgen 
   command:

   rtiddsgen -language C# -example <arch> batch_data.idl

   Example subscription of type batch_data automatically generated by 
   'rtiddsgen'. To test them, follow these steps:

   (1) Compile this file and the example publication.

   (2) Start the subscription with the command
       objs\<arch>\batch_data_subscriber <domain_id> <sample_count>

   (3) Start the publication with the command
       objs\<arch>\batch_data_publisher <domain_id> <sample_count>

   (4) [Optional] Specify the list of discovery initial peers and 
       multicast receive addresses via an environment variable or a file 
       (in the current working directory) called NDDS_DISCOVERY_PEERS. 

   You can run any number of publishers and subscribers programs, and can 
   add and remove them dynamically from the domain.
                                   
   Example:
        
       To run the example application on domain <domain_id>:
                          
       bin\<Debug|Release>\batch_data_publisher <domain_id> <turbo_mode> 
                   <sample_count>  
       bin\<Debug|Release>\batch_data_subscriber <domain_id> <turbo_mode>
                   <sample_count>
              
       
modification history
------------ -------
*/

public class batch_dataSubscriber {

    public class batch_dataListener : DDS.DataReaderListener {

        public override void on_requested_deadline_missed(
            DDS.DataReader reader,
            ref DDS.RequestedDeadlineMissedStatus status) {}
    
        public override void on_requested_incompatible_qos(
            DDS.DataReader reader,
            DDS.RequestedIncompatibleQosStatus status) {}
    
        public override void on_sample_rejected(
            DDS.DataReader reader,
            ref DDS.SampleRejectedStatus status) {}

        public override void on_liveliness_changed(
            DDS.DataReader reader,
            ref DDS.LivelinessChangedStatus status) {}

        public override void on_sample_lost(
            DDS.DataReader reader,
            ref DDS.SampleLostStatus status) {}

        public override void on_subscription_matched(
            DDS.DataReader reader,
            ref DDS.SubscriptionMatchedStatus status) {}

        public override void on_data_available(DDS.DataReader reader) {
            batch_dataDataReader batch_data_reader =
                (batch_dataDataReader)reader;
            
            try {
                batch_data_reader.take(
                    data_seq,
                    info_seq,
                    DDS.ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                    DDS.SampleStateKind.ANY_SAMPLE_STATE,
                    DDS.ViewStateKind.ANY_VIEW_STATE,
                    DDS.InstanceStateKind.ANY_INSTANCE_STATE);
            }
            catch(DDS.Retcode_NoData) {
                return;
            }
            catch(DDS.Exception e) {
                Console.WriteLine("take error {0}", e);
                return;
            }

            System.Int32 data_length = data_seq.length;
            for (int i = 0; i < data_length; ++i) {
                if (info_seq.get_at(i).valid_data) {
                    batch_dataTypeSupport.print_data(data_seq.get_at(i));
                }
            }

            try {
                batch_data_reader.return_loan(data_seq, info_seq);
            }
            catch(DDS.Exception e) {
                Console.WriteLine("return loan error {0}", e);
            }
        }

        public batch_dataListener() {
            data_seq = new batch_dataSeq();
            info_seq = new DDS.SampleInfoSeq();
        }

        private batch_dataSeq data_seq;
        private DDS.SampleInfoSeq info_seq;
    };

    public static void Main(string[] args) {

        // --- Get domain ID --- //
        int domain_id = 0;
        if (args.Length >= 1) {
            domain_id = Int32.Parse(args[0]);
        }
        
        // --- Get if the turbo mode should be enabled --- //
        int turbo_mode_on = 0;
        if (args.Length >= 2) {
            turbo_mode_on = Int32.Parse(args[1]);
        }

        // --- Get max loop count; 0 means infinite loop  --- //
        int sample_count = 0;
        if (args.Length >= 3) {
            sample_count = Int32.Parse(args[2]);
        }

        /* Uncomment this to turn on additional logging
        NDDS.ConfigLogger.get_instance().set_verbosity_by_category(
            NDDS.LogCategory.NDDS_CONFIG_LOG_CATEGORY_API, 
            NDDS.LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
        */

        // --- Run --- //
        try {
            batch_dataSubscriber.subscribe(
                domain_id, sample_count, turbo_mode_on);
        }
        catch(DDS.Exception) {
            Console.WriteLine("error in subscriber");
        }
    }

    static void subscribe(int domain_id, int sample_count, int turbo_mode_on) {

        System.String profile_name = null;
        System.String library_name = "batching_Library";

        /* We pick the profile name if the turbo_mode is selected or not.
         * If Turbo_mode is not selected, the batching profile will be used.
         */

        if (turbo_mode_on == 1) {
            profile_name = "turbo_mode_profile";
            Console.WriteLine("Turbo Mode enable");
        } else {
            profile_name = "batch_profile";
            Console.WriteLine("Manual batching enable");
        }
        
        // --- Create participant --- //

        /* To customize the participant QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.DomainParticipant participant =
            DDS.DomainParticipantFactory.get_instance().
            create_participant_with_profile(
                domain_id,
                library_name, profile_name, 
                null /* listener */,
                DDS.StatusMask.STATUS_MASK_NONE);
        if (participant == null) {
            shutdown(participant);
            throw new ApplicationException("create_participant error");
        }

        // --- Create subscriber --- //

        /* To customize the subscriber QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.Subscriber subscriber = participant.create_subscriber_with_profile(
            library_name, profile_name,
            null /* listener */,
            DDS.StatusMask.STATUS_MASK_NONE);
        if (subscriber == null) {
            shutdown(participant);
            throw new ApplicationException("create_subscriber error");
        }

        // --- Create topic --- //

        /* Register the type before creating the topic */
        System.String type_name = batch_dataTypeSupport.get_type_name();
        try {
            batch_dataTypeSupport.register_type(
                participant, type_name);
        }
        catch(DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            shutdown(participant);
            throw e;
        }

        /* To customize the topic QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.Topic topic = participant.create_topic(
            "Example batch_data",
            type_name,
            DDS.DomainParticipant.TOPIC_QOS_DEFAULT,
            null /* listener */,
            DDS.StatusMask.STATUS_MASK_NONE);
        if (topic == null) {
            shutdown(participant);
            throw new ApplicationException("create_topic error");
        }

        // --- Create reader --- //

        /* Create a data reader listener */
        batch_dataListener reader_listener =
            new batch_dataListener();

        /* To customize the data reader QoS, use 
           the configuration file USER_QOS_PROFILES.xml */
        DDS.DataReader reader = subscriber.create_datareader_with_profile(
            topic,
            library_name, profile_name,
            reader_listener,
            DDS.StatusMask.STATUS_MASK_ALL);
        if (reader == null) {
            shutdown(participant);
            reader_listener = null;
            throw new ApplicationException("create_datareader error");
        }

        // --- Wait for data --- //

        /* Main loop */
        const System.Int32 receive_period = 4000; // milliseconds
        for (int count=0;
             (sample_count == 0) || (count < sample_count);
             ++count) {
            Console.WriteLine(
                "batch_data subscriber sleeping for {0} sec...",
                receive_period / 1000);

            System.Threading.Thread.Sleep(receive_period);
        }

        // --- Shutdown --- //

        /* Delete all entities */
        shutdown(participant);
        reader_listener = null;
    }


    static void shutdown(
        DDS.DomainParticipant participant) {

        /* Delete all entities */

        if (participant != null) {
            participant.delete_contained_entities();
            DDS.DomainParticipantFactory.get_instance().delete_participant(
                ref participant);
        }

        /* RTI Connext provides finalize_instance() method on
           domain participant factory for users who want to release memory
           used by the participant factory. Uncomment the following block of
           code for clean destruction of the singleton. */
        /*
        try {
            DDS.DomainParticipantFactory.finalize_instance();
        }
        catch(DDS.Exception e) {
            Console.WriteLine("finalize_instance error {0}", e);
            throw e;
        }
        */
    }
}

