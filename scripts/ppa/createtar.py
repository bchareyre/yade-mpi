#!/usr/bin/env python
import argparse, os, git, shutil


jobsNumber = 6

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument("g", help="git debian-directory")
parser.add_argument("p", help="direcrory, where will be created pbuilder tarballs")
parser.add_argument("u", help="git upstream-direcrory")
parser.add_argument("-u","--update", help="update tarballs, if they are exist", action='store_true')
args = parser.parse_args() 

gitdebdir = args.g
gitupsdir = args.u
pbdir     = args.p

if not(os.path.isdir(gitdebdir)):
    raise RuntimeError('Git-debian-directory does not exists')

if not(os.path.isdir(gitupsdir)):
    raise RuntimeError('Git-upstream-directory does not exists')

if not(os.path.isdir(pbdir)):
    print ('Pbuilder-directory does not exists. Creating')
    os.mkdir(pbdir)

repodeb = git.Repo(gitdebdir)
repoups = git.Repo(gitupsdir)
assert repodeb.bare == False
assert repoups.bare == False

if (repodeb.is_dirty()):
    raise RuntimeError('Git-debian-repo has an uncommitted changes. Exiting.')

if (repoups.is_dirty()):
    raise RuntimeError('Git-upstream-repo has an uncommitted changes. Exiting.')

for branch in repodeb.branches:
    branchstr = str(branch)
    if (branchstr<>'master'):
        print "Switching to branch %s"%(branch)
        repodeb.git.checkout(branch)
        infile = open(gitdebdir+"/pbuilder")
        lines = infile.readlines()
        mirror = lines[0].strip()
        components = lines[1].strip()
        archs = lines[2].split()
        keyringuse = lines[3].strip()
        othermirror = lines[4].strip()
        infile.close()
        for a in archs:
            tarball = "%s/%s_%s.tgz"%(pbdir, branchstr, a.strip())
            addAllowuntrusted = ""
            if (othermirror!="#"):
                addAllowuntrusted =  " --allow-untrusted "
            if not(os.path.isfile(tarball)):
                createPbTar = ('sudo pbuilder --create --distribution %s --mirror %s --components "%s" --architecture %s --debootstrapopts "--keyring=%s" --basetgz %s'%
                        (branchstr, mirror, components, a, keyringuse, tarball))
                if (othermirror!="#"):
                    createPbTar += ' --othermirror "' + othermirror + '"'
                    addAllowuntrusted =  " --allow-untrusted "
                print createPbTar

                print "Creating tarball %s"%(tarball)
                os.system(createPbTar)
            else:
                print "Tarball %s exists"%(tarball)
                if (args.update):
                    print "Updating %s as requested" %(tarball)
                    updatePbTar = ('sudo pbuilder --update --basetgz %s'%(tarball))
                    os.system(updatePbTar)
            
            # Creating dir for building
            builddirup="%s_%s/"%(branchstr, a)
            builddirdeb="%s/build/"%(builddirup)
            builddirres="%s/result/"%(builddirup)
            shutil.rmtree(builddirdeb,ignore_errors=True)
            shutil.copytree(gitupsdir, builddirdeb )
            shutil.rmtree(builddirdeb+".git")
            # Get package version
            versiondebian = repoups.git.describe() + "~" + branchstr

            # Get package name
            infilepname = open(gitdebdir+"/changelog"); sourcePackName = infilepname.readlines()[0].split()[0]
            print sourcePackName

            os.system('cd %s; apack %s_%s.orig.tar.xz build'%(builddirup,sourcePackName,versiondebian))
            shutil.copytree(gitdebdir, builddirdeb+"/debian")

            os.system('sed -i.bak -e s/VERSION/%s/ -e s/DISTRIBUTION/%s/ %s/debian/changelog'%(versiondebian,branch,builddirdeb))
            os.system('cd %s; dpkg-source -b -I build'%(builddirup))
            os.mkdir(builddirres)
            print "Building package %s_%s"%(sourcePackName, versiondebian)
            buildPackage = ('sudo pbuilder --build --architecture %s --basetgz %s %s --logfile %s/pbuilder.log --debbuildopts "-j%d" --buildresult %s %s/*.dsc'%
                (a, tarball, addAllowuntrusted, builddirup, jobsNumber, builddirres, builddirup))
            print buildPackage
            os.system(buildPackage)
