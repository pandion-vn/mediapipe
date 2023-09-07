#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/statusor.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

ABSL_FLAG(std::string, input_side_packets, "",
          "Comma-separated list of key=value pairs specifying side packets "
          "for the CalculatorGraph. All values will be treated as the "
          "string type even if they represent doubles, floats, etc.");

// Local file output flags.
// Output stream
ABSL_FLAG(std::string, output_stream, "",
          "The output stream to output to the local file in csv format.");
ABSL_FLAG(std::string, output_stream_file, "",
          "The name of the local file to output all packets sent to "
          "the stream specified with --output_stream. ");
ABSL_FLAG(bool, strip_timestamps, false,
          "If true, only the packet contents (without timestamps) will be "
          "written into the local file.");

// Output side packets
ABSL_FLAG(std::string, output_side_packets, "",
          "A CSV of output side packets to output to local file.");
ABSL_FLAG(std::string, output_side_packets_file, "",
          "The name of the local file to output all side packets specified "
          "with --output_side_packets. ");


absl::Status OutputStreamToLocalFile(mediapipe::OutputStreamPoller& poller) {
    std::ofstream file;
    file.open(absl::GetFlag(FLAGS_output_stream_file));
    mediapipe::Packet packet;
    while (poller.Next(&packet)) {
        std::string output_data;
        if (!absl::GetFlag(FLAGS_strip_timestamps)) {
            absl::StrAppend(&output_data, packet.Timestamp().Value(), ",");
        }
        absl::StrAppend(&output_data, packet.Get<std::string>(), "\n");
        file << output_data;
    }
    file.close();
    return absl::OkStatus();
}

absl::Status OutputStreamToConsole(mediapipe::OutputStreamPoller& poller) {
    mediapipe::Packet packet;
    while (poller.Next(&packet)) {
        std::string output_data;
        if (!absl::GetFlag(FLAGS_strip_timestamps)) {
            absl::StrAppend(&output_data, packet.Timestamp().Value(), ",");
        }
        absl::StrAppend(&output_data, packet.Get<std::string>(), "\n");
        LOG(INFO) << output_data;
    }
    
    return absl::OkStatus();
}

absl::Status OutputSidePacketsToLocalFile(mediapipe::CalculatorGraph& graph) {
    std::ofstream file;
    file.open(absl::GetFlag(FLAGS_output_side_packets_file));
    std::vector<std::string> side_packet_names =
        absl::StrSplit(absl::GetFlag(FLAGS_output_side_packets), ',');
    for (const std::string& side_packet_name : side_packet_names) {
        ASSIGN_OR_RETURN(auto status_or_packet,
                         graph.GetOutputSidePacket(side_packet_name));
        file << absl::StrCat(side_packet_name, ":",
                             status_or_packet.Get<std::string>(), "\n");
    }
    file.close();
    return absl::OkStatus();
}

absl::Status OutputSidePacketsToConsole(mediapipe::CalculatorGraph& graph) {
    std::vector<std::string> side_packet_names =
        absl::StrSplit(absl::GetFlag(FLAGS_output_side_packets), ',');
    for (const std::string& side_packet_name : side_packet_names) {
        ASSIGN_OR_RETURN(auto status_or_packet,
                         graph.GetOutputSidePacket(side_packet_name));
        LOG(INFO) << absl::StrCat(side_packet_name, ":",
                                  status_or_packet.Get<std::string>(), "\n");
    }
    return absl::OkStatus();
}

absl::Status OutputSidePackets(mediapipe::CalculatorGraph& graph) {
    if (!absl::GetFlag(FLAGS_output_side_packets_file).empty()) {
        return OutputSidePacketsToLocalFile(graph);
    } else {
        return OutputSidePacketsToConsole(graph);
    }

    return absl::OkStatus();
}

absl::Status InputStreamSample(mediapipe::CalculatorGraph &graph) {
    // Give 10 input packets that contains the same string "Hello World!".
    for (int i = 0; i < 10; ++i) {
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
            "in", mediapipe::MakePacket<std::string>("Hello World!").At(mediapipe::Timestamp(i))));
    }
    MP_RETURN_IF_ERROR(graph.CloseInputStream("in"));
    return absl::OkStatus();
}

absl::StatusOr<std::map<std::string, mediapipe::Packet>> InitialInputSidePackets() {
    std::map<std::string, mediapipe::Packet> input_side_packets;
    if (!absl::GetFlag(FLAGS_input_side_packets).empty()) {
        std::vector<std::string> kv_pairs =
                absl::StrSplit(absl::GetFlag(FLAGS_input_side_packets), ',');
        for (const std::string& kv_pair : kv_pairs) {
            std::vector<std::string> name_and_value = absl::StrSplit(kv_pair, '=');
            RET_CHECK(name_and_value.size() == 2);
            RET_CHECK(!mediapipe::ContainsKey(input_side_packets, name_and_value[0]));
            input_side_packets[name_and_value[0]] =
                mediapipe::MakePacket<std::string>(name_and_value[1]);
        }
    }
    return input_side_packets;
}

absl::StatusOr<mediapipe::CalculatorGraphConfig> GetGraphConfig() {
    std::string calculator_graph_config_contents;
    MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
        absl::GetFlag(FLAGS_calculator_graph_config_file),
        &calculator_graph_config_contents));

    LOG(INFO) << "Get calculator graph config contents: \n"
              << calculator_graph_config_contents;

    mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);

    return config;
}

absl::Status RunMPPGraph() {
    ASSIGN_OR_RETURN(auto config, GetGraphConfig());
    ASSIGN_OR_RETURN(auto input_side_packets, InitialInputSidePackets());

    LOG(INFO) << "Initialize the calculator graph.";
    mediapipe::CalculatorGraph graph;
    MP_RETURN_IF_ERROR(graph.Initialize(config, input_side_packets));

    LOG(INFO) << "Output stream poller";
    if (!absl::GetFlag(FLAGS_output_stream).empty()) {
        ASSIGN_OR_RETURN(auto poller, graph.AddOutputStreamPoller(
                                        absl::GetFlag(FLAGS_output_stream)));
        LOG(INFO) << "Start running the calculator graph - stream.";
        MP_RETURN_IF_ERROR(graph.StartRun({}));

        MP_RETURN_IF_ERROR(InputStreamSample(graph));

        if (!absl::GetFlag(FLAGS_output_stream_file).empty()) {
            MP_RETURN_IF_ERROR(OutputStreamToLocalFile(poller));
        } else {
            MP_RETURN_IF_ERROR(OutputStreamToConsole(poller));
        }
    } else {
        RET_CHECK(absl::GetFlag(FLAGS_output_stream).empty())
            << "--output_stream should be specified.";
    }
    MP_RETURN_IF_ERROR(graph.WaitUntilDone());
    
    // return absl::OkStatus();
    return OutputSidePackets(graph);
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "3");
    absl::ParseCommandLine(argc, argv);
    absl::Status run_status = RunMPPGraph();

    if (!run_status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << run_status.message();
    return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}
