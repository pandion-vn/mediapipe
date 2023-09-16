fswatch -o . | xargs -n1 -I{} rsync -azP --exclude=.git --exclude=bazel-bin --exclude=bazel-mediapipe --exclude=bazel-out --exclude=bazel-testlogs . deploy@pa1s020:~/hdd/Workspace/mediapipe
