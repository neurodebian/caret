# caret
Debian packaging of good old caret


# Building on Ubuntu

## pre-patch

The debianization requires that all modifications to the upstream source be put in patches. If changes are made to files
*outside of the debian/ folder*, then the *patch* should be added to git, but the patched file should remain unmodified. 
The *dquilt patch -a* command applies the patch and restores the modifications. 

Changes made to files *inside the debian/ folder and subfolders* should be committed directly - do not patch files in debian/!

I've done away with this nonsense and hassle with the patched branches - see below. These notes are for posterity....

1. Build deps
1. git clone
1. dquilt patch -a
1. debian/rules build | tee build.log 2>&1
1. dpkg-buildpackage -b -us -uc -nc -rfakeroot
 * -b : binary package, please
 * -us : unsigned package
 * -uc : unsigned changes file
 * -nc : no clean - saves a LOT of time in building. Without this, build will clean first.
 * -rfakeroot : needed to build deb package right
 
 
## post-patch branches
 
These branches have the patches applied and changes committed. For one thing this makes development on windows a lot easier. 
For another thing, why not? Upstream caret development is basically dead, so preserving these patches in the event some 
upstream change makes its way down here to caret is, well, a waste IMHO.

