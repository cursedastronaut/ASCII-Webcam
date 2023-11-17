# ASCII Webcam
It's a simple program in C++ that takes the camera stream and converts it to ASCII characters that it outputs in the terminal.
It currently only works in **Linux**.

## Options
Using command line arguments, you can easily change the program's behavior.

- `-video [device]` | Allows user to enter another camera to use. By default, it is `/dev/video0`.
- `-x [width]` | Allows user to enter a different width. Cannot be above 320.

- `-y [height]` | Allows user to enter a different height. Cannot be above 180.

## Credits
Webcam calls by @sammy17.