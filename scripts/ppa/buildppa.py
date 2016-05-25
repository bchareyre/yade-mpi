#!/usr/bin/env python3
import argparse, os, git, shutil, sys, time

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument("-i", help="config file")
parser.add_argument("-u","--update", help="update tarballs, if they are exist", action='store_true')
parser.add_argument("-j", help="number of processors to build",dest='jobsNumber',default=6, type=int)
args = parser.parse_args() 


configfile = args.i
jobsNumber = args.jobsNumber

infileconf  = open(configfile, 'r')
linesconf   = infileconf.readlines()
gitdebdir   = linesconf[1].strip()
pbdir       = linesconf[4].strip()
gitupsdir   = linesconf[7].strip()
userg       = linesconf[10].strip()
groupg      = linesconf[13].strip()
dputg       = linesconf[16].strip()
keyg        = linesconf[19].strip()
keypasspath = linesconf[22].strip()
keysecrpath = linesconf[25].strip()
patharchive = linesconf[28].strip()

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
    print (repodeb.untracked_files)
    raise RuntimeError('Git-debian-repo has an uncommitted changes. Exiting.')

if (repoups.is_dirty()):
    print (repoups.untracked_files)
    raise RuntimeError('Git-upstream-repo has an uncommitted changes. Exiting.')

for branch in repodeb.branches:
    branchstr = str(branch)
    if (branchstr!='master'):
        print ("Switching to branch %s"%(branch))
        repodeb.git.checkout(branch)
        infile = open(gitdebdir+"/pbuilder", 'r')
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
                print (createPbTar)

                print ("Creating tarball %s"%(tarball))
                os.system(createPbTar)
            else:
                print ("Tarball %s exists"%(tarball))
                if (args.update):
                    print ("Updating %s as requested" %(tarball))
                    updatePbTar = ('sudo pbuilder --update --basetgz %s'%(tarball))
                    os.system(updatePbTar)
            
            # Creating dir for building

            builddirup="%s_%s/"%(branchstr, a)
            builddirdeb="%s/build/"%(builddirup)
            builddirres="%s/result/"%(builddirup)
            #shutil.rmtree(builddirdeb,ignore_errors=True)
            shutil.rmtree(builddirup,ignore_errors=True)
            shutil.copytree(gitupsdir, builddirdeb )
            shutil.rmtree(builddirdeb+".git")
            # Get package version
            versiondebian = repoups.git.describe()[0:-8] + repoups.head.commit.hexsha[0:7] + "~" + branchstr

            # Get package name
            infilepname = open(gitdebdir+"/changelog", 'r'); sourcePackName = infilepname.readlines()[0].split()[0]
            print (sourcePackName)

            os.system('cd %s; apack %s_%s.orig.tar.xz build'%(builddirup,sourcePackName,versiondebian))
            shutil.copytree(gitdebdir, builddirdeb+"/debian")

            os.system('sed -i.bak -e s/VERSION/%s/ -e s/DISTRIBUTION/%s/ %s/debian/changelog'%(versiondebian,branch,builddirdeb))
            os.system('sed -i.bak -e s/VERSIONYADEREPLACE/%s/ %s/debian/rules'%(versiondebian,builddirdeb))
            os.system('cd %s; dpkg-source -b -I build'%(builddirup))
            os.mkdir(builddirres)
            
            buildarch = ''
            if (len(archs)>1 and a != archs[0]):
              buildarch = '--binary-arch'     #Build only arch-packages
            
            print ("Building package %s_%s"%(sourcePackName, versiondebian))
            buildPackage = ('sudo pbuilder --build --architecture %s --basetgz %s %s --logfile %s/pbuilder.log --debbuildopts "-j%d" --buildresult %s %s %s/*.dsc'%
                (a, tarball, addAllowuntrusted, builddirup, jobsNumber, builddirres, buildarch, builddirup))
            print (buildPackage)
            os.system(buildPackage)
            os.system('sudo chown %s:%s %s * -R'%(userg, groupg, builddirup))
            os.system('sudo chown %s:%s %s * -R'%(userg, groupg, gitdebdir))
            os.system('sudo chown %s:%s %s * -R'%(userg, groupg, gitupsdir))
            os.system('cd %s ; su %s -c \'dput %s *.changes\''%(builddirres, userg, dputg))

for branch in repodeb.branches:
    branchstr = str(branch)
    if (branchstr!='master'):
        os.system('rm -rf %s/%s/Release.gpg ; su %s -c \'gpg --no-tty --digest-algo SHA512 --batch --default-key "%s" --detach-sign --passphrase-fd=0 --passphrase-file=%s -o %s/%s/Release.gpg %s/%s/Release\''%(patharchive, branch, userg, keyg, keypasspath, patharchive, branch, patharchive, branch))
