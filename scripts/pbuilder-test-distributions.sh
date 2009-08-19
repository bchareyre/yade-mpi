#
# grab latest trunk, try building packages for various distributions
# see http://yade.wikia.com/wiki/DebianPackages for configuration of the pbuilder environment
#
set -x -e
TMP=/tmp/pbuild-yade-$$
mkdir $TMP
cd $TMP
bzr checkout --lightweight lp:yade trunk
cd trunk; scripts/debian-prep jaunty; cd .. # distro not important here
dpkg-source -b -I trunk
DSC=yade-`cat trunk/VERSION`_1.dsc
for DIST in hardy jaunty karmic lenny squeeze; do
	BASETGZ=/var/cache/pbuilder/$DIST.tgz
	sudo pbuilder --build --basetgz $BASETGZ --debbuildopts "-j5 -Zlzma" $DSC
done
rm -rf $TMP
