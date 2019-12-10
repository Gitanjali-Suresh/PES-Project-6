## PES Project 6 ReadMe <br />
# **Team members**<br /> 
Gitanjali Suresh (gisu9983) and Sarayu Managoli (sama2321)<br /><br />
# **Description**<br />
This repository contains the source codes for the implementation of Project 6 for the course Principles of Embedded Software. The repository also includes the output captures from oscilloscopes of Sine Wave generated from Programs 1 and 2.<br /><br />
# **Installation/Execution notes**<br />
1.	Open `MCUXpresso IDE`
2.	Import a new project from `SDK examples` 
3.	Select `frdmkl25z` board
4.	Click on `Next` and provide `project suffix` name
5.  From `examples` select one of the `rtos_examples` with the necessary `drivers`
6.	Click on `Next`
7.	Select `Redirect SDK “PRINTF” to C library “printf”`
8.	Click on `Finish`<br /><br />

# **Build using macros**<br />
1.	In order to run the program in the `Debug` mode, the macro `D_MODE` should be defined
2.	In order to run the program in the `Normal` mode, the program can be run as it is <br /><br />
# **Difficulties faced**<br />
1.	Generation of Sine wave using software timer<br />
2.	Understanding `FreeRTOS` with creation and scheduling of tasks.<br />
3.	Timestamps were not created with `SysTick`, hence Software timer had to be used for the same.<br />
Reading a few references and documentations for the above difficulties helped us in solving the difficulties. <br /><br />

# **Execution report**<br />
In the `Debug` mode, we get various messages such as initialization of drivers, creation of tasks, etc. In the `Normal` mode, general status messages are printed. 
In order to obtain entire sine wave in Program 2, the size of the buffer can be changed to 100 from 64.<br /><br />

# **Documentation for output**<br />
The outputs from the oscilloscope are captured in a separate PDF to show the different signals generated in the 2 programs. This includes signals generated with and without DMA transfer, signals generated with different buffer size definitions, etc.<br /><br />
