# jetson_rtl8822ce_no_ir_patch
Patching Realtek driver included with Jetson Kernel or OOT drivers.  Used by AzureWave Wifi module included with Jetson Dev Kit.  

# NO IR Legalities
The software code provided here (the "Code") is for informational and educational purposes only. The Code is provided "as is," without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement.

The NO_IR flag is present for a reason.  
Each country has its own regulations for what 5G channels can be emitted on.  
Disabling NO_IR puts the onus on the USER to properly follow local regulations.  
Make sure your hostapd.config file defines legal emission channels for your region. 

I am not responsible for negligence of regulations.  
This patch is required for use cases requiring 5G emission (Wifi Access Point).    

You can pull the latest regulations from git://git.kernel.org/pub/scm/linux/kernel/git/sforshee/wireless-regdb.git  
The db.txt file contains the human readable regulations.  
A db.txt file is included in this repo but might be out of date.  

# jetson download links
jetpack -> L4T mapping: [jetpack](https://developer.nvidia.com/embedded/jetpack-archive)  
L4T Archive: [L4T](https://developer.nvidia.com/embedded/jetson-linux-archive)  

# In-kernel vs Out-Of-Tree
Starting with Jetson Linux 36.2 the rtl8822ce drivers moved from being an kernel-driver to an out-of-tree (oot) driver.  
A different source patch and .ko file is used for kernel-driver vs oot.  

# Real World Testing
I have only preformed the patch for Jetson Linux 35.5.0 and 36.3.0.  
The patch for 35.5.0 will probably work for older version of 35.* or 34.*.  

# What about other Wifi Module Drivers?
Different wifi drivers will follow similar conventions.  
They all use the ieee80211_channel_flags definitions for setting NO_IR, DISABLE, ect.  
Read through the driver and remove the locations setting the NO_IR flag.  

# Determine What jetpack / L4T device is using

```
dpkg -l | grep "nvidia-l4t"
```

# Building Process
I built the drivers directly on the machine, I have not experimented with cross compile.  
Use [jetpack](https://developer.nvidia.com/embedded/jetpack-archive) to determine what L4T version you are using.  
Use [L4T](https://developer.nvidia.com/embedded/jetson-linux-archive) to directly select a L4T version.  

Install openssl development dependencies  
```
sudo apt install libssl-dev
```

Download the source code for the applicable L4T version.
[Driver Package (BSP) Sources (36.3.0)](https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/sources/public_sources.tbz2)

Steps to unpack and build 36.3.  For older version there is no oot, you must build the entire kernel and pull out the rtl8822ce.ko
```
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/sources/public_sources.tbz2
tar -xvf public_sources.tbz2
cd Linux_for_Tegra/source/
tar -xvf kernel_src.tbz2
tar -xvf kernel_oot_modules_src.tbz2 # there will be no oot modules for version < 36.*
tar -xvf nvidia_kernel_display_driver_source.tbz2
./nvbuild.sh -o $PWD/kernel_out -m # builds only the oot
```

## Nvidia Building Instruction
I have mixed results following nvidia build webpage: [Kernal Customization](https://docs.nvidia.com/jetson/archives/r36.3/DeveloperGuide/SD/Kernel/KernelCustomization.html#:~:text=Building%20the%20NVIDIA%20Out-of-Tree%20Modules%201%20Go%20to,update%20the%20initramfs%2C%20run%20the%20following%20commands%3A%20)  
When you unpack the Driver Package (BSP) Sources, and the kernel_source tar there is a build scirpt ./nvbuild.sh  
That build script has consistently worked for me.  

## Improving Speeds
With the Azure wifi module the max theoretical speeds are 433 Mbits/s.  Wifi 5 ac.  
Using modified 35.5.0 kernel drivers, I am unable to reach these speeds.  
If you have a better hostapd configuration file or wifi driver, create an issue/pull request.   