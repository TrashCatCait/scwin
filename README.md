# scwin:
Simple C windows is a philosophy library for creating a window or windows in C basically just made to cut down in the amount of repeated boilerplate in some of my projects. 

NOTE: THIS PROJECT IS UNDERDEVELOPMENT AND DOES NOT CURRENTLY DO WHAT IS LISTED BELOW. This code is being taken out of another project to be made independent from that project 

# Design:
For the design philosophy we just expose a opaque pointer to the user space application calling us and allow control of the window to be done through helper functions without having to know the dirty parts of the window backend. X, Wayland, etc... 

We also support creating hardware accelerated rendering surfaces for the window for opengl and vulkan. Along with a method to get the underlying display and window data structures should you want to create a custom renderer that requires this information. 

# Event management:
In order to manage window events appropriately we expose functions to set event callback functions. That will be called with generalised event data, thus avoiding your app having to handle wayland and X events differently as they will be converted to one shared format. 

The purpose of these callbacks is to allow you to create your own ways of managing events. Please note events can be multithreaded but this is something you would need to setup in the context of your app.

# Working with graphics APIs:
We do not configure any hardware acceleration APIs such as vulkan or opengl(EGL), for you instead intending that you do any needed setup for these platforms. 

However we do provide helper functions to create vulkan and opengl surfaces and displays in the case of opengl. Along with exposing methods to get the underlying details needed to setup rendering for some custom API that supports WSI.(e.g. Cario)

# Software rendering:
- TBD we do plan on allowing/exposing some method for CPU
rendering in rare cases where graphics cards are not available or simply not wanted, but this is currently undecided 

It's unsure as of right now if this would come in the form of X11 like functions to draw basic shapes to the window or just giving you a way to pass a pixel data buffer to window directly.
