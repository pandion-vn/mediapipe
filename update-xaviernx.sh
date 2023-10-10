fswatch -o . | xargs -n1 -I{} rsync -azP --exclude=.git --exclude=bazel-bin --exclude=bazel-mediapipe --exclude=bazel-out --exclude=bazel-testlogs . xaviernx@192.168.11.133:~/nvme/mediapipe
