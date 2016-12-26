Vagrant files for setting up Annabell in a VM
=============================================

Recently I had to setup Annabell's dev environment on my mac, and I found that compiling annabell in an OSX environment can be quite difficult.

So, instead I ended up setting up a vagrant instance, so I could keep working in an Ubuntu environment.

Installation and usage
----------------------

In this folder there are two files. Copy both of them to a folder of your convenience:

 - **Vagrantfile**: this file contains a very basic definition of the VM, that points to the `bootstrap.sh` file for provisioning.
 - **bootstrap.sh**: this is the shell script that will install all required software dependencies, and also build annabell for the first time so when vagrant is done provisioning and you enter the VM by `vagrant ssh`, you will already have a compiled version of annabell ready (and also, this way if something is wrong with the environment you will notice right away).

After copying the files, go the folder were you placed them and do a `vagrant up`. After it finishes provisioning, do a `vagrant ssh`, and you are all set.

For more info on installing and using vagrant, vagrant files and provisioning please refer to https://www.vagrantup.com/
