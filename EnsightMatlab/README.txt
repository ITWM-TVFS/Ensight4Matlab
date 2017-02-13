EnSightLib wrapper for MATLAB #
===============================

INSTRUCTIONS : 

>> Run 'SETUP.m' to change the path settings for the library dependencies
>> Run the 'runmex.m' script to invoke the mex-c++ compiler. 
   This should produce a file 'EnsightLib.mexa64'.
>> Run the example found in '/examples' to verify the correctness of your 
   installation. 
   If everything works fine, ignore the rest of this file.
 
>> IMPORTANT: The system needs to access  compiled versions of 
	- Qt
        - gcc
	- EnsightLib
   If you have compiled the C++ EnsightLib library locally and have not
   installed it globally, you need to ensure it can be found by MATLAB by
   setting the LD_LIBRARY_PATH environment variable.
   Therefore, add the following lines to your '.bashrc' file
   (located in ~/.bashrc ), where the path is the directory containing the
   file libEnsightLib.so:

    # Include EnSightLib
    export LD_LIBRARY_PATH=/path/to/your/ensight_lib/lib/:$LD_LIBRARY_PATH

>> Open a console and run 'source ~/.bashrc'. Then, restart MATLAB.
