ffmpeg -loop 1 -i vf-pose.jpg -c:v libx264 -t 250 -pix_fmt yuv420p -vf scale=640:480 vf-pose.mp4
