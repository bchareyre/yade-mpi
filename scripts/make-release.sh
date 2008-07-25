# this script must be run from the top-level source directory (the one with SConstruct)
RELEASE=$1
echo $1 > RELEASE
MYPWD=`/bin/pwd`
MYSUBDIR=${MYPWD##*/}
MYPARENT=${MYPWD%/*}
#echo $MYPWD $MYPARENT $MYSUBDIR
cd $MYPARENT; mv $MYSUBDIR yade-$RELEASE; tar c yade-$RELEASE | bzip2 > yade-$RELEASE.tar.bz2
# cd $MYSUBDIR
echo Release tarball is ../yade-$RELEASE.tar.bz2

