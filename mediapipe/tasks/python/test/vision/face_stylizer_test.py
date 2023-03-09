# Copyright 2022 The MediaPipe Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tests for face stylizer."""

import enum
import os
from unittest import mock

import numpy as np
from absl.testing import absltest
from absl.testing import parameterized

from mediapipe.python._framework_bindings import image as image_module
from mediapipe.tasks.python.core import base_options as base_options_module
from mediapipe.tasks.python.test import test_utils
from mediapipe.tasks.python.vision import face_stylizer
from mediapipe.tasks.python.vision.core import vision_task_running_mode as running_mode_module
from mediapipe.tasks.python.vision.core import image_processing_options as image_processing_options_module


_BaseOptions = base_options_module.BaseOptions
_Image = image_module.Image
_FaceStylizer = face_stylizer.FaceStylizer
_FaceStylizerOptions = face_stylizer.FaceStylizerOptions
_RUNNING_MODE = running_mode_module.VisionTaskRunningMode
_ImageProcessingOptions = image_processing_options_module.ImageProcessingOptions

_MODEL = 'face_stylizer_model_placeholder.tflite'
_IMAGE = 'cats_and_dogs.jpg'
_STYLIZED_IMAGE = 'stylized_image_placeholder.jpg'
_TEST_DATA_DIR = 'mediapipe/tasks/testdata/vision'


class ModelFileType(enum.Enum):
  FILE_CONTENT = 1
  FILE_NAME = 2


class FaceStylizerTest(parameterized.TestCase):

  def setUp(self):
    super().setUp()
    self.test_image = _Image.create_from_file(
        test_utils.get_test_data_path(
            os.path.join(_TEST_DATA_DIR, _IMAGE)))
    self.model_path = test_utils.get_test_data_path(
        os.path.join(_TEST_DATA_DIR, _MODEL))

  def test_create_from_file_succeeds_with_valid_model_path(self):
    # Creates with default option and valid model file successfully.
    with _FaceStylizer.create_from_model_path(self.model_path) as stylizer:
      self.assertIsInstance(stylizer, _FaceStylizer)

  def test_create_from_options_succeeds_with_valid_model_path(self):
    # Creates with options containing model file successfully.
    base_options = _BaseOptions(model_asset_path=self.model_path)
    options = _FaceStylizerOptions(base_options=base_options)
    with _FaceStylizer.create_from_options(options) as stylizer:
      self.assertIsInstance(stylizer, _FaceStylizer)

  def test_create_from_options_fails_with_invalid_model_path(self):
    with self.assertRaisesRegex(
        RuntimeError, 'Unable to open file at /path/to/invalid/model.tflite'):
      base_options = _BaseOptions(
          model_asset_path='/path/to/invalid/model.tflite')
      options = _FaceStylizerOptions(base_options=base_options)
      _FaceStylizer.create_from_options(options)

  def test_create_from_options_succeeds_with_valid_model_content(self):
    # Creates with options containing model content successfully.
    with open(self.model_path, 'rb') as f:
      base_options = _BaseOptions(model_asset_buffer=f.read())
      options = _FaceStylizerOptions(base_options=base_options)
      stylizer = _FaceStylizer.create_from_options(options)
      self.assertIsInstance(stylizer, _FaceStylizer)

  @parameterized.parameters(
      (ModelFileType.FILE_NAME, _STYLIZED_IMAGE),
      (ModelFileType.FILE_CONTENT, _STYLIZED_IMAGE))
  def test_stylize(self, model_file_type, expected_detection_result_file):
    # Creates stylizer.
    if model_file_type is ModelFileType.FILE_NAME:
      base_options = _BaseOptions(model_asset_path=self.model_path)
    elif model_file_type is ModelFileType.FILE_CONTENT:
      with open(self.model_path, 'rb') as f:
        model_content = f.read()
      base_options = _BaseOptions(model_asset_buffer=model_content)
    else:
      # Should never happen
      raise ValueError('model_file_type is invalid.')

    options = _FaceStylizerOptions(base_options=base_options)
    stylizer = _FaceStylizer.create_from_options(options)

    # Performs face stylization on the input.
    stylized_image = stylizer.detect(self.test_image)
    # Comparing results.
    self.assertTrue(
      np.array_equal(stylized_image.numpy_view(),
                     self.test_image.numpy_view()))
    # Closes the stylizer explicitly when the stylizer is not used in
    # a context.
    stylizer.close()


if __name__ == '__main__':
  absltest.main()
