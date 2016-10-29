Installation Instructions for OSX:

1. Run the following commands in the terminal:

'ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null

and press enter/return key. Wait for the command to finish. Then run:

'brew install libsndfile

2. Then navigate to the directory of the code, and run 'make' from the terminal.


To Run the Detector:

To run the detector, run './ocean_noise_detector <wav_file_name> and press enter/return key. This will generate an output file named '<wav_file_name>.out' in the same directory, which will contain AIR/WATER tags for each 10 second interval. 


To edit the threshold:

Open main.c, and at the top of the file, adjust the value of the THRESHOLD variable.
