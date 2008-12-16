%==============================
% simpledoc.cls
% miniclasse to write very simple docs  
% Vincent Richefeu 2003, 2006
% last modifications : 15/02/2006
%==============================
\ProvidesClass{gdmdoc}
\LoadClass[twoside,a4paper]{refart}

%\RequirePackage[french]{varioref}
%\RequirePackage[french]{babel} 
\RequirePackage[latin1]{inputenc}  % for PC
%\RequirePackage[applemac]{inputenc} % for apple
\RequirePackage{ifpdf}

\RequirePackage{amsmath}
\RequirePackage{graphicx}
\RequirePackage{float}
\RequirePackage[sort]{natbib}
%\RequirePackage{tocloft}

%
\RequirePackage{listings}

\lstdefinestyle{cppCode}{
language = C++,
frame = single,
numbers=left,
numberstyle=\tiny,
basicstyle=\small,
}

\lstdefinestyle{mgpdata}{
language = C++,
frame = single,
numberstyle=\tiny,
basicstyle=\small
}

%%
