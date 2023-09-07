# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/simple_graph_demo
# GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/simple_graph_demo \
#         --calculator_graph_config_file=mymediapipe/graphs/simple_io/hello_world_stream.pbtxt\
#         --output_stream=out

GLOG_logtostderr=1 bazel-bin/mymediapipe/examples/desktop/simple_io/simple_graph_demo \
        --calculator_graph_config_file=mymediapipe/graphs/simple_io/hello_world_side_packet.pbtxt \
        --output_stream=out \
        --input_side_packets=in_side=hello_world \
        --output_side_packets=out_side
