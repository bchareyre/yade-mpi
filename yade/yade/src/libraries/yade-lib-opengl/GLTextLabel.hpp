#ifndef __GLTEXTLABEL_H__
#define __GLTEXTLABEL_H__

#include "GLWindow.hpp"
#include "Vector3.hpp"

class GLTextLabel : public GLWindow
{	
	
	private : char * text;
	private : Vector3r textColor;

	private : void glDrawInsideWindow();
	private : void drawString(char * str,int ,int ,Vector3r c);

	// construction
	public : GLTextLabel (int minX=10, int minY=10, int sizeX=10, int sizeY=10,char * text=NULL);
	public : void setText(char * text);
	public : void fitTextSize();
	public : void setTextColor(float r,float g,float b);

	public : ~GLTextLabel ();
};

#endif // __GLTEXTLABEL_H__
