# this script must be run from the top-level source directory (the one with SConstruct)
RELEASE=$1
if [ -z "$RELEASE" ]; then echo "Pass the version number as first argument"; exit 1; fi
if [ ! -e "SConstruct" ]; then echo "Run this script from the top-level source directory (sh scripts/make-release.sh $RELEASE)"; exit 1; fi
echo $1 > RELEASE
MYPWD=`/bin/pwd`
MYSUBDIR=${MYPWD##*/}
MYPARENT=${MYPWD%/*}
#echo $MYPWD $MYPARENT $MYSUBDIR
cd $MYPARENT; mv $MYSUBDIR yade-$RELEASE; tar c yade-$RELEASE | bzip2 > yade-$RELEASE.tar.bz2; mv yade-$RELEASE $MYSUBDIR; cd $MYSUBDIR
rm RELEASE
ls -l ../yade-$RELEASE.tar.bz2
echo "All done, bye."
