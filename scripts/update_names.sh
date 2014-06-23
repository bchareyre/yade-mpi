#!/bin/sh
# The script modifies authors names not to get duplicated names
# Modifies the git directory!

git filter-branch --commit-filter '
  if [ "$GIT_AUTHOR_EMAIL" = "gladk@debian.org" ];
  then
          GIT_AUTHOR_EMAIL="gladky.anton@gmail.com";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_EMAIL" = "bruno.chareyre@grenoble-inp.fr" ]
  then
          GIT_AUTHOR_EMAIL="bruno.chareyre@hmg.inpg.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_EMAIL" = "christian@localhost.localdomain" ]
  then
          GIT_AUTHOR_NAME="Christian Jakob";
          GIT_AUTHOR_EMAIL="jakob@ifgt.tu-freiberg.de";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_EMAIL" = "donia.marzougui@3sr-grenoble.fr" ] || [ "$GIT_AUTHOR_NAME" = "dmarzougui" ] || [ "$GIT_AUTHOR_NAME" = "Donia" ] || [ "$GIT_AUTHOR_EMAIL" = "marzougui.donia@hotmail.fr" ]
  then
          GIT_AUTHOR_NAME="Donia Marzougui";
          GIT_AUTHOR_EMAIL="donia.marzougui@hmg.inpg.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Francois" ] ||[ "$GIT_AUTHOR_NAME" = "françois" ] || [ "$GIT_AUTHOR_NAME" = "Francois Kneib" ] || [ "$GIT_AUTHOR_NAME" = "François Kneib" ]
  then
          GIT_AUTHOR_NAME="Francois Kneib";
          GIT_AUTHOR_EMAIL="francois.kneib@gmail.com";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Jan Stransky" ] ||[ "$GIT_AUTHOR_NAME" = "Jan Stránský" ] || [ "$GIT_AUTHOR_NAME" = "Jan Stransky" ] || [ "$GIT_AUTHOR_NAME" = "Jan Stránský" ]
  then
          GIT_AUTHOR_NAME="Jan Stransky";
          GIT_AUTHOR_EMAIL="jan.stransky@fsv.cvut.cz";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Jerome Duriez" ]
  then
          GIT_AUTHOR_NAME="Jerome Duriez";
          GIT_AUTHOR_EMAIL="duriez@geo.hmg.inpg.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Luc Scholtes" ] || [ "$GIT_AUTHOR_NAME" = "scholtes" ] 
  then
          GIT_AUTHOR_NAME="Luc Scholtes";
          GIT_AUTHOR_EMAIL="lscholtes63@gmail.com";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Luc Sibille" ]
  then
          GIT_AUTHOR_NAME="Luc Sibille";
          GIT_AUTHOR_EMAIL="luc.sibille@3sr-grenoble.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Raphael Maurin" ]
  then
          GIT_AUTHOR_NAME="Raphaël Maurin";
          GIT_AUTHOR_EMAIL="raphael.maurin@irstea.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Raphael Maurin" ]
  then
          GIT_AUTHOR_NAME="Raphaël Maurin";
          GIT_AUTHOR_EMAIL="raphael.maurin@irstea.fr";
          git commit-tree "$@";
  elif [ "$GIT_AUTHOR_NAME" = "Kubeu" ]
  then
          GIT_AUTHOR_NAME="Alexander Eulitz";
          GIT_AUTHOR_EMAIL="alexander.eulitz@iwf.tu-berlin.de";
          git commit-tree "$@";
  else
          git commit-tree "$@";
  fi' HEAD
  
