# DUMBO
**D**efinitely **U**seless **M**ouse ro**B**ot **O**kay.

Project built off an RC car. Unsure of this specifics as I bought it a second hand shop (bargain!) eons ago (in a galaxy far, far away...).
These are my two guesses, but I think the FCC had other RC cars with similiar chassis, so really hard to find the exact one.
- Scientific Toys (n.d.), *R/C 2.4GHz 1:12 Sand Blaster*, http://www.scientific.com.hk/en/products/97213, accessed 24 May 2023
- FCC (2017), *FCC ID BY35058-24GR R/C Vehicle by Scientific Toys Ltd.*, FCC ID, https://fccid.io/BY35058-24GR, accessed 24 May 2023

## Design Process
Contains concept sketches, mechanical drawings (hand drawn), schematics, and flowchart. Some basic pieces of the design process.

## Protoype
All inside the folder `Prototype`. 
- Contains an .STL to 3D print to hold the rear assembly, of the RC car, in place, replacing the need of the original chassis cover.
- Code is a bit of a mess, so attempt to understand at your own peril.


## Finals
BEWARNED, things are messy; view at your own peril :D

### 3D Printing
Contains .STL's and some slicing images


### Code
`powerHD-1501MG` is the testing code to calibrate/check the servo is steering correctly.
`Version3_reversing attempt` and `Version4` were coded to use two shift registers in, cascaded. However, they were not needed in the final DUMBO. `shift_registers` is a testing file to make sure they are working, and make it easier to debug, compared to integrating it straight into the main code and figuring out "what the HECK is wrong?!??!?".
`Version6` is the last version. During testing, two Ultrasonics were needed to be registered stuck for DUMBO to be stuck, but performance might be better with only one required. Dunno.

### Images
Here are some:
![DUMBO angled image](https://github.com/NoahLobbe/DUMBO/blob/8cb51901ea5705011769643a7de06da44f109280/Finals/Images/IMG_5831.JPG)
![DUMBO insides](https://github.com/NoahLobbe/DUMBO/blob/8cb51901ea5705011769643a7de06da44f109280/Finals/Images/IMG_5832.JPG)
![Top Shell under construction](https://github.com/NoahLobbe/DUMBO/blob/7a0fb79ec815d9e6deacdc88252c144873da66a3/Finals/Images/IMG_1087.JPG)
