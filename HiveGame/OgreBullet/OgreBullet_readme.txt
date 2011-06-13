Readme for OgreBullet
==================

WINDOWS BUILD
======================
Brief build instructions for building from SVN with MSVC 8 or higher. 

1) Obtain Bullet from http://www.Bullet.org/.
2) Use CMAKE to build a project file for Bullet.
3) Build Bullet.
4) Obtain Ogre from www.Ogre3d.com
5) Use CMAKE to generate a project file.
6) Build Ogre.
7) Obtain the Ogre Dependencies for your compiler from www.Ogre3d.com and extract them into your Ogre directory ex: 'Ogre\dependencies'.
8) Setup the following Environment Variables in Windows:
   a) OGRE_HOME = The original Ogre directory ex: 'c:\Ogre'
   b) OGRE_LIBS = The location of the Ogre lib directory ex: 'c:\OgreBuild\lib'
   c) BULLET_ROOT = The root directory for Bullet ex: 'c:\Bullet'
   d) BULLET_LIBS = The location of the Bullet lib directory ex: 'c:\Bullet\BulletBuild\lib'
   e) (if OIS is built from source) OIS_HOME = The location of your OIS directory ex: 'c:\OIS'
9) Open the solution OgreBullet_VC8.sln to build the OgreBullet library with Microsoft Visual C++ 2005. The executable will be built to your 'ogrebullet\Demos' folder.
10) You may need to manually copy the Ogre DLL files and the OIS*.DLL files to your 'ogrebullet\Demos' location.
11) If you want to run the demo from within MSVC you'll need to set the 'Working Directory' to point to your 'ogrebullet\Demos' location.

*NIX BUILD
=====================
There are scripts available to build OgreBullet. They are in need of an experienced *nix developer to fix them up.