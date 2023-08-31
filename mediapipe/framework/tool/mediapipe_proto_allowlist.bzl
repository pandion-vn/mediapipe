"""mediapipe_proto_library targets enabled for package mediaipipe rewrite."""

rewrite_target_list = [
    "acceleration_proto",
    "adaptive_crop_calculator_proto",
    "affine_transform_data_proto",
    "affine_transform_from_rect_calculator_proto",
    "align_hand_to_pose_in_world_calculator_proto",
    "anchor_proto",
    "annotation_overlay_calculator_proto",
    "annotation_proto",
    "annotations_to_model_matrices_calculator_proto",
    "annotations_to_render_data_calculator_proto",
    "a_r_capture_metadata_proto",
    "association_calculator_proto",
    "audio_classifier_graph_options_proto",
    "audio_decoder_proto",
    "audio_denoiser_options_proto",
    "audio_embedder_graph_options_proto",
    "audio_to_spectrogram_calculator_proto",
    "audio_to_tensor_calculator_proto",
    "auto_relighting_proto",
    "bandpass_calculator_proto",
    "barcode_reader_calculator_proto",
    "base_options_proto",
    "belief_decoder_config_proto",
    "bert_preprocessor_calculator_proto",
    "bilateral_filter_calculator_proto",
    "body_rig_proto",
    "boxes_and_scores_decoder_proto",
    "bypass_calculator_proto",
    "calculator_contract_test_proto",
    "calculator_graph_template_proto",
    "calculator_options_proto",
    "calculator_profile_proto",
    "calculator_proto",
    "camera_parameters_proto",
    "classification_aggregation_calculator_proto",
    "classification_postprocessing_graph_options_proto",
    "classification_proto",
    "classifications_proto",
    "classifications_to_render_data_calculator_proto",
    "classifier_options_proto",
    "clip_vector_size_calculator_proto",
    "collection_has_min_size_calculator_proto",
    "color_proto",
    "combined_prediction_calculator_proto",
    "combine_joints_calculator_proto",
    "compressor_calculator_proto",
    "concatenate_vector_calculator_proto",
    "constant_side_packet_calculator_proto",
    "content_provider_calculator_proto",
    "delay_calculator_proto",
    "demux_calculator_proto",
    "denoiser_mask_processor_calculator_proto",
    "denoiser_spectrum_processor_calculator_proto",
    "dequantize_byte_array_calculator_proto",
    "detection_label_id_to_text_calculator_proto",
    "detection_proto",
    "detections_to_plm_calculator_proto",
    "detections_to_rects_calculator_proto",
    "detections_to_render_data_calculator_proto",
    "distortion_calculator_proto",
    "dominant_light_side_calculator_proto",
    "mediapipe_options_proto",
    "effect_renderer_calculator_proto",
    "embedder_options_proto",
    "embedding_postprocessing_graph_options_proto",
    "embeddings_proto",
    "env_generator_calculator_proto",
    "environment_generator_calculator_proto",
    "environment_light_estimator_calculator_proto",
    "environment_proto",
    "example_calculator_options_proto",
    "example_subgraph_options_proto",
    "external_file_proto",
    "face_blendshapes_graph_options_proto",
    "face_detection_proto",
    "face_detections_to_bool_calculator_proto",
    "face_detector_graph_options_proto",
    "face_editor_graph_options_proto",
    "face_gan_graph_options_proto",
    "face_geometry_proto",
    "face_landmarks_detector_graph_options_proto",
    "face_landmarks_output_proto",
    "face_landmarks_proto",
    "face_rig_ghum_proto",
    "face_rig_output_proto",
    "face_rig_proto",
    "feature_detector_calculator_proto",
    "feedback_tensors_calculator_proto",
    "field_data_proto",
    "filter_detection_calculator_proto",
    "filter_detections_calculator_proto",
    "filter_frame_by_classification_calculator_proto",
    "flow_container_proto",
    "flow_limiter_calculator_proto",
    "frame_annotation_to_rect_calculator_proto",
    "frame_annotation_tracker_calculator_proto",
    "frozen_generator_proto",
    "gate_calculator_proto",
    "gaze_to_render_data_calculator_proto",
    "geometry_pipeline_calculator_proto",
    "geometry_pipeline_metadata_proto",
    "gesture_classifier_graph_options_proto",
    "gesture_detection_flume_proto",
    "gesture_detection_proto",
    "gesture_embedder_graph_options_proto",
    "gesture_face_filter_calculator_proto",
    "gesture_filter_proto",
    "gesture_recognizer_graph_options_proto",
    "gesture_temporal_filter_tuner_calculator_proto",
    "get_vector_item_calculator_proto",
    "gl_animation_overlay_calculator_proto",
    "gl_context_options_proto",
    "gl_mask_transform_calculator_proto",
    "gpu_origin_proto",
    "graph_profile_calculator_proto",
    "gsenet_speech_enhancement_graph_options_proto",
    "hand_association_calculator_proto",
    "hand_detector_graph_options_proto",
    "hand_detector_result_proto",
    "hand_gesture_recognizer_graph_options_proto",
    "hand_landmarker_graph_options_proto",
    "hand_landmarks_detector_graph_options_proto",
    "hand_tracking_and_gesture_proto",
    "heatmap_to_detections_calculator_proto",
    "image_classifier_graph_options_proto",
    "image_clone_calculator_proto",
    "image_cropping_calculator_proto",
    "image_embedder_graph_options_proto",
    "image_file_properties_proto",
    "image_format_proto",
    "image_preprocessing_graph_options_proto",
    "image_segmenter_graph_options_proto",
    "image_to_batch_tensor_calculator_proto",
    "image_to_tensor_calculator_proto",
    "image_transformation_calculator_proto",
    "inference_calculator_proto",
    "inference_subgraph_proto",
    "is_speaking_from_tensors_proto",
    "joint_detector_graph_options_proto",
    "js_native_renderer_calculator_proto",
    "json_events_packer_calculator_proto",
    "json_events_unpacker_calculator_proto",
    "label_map_proto",
    "labels_to_render_data_calculator_proto",
    "landmark_projection_calculator_proto",
    "landmark_proto",
    "landmarks_detection_result_proto",
    "landmarks_detector_proto",
    "landmarks_refinement_calculator_proto",
    "landmarks_smoothing_calculator_proto",
    "landmarks_to_detection_calculator_proto",
    "landmarks_to_floats_calculator_proto",
    "landmarks_to_matrix_calculator_proto",
    "landmarks_to_render_data_calculator_proto",
    "landmarks_to_tensor_calculator_proto",
    "landmarks_transformation_calculator_proto",
    "latency_proto",
    "lift_2d_frame_annotation_to_3d_calculator_proto",
    "line_detector_graph_options_proto",
    "local_file_contents_calculator_proto",
    "location_data_proto",
    "locus_proto",
    "logic_calculator_proto",
    "mask_overlay_calculator_proto",
    "mask_to_frame_ratio_calculator_proto",
    "matrix_data_proto",
    "mediapipe_events_packer_calculator_proto",
    "mesh_3d_proto",
    "mobile_ssd_calculator_proto",
    "model_matrix_proto",
    "model_resources_calculator_proto",
    "multi_hand_rig_proto",
    "multi_hand_tracker_proto",
    "multi_mic_beamformer_calculator_proto",
    "multiply_selected_joints_calculator_proto",
    "multi_scale_rects_calculator_proto",
    "night_light_calculator_proto",
    "node_chain_subgraph_proto",
    "node_decorator_proto",
    "non_max_suppression_calculator_proto",
    "normals_gpu_proto",
    "normals_proto",
    "object_detector_options_proto",
    "object_proto",
    "ola_pitch_shift_calculator_proto",
    "opencv_encoded_image_to_image_frame_calculator_proto",
    "opencv_image_encoder_calculator_proto",
    "packet_cloner_calculator_proto",
    "packet_cloner_with_decay_calculator_proto",
    "packet_factory_proto",
    "packet_frequency_calculator_proto",
    "packet_frequency_proto",
    "packet_generator_proto",
    "packet_generator_wrapper_calculator_proto",
    "packet_latency_calculator_proto",
    "packet_resampler_calculator_proto",
    "packet_test_proto",
    "packet_thinner_calculator_proto",
    "pack_media_sequence_calculator_proto",
    "page_layout_mutator_options_proto",
    "periodic_auto_light_position_calculator_proto",
    "pose_landmarks_output_proto",
    "pose_landmarks_proto",
    "pose_rig_proto",
    "pose_tracking_and_rep_counting_proto",
    "proto_descriptor_proto",
    "pull_push_denoise_calculator_gl_proto",
    "quantize_float_vector_calculator_proto",
    "rasterization_proto",
    "raw_signals_processor_calculator_proto",
    "recolor_calculator_proto",
    "rect_proto",
    "rect_to_render_data_calculator_proto",
    "rect_to_render_scale_calculator_proto",
    "rect_transformation_calculator_proto",
    "refine_landmarks_from_heatmap_calculator_proto",
    "regex_preprocessor_calculator_proto",
    "relighting_proto",
    "relighting_textures_calculator_proto",
    "render_data_proto",
    "renderer_calculator_proto",
    "reorientation_metadata_generator_proto",
    "rep_counting_calculator_proto",
    "rep_counting_results_proto",
    "resonator_calculator_proto",
    "reverb_calculator_proto",
    "roi_tracking_calculator_proto",
    "rotation_mode_proto",
    "saturator_calculator_proto",
    "scale_image_calculator_proto",
    "scale_mode_proto",
    "scale_rect_from_anchor_calculator_proto",
    "score_calibration_calculator_proto",
    "score_classifications_calculator_proto",
    "script_detector_graph_options_proto",
    "seanet_denoiser_options_proto",
    "seanet_fft_denoiser_post_calculator_proto",
    "seanet_fft_denoiser_pre_calculator_proto",
    "seanet_fullband_denoiser_post_calculator_proto",
    "segmentation_mask_proto",
    "segmentation_smoothing_calculator_proto",
    "segmenter_gpu_cpu_proto",
    "segmenter_options_proto",
    "segmenter_output_size_calculator_proto",
    "segmenter_proto",
    "sequence_shift_calculator_proto",
    "set_alpha_calculator_proto",
    "set_joints_visibility_calculator_proto",
    "sharpen_calculator_gl_proto",
    "simple_calculator_proto",
    "single_shot_detector_gpu_cpu_proto",
    "single_shot_detector_proto",
    "size_transformation_calculator_proto",
    "sky_light_calculator_proto",
    "source_proto",
    "spatial_denoise_calculator_gl_proto",
    "spatiotemporal_denoise_calculator_gl_proto",
    "split_vector_calculator_proto",
    "ssd_anchors_calculator_proto",
    "status_handler_proto",
    "status_list_proto",
    "stream_handler_proto",
    "switch_container_proto",
    "tensor_converter_calculator_proto",
    "tensor_dumper_calculator_proto",
    "tensors_to_audio_calculator_proto",
    "tensors_to_classification_calculator_proto",
    "tensors_to_detections_calculator_proto",
    "tensors_to_embeddings_calculator_proto",
    "tensors_to_face_landmarks_graph_options_proto",
    "tensors_to_floats_calculator_proto",
    "tensors_to_image_calculator_proto",
    "tensors_to_landmarks_calculator_proto",
    "tensors_to_objects_calculator_proto",
    "tensors_to_segmentation_calculator_proto",
    "tensor_to_joints_calculator_proto",
    "tensor_view_requestor_proto",
    "test_calculators_proto",
    "test_format_proto",
    "text_classifier_graph_options_proto",
    "text_embedder_graph_options_proto",
    "text_model_type_proto",
    "text_preprocessing_graph_options_proto",
    "tflite_converter_calculator_proto",
    "tflite_custom_op_resolver_calculator_proto",
    "tflite_inference_calculator_proto",
    "tflite_task_classification_result_to_classifications_calculator_proto",
    "tflite_task_image_classifier_calculator_proto",
    "tflite_task_object_detector_calculator_proto",
    "tflite_tensors_to_classification_calculator_proto",
    "tflite_tensors_to_detections_calculator_proto",
    "tflite_tensors_to_landmarks_calculator_proto",
    "tflite_tensors_to_objects_calculator_proto",
    "tflite_tensors_to_segmentation_calculator_proto",
    "thread_pool_executor_proto",
    "thresholding_calculator_proto",
    "timed_box_list_id_to_label_calculator_proto",
    "timed_box_list_to_render_data_calculator_proto",
    "time_series_header_proto",
    "time_series_sample_framer_calculator_proto",
    "top_classifications_calculator_proto",
    "top_k_scores_calculator_proto",
    "touch_input_converter_calculator_proto",
    "trigger_extractor_calculator_proto",
    "trigger_proto",
    "tween_calculator_proto",
    "unpack_media_sequence_calculator_proto",
    "vad_frame_synchronizer_calculator_proto",
    "vibrato_calculator_proto",
    "video_data_proto",
    "visibility_copy_calculator_proto",
    "visibility_smoothing_calculator_proto",
    "visualizer_calculator_proto",
    "visualizer_proto",
    "visuals_renderer_calculator_proto",
    "vulkan_shader_calculator_proto",
    "warp_affine_calculator_proto",
    "xenomorph_proto",
    "xenomorph_result_proto",
    "xenomorph_rig_combined_proto",
    "xenomorph_rig_proto",
    "zoo_mutation_calculator_proto",
    "zoo_mutator_proto",
    "triangle_renderer_calculator_proto",
]
