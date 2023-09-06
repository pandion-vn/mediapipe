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
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

ABSL_FLAG(std::string, input_side_packets, "",
          "Comma-separated list of key=value pairs specifying side packets "
          "for the CalculatorGraph. All values will be treated as the "
          "string type even if they represent doubles, floats, etc.");


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

    LOG(INFO) << "Get calculator graph config contents: "
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
    
    return absl::OkStatus();
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
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
