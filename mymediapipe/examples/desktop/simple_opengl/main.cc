#include <cstdlib>
#include "include/common.h"

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  LOG(INFO) << "Initialize the camera or load the video.";
}