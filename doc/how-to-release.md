* Create RELEASE file in the root folder with the version number in it
* Add new changelog entries to Changelog using "git shortlog PREVVERSION.."
* Create branch using the following command and format
  "git checkout -b YYYY.MM"
* Tag release "git tag -as YYYY.MMa -m"YYYY.MMa"
* Return to master branch and remove RELEASE file
* Push master, new branch and tags to github
* Download tar.gz
* Create asc-file (signature): gpg --armor --sign --detach-sig tarball.tar.gz 
* Upload new tarball on Launchpad
* Make announcement on mailing list and on Launchpad

