-- Premake file for Dynamics lib of OgreBullet
-- Made by rmbl

package = newpackage()
package.name = "OgreBulletDynamics"
package.kind = "lib"
package.language = "c++"
package.files = {
	matchrecursive("include/*.h", "src/*.cpp")
}

if (linux) then
	package.includepaths = { "include", "../Collisions/include" }
	package.buildoptions = { "`pkg-config OGRE --cflags` `pkg-config bullet --cflags`" }
	package.linkoptions = { "`pkg-config OGRE --libs` `pkg-config bullet --libs`" }
end



